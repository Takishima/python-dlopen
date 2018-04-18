#include "PythonFuncHandler.h"

#include <string>

int main(int argc, char *argv[])
{
     // First argument is path to python library
     if (argc < 2) {
	  return -1;
     }
     
     {
	  DLScopeGuard guard(argv[1]);
	  if (!guard.bIsOk()) {
	       std::cerr << "Loading python library failed!" << std::endl;
	       return -1;
	  }
     
	  MyPy::Py_Initialize();

	  MyPy::Py_SetProgramName("Test");
	  MyPy::PyRun_SimpleString("import sys");
	  MyPy::PyRun_SimpleString("import os.path as op");
	  MyPy::PyRun_SimpleString("print(1)");
	  MyPy::Py_Finalize();
     }
     std::cout << "================================================================================\n";
     {
	  DLScopeGuard guard(argv[1]);
	  if (!guard.bIsOk()) {
	       std::cerr << "Loading python library failed!" << std::endl;
	       return -1;
	  }
     
	  MyPy::Py_Initialize();

	  MyPy::Py_SetProgramName("Test");
	  MyPy::PyRun_SimpleString("import sys");
	  MyPy::PyRun_SimpleString("import os.path as op");
	  MyPy::PyRun_SimpleString("print(1)");
	  MyPy::PyRun_SimpleString("import numpy");
	  MyPy::PyRun_SimpleString("a = numpy.array([0,1,2,3,4,5,6,7,8,9])");
	  MyPy::PyRun_SimpleString("print(a, type(a))");
	  MyPy::Py_Finalize();
     }
     std::cout << "================================================================================\n";
     {
	  DLScopeGuard guard(argv[1]);
	  if (!guard.bIsOk()) {
	       std::cerr << "Loading python library failed!" << std::endl;
	       return -1;
	  }
     
	  MyPy::Py_Initialize();

	  MyPy::Py_SetProgramName("Test");
	  MyPy::PyRun_SimpleString("import sys");
	  MyPy::PyRun_SimpleString("import os.path as op");
	  MyPy::PyRun_SimpleString("print(1)");
	  MyPy::PyRun_SimpleString("import numpy");
	  MyPy::PyRun_SimpleString("a = numpy.array([0,1,2,3,4,5,6,7,8,9])");
	  MyPy::PyRun_SimpleString("print(a, type(a))");
	  MyPy::Py_Finalize();
     }
     
     return 0;
}
