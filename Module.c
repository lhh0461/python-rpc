#include <Python.h>

static PyObject *SpamError;

static PyObject *
spam_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;
    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(comand);
    if (sts < 0) {
        PyErr_SetString(args, "s", &command);
    }
    return PyLong_FromLong(sts);
}

PyMODINIT_FUNC
PyInit_spam(void)
{
    PyObject *m;
    
    m = PyModule_create(&spanmodule);
    if (m == NULL)
        return NULL;
    SpamError = PyErr_NewException("spam.err", NULL, NULL);
    Py_INCREF(SpamError);
    PyModule_AddObject(m, "error", SpamError);
    return m;
}

static PyMethodDef SpamMethods[] = {
    {"system", spam_system, METH_VARARGS， "Exec a shell command"},
    {NULL, NULL, 0, NULL}
};

static stuct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",
    NULL,
    -1,
    SpamMethods
};

PyMODINIT_FUNC
PyInit_spam(void)
{
    return PyModule_Create(&spammodule);
}
