#include <Python.h>

#include <cstring>

#include "flo2istream.h"

Flo2istream::Flo2istream(PyObject* obj) {
    if ((this->read_meth = PyObject_GetAttrString(obj, "read")) == NULL) {
        throw PyError(PyExc_AttributeError, "Can't find \"read\" attribute, not a file like object");
    }

    this->read_args = Py_BuildValue("(i)", CHUNK_SIZE);


}

istream& Flo2istream::read (char* s, streamsize n) {
    PyObject* data;
    PyObject* enc_data;
    char* buf;
    Py_ssize_t len;
    PyObject* read_args = Py_BuildValue("(i)", n)

    /* Call read() */
    if ((data = PyObject_Call(this->read_meth, read_args, NULL)) == NULL) {
        Py_DECREF(read_args);
        return;
    }
    Py_DECREF(read_args);

    /* Check for EOF */
    if (PySequence_Length(data) == 0) {
        Py_DECREF(data);
        break;
    }

    /* Encode Unicode as Bytes for C */
    if ((enc_data = PyUnicode_AsEncodedString(data,
        "utf-8", "strict")) == NULL) {
        Py_DECREF(data);
        return;
    }

    /* Extract underlying buffer data */
    PyBytes_AsStringAndSize(enc_data, &buf, &len);

    /* Write to stdout (replace with something more useful) */
    std::memcpy(s, buf, len);

    if (len < n) {
        this->setstate(ios_base::failbit);
        this->setstate(ios_base::badbit);
    }

    /* Cleanup */
    Py_DECREF(read_args);
    Py_DECREF(enc_data);
    Py_DECREF(data);

    return *this;
}
