/* Forward declarations for forward iteration */

static PyObject *<P>_parse_range(PyObject *args, char *name,
				 Word_t *start_index, Word_t *end_index);
extern PyTypeObject <P>IterKey_Type;
static PyObject *<P>_iter_new(<CT> *, const char *,
			      Word_t start, Word_t end, int has_limit,
                              PyTypeObject *);

static PyObject *
<P>_iter(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iter",
		      START, END, 0, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iterkeys",
		      START, END, 0, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys_from(PyObject *container, PyObject *key)
{
  Word_t index;
  
  PY_TO_INDEX;
  return <P>_iter_new((<CT> *)container, "iterkeys_from",
		      index, END, 0, &<P>IterKey_Type);
}
static PyObject *
<P>_iterkeys_to(PyObject *container, PyObject *key)
{
  Word_t index;
  
  PY_TO_INDEX;
  return <P>_iter_new((<CT> *)container, "iterkeys_to",
		      START, index, 1, &<P>IterKey_Type);
}

static PyObject *
<P>_iterkeys_range(PyObject *container, PyObject *args)
{
  Word_t start, end;
  if (!<P>_parse_range(args, "iterkeys_range", &start, &end))
    return NULL;
  return <P>_iter_new((<CT> *)container, "iterkey_range",
		      start, end, 1, &<P>IterKey_Type);
}

/**** itervalues, iteritems ****/

/* It would be silly to have this for boolean values since they   */
/* would all be 1.  I could implement such thing if there's       */
/* enough demand, or you could contribute a patch if you want it. */


/* Forward empty iteration */
extern PyTypeObject <P>IterEmpty_Type;

static PyObject *<P>_iterempty(PyObject *container)
{
  return <P>_iter_new((<CT> *)container, "iterempty",
		      START, END, 0, &<P>IterEmpty_Type);
}
static PyObject *<P>_iterempty_from(PyObject *container, PyObject *key)
{
  Word_t index;
  PY_TO_INDEX;
  return <P>_iter_new((<CT> *)container, "iterempty_from",
		      index, END, 0, &<P>IterEmpty_Type);
}
static PyObject *<P>_iterempty_to(PyObject *container, PyObject *key)
{
  Word_t index;
  PY_TO_INDEX;
  return <P>_iter_new((<CT> *)container, "iterempty_to",
		      START, index, 1, &<P>IterEmpty_Type);
}
static PyObject *<P>_iterempty_range(PyObject *container, PyObject *args)
{
  Word_t start, end;
  if (!<P>_parse_range(args, "iterempty_range", &start, &end))
    return NULL;

  return <P>_iter_new((<CT> *)container, "iterempty_range",
		      start, end, 1, &<P>IterEmpty_Type);
}
