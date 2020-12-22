#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/evp.h>

static PyObject* aes256_decode(PyObject* self, PyObject* args) {
  PyObject* encoded_raw;
  char* password = NULL;
  if (!PyArg_ParseTuple(args, "S|s", &encoded_raw, &password)) {
    return NULL;
  }
  if (password == NULL) {
    PyObject* bytes = PyDict_GetItemString(PyModule_GetDict(self), "default_password");
    if (bytes != NULL) {
      password = PyBytes_AsString(bytes);
    } else {
      PyErr_SetString(PyExc_RuntimeError, "no password set");
      return NULL;
    }
  }

  unsigned char* encoded = (unsigned char*)PyBytes_AsString(encoded_raw);
  unsigned char salt[8];
  int salted = 0;
  if (encoded[0] == 'S' && encoded[1] == 'a' &&
      encoded[2] == 'l' && encoded[3] == 't' &&
      encoded[4] == 'e' && encoded[5] == 'd') {
    salted = 1;
  }
  if (salted) {
    memcpy(salt, encoded + 8, 8);
    encoded += 16;
  }
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  
  unsigned char key[EVP_MAX_KEY_LENGTH];
  unsigned char iv[EVP_MAX_IV_LENGTH];
  if (!EVP_BytesToKey(
    EVP_aes_256_cbc(),    // algorithm
    EVP_sha256(),         // algorithm for password hash
    salted ? salt : NULL, // salt
    (unsigned char*)password, strlen(password), // password
    1,                    // number of iterations
    key,                  // result: key of needed size
    iv                    // result: initial vector
  )) {
    PyErr_SetString(PyExc_RuntimeError, "couldn't get key");
    return NULL;
  }
  
  if (!EVP_DecryptInit(
    ctx,                  // context to store state
    EVP_aes_256_cbc(),    // algorithm
    key,                  // key of needed size
    iv                    // initial vector
  )) {
    PyErr_SetString(PyExc_RuntimeError, "EVP couldn't init");
    return NULL;
  }
  if (!salted) {
    EVP_CIPHER_CTX_set_padding(ctx, 0);
  }

  size_t len = PyBytes_Size(encoded_raw) - 16;
  unsigned char dec[len + 256];
  size_t sz = 0;
  if (!EVP_DecryptUpdate(ctx, dec, (int*)&sz, encoded, len)) {
    PyErr_SetString(PyExc_RuntimeError, "couldn't update");
    return NULL;
  }
  size_t sz_tmp = 0;
  if (!EVP_DecryptFinal(ctx, dec + sz, (int*)&sz_tmp)) {
    PyErr_SetString(PyExc_RuntimeError, "couldn't get final bytes");
    return NULL;
  }
  sz += sz_tmp;

  return PyBytes_FromStringAndSize((char*)dec, sz);
}

static PyObject* aes256_set_default_password(PyObject* self, PyObject* args) {
  char* default_password;
  if (!PyArg_ParseTuple(args, "s", &default_password)) {
    return NULL;
  }
  PyDict_SetItemString(PyModule_GetDict(self), "default_password", Py_BuildValue("y", default_password));
  Py_RETURN_NONE;
}

static PyMethodDef aes256methods[] = {
  {"decode", aes256_decode, METH_VARARGS, "decode with aes256"},
  {"set_default_password", aes256_set_default_password, METH_VARARGS, "set password for decode aes256"},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef aes256 = {
  PyModuleDef_HEAD_INIT,
  "aes256",
  NULL,  // doc
  -1,
  aes256methods
};

PyMODINIT_FUNC PyInit_aes256(void) {
  return PyModule_Create(&aes256);
}

