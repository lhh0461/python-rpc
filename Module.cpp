#include <Python.h>
#include <msgpack.hpp>
#include "Rpc.h"

using namespace XEngine;

static PyObject *RpcError;

static PyObject *pack(PyObject *self, PyObject *args)
{
    const char *func;
    PyObject *obj;

    if (!PyArg_ParseTuple(args, "sO", &func, &obj))
        return NULL;
    
    if (!PyTuple_CheckExact(obj)) {
        PyErr_Format(PyExc_TypeError, "pack obj type error type=%s", Py_TYPE(obj)->tp_name);
        return NULL;
    }

    msgpack::sbuffer sbuf;
    g_Rpc->Pack(g_Rpc->GetPidByName(func), obj, sbuf);

    return PyBytes_FromStringAndSize(sbuf.data(), sbuf.size());
}

static PyObject *unpack(PyObject *self, PyObject *args)
{
    PyObject *obj;

    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    
    if (!PyBytes_CheckExact(obj)) {
        PyErr_Format(PyExc_TypeError, "pack obj type error type=%s", Py_TYPE(obj)->tp_name);
        return NULL;
    }

    const char *str = PyBytes_AsString(obj);
    Py_ssize_t size = PyBytes_Size(obj);

    PyObject *unpackObj = g_Rpc->UnPack(str, size);
    if (unpackObj == NULL) {
        PyErr_SetString(RpcError, "Rpc unpack fail");
        return NULL;
    }
    return unpackObj;
}

static PyObject *rpc_call(PyObject *self, PyObject *args)
{
    PyObject *obj;

    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    
    if (!PyBytes_CheckExact(obj)) {
        PyErr_Format(PyExc_TypeError, "pack obj type error type=%s", Py_TYPE(obj)->tp_name);
        return NULL;
    }

    const char *str = PyBytes_AsString(obj);
    Py_ssize_t size = PyBytes_Size(obj);

    PyObject *unpackObj = g_Rpc->UnPack(str, size);
    if (unpackObj == NULL) {
        PyErr_SetString(RpcError, "Rpc unpack fail");
        return NULL;
    }
    return unpackObj;
}

static PyMethodDef RpcMethods[] = {
    {"pack", pack, METH_VARARGS, "rpc pack to buf"},
    {"unpack", unpack, METH_VARARGS, "rpc unpack from buf"},
    {"rpc_call", unpack, METH_VARARGS, "rpc unpack from buf"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef rpcmodule = {
    PyModuleDef_HEAD_INIT,
    "rpc",
    NULL,
    -1,
    RpcMethods
};

PyMODINIT_FUNC
PyInit_rpc(void)
{
    PyObject *m;
    
    m = PyModule_Create(&rpcmodule);
    if (m == NULL)
        return NULL;
    RpcError = PyErr_NewException("rpc.err", NULL, NULL);
    Py_INCREF(RpcError);
    PyModule_AddObject(m, "error", RpcError);

    g_Rpc = new CRpc();
    g_Rpc->Init("./rpc/");

    return m;
}
