
/* This file is auto-generated from pyjudyl_template.c */
/* Edit that file. */

#include "pyjudyl_obj_def.h"



/* Forward declarations for forward iteration */

extern PyTypeObject PyJudyLOIterKey_Type;
extern PyTypeObject PyJudyLOIterValue_Type;
extern PyTypeObject PyJudyLOIterItem_Type;
static PyObject *PyJudyLO_iter_new(PyJudyL *, char *,
				  PyObject *, PyTypeObject *);

static PyObject *
PyJudyLO_iter(PyObject *container)
{
  return PyJudyLO_iter_new((PyJudyL *)container, "iter",
			  NULL, &PyJudyLOIterKey_Type);
}
static PyObject *
PyJudyLO_iterkeys(PyObject *container, PyObject *args)
{
  return PyJudyLO_iter_new((PyJudyL *)container, "iterkeys",
			  args, &PyJudyLOIterKey_Type);
}
static PyObject *
PyJudyLO_itervalues(PyObject *container, PyObject *args)
{
  return PyJudyLO_iter_new((PyJudyL *)container, "itervalues",
			  args, &PyJudyLOIterValue_Type);
}
static PyObject *
PyJudyLO_iteritems(PyObject *container, PyObject *args)
{
  return PyJudyLO_iter_new((PyJudyL *)container, "iteritems",
			  args, &PyJudyLOIterItem_Type);
}

/* Forward declarations for reverse iteration */

extern PyTypeObject PyJudyLORIterKey_Type;
extern PyTypeObject PyJudyLORIterValue_Type;
extern PyTypeObject PyJudyLORIterItem_Type;
static PyObject *PyJudyLO_riter_new(PyJudyL *, char *, PyObject *, PyTypeObject *);

static PyObject *
PyJudyLO_riter(PyObject *container)
{
  return PyJudyLO_riter_new((PyJudyL *)container, "__reversed__",
			   NULL, &PyJudyLORIterKey_Type);
}
static PyObject *
PyJudyLO_riterkeys(PyObject *container, PyObject *args)
{
  return PyJudyLO_riter_new((PyJudyL *)container, "riterkeys",
			   args, &PyJudyLORIterKey_Type);
}
static PyObject *
PyJudyLO_ritervalues(PyObject *container, PyObject *args)
{
  return PyJudyLO_riter_new((PyJudyL *)container, "ritervalues",
			   args, &PyJudyLORIterValue_Type);
}
static PyObject *
PyJudyLO_riteritems(PyObject *container, PyObject *args)
{
  return PyJudyLO_riter_new((PyJudyL *)container, "riteritems",
			   args, &PyJudyLORIterItem_Type);
}


/* Forward declarations for iteration over the empty bits */

extern PyTypeObject PyJudyLOEmptyIter_Type;
extern PyTypeObject PyJudyLOEmptyRIter_Type;

static PyObject *PyJudyLO_empty_iter(PyObject *container, PyObject *args)
{
  return PyJudyLO_iter_new((PyJudyL *)container, "empty_iter",
			  args, &PyJudyLOEmptyIter_Type);
}
static PyObject *PyJudyLO_empty_riter(PyObject *container, PyObject *args)
{
  return PyJudyLO_riter_new((PyJudyL *)container, "empty_iter",
			  args, &PyJudyLOEmptyRIter_Type);
}

static PyObject *
PyJudyLO_make_2ple(Word_t index, PPvoid_t p)
{
  PyObject *tuple;
  PyObject *key;
  PyObject *value;

  tuple = PyTuple_New(2);
  if (tuple == NULL)
    return NULL;

  key = PyInt_FromLong(index);
  if (key == NULL) {
    Py_DECREF(tuple);
    return NULL;
  }

  value = FROM_WORD((*p));
  if (value == NULL) {
    Py_DECREF(key);
    Py_DECREF(tuple);
    return NULL;
  }
  PyTuple_SET_ITEM(tuple, 0, key);
  PyTuple_SET_ITEM(tuple, 1, value);
  return tuple;
}


/**** PyJudyL type ****/


static void
PyJudyLO_dealloc(PyJudyL* self)
{
  FREE_ELEMENTS((self->array));
  JudyLFreeArray( &(self->array), PJE0);
  PyObject_Del((PyObject *)self);
}

static PyObject *
PyJudyLO_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
  PyJudyL *self;
  static char *kwlist[] = {0};
  /* What's the right way to check for no arguments? */
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "", kwlist))
    return NULL;

  self = (PyJudyL *)PyObject_New(PyJudyL, type);
  if (self == NULL)
    return NULL;

  self->array = NULL;
  return (PyObject *)self;
}

