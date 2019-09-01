#include <Python.h>
#include <stdio.h>

//直接传入元组参数
int CallScriptFunction(const char *module, const char *function, PyObject *args)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pResult;

    pName = PyUnicode_FromString(module);
    if (pName == NULL) {
        PyErr_Print();
        return -1;
    }

    pModule = PyImport_Import(pName);
    if (pModule == NULL) {
        fprintf(stderr, "Failed to load \"%s\"\n", module);
        PyErr_Print();
        Py_DECREF(pName);
        return -1;
    }

    pFunc = PyObject_GetAttrString(pModule, function);
    if (pFunc == NULL) {
        fprintf(stderr, "Cannot find function \"%s\"\n", function);
        PyErr_Print();
        Py_DECREF(pName);
        Py_DECREF(pModule);
        return -1;
    }

    if (!PyCallable_Check(pFunc)) {
        fprintf(stderr, "Function must be callable\"%s\"\n", function);
        Py_DECREF(pName);
        Py_DECREF(pModule);
        Py_DECREF(pFunc);
        return -1;
    }

    if (args != NULL) {
        if (!PyTuple_CheckExact(args)) {
            fprintf(stderr, "Args must be tuple\n");
            PyObject_Print(args, stdout, 0);
            Py_DECREF(pName);
            Py_DECREF(pModule);
            Py_DECREF(pFunc);
            return -1;
        }
    }

    pResult = PyObject_CallObject(pFunc, args);
    if (pResult == NULL) {
        fprintf(stderr,"Call failed\n");
        PyErr_Print();
        Py_DECREF(pName);
        Py_DECREF(pModule);
        Py_DECREF(pFunc);
        return -1;
    }

    Py_DECREF(pName);
    Py_DECREF(pResult);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    return 0;
}
