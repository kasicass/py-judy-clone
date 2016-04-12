
static PyObject *
as_unsigned(Word_t value)
{
  /* XXX is this logic correct? */
  if (value < LONG_MAX) {
    return PyInt_FromLong(value);
  } else {
    return PyLong_FromUnsignedLong(value);
  }
}

/* Used for JudySL */

#define MAX_STRING_LEN 100000
uint8_t index_buffer[MAX_STRING_LEN+1];

/* Helper function to get the string information */
/* Modified from PyString_AsStringAndSize */
static int
check_string(PyObject *obj, char **s)
{
  long len;
  if (!PyString_Check(obj)) {
    PyErr_Format(PyExc_TypeError,
		 "expected string object, "
		 "%.200s found", obj->ob_type->tp_name);
    return -1;
  }
  *s = PyString_AS_STRING(obj);
  len = PyString_GET_SIZE(obj);
  if ((int)strlen(*s) != len) {
    PyErr_SetString(PyExc_TypeError,
		    "expected string without null bytes");
    return -1;
  }
  if (len > MAX_STRING_LEN) {
   PyErr_Format(PyExc_TypeError,
		"string longer than compiled limit of "
		"%d characters", MAX_STRING_LEN);
    return -1;
  } 
  return 0;
}