/* standard Judy methods */

static PyObject *
PyJudyLO_MemUsed(PyJudyL *self)
{
  Word_t size;
  size = JudyLMemUsed(self->array);
  return as_unsigned(size);
}

static PyObject *
PyJudyLO_Ins(PyJudyL *self, PyObject *args)
{
  PyObject *index_ref, *value_ref;

  Word_t index, word;  /* These are unsigned */
  PPvoid_t p;

  if (!PyArg_UnpackTuple(args, "Ins", 2, 2, &index_ref, &value_ref))
    return 0;

  index = PyInt_AsLong(index_ref);
  if (index == -1 && PyErr_Occurred())
    return NULL;

  TO_WORD(word, value_ref, NULL);
  p = JudyLIns(&(self->array), index, PJE0);
  *((Word_t *) p) = word;

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
PyJudyLO_Del(PyJudyL *self, PyObject *args)
{
  Word_t index;
  long success;
  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  success = JudyLDel(&(self->array), index, PJE0);
  return PyInt_FromLong(success);
}

static PyObject *
PyJudyLO_Get(PyJudyL *self, PyObject *args)
{
  Word_t index;
  PPvoid_t p;
  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  p = JudyLGet(self->array, index, PJE0);
  if (p == NULL) {
    /* Not present; map that to None */
    Py_INCREF(Py_None);
    return Py_None;
  }
  return FROM_WORD((*p));
}

static PyObject *
PyJudyLO_Count(PyJudyL *self, PyObject *args)
{
  Word_t count;
  Word_t start = 0, end = -1;
  /* XXX Need to allow values >= LONG_MAX */
  if (!PyArg_ParseTuple(args, "|ii", &start, &end)) {
    return NULL;
  }
  count = JudyLCount(self->array, start, end, PJE0);
  return as_unsigned(count);
}

static PyObject *
PyJudyLO_ByCount(PyJudyL *self, PyObject *args)
{
  PPvoid_t p;
  Word_t index;
  Word_t Nth;
  /* XXX Need to allow Nth >= LONG_MAX */
  if (!PyArg_ParseTuple(args, "i", &Nth)) {
    return NULL;
  }
  p = JudyLByCount(self->array, Nth, &index, PJE0);
  if (p == NULL) {
    /* Not present; map that to None */
    Py_INCREF(Py_None);
    return Py_None;
  }
  /* Return the 2-ple of (index, value) */
  return PyJudyLO_make_2ple(index, p);
}

static PyObject *
PyJudyLO_FreeArray(PyJudyL *self)
{
  FREE_ELEMENTS(self->array);
  JudyLFreeArray( &(self->array), PJE0);
  /* not returning the size */
  Py_INCREF(Py_None);
  return Py_None;
}

/* ******** */

static PyObject *
PyJudyLO_First(PyJudyL *self, PyObject *args)
{
  Word_t index = 0;
  PPvoid_t p;

  if (!PyArg_ParseTuple(args, "|i", &index)) {
    return NULL;
  }
  p = JudyLFirst(self->array, &index, PJE0);
  if (p == NULL) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyJudyLO_make_2ple(index, p);
}

static PyObject *
PyJudyLO_Next(PyJudyL *self, PyObject *args)
{
  Word_t index;
  PPvoid_t p;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  p = JudyLNext(self->array, &index, PJE0);
  if (p == NULL) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  return PyJudyLO_make_2ple(index, p);
}

static PyObject *
PyJudyLO_Last(PyJudyL *self, PyObject *args)
{
  Word_t index = -1;
  PPvoid_t p;

  if (!PyArg_ParseTuple(args, "|i", &index)) {
    return NULL;
  }
  p = JudyLLast(self->array, &index, PJE0);
  if (p == NULL) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyJudyLO_make_2ple(index, p);
}

static PyObject *
PyJudyLO_Prev(PyJudyL *self, PyObject *args)
{
  Word_t index;
  PPvoid_t p;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  p = JudyLPrev(self->array, &index, PJE0);
  if (p == NULL) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyJudyLO_make_2ple(index, p);
}


/* ******** */

static PyObject *
PyJudyLO_FirstEmpty(PyJudyL *self, PyObject *args)
{
  Word_t index = 0;
  int rc;

  if (!PyArg_ParseTuple(args, "|i", &index)) {
    return NULL;
  }
  rc = JudyLFirstEmpty(self->array, &index, PJE0);
  if (!rc) {
    /* Nothing found - XXX raise exception? */
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyInt_FromLong(index);
}

static PyObject *
PyJudyLO_NextEmpty(PyJudyL *self, PyObject *args)
{
  Word_t index;
  int rc;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  rc = JudyLNextEmpty(self->array, &index, PJE0);
  if (!rc) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyInt_FromLong(index);
}

static PyObject *
PyJudyLO_LastEmpty(PyJudyL *self, PyObject *args)
{
  Word_t index = -1;
  int rc;

  if (!PyArg_ParseTuple(args, "|i", &index)) {
    return NULL;
  }
  rc = JudyLLastEmpty(self->array, &index, PJE0);
  if (!rc) {
    /* Nothing found - XXX raise exception? */
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyInt_FromLong(index);
}

static PyObject *
PyJudyLO_PrevEmpty(PyJudyL *self, PyObject *args)
{
  Word_t index;
  int rc;

  if (!PyArg_ParseTuple(args, "i", &index)) {
    return NULL;
  }
  rc = JudyLPrevEmpty(self->array, &index, PJE0);
  if (!rc) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  return PyInt_FromLong(index);
}

/* Python object adapters */

static PyObject *
PyJudyLO_keys(PyJudyL *self)
{
  PyObject *v;
  Word_t n, Index;
  PPvoid_t p;
  int i=0;

  n = JudyLCount(self->array, 0, -1, PJE0);
  v = PyList_New(n);
  Index = 0;
  p = JudyLFirst(self->array, &Index, PJE0);
  while (i<n) {
    if (!p) {
      PyErr_SetString(PyExc_SystemError,
		      "JudyL size changed while creating list of keys");
      Py_DECREF(v);
      return NULL;
    }
    PyList_SET_ITEM(v, i, PyInt_FromLong(Index) );
    i++;
    p = JudyLNext(self->array, &Index, PJE0);
  }
  return v;
}

static PyObject *
PyJudyLO_values(PyJudyL *self)
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
    PyList_SET_ITEM(v, i, FROM_WORD((*p)));
    i++;
    p = JudyLNext(self->array, &index, PJE0);
  }
  return v;
}

static PyObject *
PyJudyLO_items(PyJudyL *self)
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
    tuple = PyJudyLO_make_2ple(index, p);
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

static PyObject *
PyJudyLO_contains(PyJudyL *self, PyObject *key)
{
  long index;
  Pvoid_t p;

  index = PyInt_AsLong(key);
  if (index == -1 && PyErr_Occurred())
    return NULL;

  p = JudyLGet(self->array, (Word_t) index, PJE0);  
  if (!p) {
    Py_INCREF(Py_False);
    return Py_False;
  } else {
    Py_INCREF(Py_True);
    return Py_True;
  }
}

/* Meet the mapping protocol */
static int
PyJudyLO_length(PyJudyL* self)
{
  /* XXX what if count > 2**31? */
  return JudyLCount(self->array, 0, -1, PJE0);
}

static PyObject *
PyJudyLO_subscript(PyJudyL* self, PyObject *key)
{
  long index;
  PPvoid_t p;
  index = PyInt_AsLong(key);
  if (index == -1 && PyErr_Occurred()) {
    return NULL;
  }
  p = JudyLGet(self->array, (Word_t) index, PJE0);
  if (!p) {
    PyErr_SetObject(PyExc_KeyError, key);
    return NULL;
  }
  return FROM_WORD((*p));
}

static int
PyJudyLO_ass_sub(PyJudyL* self, PyObject *key, PyObject *value)
{
  if (!value) {
    /* Delete */
    long index;
    long success;

    index = PyInt_AsLong(key);
    if (index == -1 && PyErr_Occurred()) {
      return -1;
    }
    FREE_ELEMENT((self->array), index, PJE0);
    success = JudyLDel(&(self->array), (Word_t) index, PJE0);
    if (!success) {
      PyErr_SetObject(PyExc_KeyError, key);
      return -1;
    }
    return 0;
      
  } else {
    /* Insert */
    long index;
    Word_t word;
    PPvoid_t p;

    index = PyInt_AsLong(key);
    if (index == -1 && PyErr_Occurred()) {
      return -1;
    }
    TO_WORD(word, value, -1);
    p = JudyLIns(&(self->array), (Word_t) index, PJE0);
    /* XXX Check for error */
    *((Word_t *) p) = word;
    return 0;
  }
}


static PyMappingMethods PyJudyLO_as_mapping = {
  (inquiry) PyJudyLO_length,
  (binaryfunc) PyJudyLO_subscript,
  (objobjargproc)PyJudyLO_ass_sub,
};



static PyMethodDef PyJudyLO_methods[] = {
  {"MemUsed", (PyCFunction) PyJudyLO_MemUsed, METH_NOARGS, "memory used"},
  {"Ins", (PyCFunction) PyJudyLO_Ins, METH_VARARGS, "Insert"},
  {"Del", (PyCFunction) PyJudyLO_Del, METH_VARARGS, "Delete"},
  {"Get", (PyCFunction) PyJudyLO_Get, METH_VARARGS, "Get"},
  {"Count", (PyCFunction) PyJudyLO_Count, METH_VARARGS, "Count"},
  {"ByCount", (PyCFunction) PyJudyLO_ByCount, METH_VARARGS, "ByCount"},
  {"FreeArray", (PyCFunction) PyJudyLO_FreeArray, METH_NOARGS, "FreeArray"},

  {"First", (PyCFunction) PyJudyLO_First, METH_VARARGS, "First"},
  {"Next", (PyCFunction) PyJudyLO_Next, METH_VARARGS, "Next"},
  {"Last", (PyCFunction) PyJudyLO_Last, METH_VARARGS, "Last"},
  {"Prev", (PyCFunction) PyJudyLO_Prev, METH_VARARGS, "Prev"},

  {"FirstEmpty", (PyCFunction) PyJudyLO_FirstEmpty, METH_VARARGS, "FirstEmpty"},
  {"NextEmpty", (PyCFunction) PyJudyLO_NextEmpty, METH_VARARGS, "NextEmpty"},
  {"LastEmpty", (PyCFunction) PyJudyLO_LastEmpty, METH_VARARGS, "LastEmpty"},
  {"PrevEmpty", (PyCFunction) PyJudyLO_PrevEmpty, METH_VARARGS, "PrevEmpty"},

  {"keys", (PyCFunction) PyJudyLO_keys, METH_NOARGS, "keys"},
  {"values", (PyCFunction) PyJudyLO_values, METH_NOARGS, "values"},
  {"items", (PyCFunction) PyJudyLO_items, METH_NOARGS, "items"},
  {"__contains__", (PyCFunction) PyJudyLO_contains, METH_O, "__contains__"},

  {"iterkeys", (PyCFunction) PyJudyLO_iterkeys, METH_VARARGS, "iterkeys"},
  {"itervalues", (PyCFunction) PyJudyLO_itervalues, METH_VARARGS, "itervalues"},
  {"iteritems", (PyCFunction) PyJudyLO_iteritems, METH_VARARGS, "iteritems"},

  {"riterkeys", (PyCFunction) PyJudyLO_riterkeys, METH_VARARGS, "riterkeys"},
  {"ritervalues", (PyCFunction) PyJudyLO_ritervalues, METH_VARARGS, "ritervalues"},
  {"riteritems", (PyCFunction) PyJudyLO_riteritems, METH_VARARGS, "riteritems"},
  {"__reversed__", (PyCFunction) PyJudyLO_riter, METH_NOARGS, "__reversed__"},

  {"empty_iter", (PyCFunction) PyJudyLO_empty_iter, METH_VARARGS, "empty_iter"},
  {"empty_riter", (PyCFunction) PyJudyLO_empty_riter, METH_VARARGS, "empty_riter"},
  
  {NULL}
};

/* JudyL */
static PyTypeObject PyJudyLOType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyjudy.JudyL",             /*tp_name*/
    sizeof(PyJudyL), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyJudyLO_dealloc,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &PyJudyLO_as_mapping,                         /*tp_as_mapping*/
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
    PyJudyLO_iter,                       /* tp_iter */
    0,                         /* tp_iternext */
    PyJudyLO_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0, /*(initproc)PyJudyLO_init,*/      /* tp_init */
    0,                         /* tp_alloc */
    PyJudyLO_new,                 /* tp_new */

};



