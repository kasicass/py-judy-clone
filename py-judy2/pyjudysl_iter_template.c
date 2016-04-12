/* Forward declarations for forward iteration */

extern PyTypeObject <P>IterKey_Type;
extern PyTypeObject <P>IterValue_Type;
extern PyTypeObject <P>IterItem_Type;
static PyObject *<P>_iter_new(<CT> *, const char *,
			      PyObject *start, PyObject *end,
                              PyTypeObject *);

static PyObject *
<P>_iter(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iter",
		      NULL, NULL, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iterkeys",
		      NULL, NULL, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys_from(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "iterkeys_from",
		      key, NULL, &<P>IterKey_Type);
}
static PyObject *
<P>_iterkeys_to(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "iterkeys_to",
		      NULL, key, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys_range(PyObject *container, PyObject *args)
{
  PyObject *start, *end;
  if (!PyArg_UnpackTuple(args, "iterkeys_range", 2, 2, &start, &end))
    return NULL;
  return <P>_iter_new((<CT> *)container, "iterkey_range",
		      start, end, &<P>IterKey_Type);
}

/**** itervalues ****/

static PyObject *
<P>_itervalues(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "itervalues",
		      NULL, NULL, &<P>IterValue_Type);
}
static PyObject *
<P>_itervalues_from(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "itervalues_to",
		      key, NULL, &<P>IterValue_Type);

}
static PyObject *
<P>_itervalues_to(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "itervalues_to",
		      NULL, key, &<P>IterValue_Type);

}
static PyObject *
<P>_itervalues_range(PyObject *container, PyObject *args)
{
  PyObject *start, *end;
  if (!PyArg_UnpackTuple(args, "iterkeys_range", 2, 2, &start, &end))
    return NULL;
  return <P>_iter_new((<CT> *)container, "itervalues_range",
		      start, end, &<P>IterValue_Type);

}

/********* iteritems ***********/

static PyObject *
<P>_iteritems(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iteritems",
		      NULL, NULL, &<P>IterItem_Type);
}
static PyObject *
<P>_iteritems_from(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "iteritems_to",
		      key, NULL, &<P>IterItem_Type);

}
static PyObject *
<P>_iteritems_to(PyObject *container, PyObject *key)
{
  return <P>_iter_new((<CT> *)container, "iteritems_to",
		      NULL, key, &<P>IterItem_Type);

}
static PyObject *
<P>_iteritems_range(PyObject *container, PyObject *args)
{
  PyObject *start, *end;
  if (!PyArg_UnpackTuple(args, "iterkeys_range", 2, 2, &start, &end))
    return NULL;
  return <P>_iter_new((<CT> *)container, "iteritems_range",
		      start, end, &<P>IterItem_Type);

}
