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

static void
<P>_dealloc(<CT>* self)
{
  FREE_ELEMENTS
  Judy1FreeArray( &(self->array), PJE0);
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
  size = Judy1MemUsed(self->array);
  return as_unsigned(size);
}


static PyObject *
<P>_Set(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;

  rc = Judy1Set(&(self->array), index, PJE0);

  if (rc == 0) {
    /* Already in the array - don't need to incr again */
    Py_INCREF(Py_False);
    return Py_False;
  } else {
    INCR_KEY;
    Py_INCREF(Py_True);
    return Py_True;
  }
}

static PyObject *
<P>_add(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;

  rc = Judy1Set(&(self->array), index, PJE0);

  if (rc != 0) {
    INCR_KEY;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
<P>_Unset(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;
  
  PY_TO_INDEX;

  rc = Judy1Unset(&(self->array), index, PJE0);
  
  if (rc == 0) {
    /* was already unset */
    Py_INCREF(Py_False);
    return Py_False;
  } else {
    DECR_KEY;
    Py_INCREF(Py_True);
    return Py_True;
  }
}

static PyObject *
<P>_remove(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;
  
  PY_TO_INDEX;

  rc = Judy1Unset(&(self->array), index, PJE0);
  
  if (rc == 0) {
    /* was already unset */
    PyErr_SetObject(PyExc_KeyError, key);
    return NULL;
  }
  DECR_KEY;
  Py_INCREF(Py_None);
  return Py_None;
}



static PyObject *
<P>_Test(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;
  
  PY_TO_INDEX;

  rc = Judy1Test(self->array, index, PJE0);
  
  if (rc == 0) {
    Py_INCREF(Py_False);
    return Py_False;
  } else {
    Py_INCREF(Py_True);
    return Py_True;
  }
}



static PyObject *
<P>_Count(<CT> *self)
{
  Word_t count;
  count = Judy1Count(self->array, 0, -1, PJE0);
  return as_unsigned(count);
}
static PyObject *
<P>_Count_from(<CT> *self, PyObject *key)
{
  Word_t count, index;
  PY_TO_INDEX;
  count = Judy1Count(self->array, index, -1, PJE0);
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
    count = Judy1Count(self->array, 0, index-1, PJE0);
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
    count = Judy1Count(self->array, start, end-1, PJE0);
  }
  return as_unsigned(count);
}


static PyObject *
<P>_ByCount(<CT> *self, PyObject *key)
{
  int rc;
  Word_t index;
  long Nth;
  /* XXX Need to allow Nth >= LONG_MAX */
  Nth = PyInt_AsLong(key);
  if (Nth == -1 && PyErr_Occurred())
    return NULL;
  rc = Judy1ByCount(self->array, (Word_t) Nth, &index, PJE0);
  if (rc == 0) {
    /* Not found */
    PyErr_SetObject(PyExc_IndexError, key);
    return NULL;
  } else {
    return PY_FROM_INDEX;
  }
}

static PyObject *
<P>_FreeArray(<CT> *self)
{
  FREE_ELEMENTS
  Judy1FreeArray( &(self->array), PJE0);
  /* not returning the size */
  Py_INCREF(Py_None);
  return Py_None;
}

/* ******** */

static PyObject *
<P>_First(<CT> *self, PyObject *args)
{
  Word_t index = 0;
  PyObject *key=NULL;
  int rc;

  if (!PyArg_UnpackTuple(args, "First", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  rc = Judy1First(self->array, &index, PJE0);
  if (rc == 0) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    key = PY_FROM_INDEX;
    INCR_KEY;
    return key;
  }
}

static PyObject *
<P>_Next(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;
  rc = Judy1Next(self->array, &index, PJE0);
  if (rc == 0) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    key = PY_FROM_INDEX;
    INCR_KEY;
    return key;
  }
}

static PyObject *
<P>_Last(<CT> *self, PyObject *args)
{
  Word_t index = -1;
  PyObject *key = NULL;
  int rc;

  if (!PyArg_UnpackTuple(args, "Last", 0, 1, &key))
    return NULL;
  if (key) {
    PY_TO_INDEX;
  }

  rc = Judy1Last(self->array, &index, PJE0);
  if (rc == 0) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    key = PY_FROM_INDEX;
    INCR_KEY;
    return key;
  }
}

