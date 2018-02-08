#include <stdio.h>
#include <Python.h>
#include <frameobject.h>

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
        PyErr_SetString(PyExc_ValueError, "Stack item is C NULL (0x0).");
        return NULL;
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

static PyMethodDef module_methods[] = {
    {"get_frame_stack_item", (PyCFunction)_eval_tracer_get_frame_stack_item, METH_VARARGS, ""},
    {"get_frame_stack_depth", (PyCFunction)_eval_tracer_get_frame_stack_depth, METH_VARARGS, ""},
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
    return PyModule_Create(&module_def);
}
