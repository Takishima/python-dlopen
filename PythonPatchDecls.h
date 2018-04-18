#ifndef PYTHONPATCHDECLS_H_INCLUDED
#define PYTHONPATCHDECLS_H_INCLUDED

/*
 *       / \                                                           / \      
 *      / | \        This file is not intended to be directly         / | \     
 *     /  |  \       included in user code !                         /  |  \    
 *    /       \                                                     /       \   
 *   /    *    \                                                   /    *    \  
 *  -------------                                                 -------------
 */

// =============================================================================
// Redefine some macros

#ifdef Py_None
#  undef Py_None
#endif
#define Py_None (MyPy::_Py_NoneStruct)

#ifdef Py_False
#  undef Py_False
#endif
#if PY_MAJOR_VERSION == 2
#  define Py_False ((PyObject *) &MyPy::_Py_ZeroStruct)
#elif PY_MAJOR_VERSION == 3
#  define Py_False ((PyObject *) &MyPy::_Py_FalseStruct)
#endif /* PY_MAJOR_VERSION */

#ifdef Py_True
#  undef Py_True
#endif
#define Py_True ((PyObject *) &MyPy::_Py_TrueStruct)

#ifdef Py_RETURN_NAN
#  undef Py_RETURN_NAN
#endif
#define Py_RETURN_NAN return MyPy::PyFloat_FromDouble(Py_NAN)

#ifdef Py_RETURN_INF
#  undef Py_RETURN_INF
#endif
#define Py_RETURN_INF(sign) do					\
	  if (copysign(1., sign) == 1.) {			\
	       return MyPy::PyFloat_FromDouble(Py_HUGE_VAL);	\
	  } else {						\
	       return MyPy::PyFloat_FromDouble(-Py_HUGE_VAL);	\
	  } while(0)

// -----------------------------------------------------------------------------

#ifdef PyBool_Check
#  undef PyBool_Check
#endif
#define PyBool_Check(x) (Py_TYPE(x) == MyPy::PyBool_Type)

// -------------------------------------

// PyDict_Check does not need to be patched

#ifdef PyDict_CheckExact
#  undef PyDict_CheckExact
#endif
#define PyDict_CheckExact(op) (Py_TYPE(op) == MyPy::PyDict_Type)

// -------------------------------------

// PyInt_Check does not need to be patched

#ifdef PyInt_CheckExact
#  undef PyInt_CheckExact
#endif
#define PyInt_CheckExact(op) ((op)->ob_type == MyPy::PyInt_Type)

// -------------------------------------

#ifdef PyFloat_Check
#  undef PyFloat_Check
#endif
#define PyFloat_Check(op) PyObject_TypeCheck(op, MyPy::PyFloat_Type)

#ifdef PyFloat_CheckExact
#  undef PyFloat_CheckExact
#endif
#define PyFloat_CheckExact(op) ((op)->ob_type == MyPy::PyFloat_Type)

// -------------------------------------

// PyList_Check does not need to be patched

#ifdef PyList_CheckExact
#  undef PyList_CheckExact
#endif
#define PyList_CheckExact(op) (Py_TYPE(op) == MyPy::PyList_Type)

// -------------------------------------

// PyLong_Check does not need to be patched

#ifdef PyLong_CheckExact
#  undef PyLong_CheckExact
#endif
#define PyLong_CheckExact(op) (Py_TYPE(op) == MyPy::PyLong_Type)

// -------------------------------------

#ifdef PyModule_Check
#  undef PyModule_Check
#endif
#define PyModule_Check(op) PyObject_TypeCheck(op, MyPy::PyModule_Type)

#ifdef PyModule_CheckExact
#  undef PyModule_CheckExact
#endif
#define PyModule_CheckExact(op) (Py_TYPE(op) == MyPy::PyModule_Type)

// -------------------------------------

// PyString_Check does not need to be patched

#ifdef PyString_CheckExact
#  undef PyString_CheckExact
#endif
#define PyString_CheckExact(op) (Py_TYPE(op) == MyPy::PyString_Type)

// -----------------------------------------------------------------------------

#ifdef Py_InitModule
#  undef Py_InitModule
#endif
#define Py_InitModule(name, methods)					\
     MyPy::Py_InitModule4(name, methods, (char *)NULL, (PyObject *)NULL, \
			   PYTHON_API_VERSION)

// ==============================================================================
// Redefine functions

#define DECL_PYTHON_OBJ_NAME(type, name, search_name)			\
     static type* name = internal_::PH::get().add_ptr(search_name, name)

