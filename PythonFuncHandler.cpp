#include "PythonFuncHandler.h"

#ifdef WIN32
#  include <windows.h>
#  include <psapi.h>
#else
std::vector<std::string> DLScopeGuard::m_asDlListStatic;
#endif /* WIN32 */

// =============================================================================

internal_::DLScopeGuard::DLScopeGuard()
     : m_pHandle(NULL)
     , m_bDoCleanup(false)
{
     m_pHandle = internal_::PythonHandler::get().load_python_library(NULL);
     if (m_pHandle != NULL) {
	  m_bDoCleanup = true;
     }
}

internal_::DLScopeGuard::DLScopeGuard(const char* tLibName)
     : m_pHandle(NULL)
     , m_bDoCleanup(false)
{
     m_pHandle = internal_::PythonHandler::get().load_python_library(tLibName);
     if (m_pHandle == NULL) {
	  ERR("internal_::PythonHandler: Failed to open " << tLibName);
     }
     else {
	  m_bDoCleanup = true;
     }
}
     
internal_::DLScopeGuard::~DLScopeGuard()
{
     if (m_bDoCleanup) {
	  if (m_pHandle != NULL) {
	       OUT("--------------------------------------------------------------------------------");
	       OUT("Doing cleanup!");
	       _cleanup();
	  }
	  OUT("--------------------------------------------------------------------------------");

	  std::vector<std::string> asDllList(_list_loaded_libraries());
	  OUT("internal_::DLScopeGuard::~DLScopeGuard(): List of loaded libraries:");
	  for (unsigned int i(0); i < asDllList.size(); ++i) {
	       OUT("internal_::DLScopeGuard::~DLScopeGuard():   " << asDllList[i]);	  
	  }
	  OUT("--------------------------------------------------------------------------------");
     }
     else {
	  OUT("internal_::DLScopeGuard::~DLScopeGuard(): skipping cleanup routines");
     }
}

// --------------------------------

bool internal_::DLScopeGuard::to_ignore_predicate(const std::string& rsLibName)
{
     if (rsLibName.size() < 20) {
	  return true;
     }
     return false;
}

// --------------------------------

void internal_::DLScopeGuard::_cleanup()
{
     if (MyPy::Py_Finalize != NULL) {
	  OUT("internal_::DLScopeGuard::_cleanup(): calling Py_Finalize()");
	  MyPy::Py_Finalize();
     }
     
     OUT("internal_::DLScopeGuard::_cleanup(): calling unload_python_library()");
     internal_::PythonHandler::get().unload_python_library();
     
     OUT("internal_::DLScopeGuard::_cleanup(): calling list_loaded_libraries");
     std::vector<std::string> asDllList(_list_loaded_libraries());

     asDllList.erase(std::remove_if(asDllList.begin(),
				    asDllList.end(),
				    to_ignore_predicate),
		     asDllList.end());
     
     OUT("internal_::DLScopeGuard::_cleanup(): BEGIN cleanup");
     while (!asDllList.empty()) {
	  OUT("internal_::DLScopeGuard::_cleanup(): processing " << asDllList.back());
	  void* pHandle = dlopen(asDllList.back().c_str(),
#ifdef WIN32
				 RTLD_NOW // no support for RTLD_NOLOAD
#else
				 RTLD_NOW|RTLD_NOLOAD				      
#endif /* WIN32 */
	       );
	       
	  if (pHandle) {
	       const unsigned int ulNmax(50);
	       for (unsigned int ulI(0) ;
		    ulI < ulNmax && !dlclose(pHandle) ;
		    ++ulI);
	  }
	  
	  asDllList.pop_back();
     }
     OUT("internal_::DLScopeGuard::_cleanup(): DONE cleanup");     
}

// --------------------------------

#ifdef WIN32
std::vector<std::string> internal_::DLScopeGuard::_list_loaded_libraries()
{
     std::vector<std::string> m_asDllList;
     HANDLE hProcess(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				 FALSE, GetCurrentProcessId()));
     if (hProcess) {
	  HMODULE hMods[1024];
	  DWORD cbNeeded;

	  if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
	       const DWORD SIZE(cbNeeded / sizeof(HMODULE));
	       for (DWORD i(0); i < SIZE; ++i) {
		    TCHAR szModName[MAX_PATH];

		    // Get the full path to the module's file.
		    if (GetModuleFileNameEx(hProcess,
					    hMods[i],
					    szModName,
					    sizeof(szModName) / sizeof(TCHAR))) {
#  ifdef UNICODE
			 std::wstring wStr(szModName);
			 std::string tModuleName(wStr.begin(), wStr.end());
#  else
			 std::string tModuleName(szModName);
#  endif /* UNICODE */
			 if (tModuleName.substr(tModuleName.size()-3) == "dll") {
			      m_asDllList.push_back(tModuleName);
			 }
		    }
	       }
	  }
	  CloseHandle(hProcess);
     }
     return m_asDllList;
}
#else
int internal_::DLScopeGuard::dl_list_callback(struct dl_phdr_info *info,
					      size_t,
					      void *)
{
     m_asDlListStatic.push_back(info->dlpi_name);
     if (m_asDlListStatic.back().empty()) {
	  m_asDlListStatic.pop_back();
     }
     return 0;
}
     
// --------------------------------

std::vector<std::string> internal_::DLScopeGuard::_list_loaded_libraries()
{
     m_asDlListStatic.clear();
     dl_iterate_phdr(DLScopeGuard::dl_list_callback, NULL);
     return m_asDlListStatic;
}
#endif /* WIN32 */ 

// =============================================================================
// =============================================================================

internal_::PythonHandler& internal_::PythonHandler::get()
{
     static PythonHandler singleton;
     return singleton;
}

// =============================================================================

internal_::PythonHandler::PythonHandler()
     : m_pDLHandle(NULL),
       m_asList()
{}

// -----------------------------------------------------------------------------

void* internal_::PythonHandler::load_python_library(const char* tLibName)
{
     if (m_pDLHandle != NULL) {
	  // if m_pDLHandle is not NULL, then m_asList was also updated
	  // successfully
	  return m_pDLHandle;
     }
     else if (tLibName == NULL) {
	  /* This is the case where the library was not previously loaded
	   * and we don't want to load it.
	   */
	  return NULL;
     }
     else {
	  m_pDLHandle = dlopen(tLibName, RTLD_NOW | RTLD_GLOBAL);
	  if (!m_pDLHandle) {
	       ERR("internal_::PythonHandler::load_python_library(): "
		   << " Cannot open library: " << dlerror() << '\n');
	       m_pDLHandle = NULL;
	       return NULL;
	  }
	  
	  if (m_asList.update(m_pDLHandle)) {
	       return m_pDLHandle;
	  }
	  else {
	       dlclose(m_pDLHandle);
	       m_pDLHandle = NULL;	  
	       return NULL;
	  }
     }
}

// -----------------------------------------------------------------------------

void internal_::PythonHandler::unload_python_library()
{
     m_asList.reset_ptr();
     if (m_pDLHandle) {
	  dlclose(m_pDLHandle);
	  m_pDLHandle = NULL;
     }
}

// =============================================================================

void* load_python_library(const char* tLibName)
{
     return internal_::PythonHandler::get().load_python_library(tLibName);
}

void unload_python_library()
{
     internal_::PythonHandler::get().unload_python_library();
}

// =============================================================================
