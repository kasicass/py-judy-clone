/* This is not a limitation in Judy per se.  It's a limitation in the      */
/* Judy modifies the string 'index' buffer in-place.  Python strings don't */
/* like that.  I could have a per-array buffer, perhaps configurable when  */
/* you create the object, or an optional per-call 'buffer' parameter for   */
/* those who want a thread-safe interface.  This works for now.            */

/* Helper function to make a 2-element tuple from key/value */

static PyObject *
<P>_make_2ple(uint8_t *index, PPvoid_t p)
{
  PyObject *tuple;
  PyObject *key;
  PyObject *value;

  tuple = PyTuple_New(2);
  if (tuple == NULL)
    return NULL;

  key = PyString_FromString((char *) index);
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
  JudySLFreeArray( &(self->array), PJE0);
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
  self->count = 0;
  self->max_seen = 0;
  return (PyObject *)self;
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
  char *s;

  PY_TO_INDEX;

  DELITEM;
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
  PPvoid_t p;
  char *s;

  PY_TO_INDEX;
  p = JudySLGet(self->array, s, PJE0);
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
  PPvoid_t p;
  char *s;
  PyObject *key, *failobj = Py_None;

  if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj))
    return NULL;

  PY_TO_INDEX;
  p = JudySLGet(self->array, s, PJE0);
  if (p == NULL) {
    /* Not present; return the default */
    Py_INCREF(failobj);
    return failobj;
  }
  return PY_FROM_PWORD;
}

static PyObject *
<P>_FreeArray(<CT> *self)
{
  FREE_ELEMENTS
  self->count=0;
  self->max_seen=0;
  JudySLFreeArray( &(self->array), PJE0);
  /* not returning the size */
  Py_INCREF(Py_None);
  return Py_None;
}

/* ******** */

static PyObject *
<P>_First(<CT> *self, PyObject *args)
{
  PPvoid_t p;
  PyObject *key=NULL;
  char *s;

  if (!PyArg_UnpackTuple(args, "First", 0, 1, &key))
    return NULL;
  if (key != NULL) {
    PY_TO_INDEX;
    strcpy(index_buffer, s);
  } else {
    index_buffer[0] = '\0';
  }

  p = JudySLFirst(self->array, (uint8_t *)index_buffer, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index_buffer, p);
 }

static PyObject *
<P>_Next(<CT> *self, PyObject *key)
{
  char *s;
  PPvoid_t p;

  PY_TO_INDEX;
  strcpy(index_buffer, s);

  p = JudySLNext(self->array, (uint8_t *)index_buffer, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }

  return <P>_make_2ple(index_buffer, p);
}

static PyObject *
<P>_Last(<CT> *self, PyObject *args)
{
  char *s;
  PPvoid_t p;
  PyObject *key = NULL;

  if (!PyArg_UnpackTuple(args, "Last", 0, 1, &key))
    return NULL;
  if (key != NULL) {
    PY_TO_INDEX;
    strcpy(index_buffer, s);
  } else {
    memset(index_buffer, 255, self->max_seen);
    index_buffer[self->max_seen] = '\0';
  }

  p = JudySLLast(self->array, (uint8_t *)index_buffer, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index_buffer, p);
}

static PyObject *
<P>_Prev(<CT> *self, PyObject *key)
{
  char *s;
  PPvoid_t p;

  PY_TO_INDEX;
  strcpy(index_buffer, s);
  p = JudySLPrev(self->array, (uint8_t *)index_buffer, PJE0);
  if (p == NULL) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
  return <P>_make_2ple(index_buffer, p);
}


/* Python object adapters */