#define DECL_PYTHON_OBJ(type, name)		\
     DECL_PYTHON_OBJ_NAME(type, name, #name)

#define DECL_PYTHON_FUNC_NAME(name, search_name, ret_type, ...)		\
     typedef ret_type (*name ## _t)(__VA_ARGS__);			\
     static name ## _t name = internal_::PH::get().add_ptr(search_name, name)

#define DECL_PYTHON_FUNC(name, ret_type, ...)			\
     DECL_PYTHON_FUNC_NAME(name, #name, ret_type, __VA_ARGS__)

// -----------------------------------------------------------------------------

namespace MyPy {
     DECL_PYTHON_OBJ(PyTypeObject, PyBool_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyDict_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyFloat_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyList_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyLong_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyModule_Type);
     DECL_PYTHON_OBJ(PyObject,     _Py_NoneStruct);
#if PY_MAJOR_VERSION == 2
     DECL_PYTHON_OBJ(PyTypeObject, PyInt_Type);
     DECL_PYTHON_OBJ(PyTypeObject, PyString_Type);
     DECL_PYTHON_OBJ(PyIntObject,  _Py_ZeroStruct);
     DECL_PYTHON_OBJ(PyIntObject,  _Py_TrueStruct);
#elif PY_MAJOR_VERSION == 3
     DECL_PYTHON_OBJ(_longobject,  _Py_FalseStruct);
     DECL_PYTHON_OBJ(_longobject,  _Py_TrueStruct);
#endif /* PY_MAJOR_VERSION */

     // --------------------------------

#ifdef COUNT_ALLOCS
     DECL_PYTHON_FUNC(inc_count,                     void,       PyTypeObject*);
     DECL_PYTHON_FUNC(dec_count,                     void,       PyTypeObject*);
#endif /* COUNT_ALLOCS */
     
#ifdef Py_TRACE_REFS
     DECL_PYTHON_FUNC(_Py_NewReference,              void,       PyObject*);
     DECL_PYTHON_FUNC(_Py_ForgetReference,           void,       PyObject*);
     DECL_PYTHON_FUNC(_Py_Dealloc,                   void,       PyObject*);
     DECL_PYTHON_FUNC(_Py_PrintReferences,           void,       FILE*);
     DECL_PYTHON_FUNC(_Py_PrintReferenceAddresses,   void,       FILE*);
     DECL_PYTHON_FUNC(_Py_AddToAllObjects,           void,       PyObject*, int);
#endif /* Py_TRACE_REFS */

     DECL_PYTHON_FUNC(Py_BuildValue,                 PyObject*,  const char*, PyObject*);
     DECL_PYTHON_FUNC(Py_Initialize,	             void,	 void);
     DECL_PYTHON_FUNC(Py_Finalize,	             void,	 void);
#if PY_MAJOR_VERSION == 2
#  if SIZEOF_SIZE_T == SIZEOF_INT
     DECL_PYTHON_FUNC(Py_InitModule4,                PyObject*,  const char*, PyMethodDef*, const char*, PyObject*, int);
#  else
     DECL_PYTHON_FUNC(Py_InitModule4_64,             PyObject*,  const char*, PyMethodDef*, const char*, PyObject*, int);
#  endif /* SIZEOF_SIZE_T == SIZEOF_INT */
#endif /* PY_MAJOR_VERSION */
     DECL_PYTHON_FUNC(Py_IsInitialized,	             int,	 void);
     DECL_PYTHON_FUNC(Py_SetProgramName,             void,	 char*);
     DECL_PYTHON_FUNC(Py_SetPythonHome,	             void,	 char*);

     DECL_PYTHON_FUNC(PyArg_ParseTuple,	             int,	 PyObject*, const char*);
     DECL_PYTHON_FUNC(PyBool_FromLong,	             PyObject*,  long);
     DECL_PYTHON_FUNC(PyCapsule_Import,	             void*,	 const char*, int);
     DECL_PYTHON_FUNC(PyCapsule_New,	             PyObject*,  void*, const char*, PyCapsule_Destructor);
     DECL_PYTHON_FUNC(PyDict_New,	             PyObject*,  void);
     DECL_PYTHON_FUNC(PyDict_Contains,	             int,        PyObject*,  PyObject*);
     DECL_PYTHON_FUNC(PyDict_SetItemString,          int,	 PyObject*, const char*, PyObject*);
     DECL_PYTHON_FUNC(PyErr_Clear,	             void,	 void);
     DECL_PYTHON_FUNC(PyErr_Fetch,	             void,	 PyObject**, PyObject**, PyObject**);
     DECL_PYTHON_FUNC(PyErr_Occurred,	             PyObject*,  void);
     DECL_PYTHON_FUNC(PyEval_CallObjectWithKeywords, PyObject*,  PyObject*, PyObject*, PyObject*);
#if PY_MAJOR_VERSION == 2
     DECL_PYTHON_FUNC(PyFile_AsFile,	             FILE*,	 PyObject*);
     DECL_PYTHON_FUNC(PyFile_FromString,             PyObject*,  char*, char*);
#endif /* PY_MAJOR_VERSION */
     DECL_PYTHON_FUNC(PyFloat_AsDouble,	             double,	 PyObject*);
     DECL_PYTHON_FUNC(PyFloat_FromDouble,            PyObject*,  double);
     DECL_PYTHON_FUNC(PyImport_AddModule,            PyObject*,  const char*);
     DECL_PYTHON_FUNC(PyImport_ImportModule,         PyObject*,  const char*);
     DECL_PYTHON_FUNC(PyImport_AppendInittab,        int,        const char*, void (*initfunc)(void));
#if PY_MAJOR_VERSION == 2
     DECL_PYTHON_FUNC(PyInt_AsLong,	             long,	 PyObject*);
     DECL_PYTHON_FUNC(PyInt_FromLong,	             PyObject*,  long);
#elif PY_MAJOR_VERSION == 2
     DECL_PYTHON_FUNC(PyLong_AsLong,	             long,	 PyObject*);
     DECL_PYTHON_FUNC(PyLong_FromLong,	             PyObject*,  long);
#endif /* PY_MAJOR_VERSION */
     DECL_PYTHON_FUNC(PyList_New,	             PyObject*,  Py_ssize_t);
     DECL_PYTHON_FUNC(PyList_SetItem,	             int,	 PyObject*, Py_ssize_t, PyObject*);
     DECL_PYTHON_FUNC(PyLong_FromLong,	             PyObject*,  long);
     DECL_PYTHON_FUNC(PyModule_AddObject,            int,	 PyObject*, const char*, PyObject*);
     DECL_PYTHON_FUNC(PyModule_GetDict,              PyObject*,  PyObject*);
     DECL_PYTHON_FUNC(PyNumber_Check,                int,        PyObject*);
     DECL_PYTHON_FUNC(PyNumber_Float,	             PyObject*,  PyObject*);
     DECL_PYTHON_FUNC(PyObject_CallObject,           PyObject*,  PyObject*, PyObject *);
     DECL_PYTHON_FUNC(PyObject_GetAttrString,        PyObject*,  PyObject*, const char*);
     DECL_PYTHON_FUNC(PyObject_Repr,	             PyObject*,  PyObject*);
     DECL_PYTHON_FUNC(PyObject_Str,	             PyObject*,  PyObject*);
     DECL_PYTHON_FUNC(PyRun_FileExFlags,             PyObject*,  FILE*, const char*, int, PyObject*, PyObject*, int, PyCompilerFlags*);
     DECL_PYTHON_FUNC(PyRun_SimpleFileEx,            int,	 FILE*, const char*, int);
     DECL_PYTHON_FUNC(PyRun_SimpleFileExFlags,       int,        FILE*, const char*, int, PyCompilerFlags*);
     DECL_PYTHON_FUNC(PyRun_SimpleString,            int,	 const char*);
     DECL_PYTHON_FUNC(PyRun_SimpleStringFlags,       int,        const char*, PyCompilerFlags*);
     DECL_PYTHON_FUNC(PySequence_GetItem,            PyObject*,  PyObject*, Py_ssize_t);
     DECL_PYTHON_FUNC(PySequence_Check,              int,        PyObject*);
#if PY_MAJOR_VERSION == 2
     DECL_PYTHON_FUNC(PyString_AsString,             char*,      PyObject*);
     DECL_PYTHON_FUNC(PyString_FromString,           PyObject*,  const char*);
     DECL_PYTHON_FUNC(PyString_Size,                 Py_ssize_t, PyObject*);
#endif /* PY_MAJOR_VERSION == 2 */
     DECL_PYTHON_FUNC(PyTuple_New,                   PyObject*,  Py_ssize_t);
     DECL_PYTHON_FUNC(PyTuple_GetItem,               PyObject*,  PyObject*, Py_ssize_t);
     DECL_PYTHON_FUNC(PyTuple_SetItem,               int,        PyObject*, Py_ssize_t, PyObject*);
     DECL_PYTHON_FUNC(PyTuple_Size,                  Py_ssize_t, PyObject*);
     DECL_PYTHON_FUNC(PyType_IsSubtype,	             int,        PyTypeObject*, PyTypeObject*);
} // namespace MyPy

// -----------------------------------------------------------------------------

#endif //PYTHONPATCHDECLS_H_INCLUDED
