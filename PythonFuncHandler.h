#ifndef PYTHONFUNCTIONHANDLER_H_INCLUDED
#define PYTHONFUNCTIONHANDLER_H_INCLUDED

#include <iostream>
#define OUT(out) std::cout << out << std::endl
#define ERR(out) std::cerr << out << std::endl

#include <algorithm>
#include <string>
#include <vector>

#include "Python.h"

// =============================================================================

#ifdef WIN32
// Use local version of include file for Windows
#  include "dlfcn_win32.h"
#else
#  include <dlfcn.h>
#  include <link.h> // for dl_iterate_phdr
#endif /* WIN32 */

// =============================================================================
     
namespace internal_ {
     class DLScopeGuard
     {
     public:
	  DLScopeGuard();
	  DLScopeGuard(const char* tLibFilename);
	  ~DLScopeGuard();

	  bool bIsOk() const { return m_pHandle != NULL; }
	  void disableCleanup() { m_bDoCleanup = false; }

	  static bool to_ignore_predicate(const std::string& rsLibName);

#ifndef WIN32
	  static int dl_list_callback(struct dl_phdr_info *info, size_t, void *);
#endif /* !WIN32 */

     private:
	  void _cleanup();
	  
	  std::vector<std::string> _list_loaded_libraries();
	  
	  void* m_pHandle;
	  bool  m_bDoCleanup;

#ifndef WIN32
	  // Definition in PythonFunctionHandler.cpp
	  static std::vector<std::string> m_asDlListStatic;
#endif /* !WIN32 */
     };

     // ========================================================================

     struct Node
     {
	  Node() : pNext(NULL) {}
	  virtual ~Node() {}
	  virtual bool update(void*) = 0;
	  virtual void reset_ptr() = 0;

	  virtual const std::string& name() const = 0;

	  Node* pNext;
     };

     // ------------------------------------------------------------------------

     class PythonList
     {
	  template <typename ptr_t>
	  struct PythonNode : public Node
	  {
	       PythonNode(std::string tPythonName, ptr_t& pPython);

	       virtual bool update(void* pHandle);
	       virtual void reset_ptr();

	       virtual const std::string& name() const {return m_tName;}
	  
	       std::string m_tName;
	       ptr_t& m_rPtr;
	  };

     public:
	  PythonList() : m_pHead(NULL) {}
	  ~PythonList() { clear(); }

	  template <typename ptr_t>
	  void append(const std::string& tName, ptr_t& pFunc);
	  inline bool update(void* pHandle);

	  inline void reset_ptr();
	  inline void clear();

	  Node* head() const { return m_pHead; }

     private:
	  Node* m_pHead;
     };

     // ------------------------------------------------------------------------
     
     /*
      * This class is not meant to be used directly!
      * Use the macros below instead unless you know what you're doing...
      */
     class PythonHandler
     {
     public:
	  static PythonHandler& get();

	  /*
	   * Try to load the python library pointed by tLibName.
	   *
	   * If a python library has previously been loaded, this function simply 
	   * return the original handle.
	   *
	   * If tLibName is NULL and the python library was not previouly loaded
	   * this method simply returns NULL without doing anything else.
	   *
	   * In all other cases, the function will try to load the python library
	   * indicated by tLibName and return a non-NULL value on success.
	   */
	  void* load_python_library(const char* tLibName);
	  void unload_python_library();

	  template <typename func_t>
	  func_t add_ptr(const std::string& tName, func_t& pFunc);

	  void clear() { m_asList.clear(); }

	  void* get_handle() const { return m_pDLHandle; }
     
     private:
	  void* m_pDLHandle;
	  PythonList m_asList;
     
	  PythonHandler();

	  // Not implemented!
	  PythonHandler(const PythonHandler&);
	  PythonHandler& operator=(const PythonHandler&);
     };

     typedef PythonHandler PH;
} // namespace internal_

using internal_::DLScopeGuard;

// -----------------------------------------------------------------------------

/*
 * These functions are provided for testing purposes only.
 * In your code you should be using the DLScopeGuard class that takes care of
 * loading/unloading of the library automatically, even in case of exception
 * throwing.
 */
void* load_python_library(const char* tLibName);
void unload_python_library();

// =============================================================================
// Functions/methods definitions

namespace internal_ {
     // ------------------------------------------------------------------------
     // PythonList::PythonNode

     template <typename ptr_t>
     PythonList::PythonNode<ptr_t>::PythonNode(std::string tName, ptr_t& rPtr)
	  : m_tName(tName),
	    m_rPtr(rPtr)
     {}

     
     template <typename ptr_t>
     void PythonList::PythonNode<ptr_t>::reset_ptr()
     {
	  m_rPtr = NULL;
     }
     
     template <typename ptr_t>
     bool PythonList::PythonNode<ptr_t>::update(void* pHandle)
     {
	  if (!pHandle) {
	       ERR("Invalid DLL handle pointer");
	       return false;
	  }

	  dlerror();
	  void* pPtr(dlsym(pHandle, m_tName.c_str()));
	  const char *dlsym_error = dlerror();
	  if (dlsym_error) {
	       ERR("Cannot load symbol '"
		   << m_tName << "': "
		   << dlsym_error);
	       return false;
	  }

	  if (!pPtr) {
	       ERR("Invalid pointer detected for '"
		   << m_tName << "'");
	       return false;
	  }
	  m_rPtr = reinterpret_cast<ptr_t>(pPtr);
	  return true;
     }

     // ------------------------------------------------------------------------
     // PythonList
     
     template <typename ptr_t>
     void PythonList::append(const std::string& tName, ptr_t& rPtr)
     {
	  Node *n(new PythonNode<ptr_t>(tName, rPtr));
	  n->pNext = m_pHead;
	  m_pHead = n;
     }
     
     bool PythonList::update(void *pHandle)
     {
	  for (Node* pPtr(m_pHead); pPtr ; pPtr = pPtr->pNext) {
	       if (!pPtr->update(pHandle)) {
		    return false;
	       }
	  }
	  return true;
     }

     void PythonList::reset_ptr()
     {
	  for (Node* pPtr(m_pHead); pPtr ; pPtr = pPtr->pNext) {
	       pPtr->reset_ptr();
	  }
     }
     
     void PythonList::clear()
     {
	  while (m_pHead) {
	       Node* n(m_pHead);
	       m_pHead = m_pHead->pNext;
	       delete n;
	  }
     }

     // ------------------------------------------------------------------------
     // FuncHandler

     template <typename ptr_t>
     ptr_t PythonHandler::add_ptr(const std::string& tName, ptr_t& rPtr)
     {
	  m_asList.append(tName, rPtr);
	  return NULL;
     }
} // namespace internal_

// =============================================================================

#include "PythonPatchDecls.h"

// =============================================================================


#endif //PYTHONFUNCTIONHANDLER_H_INCLUDED