/******* forward iteration *******/


typedef struct {
  PyObject_HEAD
  short found_first;
  short has_limit;
  PyObject *result;  /* reusable result tuple for iteritems */
  Word_t index;
  Word_t stop;
  PyJudyL *container;   /* Set to NULL when iterator is exhausted */
} PyJudyLO_iterobject;

static PyObject *
PyJudyLO_iter_new(PyJudyL *container, char *name,
		 PyObject *args, PyTypeObject *itertype)
{
  PyJudyLO_iterobject *it;
  Word_t start=0, stop=-1;
  short has_limit=0;

  if (!parse_start_stop(args, name, &start, &stop, &has_limit))
    return NULL;

  it = PyObject_New(PyJudyLO_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->found_first = 0;
  it->index = start;
  it->stop = stop;
  it->has_limit = has_limit;

  Py_INCREF(container);
  it->container = container;

  if (itertype == &PyJudyLOIterItem_Type) {
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
PyJudyLO_iter_dealloc(PyJudyLO_iterobject *it)
{
  Py_XDECREF(it->container);
  Py_XDECREF(it->result);
  PyObject_Del(it);
}

/***************************/

static PyObject *PyJudyLOIter_iternextkey(PyJudyLO_iterobject *it)
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
  return PyInt_FromLong(it->index);
}

PyTypeObject PyJudyLOIterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-keyiterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLOIter_iternextkey,    /* tp_iternext */
};


static PyObject *PyJudyLOIter_iternextvalue(PyJudyLO_iterobject *it)
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

  return FROM_WORD((*p));
}

