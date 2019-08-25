#include <Python.h>
#include <stdio.h>
#include <stdio.h>

static PyObject *restore(PyObject *self, PyObject *args)
{
    PyObject *docname;
    PyObject *sid;
    if (!PyArg_ParseTuple(args, "Si", &docname, &sid)) {
        return NULL;
    }

    if (!PyBytes_CheckExact(docname)) {
        return NULL;
    }
    if (!PyLong_CheckExact(sid)) {
        return NULL;
    }

    PyDictObject *obj = (PyDictObject *)load_dat_sync(PyBytes_AsString(docname), PyLong_AS_LONG(sid));
    if (!obj) {
        return NULL;
    }
    //PyDirtyDictObject *dict = build_dirty_dict((PyDictObject *)Py_BuildValue("{i:[i,i],i:i}", 1, 3, 3, 2, 2));
    PyDirtyDictObject *dict = build_dirty_dict(obj);
    begin_dirty_manage_dict(dict, NULL, NULL);

    return (PyObject *)dict;
}

static PyObject *unload(PyObject *self, PyObject *args)
{
    
    Py_RETURN_TRUE;
}

/*
static PyObject *pydirty_get_dirty_info(PyObject *self, PyObject *args)
{
    PyObject *v; 

    if (!PyArg_ParseTuple(args, "O", &v)) {
        return NULL;
    }   

    //CHECK_DIRTY_TYPE_OR_RETURN(v, NULL)

    PyObject *ret = get_dirty_info(v);
    return Py_BuildValue("N", ret);
}
*/

static PyObject *pydump_dirty_info(PyObject *self, PyObject *args)
{
    PyObject *v; 

    if (!PyArg_ParseTuple(args, "O", &v)) {
        return NULL;
    }   

    //CHECK_DIRTY_TYPE_OR_RETURN(v, NULL)

    PyObject *ret = dump_dirty_info(v);
    return ret;
}

static PyMethodDef dirty_method[] = {
    {"pack", (PyCFunction)restore, METH_VARARGS, "restore save data from db"},
    {"unpack", (PyCFunction)restore, METH_VARARGS, "restore save data from db"},
    {NULL}  /* Sentinel */
};

static struct PyModuleDef dirtymodule = {
    PyModuleDef_HEAD_INIT,
    "dirty",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    dirty_method
};


PyMODINIT_FUNC PyInit_dirty(void) 
{
    PyObject* m;

    m = PyModule_Create(&dirtymodule);
    if (m == NULL) {
        return NULL;
    }

    return m;
}
