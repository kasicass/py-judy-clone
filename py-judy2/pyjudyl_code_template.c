/** Helper function to parse ranges  **/

static PyObject *
<P>_parse_range(PyObject *args, char *name,
		Word_t *start_index, Word_t *end_index)
{
  PyObject *start_key, *end_key, *key;
  Word_t index;
  if (!PyArg_UnpackTuple(args, name, 2, 2, &start_key, &end_key))
    return NULL;
  
  key = start_key;
  PY_TO_INDEX;
  *start_index = index;

  key = end_key;
  PY_TO_INDEX;
  *end_index = index;
  
  /* I need to return something which isn't NULL */
  return args;
}

/* Helper function to make a 2-element tuple from key/value */

static PyObject *
<P>_make_2ple(Word_t index, PPvoid_t p)
{
  PyObject *tuple;
  PyObject *key;
  PyObject *value;

  tuple = PyTuple_New(2);
  if (tuple == NULL)
    return NULL;

  key = PY_FROM_INDEX;
  if (key == NULL) {
    Py_DECREF(tuple);
    return NULL;
  }

  value = PY_FROM_PWORD;
  if (value == NULL) {
    Py_DECREF(key);
    Py_DECREF(tuple);
    return NULL;
  }
  PyTuple_SET_ITEM(tuple, 0, key);
  PyTuple_SET_ITEM(tuple, 1, value);
  return tuple;
}

static void
<P>_dealloc(<CT>* self)
{
  FREE_ELEMENTS
  JudyLFreeArray( &(self->array), PJE0);
  PyObject_Del((PyObject *)self);
}

static PyObject *
<P>_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
  <CT> *self;
  /* What is the right way to check for no arguments? */
  if (!PyArg_UnpackTuple(args, "new", 0, 0))
    return NULL;

  self = (<CT> *)PyObject_New(<CT>, type);
  if (self == NULL)
    return NULL;

  self->array = NULL;
  return (PyObject *)self;
}

static PyObject *
<P>_MemUsed(<CT> *self)
{
  Word_t size;
  size = JudyLMemUsed(self->array);
  return as_unsigned(size);
}

/* Forward declaration */
static int
<P>_ass_sub(<CT> *self, PyObject *key, PyObject *value);

static PyObject *
<P>_Ins(<CT> *self, PyObject *args)
{
  PyObject *key, *value;
  int status;
  if (!PyArg_UnpackTuple(args, "Ins", 2, 2, &key, &value))
    return NULL;

  status = <P>_ass_sub(self, key, value);
  if (status == 0) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return NULL;
}


static int
<P>_delitem(<CT> *self, PyObject *key)
{
  DELITEM
}

static PyObject *
<P>_Del(<CT> *self, PyObject *key)
{
  int success;
  success = <P>_delitem(self, key);
  if (success == 0) {
    /* failed because the key does not exist */
    Py_INCREF(Py_False);
    return Py_False;
  } else if (success == 1) {
    /* successful deletion */
    Py_INCREF(Py_True);
    return Py_True;
  }
  /* Some other sort of error; exception already set */
  return NULL;
}

static PyObject *
<P>_Get(<CT> *self, PyObject *key)
{
  Word_t index;
  PPvoid_t p;

  PY_TO_INDEX;
  p = JudyLGet(self->array, index, PJE0);
  if (p == NULL) {
    /* Not present; map that to an IndexError */
    PyErr_SetObject(PyExc_IndexError, key);
    return NULL;
  }
  return PY_FROM_PWORD;
}

static PyObject *
<P>_get(<CT> *self, PyObject *args)
{
  Word_t index;
  PPvoid_t p;
  PyObject *key, *failobj = Py_None;

  if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj))
    return NULL;

  PY_TO_INDEX;
  p = JudyLGet(self->array, index, PJE0);
  if (p == NULL) {
    /* Not present; return the default */
    Py_INCREF(failobj);
    return failobj;
  }
  return PY_FROM_PWORD;
}

static PyObject *
<P>_Count(<CT> *self)
{
  Word_t count;
  count = JudyLCount(self->array, 0, -1, PJE0);
  return as_unsigned(count);
}

static PyObject *
<P>_Count_from(<CT> *self, PyObject *key)
{
  Word_t count, index;
  PY_TO_INDEX;
  count = JudyLCount(self->array, index, -1, PJE0);
  return as_unsigned(count);
}