static PyObject *
<P>_keys(<CT> *self)
{
  PyObject *v;
  Word_t n;
  PPvoid_t p;
  int i=0;
  char *s;

  n = self->count;
  v = PyList_New(n);
  if (v == NULL)
    return NULL;
  index_buffer[0] = '\0';
  p = JudySLFirst(self->array, (uint8_t *)index_buffer, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudySL size changed while creating list of keys");
      Py_DECREF(v);
      return NULL;
    }
    s = index_buffer;
    PyList_SET_ITEM(v, i, PY_FROM_INDEX);
    i++;
    p = JudySLNext(self->array, (uint8_t *)index_buffer, PJE0);
  }
  return v;
}

static PyObject *
<P>_values(<CT> *self)
{
  PyObject *v;
  Word_t n;
  PPvoid_t p;
  int i=0;

  n = self->count;
  v = PyList_New(n);
  index_buffer[0] = '\0';
  p = JudySLFirst(self->array, (uint8_t *)index_buffer, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudySL size changed while creating list of values");
      Py_DECREF(v);
      return NULL;
    }
    PyList_SET_ITEM(v, i, PY_FROM_PWORD);
    i++;
    p = JudySLNext(self->array, (uint8_t *)index_buffer, PJE0);
  }
  return v;
}

static PyObject *
<P>_items(<CT> *self)
{
  PyObject *v, *tuple;
  Word_t n;
  PPvoid_t p;
  int i=0;

  n = self->count;
  v = PyList_New(n);
  index_buffer[0] = '\0';
  p = JudySLFirst(self->array, (uint8_t *)index_buffer, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudySL size changed while creating list of items");
      goto error;
    }
    tuple = <P>_make_2ple(index_buffer, p);
    if (tuple == NULL)
      goto error;
    PyList_SET_ITEM(v, i, tuple);
    i++;
    p = JudySLNext(self->array, (uint8_t *)index_buffer, PJE0);
  }
  return v;

 error:
  Py_XDECREF(v);
  return NULL;
}

int
<P>_contains(<CT> *self, PyObject *key)
{
  char *s;
  Pvoid_t p;
  PY_TO_INDEX;
  p = JudySLGet(self->array, s, PJE0);
  return p != NULL;
}

/* Meet the mapping protocol */
static int
<P>_length(<CT> *self)
{
  /* XXX what if count > 2**31? */
  return self->count;
}

static PyObject *
<P>_Count(<CT> *self)
{
  return PyInt_FromLong(self->count);
}