static PyObject *
<P>_Prev(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;
  rc = Judy1Prev(self->array, &index, PJE0);
  if (rc == 0) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  } else {
    key = PY_FROM_INDEX;
    INCR_KEY;
    return key;
  }
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

  rc = Judy1FirstEmpty(self->array, &index, PJE0);
  if (rc == 0) {
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
  rc = Judy1NextEmpty(self->array, &index, PJE0);
  if (rc == 0) {
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

  rc = Judy1LastEmpty(self->array, &index, PJE0);
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
  rc = Judy1PrevEmpty(self->array, &index, PJE0);
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
  int i=0, rc;

  n = Judy1Count(self->array, 0, -1, PJE0);
  v = PyList_New(n);
  if (v == NULL)
    return NULL;
  index = 0;
  rc = Judy1First(self->array, &index, PJE0);
  while (i<n) {
    if (!rc) {
      PyErr_SetString(PyExc_SystemError,
		      "Judy1 size changed while creating list of keys");
      Py_DECREF(v);
      /* XXX Memory leak here? */
      return NULL;
    }
    PyList_SET_ITEM(v, i, PY_FROM_INDEX);
    i++;
    rc = Judy1Next(self->array, &index, PJE0);
  }
  return v;
}

int
<P>_contains(<CT> *self, PyObject *key)
{
  Word_t index;
  int rc;

  PY_TO_INDEX;

  rc = Judy1Test(self->array, (Word_t) index, PJE0);
  return (rc != 0);
}

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
  {"Set", (PyCFunction) <P>_Set, METH_O,
   "set the corresponding bit to 1"},
  {"add", (PyCFunction) <P>_add, METH_O,
   "add the object to the set"},
  {"Unset", (PyCFunction) <P>_Unset, METH_O,
   "set the corresponding bit to 0"},
  {"remove", (PyCFunction) <P>_remove, METH_O,
   "remove the object from the set"},
  {"Test", (PyCFunction) <P>_Test, METH_O,
   "return True if the bit is set, else False"},
  {"Count", (PyCFunction) <P>_Count, METH_NOARGS, 
   "total number of bits set in the array"},
  {"__len__", (PyCFunction) <P>_Count, METH_NOARGS, "__len__"},
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
   "the first set key in the array where the key is at or after the input key. "
   "If no input is given, returns the first key in the array.  If there are no "
   "set keys at or after the input key, raises StopIteration"},
  {"Next", (PyCFunction) <P>_Next, METH_O,
   "the next set key in the array after the given input key.  If there are no "
   "later set keys, raises StopIteration"},
  {"Last", (PyCFunction) <P>_Last, METH_VARARGS, 
   "the last set key in the array where the key is at or before the input key. "
   "If no input key is given, returns the last key in the array.  If there are no "
   "set keys at or before the input, raises StopIteration"},
  {"Prev", (PyCFunction) <P>_Prev, METH_O, 
   "the previous set key in the array where the key is before the given input key. "
   "If there are no earlier keys, raises StopIteration"},

  {"FirstEmpty", (PyCFunction) <P>_FirstEmpty, METH_VARARGS,
   "the first unset key in the array where the key is at or after the input key. "
   "If no input key is given, returns the first key in the array.  If there are no "
   "unset keys at or after the key, raises StopIteration"},
  {"NextEmpty", (PyCFunction) <P>_NextEmpty, METH_O,
   "the next unset key in the array after the given input key.  If there are no "
   "later unset keys, raises StopIteration"},
  {"LastEmpty", (PyCFunction) <P>_LastEmpty, METH_VARARGS,
   "the last unset key in the array where the key is at or before the input key. "
   "If no input key is given, returns the last key in the array.  If there are no "
   "unset keys at or before the input, raises StopIteration"},
  {"PrevEmpty", (PyCFunction) <P>_PrevEmpty, METH_O,
   "the previous unset key in the array where the key is before the given input key. "
   "If there are no earlier keys, raises StopIteration"},

  {"keys", (PyCFunction) <P>_keys, METH_NOARGS,
   "a list of all set keys"},
  {"__contains__", (PyCFunction) <P>_contains, METH_O,
   "True if the object is set, otherwise False"},

  {"iterkeys", (PyCFunction) <P>_iterkeys, METH_NOARGS,
   "iterate through all set keys"},
  {"iterkeys_from", (PyCFunction) <P>_iterkeys_from, METH_O,
   "iterate through the set keys starting with the given input key, going up"},
  {"iterkeys_to", (PyCFunction) <P>_iterkeys_to, METH_O,
   "iterate through the set keys up to but not including the given input key"},
  {"iterkeys_range", (PyCFunction) <P>_iterkeys_range, METH_VARARGS,
   "iterate through the set keys starting at or after the first input and going up "
   "to but not including the second"},


  {"riterkeys", (PyCFunction) <P>_riterkeys, METH_NOARGS,
   "reverse iteration through all the set keys"},
  {"riterkeys_from", (PyCFunction) <P>_riterkeys_from, METH_O,
   "reverse iteration through the set keys starting with the given input key, "
   "going down"},
  {"riterkeys_to", (PyCFunction) <P>_riterkeys_to, METH_O,
   "reverse iteration through the set keys down to but not including the given "
   "input key"},
  {"riterkeys_range", (PyCFunction) <P>_riterkeys_range, METH_VARARGS,
   "reverse iteration through the set keys starting at or before the first input "
   "and going down to but not including the second"},

  {"__reversed__", (PyCFunction) <P>_riter, METH_NOARGS, 
   "reverse iteration through all the set keys"},

  {"iterempty", (PyCFunction) <P>_iterempty, METH_NOARGS,
   "iterate through all the unset keys (as signed integers)"},
  {"iterempty_from", (PyCFunction) <P>_iterempty_from, METH_O, 
   "iterate through all unset keys (as signed integers) at or "
   "above the input key"},
  {"iterempty_to", (PyCFunction) <P>_iterempty_to, METH_O,
   "iterate through all the unset keys (as signed integers) up to "
   "but not including the input key"},
  {"iterempty_range", (PyCFunction) <P>_iterempty_range, METH_VARARGS,
   "iterate through all the unset keys (as signed integers) starting "
   "at or above the first input key and going up to but not including the second"},

  {"riterempty", (PyCFunction) <P>_riterempty, METH_NOARGS,
   "reverse iteration through all the unset keys (as signed integers)"},
  {"riterempty_from", (PyCFunction) <P>_riterempty_from, METH_O, 
   "reverse iteration through all the unset keys (as signed integers) "
   "at or below the given input key"},
  {"riterempty_to", (PyCFunction) <P>_riterempty_to, METH_O,
   "reverse iteration through all the unset keys (as signed integers) "
   "down to but not including the given input key"},
  {"riterempty_range", (PyCFunction) <P>_riterempty_range, METH_VARARGS,
   "iterate through all the unset keys (as signed integers) starting "
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
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Judy1 arrays",           /* tp_doc */
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

  return (PyObject *)it;
}

