# python-dlopen

This project was born from the need to be able to call `Py_Initialize`/`Py_Finalize` multiple times when embedding Python in C++ code

# Compatibility

It should work for both Python 2.7 and Python 3.6, though I have not tested the latter as thoroughly as the former...

# Disclaimer

This is intended as a proof-of-concept implementation of the solution I have outlined on StackExchange [https://stackoverflow.com/questions/14843408/python-c-embedded-segmentation-fault/43166617#43166617]

It is most likely not the optimal solution but should work for most use cases. Note that the `PythonPatchDecls.h` file should probably be modified to suit your needs.