static PyObject *
<P>_subscript(<CT> *self, PyObject *key)
{
  char *s;
  PPvoid_t p;

  PY_TO_INDEX;
  p = JudySLGet(self->array, s, PJE0);
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
  {"Ins", (PyCFunction) <P>_Ins, METH_VARARGS, 
   "insert a string and value into the array"},
  {"Del", (PyCFunction) <P>_Del, METH_O, 
   "delete the given key; return True if a deletion occured, else False"},
  {"Get", (PyCFunction) <P>_Get, METH_O,
   "given a key get the corresponding value; raise IndexError there is not one"},
  {"get", (PyCFunction) <P>_get, METH_VARARGS,
   "given a key get the corresponding value; else return failobj (default failobj is None)"},
  {"Count", (PyCFunction) <P>_Count, METH_NOARGS,
   "total number of elements in the array"},
  {"FreeArray", (PyCFunction) <P>_FreeArray, METH_NOARGS,
   "free the entire array"},
  {"clear", (PyCFunction) <P>_FreeArray, METH_NOARGS,
   "free the entire array"},
  {"First", (PyCFunction) <P>_First, METH_VARARGS,
   "the first (key, value) tuple where the key is at or after the input. "
   "If no input, returns the first tuple in the array.  If there are no "
   "later tuples, raises StopIteration"},
  {"Next", (PyCFunction) <P>_Next, METH_O,
   "the next (key, value) tuple where the key is after the input.  If there "
   "are no later tuples, raises StopIteration"},
  {"Last", (PyCFunction) <P>_Last, METH_VARARGS,
   "the first (key, value) tuple where the key is at or before the input. "
   "If no input, returns the last tuple in the array.  If there are no "
   "earlier tuples, raises StopIteration"},
  {"Prev", (PyCFunction) <P>_Prev, METH_O,
   "the previous (key, value) tuple where the key is before the input.  If "
   "there are no earlier tuples, raises StopIteration"},

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
   "iterate through the keys starting at or after the first input and going up "
   "to but not including the second"},

  {"itervalues", (PyCFunction) <P>_itervalues, METH_NOARGS,
   "iterate through all values"},
  {"itervalues_from", (PyCFunction) <P>_itervalues_from, METH_O,
   "iterate through the values corresponding to keys at or after the input"},
  {"itervalues_to", (PyCFunction) <P>_itervalues_to, METH_O,
   "iterate through the values corresponding to keys up to but not including "
   "the input"},
  {"itervalues_range", (PyCFunction) <P>_itervalues_range, METH_VARARGS, 
   "iterate through the values corresponding to keys starting at or after the "
   "first input and going up to but not including the second"},

  {"iteritems", (PyCFunction) <P>_iteritems, METH_NOARGS,
   "iterate through all (key,value) pairs"},
  {"iteritems_from", (PyCFunction) <P>_iteritems_from, METH_O, 
   "iterate through the (key,value) pairs starting with the key at or after "
   "the given input"},
  {"iteritems_to", (PyCFunction) <P>_iteritems_to, METH_O,
   "iterate through the (key,value) pairs where the key is before the input"},
  {"iteritems_range", (PyCFunction) <P>_iteritems_range, METH_VARARGS, 
   "iterate through the (key,value) pairs starting with the key at or above "
   "the first input and going up to but not including the second input"},

  {"riterkeys", (PyCFunction) <P>_riterkeys, METH_NOARGS, 
   "reverse iteration through all the keys"},
  {"riterkeys_from", (PyCFunction) <P>_riterkeys_from, METH_O,
   "reverse iteration through the keys starting with the given input key, "
   "going down"},
  {"riterkeys_to", (PyCFunction) <P>_riterkeys_to, METH_O,
   "reverse iteration through the keys down to but not including the given "
   "input key"},
  {"riterkeys_range", (PyCFunction) <P>_riterkeys_range, METH_VARARGS,
   "reverse iteration through the keys starting at or before the first input "
   "and going down to but not including the second"},

  {"ritervalues", (PyCFunction) <P>_ritervalues, METH_NOARGS,
   "reverse iteration through all values"},
  {"ritervalues_from", (PyCFunction) <P>_ritervalues_from, METH_O,
   "reverse iteration through the values corresponding to keys at or before "
   "the input"},
  {"ritervalues_to", (PyCFunction) <P>_ritervalues_to, METH_O, 
   "reverse iteration through the values corresponding to keys down to but "
   "not including the input"},
  {"ritervalues_range", (PyCFunction) <P>_ritervalues_range, METH_VARARGS,
   "reverse iteration through the keys starting at or before the first input "
   "and going down to but not including the second"},

  {"riteritems", (PyCFunction) <P>_riteritems, METH_NOARGS,
   "reverse iteration through all (key,value) pairs"},
  {"riteritems_from", (PyCFunction) <P>_riteritems_from, METH_O,
   "reverse iteration through the (key,value) pairs starting with the "
   "key at or before the given input"},
  {"riteritems_to", (PyCFunction) <P>_riteritems_to, METH_O, 
   "reverse iteration through the (key,value) pairs while the key is above "
   "the given input"},
  {"riteritems_range", (PyCFunction) <P>_riteritems_range, METH_VARARGS, 
   "iterate through the (key,value) pairs starting with the key at or below "
   "the first input and going down to but not including the second input"},

  {"__reversed__", (PyCFunction) <P>_riter, METH_NOARGS,
   "reverse iteration through the keys"},

  
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
    "JudySL arrays",           /* tp_doc */
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
  PyObject *result;  /* reusable result tuple for iteritems */
  PyObject *end;
  const char *end_s;
  <CT> *container;   /* Set to NULL when iterator is exhausted */
  uint8_t iter_buffer[MAX_STRING_LEN];
} <P>_iterobject;