static void
<P>_iter_dealloc(<P>_iterobject *it)
{
  Py_XDECREF(it->container);
  PyObject_Del(it);
}

/***************************/

static PyObject *<P>Iter_iternextkey(<P>_iterobject *it)
{
  int rc;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    rc = Judy1Next(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = Judy1First(it->container->array, &(it->index), PJE0);
  }
  if (!rc ||  /* Reached the end */
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

  return (PyObject *)it;
}

/***************************/

static PyObject *<P>RIter_iternextkey(<P>_iterobject *it)
{
  int rc;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    rc = Judy1Prev(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = Judy1Last(it->container->array, &(it->index), PJE0);
  }
  if (!rc ||  /* Reached the end */
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


/***** Forward iteration over the empty keys *****/

static PyObject *<P>_iterempty_next(<P>_iterobject *it)
{
  int rc;
  Word_t index;
  
  if (it->container == NULL)
    return NULL;   /* container previously exhausted */
  
  if (it->found_first) {
    rc = Judy1NextEmpty(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = Judy1FirstEmpty(it->container->array, &(it->index), PJE0);
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
    rc = Judy1PrevEmpty(it->container->array, &(it->index), PJE0);
  } else {
    it->found_first = 1;
    rc = Judy1LastEmpty(it->container->array, &(it->index), PJE0);
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