static PyObject *
<P>_Count_to(<CT> *self, PyObject *key)
{
  Word_t count, index;
  PY_TO_INDEX;
  /* in the Python manner, exclude the final */
  if (index == 0) {
    count = 0;
  } else {
    count = JudyLCount(self->array, 0, index-1, PJE0);
  }
  return as_unsigned(count);
}

static PyObject *
<P>_Count_range(<CT> *self, PyObject *args)
{
  Word_t count, start, end;
  if (!<P>_parse_range(args, "Count_range", &start, &end))
    return NULL;
  if (end == 0) {
    count = 0;
  } else {
    count = JudyLCount(self->array, start, end-1, PJE0);
  }
  return as_unsigned(count);
}

static PyObject *
<P>_ByCount(<CT> *self, PyObject *key)
{
  PPvoid_t p;
  Word_t index;
  Word_t Nth;
  /* XXX Need to allow Nth >= LONG_MAX */
  Nth = PyInt_AsLong(key);
  if (Nth == -1 && PyErr_Occurred())
    return NULL;
  p = JudyLByCount(self->array, Nth, &index, PJE0);
  if (p == NULL) {
    /* Not found; raise exception */
    PyErr_SetObject(PyExc_IndexError, key);
    return NULL;
  }
  /* Return the 2-ple of (index, value) */
  return <P>_make_2ple(index, p);
}

static PyObject *
<P>_FreeArray(<CT> *self)
{
  FREE_ELEMENTS
  JudyLFreeArray( &(self->array), PJE0);
  /* not returning the size */
  Py_INCREF(Py_None);
  return Py_None;
}

/* ******** */