static PyObject *
<P>_iter_new(<CT> *container, const char *name,
	     PyObject *start, PyObject *end,
	     PyTypeObject *itertype)
{
  <P>_iterobject *it;
  char *start_s=NULL, *end_s=NULL;

  if ((start != NULL) && (check_string(start, &start_s) != 0))
    return NULL;
  if ((end != NULL) && (check_string(end, &end_s) != 0))
    return NULL;

  it = PyObject_New(<P>_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->iter_buffer[MAX_STRING_LEN] = '\0';
  if (start != NULL) {
    strcpy(it->iter_buffer, start_s);
  } else {
    it->iter_buffer[0] = '\0';
  }

  it->found_first = 0;
  it->end = end;
  if (end)
    Py_INCREF(end);
  it->end_s = end_s;
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
  Py_XDECREF(it->end);
  PyObject_Del(it);
}

/***************************/

static PyObject *<P>Iter_iternextkey(<P>_iterobject *it)
{
  PPvoid_t p;
  char *s;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudySLNext(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLFirst(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or- */
             /* Reached the upper limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)>=0)
      ) {
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  s = it->iter_buffer;
  return PY_FROM_INDEX;
}

PyTypeObject <P>IterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudysl-keyiterator",              /* tp_name */
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
    p = JudySLNext(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLFirst(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or- */
             /* Reached the upper limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)>=0)
      ) { 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }

  return PY_FROM_PWORD;
}

PyTypeObject <P>IterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudysl-valueiterator",              /* tp_name */
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
  char *s;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  result = it->result;
  
  if (it->found_first) {
    p = JudySLNext(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLFirst(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or-*/
             /* Reached the upper limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)>=0)
      ) {
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
  s = it->iter_buffer;
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
        "pyjudysl-itemiterator",              /* tp_name */
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
	      PyObject *start, PyObject *end,
	      PyTypeObject *itertype)
{
  <P>_iterobject *it;
  char *start_s, *end_s=NULL;

  if ((start != NULL) && (check_string(start, &start_s) != 0))
    return NULL;
  if ((end != NULL) && (check_string(end, &end_s) != 0))
    return NULL;

  it = PyObject_New(<P>_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->iter_buffer[container->max_seen] = '\0';
  if (start != NULL) {
    strcpy(it->iter_buffer, start_s);
  } else {
    memset(it->iter_buffer, 255, container->max_seen);
  }
  it->found_first = 0;
  it->end = end;
  if (end)
    Py_INCREF(end);
  it->end_s = end_s;

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
  char *s;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    p = JudySLPrev(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLLast(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or- */
             /* Reached the lower limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)<=0)
      ) { 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  s = it->iter_buffer;
  return PY_FROM_INDEX;
}

PyTypeObject <P>RIterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudysl-keyriterator",              /* tp_name */
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
    p = JudySLPrev(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLLast(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or- */
             /* Reached the lower limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)<=0)
      ) { 
    Py_DECREF(it->container);
    it->container = NULL;
    return NULL;
  }
  return PY_FROM_PWORD;
}

PyTypeObject <P>RIterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudysl-valueiterator",              /* tp_name */
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
  char *s;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  result = it->result;
  
  if (it->found_first) {
    p = JudySLPrev(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  } else {
    it->found_first = 1;
    p = JudySLLast(it->container->array, (uint8_t *)it->iter_buffer, PJE0);
  }
  if (!p ||  /* Reached the end -or- */
             /* Reached the lower limit */ 
      (it->end && strcmp(it->iter_buffer, it->end_s)<=0)
      ) {
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
  s = it->iter_buffer;
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
        "pyjudysl-itemriterator",              /* tp_name */
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

