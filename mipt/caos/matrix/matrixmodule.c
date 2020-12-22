#define PY_SSIZE_T_CLEAN
#include <Python.h>

size_t min2(size_t a, size_t b) {
  return a < b ? a : b;
}

size_t min3(size_t a, size_t b, size_t c) {
  return min2(a, min2(b, c));
}

static PyObject* matrix_dot(PyObject* self, PyObject* args) {
  size_t n;
  PyObject* A;
  PyObject* B;
  if (!PyArg_ParseTuple(args, "kOO", &n, &A, &B)) {
    return NULL;
  }

  // c_len = column_len
  // l_len = line_len
  size_t A_c_len = PyList_Size(A);
  size_t A_l_len = 0;
  if (A_c_len > 0) {
    PyObject* tmp = PyList_GetItem(A, 0);
    A_l_len = PyList_Size(tmp);
  }

  size_t B_c_len = PyList_Size(B);
  size_t B_l_len = 0;
  if (B_c_len > 0) {
    PyObject* tmp = PyList_GetItem(B, 0);
    B_l_len = PyList_Size(tmp);
  }
  
  PyObject* result = PyList_New(n);
  for (size_t i = 0; i < n; ++i) {
    PyObject* pyline = PyList_New(n);

    if (i < A_c_len) {
      PyObject* line = PyList_GetItem(A, i);

      for (size_t j = 0; j < n; ++j) {
        double elem = 0;
        if (j < B_l_len) {
          for (size_t k = 0; k < min3(n, A_l_len, B_c_len); ++k) {
            PyObject* column = PyList_GetItem(B, k);
            elem += PyFloat_AsDouble(PyList_GetItem(line, k)) *
                    PyFloat_AsDouble(PyList_GetItem(column, j));
          }
        }
        PyList_SetItem(pyline, j, Py_BuildValue("d", elem));
      }
    } else {
      for (size_t j = 0; j < n; ++j) {
        PyList_SetItem(pyline, j, Py_BuildValue("d", (double)0));
      }
    }
    PyList_SetItem(result, i, pyline);
  }

  return result;
}

static PyMethodDef matrix_methods[] = {
  {"dot", matrix_dot, METH_VARARGS, "Dot the matrices"},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef matrix = {
  PyModuleDef_HEAD_INIT,
  "matrix",
  NULL,  // doc
  -1,
  matrix_methods
};

PyMODINIT_FUNC PyInit_matrix(void) {
  return PyModule_Create(&matrix);
}