static PyObject *
<P>_First(<CT> *self, PyObject *args)
{
  Word_t index = 0;
  PPvoid_t p;
  PyObject *key=NULL;

  if (!PyArg_UnpackTuple(args, "First", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  p = JudyLFirst(self->array, &index, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index, p);
}

static PyObject *
<P>_Next(<CT> *self, PyObject *key)
{
  Word_t index;
  PPvoid_t p;

  PY_TO_INDEX;
  p = JudyLNext(self->array, &index, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }

  return <P>_make_2ple(index, p);
}

static PyObject *
<P>_Last(<CT> *self, PyObject *args)
{
  Word_t index = -1;
  PPvoid_t p;
  PyObject *key = NULL;

  if (!PyArg_UnpackTuple(args, "Last", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  p = JudyLLast(self->array, &index, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index, p);
}

static PyObject *
<P>_Prev(<CT> *self, PyObject *key)
{
  Word_t index;
  PPvoid_t p;

  PY_TO_INDEX;
  p = JudyLPrev(self->array, &index, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index, p);
}


/* ******** */

static PyObject *
<P>_FirstEmpty(<CT> *self, PyObject *args)
{
  Word_t index = 0;
  int rc;
  PyObject *key = NULL;

  if (!PyArg_UnpackTuple(args, "FirstEmpty", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  rc = JudyLFirstEmpty(self->array, &index, PJE0);
  if (!rc) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

static PyObject *
<P>_NextEmpty(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;
  rc = JudyLNextEmpty(self->array, &index, PJE0);
  if (!rc) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

static PyObject *
<P>_LastEmpty(<CT> *self, PyObject *args)
{
  Word_t index = -1;
  int rc;
  PyObject *key = NULL;

  if (!PyArg_UnpackTuple(args, "LastEmpty", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  rc = JudyLLastEmpty(self->array, &index, PJE0);
  if (!rc) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

static PyObject *
<P>_PrevEmpty(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;
  rc = JudyLPrevEmpty(self->array, &index, PJE0);
  if (!rc) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

/* Python object adapters */

static PyObject *
<P>_keys(<CT> *self)
{
  PyObject *v;
  Word_t n, index;
  PPvoid_t p;
  int i=0;

  n = JudyLCount(self->array, 0, -1, PJE0);
  v = PyList_New(n);
  if (v == NULL)
    return NULL;
  index = 0;
  p = JudyLFirst(self->array, &index, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudyL size changed while creating list of keys");
      Py_DECREF(v);
      return NULL;
    }
    PyList_SET_ITEM(v, i, PY_FROM_INDEX);
    i++;
    p = JudyLNext(self->array, &index, PJE0);
  }
  return v;
}

static PyObject *
<P>_values(<CT> *self)
{
  PyObject *v;
  Word_t n, index;
  PPvoid_t p;
  int i=0;

  n = JudyLCount(self->array, 0, -1, PJE0);
  v = PyList_New(n);
  index = 0;
  p = JudyLFirst(self->array, &index, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudyL size changed while creating list of values");
      Py_DECREF(v);
      return NULL;
    }
    PyList_SET_ITEM(v, i, PY_FROM_PWORD);
    i++;
    p = JudyLNext(self->array, &index, PJE0);
  }
  return v;
}

static PyObject *
<P>_items(<CT> *self)
{
  PyObject *v, *tuple;
  Word_t n, index;
  PPvoid_t p;
  int i=0;

  n = JudyLCount(self->array, 0, -1, PJE0);
  v = PyList_New(n);
  index = 0;
  p = JudyLFirst(self->array, &index, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudyL size changed while creating list of items");
      goto error;
    }
    tuple = <P>_make_2ple(index, p);
    if (tuple == NULL)
      goto error;
    PyList_SET_ITEM(v, i, tuple);
    i++;
    p = JudyLNext(self->array, &index, PJE0);
  }
  return v;

 error:
  Py_XDECREF(v);
  return NULL;
}

int
<P>_contains(<CT> *self, PyObject *key)
{
  Word_t index;
  Pvoid_t p;

  PY_TO_INDEX

  p = JudyLGet(self->array, (Word_t) index, PJE0);  
  return (p != NULL);
}

/* Meet the mapping protocol */
static int
<P>_length(<CT> *self)
{
  /* XXX what if count > 2**31? */
  return JudyLCount(self->array, 0, -1, PJE0);
}

static PyObject *
<P>_subscript(<CT> *self, PyObject *key)
{
  Word_t index;
  PPvoid_t p;

  PY_TO_INDEX
  p = JudyLGet(self->array, (Word_t) index, PJE0);
  if (!p) {
    PyErr_SetObject(PyExc_KeyError, key);
    return NULL;
  }
  return PY_FROM_PWORD;
}

static int
<P>_ass_sub(<CT> *self, PyObject *key, PyObject *value)
{
  if (value == NULL) {

    int success;
    success = <P>_delitem(self, key);
    if (success == 1) {
      return 0;
    }
    if (success == 0) {
      /* Key not found */
      PyErr_SetObject(PyExc_KeyError, key);
      return -1;
    }
    /* Some other error - exception already set */
    return -1;
      

  } else {

    SETITEM

  }
}


static PyMappingMethods <P>_as_mapping = {
  (inquiry) <P>_length,
  (binaryfunc) <P>_subscript,
  (objobjargproc)<P>_ass_sub,
};


/* Hack to implement "key in array" */
static PySequenceMethods <P>_as_sequence = {
        0,                                      /* sq_length */
        0,                                      /* sq_concat */
        0,                                      /* sq_repeat */
        0,                                      /* sq_item */
        0,                                      /* sq_slice */
        0,                                      /* sq_ass_item */
        0,                                      /* sq_ass_slice */
        (objobjproc)<P>_contains,               /* sq_contains */
        0,                                      /* sq_inplace_concat */
        0,                                      /* sq_inplace_repeat */
};


static PyMethodDef <P>_methods[] = {
  {"MemUsed", (PyCFunction) <P>_MemUsed, METH_NOARGS, 
   "number of bytes of malloced memory used by this array"},
  {"Ins", (PyCFunction) <P>_Ins, METH_VARARGS, 
   "insert a key and value into the array"},
  {"Del", (PyCFunction) <P>_Del, METH_O, 
   "delete the given key; return True if a deletion occured, else False"},
  {"Get", (PyCFunction) <P>_Get, METH_O,
   "given a key get the corresponding value; raise IndexError there is not one"},
  {"get", (PyCFunction) <P>_get, METH_VARARGS,
   "given a key get the corresponding value; else return failobj (default failobj is None)"},
  {"Count", (PyCFunction) <P>_Count, METH_NOARGS,
   "total number of elements in the array"},
  {"Count_from", (PyCFunction) <P>_Count_from, METH_O,
   "number of elements between the given key and the end; including the key"},
  {"Count_to", (PyCFunction) <P>_Count_to, METH_O,
   "number of elements from the beginning to the given key; excluding the key"},
  {"Count_range", (PyCFunction) <P>_Count_range, METH_VARARGS,
   "number of elements between the start and end keys; excluding the end key"},

  {"ByCount", (PyCFunction) <P>_ByCount, METH_O,
   "the Nth (key, value) tuple; N==1 is the first tuple"},

  {"FreeArray", (PyCFunction) <P>_FreeArray, METH_NOARGS,
   "free the entire array"},
  {"clear", (PyCFunction) <P>_FreeArray, METH_NOARGS,
   "free the entire array"},

  {"First", (PyCFunction) <P>_First, METH_VARARGS,
   "the first (key, value) tuple where the key is at or after the input key. "
   "If no input key, returns the first tuple in the array.  If there are no "
   "later tuples, raises StopIteration"},
  {"Next", (PyCFunction) <P>_Next, METH_O,
   "the next (key, value) tuple where the key is after the input key.  If there "
   "are no later tuples, raises StopIteration"},
  {"Last", (PyCFunction) <P>_Last, METH_VARARGS,
   "the first (key, value) tuple where the key is at or before the input key. "
   "If no input, returns the last tuple in the array.  If there are no "
   "earlier tuples, raises StopIteration"},
  {"Prev", (PyCFunction) <P>_Prev, METH_O,
   "the previous (key, value) tuple where the key is before the input key.  If "
   "there are no earlier tuples, raises StopIteration"},
  {"FirstEmpty", (PyCFunction) <P>_FirstEmpty, METH_VARARGS,
   "the location of the first empty key position (as a signed integer) at or "
   "after the input key. If no input, the first empty position.  If no empty "
   "locations remain, raises StopIteration."},
  {"NextEmpty", (PyCFunction) <P>_NextEmpty, METH_O,
   "the location of the next empty key position (as a signed integer) after "
   "the input key. If no empty locations remain, raises StopIteration."},
  {"LastEmpty", (PyCFunction) <P>_LastEmpty, METH_VARARGS,
   "the location of the first empty key position (as a signed integer) at or "
   "before the input key. If no input, the last empty position.  If no empty "
   "locations remain, raises StopIteration."},
  {"PrevEmpty", (PyCFunction) <P>_PrevEmpty, METH_O,
   "the location of the previous empty key position (as a signed integer) before "
   "the input key. If no empty locations remain, raises StopIteration."},

  {"keys", (PyCFunction) <P>_keys, METH_NOARGS,
   "the keys as a list"},
  {"values", (PyCFunction) <P>_values, METH_NOARGS,
   "the values as a list"},
  {"items", (PyCFunction) <P>_items, METH_NOARGS,
   "the items as a list of (key, value) tuples"},
  {"iterkeys", (PyCFunction) <P>_iterkeys, METH_NOARGS,
   "iterate through all keys"},
  {"iterkeys_from", (PyCFunction) <P>_iterkeys_from, METH_O,
   "iterate through the keys starting with the given input key, going up"},
  {"iterkeys_to", (PyCFunction) <P>_iterkeys_to, METH_O, 
   "iterate through the keys up to but not including the given input key"},
  {"iterkeys_range", (PyCFunction) <P>_iterkeys_range, METH_VARARGS,
   "iterate through the keys starting at or after the first input key and "
   "going up to but not including the second"},

  {"itervalues", (PyCFunction) <P>_itervalues, METH_NOARGS,
   "iterate through all values"},
  {"itervalues_from", (PyCFunction) <P>_itervalues_from, METH_O,
   "iterate through the values corresponding to keys at or after the input key"},
  {"itervalues_to", (PyCFunction) <P>_itervalues_to, METH_O,
   "iterate through the values corresponding to keys up to but not including "
   "the input key"},
  {"itervalues_range", (PyCFunction) <P>_itervalues_range, METH_VARARGS, 
   "iterate through the values corresponding to keys starting at or after the "
   "first input key and going up to but not including the second"},

  {"iteritems", (PyCFunction) <P>_iteritems, METH_NOARGS,
   "iterate through all (key,value) pairs"},
  {"iteritems_from", (PyCFunction) <P>_iteritems_from, METH_O, 
   "iterate through the (key,value) pairs starting with the key at or after "
   "the given input key"},
  {"iteritems_to", (PyCFunction) <P>_iteritems_to, METH_O,
   "iterate through the (key,value) pairs where the key is before the input key"},
  {"iteritems_range", (PyCFunction) <P>_iteritems_range, METH_VARARGS, 
   "iterate through the (key,value) pairs starting with the key at or above "
   "the first input key and going up to but not including the second "},

  {"riterkeys", (PyCFunction) <P>_riterkeys, METH_NOARGS, 
   "reverse iteration through all the keys"},
  {"riterkeys_from", (PyCFunction) <P>_riterkeys_from, METH_O,
   "reverse iteration through the keys starting with the given input key, "
   "going down"},
  {"riterkeys_to", (PyCFunction) <P>_riterkeys_to, METH_O,
   "reverse iteration through the keys down to but not including the given "
   "input key"},
  {"riterkeys_range", (PyCFunction) <P>_riterkeys_range, METH_VARARGS,
   "reverse iteration through the keys starting at or before the first input key "
   "and going down to but not including the second"},

  {"ritervalues", (PyCFunction) <P>_ritervalues, METH_NOARGS,
   "reverse iteration through all values"},
  {"ritervalues_from", (PyCFunction) <P>_ritervalues_from, METH_O,
   "reverse iteration through the values corresponding to keys at or before "
   "the input key"},
  {"ritervalues_to", (PyCFunction) <P>_ritervalues_to, METH_O, 
   "reverse iteration through the values corresponding to keys down to but "
   "not including the input key"},
  {"ritervalues_range", (PyCFunction) <P>_ritervalues_range, METH_VARARGS,
   "reverse iteration through the keys starting at or before the first input key "
   "and going down to but not including the second"},

  {"riteritems", (PyCFunction) <P>_riteritems, METH_NOARGS,
   "reverse iteration through all (key,value) pairs"},
  {"riteritems_from", (PyCFunction) <P>_riteritems_from, METH_O,
   "reverse iteration through the (key,value) pairs starting with the "
   "key at or before the given input key"},
  {"riteritems_to", (PyCFunction) <P>_riteritems_to, METH_O, 
   "reverse iteration through the (key,value) pairs while the key is above "
   "the given input key"},
  {"riteritems_range", (PyCFunction) <P>_riteritems_range, METH_VARARGS, 
   "iterate through the (key,value) pairs starting with the key at or below "
   "the first input key and going down to but not including the second"},

  {"__reversed__", (PyCFunction) <P>_riter, METH_NOARGS,
   "reverse iteration through the keys"},

  {"iterempty", (PyCFunction) <P>_iterempty, METH_NOARGS,
   "iterate through all the possible empty keys (as signed integers)"},
  {"iterempty_from", (PyCFunction) <P>_iterempty_from, METH_O, 
   "iterate through all the possible empty keys (as signed integers) at or "
   "above the given input key"},
  {"iterempty_to", (PyCFunction) <P>_iterempty_to, METH_O,
   "iterate through all the possible empty keys (as signed integers) up to "
   "but not including the given input key"},
  {"iterempty_range", (PyCFunction) <P>_iterempty_range, METH_VARARGS,
   "iterate through all the possible empty keys (as signed integers) starting "
   "at or above the first input key and going up to but not including the second"},

  {"riterempty", (PyCFunction) <P>_riterempty, METH_NOARGS,
   "reverse iteration through all the possible empty keys (as signed integers)"},
  {"riterempty_from", (PyCFunction) <P>_riterempty_from, METH_O, 
   "reverse iteration through all the possible empty keys (as signed integers) "
   "at or below the given input key"},
  {"riterempty_to", (PyCFunction) <P>_riterempty_to, METH_O,
   "reverse iteration through all the possible empty keys (as signed integers) "
   "down to but not including the given input key"},
  {"riterempty_range", (PyCFunction) <P>_riterempty_range, METH_VARARGS,
   "iterate through all the possible empty keys (as signed integers) starting "
   "at or below the first input key and going down to but not including the second"},
  
  {NULL}
};

/* <P> */
static PyTypeObject <P>Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyjudy.<P>",             /*tp_name*/
    sizeof(<CT>), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)<P>_dealloc,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    &<P>_as_sequence,                         /*tp_as_sequence*/
    &<P>_as_mapping,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "JudyL arrays",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    <P>_iter,                       /* tp_iter */
    0,                         /* tp_iternext */
    <P>_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0, /*(initproc)<P>_init,*/      /* tp_init */
    0,                         /* tp_alloc */
    <P>_new,                 /* tp_new */

};


/******* forward iteration *******/


typedef struct {
  PyObject_HEAD
  short found_first;
  short has_limit;
  PyObject *result;  /* reusable result tuple for iteritems */
  Word_t index;
  Word_t stop;
  <CT> *container;   /* Set to NULL when iterator is exhausted */
} <P>_iterobject;

static PyObject *
<P>_iter_new(<CT> *container, const char *name,
	     Word_t start, Word_t end, int has_limit,
	     PyTypeObject *itertype)
{
  <P>_iterobject *it;

  it = PyObject_New(<P>_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->found_first = 0;
  it->index = start;
  it->stop = end;
  it->has_limit = has_limit;

  Py_INCREF(container);
  it->container = container;

  if (itertype == &<P>IterItem_Type) {
    it->result = PyTuple_New(2);
    if (it->result == NULL) {
      Py_DECREF(it);
      return NULL;
    }
    Py_INCREF(Py_None);
    PyTuple_SET_ITEM(it->result, 0, Py_None);
    Py_INCREF(Py_None);
    PyTuple_SET_ITEM(it->result, 1, Py_None);
  } else
    it->result = NULL;
  
  return (PyObject *)it;
}

static void
<P>_iter_dealloc(<P>_iterobject *it)
{
  Py_XDECREF(it->container);
  Py_XDECREF(it->result);
  PyObject_Del(it);
}

/***************************/

static PyObject *<P>Iter_iternextkey(<P>_iterobject *it)
{
  PPvoid_t p;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudyLNext(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLFirst(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index >= it->stop)
      ) { /* Reached the upper limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  index = it->index;
  return PY_FROM_INDEX;
}

PyTypeObject <P>IterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-keyiterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>Iter_iternextkey,    /* tp_iternext */
};


static PyObject *<P>Iter_iternextvalue(<P>_iterobject *it)
{
  PPvoid_t p;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudyLNext(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLFirst(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index >= it->stop)
      ) { /* Reached the upper limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }

  return PY_FROM_PWORD;
}

PyTypeObject <P>IterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-valueiterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>Iter_iternextvalue,    /* tp_iternext */
};




static PyObject *<P>Iter_iternextitem(<P>_iterobject *it)
{
  PyObject *key, *value, *result;
  PPvoid_t p;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  result = it->result;
  
  if (it->found_first) {
    p = JudyLNext(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLFirst(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index >= it->stop)
      ) { /* Reached the upper limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }

  /* Performance tweak by reusing a tuple not used by anyone else */
  if (result->ob_refcnt == 1) {
    Py_INCREF(result);
    Py_DECREF(PyTuple_GET_ITEM(result, 0));
    Py_DECREF(PyTuple_GET_ITEM(result, 1));
  } else {
    result = PyTuple_New(2);
    if (result == NULL)
      return NULL;
  }
  index = it->index;
  key = PY_FROM_INDEX;
  if (!key)
    return NULL;
  value = PY_FROM_PWORD;
  if (!value) {
    Py_DECREF(key);
    return NULL;
  }

  PyTuple_SET_ITEM(result, 0, key);
  PyTuple_SET_ITEM(result, 1, value);
  return result;
}

PyTypeObject <P>IterItem_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-itemiterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>Iter_iternextitem,    /* tp_iternext */
};

/******* reverse iteration *******/

static PyObject *
<P>_riter_new(<CT> *container, const char *name,
	      Word_t start, Word_t end, int has_limit,
	      PyTypeObject *itertype)
{
  <P>_iterobject *it;

  it = PyObject_New(<P>_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->found_first = 0;
  it->index = start;
  it->stop = end;
  it->has_limit = has_limit;

  Py_INCREF(container);
  it->container = container;

  if (itertype == &<P>RIterItem_Type) {
    it->result = PyTuple_New(2);
    if (it->result == NULL) {
      Py_DECREF(it);
      return NULL;
    }
    Py_INCREF(Py_None);
    PyTuple_SET_ITEM(it->result, 0, Py_None);
    Py_INCREF(Py_None);
    PyTuple_SET_ITEM(it->result, 1, Py_None);
  } else
    it->result = NULL;
  
  return (PyObject *)it;
}

/***************************/

static PyObject *<P>RIter_iternextkey(<P>_iterobject *it)
{
  PPvoid_t p;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudyLPrev(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLLast(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index <= it->stop)
      ) { /* Reached the lower limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  index = it->index;
  return PY_FROM_INDEX;
}

PyTypeObject <P>RIterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-keyriterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>RIter_iternextkey,    /* tp_iternext */
};


static PyObject *<P>RIter_iternextvalue(<P>_iterobject *it)
{
  PPvoid_t p;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudyLPrev(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLLast(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index <= it->stop)
      ) { /* Reached the lower limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  return PY_FROM_PWORD;
}

PyTypeObject <P>RIterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-valueiterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>RIter_iternextvalue,    /* tp_iternext */
};




static PyObject *<P>RIter_iternextitem(<P>_iterobject *it)
{
  PyObject *key, *value, *result;
  PPvoid_t p;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  result = it->result;
  
  if (it->found_first) {
    p = JudyLPrev(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    p = JudyLLast(it->container->array, &(it->index), PJE0);
  }
  if (!p ||  /* Reached the end */
      (it->has_limit && it->index <= it->stop)
      ) { /* Reached the upper limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }

  /* Performance tweak by reusing a tuple not used by anyone else */
  if (result->ob_refcnt == 1) {
    Py_INCREF(result);
    Py_DECREF(PyTuple_GET_ITEM(result, 0));
    Py_DECREF(PyTuple_GET_ITEM(result, 1));
  } else {
    result = PyTuple_New(2);
    if (result == NULL)
      return NULL;
  }
  index = it->index;
  key = PY_FROM_INDEX;
  if (!key)
    return NULL;

  value = PY_FROM_PWORD;
  if (!value) {
    Py_DECREF(key);
    return NULL;
  }

  PyTuple_SET_ITEM(result, 0, key);
  PyTuple_SET_ITEM(result, 1, value);
  return result;
}

PyTypeObject <P>RIterItem_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-itemriterator",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>RIter_iternextitem,    /* tp_iternext */
};

/***** Forward iteration over the empty keys *****/

static PyObject *<P>_iterempty_next(<P>_iterobject *it)
{
  int rc;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    rc = JudyLNextEmpty(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = JudyLFirstEmpty(it->container->array, &(it->index), PJE0);
  }
  if (rc == 0 ||  /* Reached the end */
      (it->has_limit && it->index >= it->stop)
      ) { /* Reached the upper limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  index = it->index;

  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

PyTypeObject <P>IterEmpty_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-emptyiter",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>_iterempty_next,       /* tp_iternext */
};


/***** Reverse iteration over the empty keys *****/

static PyObject *<P>_riterempty_next(<P>_iterobject *it)
{
  int rc;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    rc = JudyLPrevEmpty(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = JudyLLastEmpty(it->container->array, &(it->index), PJE0);
  }
  if (rc == 0 ||  /* Reached the end */
      (it->has_limit && it->index <= it->stop)
      ) { /* Reached the lower limit */ 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  index = it->index;
  /* This can only return an integer - it makes no sense otherwise */
  /* For that matter, this function makes no sense for object keys. */
  return PyInt_FromLong(index);
}

PyTypeObject <P>RIterEmpty_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-remptyiter",              /* tp_name */
        sizeof(<P>_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)<P>_iter_dealloc,           /* tp_dealloc */
        0,                                      /* tp_print */
        0,                                      /* tp_getattr */
        0,                                      /* tp_setattr */
        0,                                      /* tp_compare */
        0,                                      /* tp_repr */
        0,                                      /* tp_as_number */
        0, /*&dictiter_as_sequence,*/                  /* tp_as_sequence */
        0,                                      /* tp_as_mapping */
        0,                                      /* tp_hash */
        0,                                      /* tp_call */
        0,                                      /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                      /* tp_setattro */
        0,                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                     /* tp_flags */
        0,                                      /* tp_doc */
        0,                                      /* tp_traverse */
        0,                                      /* tp_clear */
        0,                                      /* tp_richcompare */
        0,                                      /* tp_weaklistoffset */
        PyObject_SelfIter,                      /* tp_iter */
        (iternextfunc)<P>_riterempty_next,    /* tp_iternext */
};
