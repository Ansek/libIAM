#include <Python.h>
#include <iam/init.h>
#include <iam/info.h>
#include <iam/algorithm.h>
#include <iam/setting.h>
#include <stdio.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

void set_mode(const char *alg_name, const char* mode, uint8_t det_id) {
    iam_id_t id;
    iam_setting_t *s;
    iam_module_rewind();
    while(id = iam_module_read())
        if (strcmp(alg_name, id->info->name) == 0) {
            if (strcmp(alg_name, "NSA_RV") == 0) {
                iam_setting_rewind(id);
                while (s = iam_setting_read(id)) {
                    if (strcmp(s->info->name, "isVdetectors") == 0) {
                        iam_setting_set_bool(s, 
                            strcmp(mode, "Vdetectors") == 0);
                    }
                    if (strcmp(s->info->name, "det_id") == 0) {
                        iam_setting_set_uint8(s, det_id);
                    }
                }
            }   
            break;
        }
}

static PyObject *fit(PyObject* self, PyObject* args) {
    PyObject *argX, *argY;
    PyArrayObject *arrX, *arrY;
    npy_intp row_n, col_n;
    double *inX;
    uint8_t *inY, det_id;
    const char *alg_name, *mode;

    if (!PyArg_ParseTuple(args, "sOO!sB", &alg_name, &argX, &PyArray_Type,
            &argY, &mode, &det_id))
        return NULL;
    if ((arrX = (PyArrayObject *)PyArray_FROM_OTF(argX,
         NPY_DOUBLE, NPY_ARRAY_IN_ARRAY)) == NULL)
        return NULL;
    if ((arrY = (PyArrayObject *)PyArray_FROM_OTF(argY,
         NPY_UBYTE, NPY_ARRAY_IN_ARRAY)) == NULL) {
        Py_XDECREF(arrX);
        return NULL;
    }
    if (PyArray_NDIM(arrX) != 2) {
        Py_DECREF(arrX);
        Py_DECREF(arrY);
        PyErr_SetString(PyExc_ValueError, "X must be 2-D");
        return NULL;
    }

    set_mode(alg_name, mode, det_id);
    row_n = PyArray_DIM(arrX, 0);
    col_n = PyArray_DIM(arrX, 1);
    inX = (double *)PyArray_DATA(arrX);
    inY = (unsigned char *)PyArray_DATA(arrY);
    iam_real_alg_fit(alg_name, inX, inY, row_n, col_n); 
        
    Py_DECREF(arrX);
    Py_DECREF(arrY);
    Py_RETURN_NONE;
}

static PyObject *predict(PyObject* self, PyObject* args) {
    PyObject *argX;
    PyArrayObject *arrX, *arrY;
    npy_intp row_n, col_n;
    double *inX;
    uint8_t *outY, det_id;
    const char *alg_name, *mode;
   
    if (!PyArg_ParseTuple(args, "sO!sB", &alg_name, &PyArray_Type, &argX,
            &mode, &det_id))
        return NULL;
    if ((arrX = (PyArrayObject *)PyArray_FROM_OTF(argX,
         NPY_DOUBLE, NPY_ARRAY_IN_ARRAY)) == NULL) return NULL;
    set_mode(alg_name, mode, det_id);
    row_n = PyArray_DIM(arrX, 0);
    col_n = PyArray_DIM(arrX, 1);
    arrY = (PyArrayObject *)PyArray_SimpleNew(1, &row_n, NPY_UBYTE);
    inX = (double *)PyArray_DATA(arrX);
    outY = (unsigned char *)PyArray_DATA(arrY);
    iam_real_alg_predict(alg_name, inX, outY, row_n, col_n); 
 
    Py_DECREF(arrX);
    Py_INCREF(arrY);
    return (PyObject *)arrY;
}

static PyObject *init_lib(PyObject* self, PyObject* args) {
    iam_init();
    Py_RETURN_NONE;
}

static PyObject *exit_lib(PyObject* self, PyObject* args) {  
    iam_exit();
    Py_RETURN_NONE;
}

static PyMethodDef methods[] = {
     {"fit", fit, METH_VARARGS, "fit"},
     {"predict", predict, METH_VARARGS, "predict"},
     {"init_lib", init_lib, METH_VARARGS, "init_lib"},
     {"exit_lib", exit_lib, METH_VARARGS, "exit_lib"},
     {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
/* Python version 3*/
static struct PyModuleDef cModPyDem = { PyModuleDef_HEAD_INIT,
    "iam", "Immune Algorithm Manager", -1, methods
};
PyMODINIT_FUNC PyInit_iam(void) {
    PyObject *module;
    module = PyModule_Create(&cModPyDem);
    if(module==NULL) return NULL;
    import_array();
    if (PyErr_Occurred()) return NULL;
    return module;
}
#else
/* Python version 2 */
PyMODINIT_FUNC initiam(void) {
    PyObject *module;
    module = Py_InitModule("iam", methods);
    if(module==NULL) return;
    import_array();
    return;
}
#endif