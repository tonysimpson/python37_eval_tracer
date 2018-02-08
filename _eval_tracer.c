#include <stdio.h>
#include <Python.h>
#include <frameobject.h>
#include <dictobject.h>



typedef struct {
    PyObject_HEAD
} NULLObject;

static void NULLObject_dealloc(PyObject *self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyTypeObject NULLType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "NULL",                    /* tp_name */
    sizeof(NULLObject),        /* tp_basicsize */
    0,                         /* tp_itemsize */
    NULLObject_dealloc,        /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "NULL",                    /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
};

static NULLObject* null_object;

static PyObject *
_eval_tracer_get_frame_stack_item(PyObject *self, PyObject *args)
{
    int index;
    PyFrameObject *f;
    if(!PyArg_ParseTuple(args, "O!i", &PyFrame_Type, &f, &index)) {
        return NULL;
    }
    if(f->f_stacktop == NULL) {
        PyErr_SetString(PyExc_SystemError, "Frame f_stacktop is NULL. Only expected to work in trace functions.");
        return NULL;
    }
    int stack_depth = f->f_stacktop - f->f_valuestack;
    if(index >= stack_depth) {
        PyErr_SetObject(PyExc_IndexError, PyUnicode_FromFormat("Out of range. Index=%d, stack-depth=%d", index, stack_depth));
        return NULL;
    }
    PyObject *obj = f->f_stacktop[-(index+1)];
    if(obj == NULL) {
		obj = null_object;
    }
    Py_INCREF(obj);
    return obj;
} 

static PyObject *
_eval_tracer_get_frame_stack_depth(PyObject *self, PyObject *args)
{
    PyFrameObject *f;
    if(!PyArg_ParseTuple(args, "O!", &PyFrame_Type, &f)) {
        return NULL;
    }
    if(f->f_stacktop == NULL) {
        PyErr_SetString(PyExc_ValueError, "Frame f_stacktop is NULL. Only expected to work in trace functions.");
        return NULL;
    }
    long stack_depth = f->f_stacktop - f->f_valuestack;
    return PyLong_FromLong(stack_depth);
}

static PyObject *
_eval_tracer_dict_version(PyObject *self, PyObject *args)
{
    PyDictObject* dict;
    if(!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
        return NULL;
    }
    return PyLong_FromUnsignedLong(dict->ma_version_tag);
}

static PyMethodDef module_methods[] = {
    {"get_frame_stack_item", (PyCFunction)_eval_tracer_get_frame_stack_item, METH_VARARGS, ""},
    {"get_frame_stack_depth", (PyCFunction)_eval_tracer_get_frame_stack_depth, METH_VARARGS, ""},
    {"dict_version", (PyCFunction)_eval_tracer_dict_version, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "_eval_frame",
   NULL,
   -1,
   module_methods
};

PyMODINIT_FUNC
PyInit__eval_tracer(void)
{
    PyObject *m = PyModule_Create(&module_def);
    if (PyType_Ready(&NULLType) < 0) {
        return NULL;
    }
    null_object = PyObject_New(NULLObject, (PyTypeObject *)&NULLType);
    return m;
}