PyTypeObject PyJudyLOIterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-valueiterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLOIter_iternextvalue,    /* tp_iternext */
};




static PyObject *PyJudyLOIter_iternextitem(PyJudyLO_iterobject *it)
{
  PyObject *key, *value, *result;
  PPvoid_t p;
  
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
  key = PyInt_FromLong(it->index);
  if (!key)
    return NULL;
  value = FROM_WORD((*p));
  if (!value) {
    Py_DECREF(key);
    return NULL;
  }

  PyTuple_SET_ITEM(result, 0, key);
  PyTuple_SET_ITEM(result, 1, value);
  return result;
}

PyTypeObject PyJudyLOIterItem_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-itemiterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLOIter_iternextitem,    /* tp_iternext */
};

/******* reverse iteration *******/

static PyObject *
PyJudyLO_riter_new(PyJudyL *container, char *name,
		 PyObject *args, PyTypeObject *itertype)
{
  PyJudyLO_iterobject *it;
  Word_t start=-1, stop=0;
  short has_limit=0;

  if (!parse_start_stop(args, name, &start, &stop, &has_limit))
    return NULL;

  it = PyObject_New(PyJudyLO_iterobject, itertype);
  if (it == NULL)
    return NULL;

  it->found_first = 0;
  it->index = start;
  it->stop = stop;
  it->has_limit = has_limit;

  Py_INCREF(container);
  it->container = container;

  if (itertype == &PyJudyLORIterItem_Type) {
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

static PyObject *PyJudyLORIter_iternextkey(PyJudyLO_iterobject *it)
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
  /* QQQ return */
  return PyInt_FromLong(it->index);
}

PyTypeObject PyJudyLORIterKey_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-keyriterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLORIter_iternextkey,    /* tp_iternext */
};


static PyObject *PyJudyLORIter_iternextvalue(PyJudyLO_iterobject *it)
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
  return FROM_WORD((*p));
}

PyTypeObject PyJudyLORIterValue_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-valueiterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLORIter_iternextvalue,    /* tp_iternext */
};




static PyObject *PyJudyLORIter_iternextitem(PyJudyLO_iterobject *it)
{
  PyObject *key, *value, *result;
  PPvoid_t p;
  
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
  key = PyInt_FromLong(it->index);
  if (!key)
    return NULL;

  value = FROM_WORD((*p));
  if (!value) {
    Py_DECREF(key);
    return NULL;
  }

  PyTuple_SET_ITEM(result, 0, key);
  PyTuple_SET_ITEM(result, 1, value);
  return result;
}

PyTypeObject PyJudyLORIterItem_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-itemriterator",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLORIter_iternextitem,    /* tp_iternext */
};

/***** Forward iteration over the empty keys *****/

static PyObject *PyJudyLO_empty_iter_next(PyJudyLO_iterobject *it)
{
  int rc;
  
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
  return PyInt_FromLong(it->index);
}

PyTypeObject PyJudyLOEmptyIter_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-emptyiter",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLO_empty_iter_next,    /* tp_iternext */
};


/***** Reverse iteration over the empty keys *****/

static PyObject *PyJudyLO_empty_riter_next(PyJudyLO_iterobject *it)
{
  int rc;
  
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
  return PyInt_FromLong(it->index);
}

PyTypeObject PyJudyLOEmptyRIter_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                      /* ob_size */
        "pyjudyl-remptyiter",              /* tp_name */
        sizeof(PyJudyLO_iterobject),                 /* tp_basicsize */
        0,                                      /* tp_itemsize */
        /* methods */
        (destructor)PyJudyLO_iter_dealloc,           /* tp_dealloc */
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
        (iternextfunc)PyJudyLO_empty_riter_next,    /* tp_iternext */
};




#include "pyjudyl_undef.h"
