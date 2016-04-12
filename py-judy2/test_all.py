import sys
import pyjudy
import struct

MAX = struct.unpack("I", struct.pack("i",-1))[0]

def TRACE():
    return
    f = sys._getframe(2)
    print "Lineno", f.f_lineno

def EQ(x, y):
    TRACE()
    if not (x == y):
        raise AssertionError(x, y)

def EQL(x, y):
    TRACE()
    x = list(x)
    if not (x == y):
        raise AssertionError(x, y)


def IS(x, y):
    TRACE()
    if not (x is y):
        raise AssertionError(x, y)

def NE(x, y):
    TRACE()
    if not (x != y):
        raise AssertionError(x, y)

def must_stop_iteration(f, *args):
    try:
        f(*args)
    except StopIteration:
        pass
    else:
        raise AssertionError( (f, args) )
def must_raise_index_error(f, *args):
    try:
        f(*args)
    except IndexError:
        pass
    else:
        raise AssertionError( (f, args) )
def must_raise_type_error(f, *args):
    try:
        f(*args)
    except TypeError:
        pass
    else:
        raise AssertionError( (f, args) )

def must_not_have_key(arr, key):
    try:
        arr[key]
    except KeyError:
        return
    raise AssertionError( (arr, key) )

def to_unsigned_int(x):
    return struct.unpack("I", struct.pack("i", x))[0]

def to_signed(x):
    return struct.unpack("i", struct.pack("I", x))[0]

def to_unsigned_id(x):
    return struct.unpack("I", struct.pack("i", id(x)))[0]


# Check that forward and reverse iterators work as expected
def check_iterators(arr,
                    k01, k1, k12, k2, k23, k3, k34,
                    v1, v2, v3):
    L = {"k01": k01,
         "k1": k1,
         "k12": k12,
         "k2": k2,
         "k23": k23,
         "k3": k3,
         "k34": k34,
         "v1": v1,
         "v2": v2,
         "v3": v3,}

    # Check forward iteration
    for (suffix, args, digits) in (
        ("", (), "123"),
        ("_from", (k01,), "123"),
        ("_from", (k1,), "123"),
        ("_from", (k12,), "23"),
        ("_from", (k2,), "23"),
        ("_from", (k23,), "3"),
        ("_from", (k3,), "3"),
        ("_from", (k34,), ""),

        ("_range", (k01, k34), "123"),
        ("_range", (k01, k3), "12"),
        ("_range", (k1, k3), "12"),
        ("_range", (k1, k23), "12"),
        ("_range", (k12, k2), ""),
        ("_range", (k2, k2), ""),
        ("_range", (k3, k34), "3"),

        ("_to", (k01,), ""),
        ("_to", (k1,), ""),
        ("_to", (k12,), "1"),
        ("_to", (k2,), "1"),
        ("_to", (k23,), "12"),
        ("_to", (k3,), "12"),
        ("_to", (k34,), "123"),
        ):
        expect_keys = [L["k"+digit] for digit in digits]
        expect_values = [L["v"+digit] for digit in digits]
        expect_items = zip(expect_keys, expect_values)
        for (prefix, expect) in (
            ("iterkeys", expect_keys),
            ("itervalues", expect_values),
            ("iteritems", expect_items)
            ):
            it = getattr(arr, prefix + suffix)(*args)
            it.next # quick check that it's an iterator
            got = list(it)
            EQ(got, expect)
    # Check forward iteration
    it = iter(arr)
    it.next
    got = list(it)
    EQ(got, [k1, k2, k3])
        

    # Now check the reverse iterators
    for (suffix, args, digits) in (
        ("", (), "321"),

        ("_from", (k01,), ""),
        ("_from", (k1,), "1"),
        ("_from", (k12,), "1"),
        ("_from", (k2,), "21"),
        ("_from", (k23,), "21"),
        ("_from", (k3,), "321"),
        ("_from", (k34,), "321"),

        ("_range", (k34, k01), "321"),
        ("_range", (k3, k01), "321"),
        ("_range", (k3, k1), "32"),
        ("_range", (k23, k1), "2"),
        ("_range", (k2, k12), "2"),
        ("_range", (k2, k2), ""),
        ("_range", (k34, k3), ""),

        ("_to", (k01,), "321"),
        ("_to", (k1,), "32"),
        ("_to", (k12,), "32"),
        ("_to", (k2,), "3"),
        ("_to", (k23,), "3"),
        ("_to", (k3,), ""),
        ("_to", (k34,), ""),
        ):
        expect_keys = [L["k"+digit] for digit in digits]
        expect_values = [L["v"+digit] for digit in digits]
        expect_items = zip(expect_keys, expect_values)
        for (prefix, expect) in (
            ("riterkeys", expect_keys),
            ("ritervalues", expect_values),
            ("riteritems", expect_items)
            ):
            it = getattr(arr, prefix + suffix)(*args)
            if not hasattr(it, "next"): raise AssertionError
            got = list(it)
            EQ(got, expect)
    # Check reverse iteration
    it = arr.__reversed__()
    it.next
    got = list(it)
    EQ(got, [k3, k2, k1])




def test_judyl_mem(klass, key = 500, value = 700):
    count = sys.getrefcount(klass)
    for i in range(100):
        klass()
    EQ(count, sys.getrefcount(klass))

    # Make sure the key and item are refcounted correctly
    # when doing inserts

    arr = klass()
    arr.Ins(key, value)

    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr.Ins(key, value)
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Make sure the key and item are refcounted correctly
    # when doing setitem[]
    for i in range(100):
        arr[key] = value
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Check refcounts on insert + delete
    arr = klass()
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr.Ins(key, value)
        success = arr.Del(key)
        if success != 1:
            raise AssertionError, (key, value, i,success)
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Check refcounts on delitem
    arr = klass()
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr[key] = value
        del arr[key]
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))


    # Check refcounts on array deletion
    del arr
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr = klass()
        arr.Ins(key, value)
        del arr
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Check refcounts on FreeArray
    arr = klass()
    for i in range(100):
        arr.Ins(key, value)
        arr.FreeArray()
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Check refcounts on iteration
    arr[key] = value
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr.keys()
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.iterkeys())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.values())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.itervalues())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.items())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.iteritems())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for x in arr:
        pass
    del x
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    if i in range(100):
        list(arr.__reversed__())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.riterkeys())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.ritervalues())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.riteritems())
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.riterempty_range(0, 50))
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        list(arr.riterempty_range(0, 50))
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))


    # Am I really deallocing the parameters correctly?
    a = 1000
    b = 2000
    a_count = sys.getrefcount(a)
    b_count = sys.getrefcount(b)
    for i in range(100):
        list(arr.iterkeys_range(a, b))
        list(arr.itervalues_range(a, b))
        list(arr.iteritems_range(a, b))
        list(arr.riterkeys_range(a, b))
        list(arr.ritervalues_range(a, b))
        list(arr.riteritems_range(a, b))
    EQ(a_count, sys.getrefcount(a))
    EQ(b_count, sys.getrefcount(b))

def test_judyl_mem_obj_value(klass, key = 500):
    # Check it with something where I'll know the exact
    # number of ref counts.  Unless you're using a debugger.  :)
    arr = klass()
    class Spam:
        pass
    EQ(sys.getrefcount(Spam), 2)
    arr[key] = Spam
    EQ(sys.getrefcount(Spam), 3)
    del arr[key]
    EQ(sys.getrefcount(Spam), 2)
    

# Passing 7 keys and 3 values
def test_judyl_access(klass, keys, values, to_unsigned):
    _keys = [(to_unsigned(k), k) for k in keys]
    _keys.sort()
    k01, k1, k12, k2, k23, k3, k34 = all_keys = [x[1] for x in _keys]
    unsigned_keys = [to_unsigned(k) for k in (k1, k2, k3)]

    v1, v2, v3 = values

    # Test cleanup -- use numbers > 100 because -2 through 100 are
    # singletons and are not gc'ed.
    # Integers are objects so this works for any JudyL
    jl = klass()
    for i in range(400, 405):
        jl.Ins(i, i+1)
    del jl
    
    for i in range(400, 500):
        jl = klass()
        jl.Ins(i, i+1)
        del jl
    for i in range(100):
        jl = klass()
        jl.Ins(k01, v1)
        jl.Ins(k1, v1)
        jl.Ins(k12, v1)
        jl.Ins(k2, v2)
        jl.Ins(k23, v2)
        jl.Ins(k3, v3)
        jl.Ins(k34, v3)

    jl = klass()
    EQ(jl.MemUsed(), 0)
    EQ(jl.Count(), 0)
    must_raise_index_error(jl.Get, k1)
    IS(jl.get(k1), None)
    IS(jl.Ins(k1, v1), None)
    NE(jl.MemUsed(), 0)
    EQ(jl.Count(), 1)
    EQ(jl.Get(k1), v1)
    EQ(jl.get(k1), v1)
    must_raise_index_error(jl.Get, k2)
    IS(jl.get(k2), None)
    q = "Hi!"
    IS(jl.get(k2, q), q)

    jl = klass()
    jl.Ins(k1, v1)
    jl.Ins(k2, v2)
    EQ(jl.Get(k2), v2)

    must_raise_index_error(jl.ByCount, 0)
    EQ(jl.ByCount(1), (k1, v1))
    EQ(jl.ByCount(2), (k2, v2))
    must_raise_index_error(jl.ByCount, 3)

    IS(jl.Del(k1), True)
    IS(jl.Del(k2), True)
    EQ(jl.MemUsed(), 0)
    EQ(jl.Count(), 0)
    must_raise_index_error(jl.Get, k1)
    must_raise_index_error(jl.Get, k2)
    IS(jl.Del(k1), False)
    must_raise_index_error(jl.ByCount, 1)

    IS(jl.Ins(k2, v2), None)
    IS(jl.Ins(k1, v1), None)
    EQ(jl.Count(), 2)

    EQ(jl.ByCount(1), (k1, v1))
    EQ(jl.ByCount(2), (k2, v2))
    must_raise_index_error(jl.ByCount, 3)

    IS(jl.FreeArray(), None)
    EQ(jl.Count(), 0)
    jl.Ins(k1, v1)
    EQ(jl.Count(), 1)
    jl.clear()
    EQ(jl.Count(), 0)

    jl = klass()
    jl.Ins(k1, v1)
    jl.Ins(k2, v2)
    jl.Ins(k3, v3)

    EQ(jl.First(), (k1, v1))
    EQ(jl.First(k01), (k1, v1))
    EQ(jl.First(k1), (k1, v1))

    EQ(jl.First(k12), (k2, v2))
    EQ(jl.First(k2), (k2, v2))
    EQ(jl.First(k23), (k3, v3))
    EQ(jl.First(k3), (k3, v3))
    must_stop_iteration(jl.First, k34)

    EQ(jl.Next(k01), (k1, v1))
    EQ(jl.Next(k1), (k2, v2))
    EQ(jl.Next(k12), (k2, v2))
    EQ(jl.Next(k2), (k3, v3))
    EQ(jl.Next(k23), (k3, v3))
    must_stop_iteration(jl.Next, k3)
    must_stop_iteration(jl.Next, k34)

    EQ(jl.Last(), (k3, v3))
    EQ(jl.Last(k34), (k3, v3))
    EQ(jl.Last(k3), (k3, v3))
    EQ(jl.Last(k23), (k2, v2))
    EQ(jl.Last(k2), (k2, v2))
    EQ(jl.Last(k12), (k1, v1))
    EQ(jl.Last(k1), (k1, v1))
    must_stop_iteration(jl.Last, k01)

    EQ(jl.Prev(k34), (k3, v3))
    EQ(jl.Prev(k3), (k2, v2))
    EQ(jl.Prev(k23), (k2, v2))
    EQ(jl.Prev(k2), (k1, v1))
    EQ(jl.Prev(k12), (k1, v1))
    must_stop_iteration(jl.Prev, k1)
    must_stop_iteration(jl.Prev, k01)

    EQ(jl.FirstEmpty(), 0)
    for k in all_keys:
        expect = to_unsigned(k)
        while expect in unsigned_keys:
            expect+=1
        expect = to_signed(expect)
        x = jl.FirstEmpty(k)
        EQ(x, expect)

    for k in all_keys:
        expect = to_unsigned(k)+1
        while expect in unsigned_keys:
            expect+=1
        expect = to_signed(expect)
        x = jl.NextEmpty(k)
        EQ(x, expect)

    EQ(jl.LastEmpty(), -1)
    for k in all_keys:
        expect = to_unsigned(k)
        while expect in unsigned_keys:
            expect-=1
        expect = to_signed(expect)
        x = jl.LastEmpty(k)
        EQ(x, expect)

    for k in all_keys:
        expect = to_unsigned(k)-1
        while expect in unsigned_keys:
            expect-=1
        expect = to_signed(expect)
        x = jl.PrevEmpty(k)
        EQ(x, expect)

    jl = klass()
    must_stop_iteration(jl.First)
    must_stop_iteration(jl.First, k1)
    must_stop_iteration(jl.Next, k1)
    must_stop_iteration(jl.Last)
    must_stop_iteration(jl.Last, k2)
    must_stop_iteration(jl.Prev, k3)


    jl = klass()
    must_not_have_key(jl, k2)
    jl.Ins(k2, v2)
    must_not_have_key(jl, k3)
    jl.Ins(k3, v3)
    must_not_have_key(jl, k1)
    jl.Ins(k1, v1)

    EQ(len(jl), 3)
    EQ(jl[k1], v1)
    EQ(jl[k3], v3)
    EQ(jl[k2], v2)

    jl[k2] = v3
    EQ(jl[k2], v3)
    jl[k2] = v1
    EQ(jl[k1], v1)

    jl[k1] = v3
    EQ(jl[k1], v3)
    jl[k12] = v2
    EQ(len(jl), 4)

    del jl[k12]
    EQ(len(jl), 3)
    try:
        del jl[k12]
    except KeyError:
        pass
    else:
        raise AssertionError("illegal deletion allowed")

    jl = klass()
    jl.Ins(k3, v3)
    jl.Ins(k2, v2)
    jl.Ins(k1, v1)

    EQ(k01 in jl, False)
    EQ(k1 in jl, True)
    EQ(k12 in jl, False)
    EQ(k2 in jl, True)
    EQ(k23 in jl, False)
    EQ(k3 in jl, True)
    EQ(k34 in jl, False)

    # Test key iteration
    for k, expect in zip(jl, [k1, k2, k3]):
        EQ(k, expect)

    EQ(jl.keys(), [k1, k2, k3])
    EQ(jl.values(), [v1, v2, v3])
    EQ(jl.items(), zip(jl.keys(), jl.values()))

    IS(k1 in jl, True)
    IS(k2 in jl, True)
    IS(k3 in jl, True)
    IS(k01 in jl, False)
    IS(k12 in jl, False)
    IS(k23 in jl, False)
    IS(k34 in jl, False)

    EQL(jl.iterkeys(), [k1, k2, k3])
    EQL(jl.values(), [v1, v2, v3])
    EQL(jl.iteritems(), zip([k1, k2, k3], [v1, v2, v3]))


    EQ(jl.Count(), 3)
    EQ(jl.Count_to(k01), 0)
    EQ(jl.Count_to(k1), 0)
    EQ(jl.Count_to(k12), 1)
    EQ(jl.Count_to(k2), 1)
    EQ(jl.Count_to(k23), 2)
    EQ(jl.Count_to(k3), 2)
    EQ(jl.Count_to(k34), 3)
    
    EQ(jl.Count_from(k01), 3)
    EQ(jl.Count_from(k1), 3)
    EQ(jl.Count_from(k12), 2)
    EQ(jl.Count_from(k2), 2)
    EQ(jl.Count_from(k23), 1)
    EQ(jl.Count_from(k3), 1)
    EQ(jl.Count_from(k34), 0)

    EQ(jl.Count_range(k01, k1), 0)
    EQ(jl.Count_range(k01, k12), 1)
    EQ(jl.Count_range(k01, k2), 1)
    EQ(jl.Count_range(k01, k23), 2)
    EQ(jl.Count_range(k01, k34), 3)
    EQ(jl.Count_range(k1, k1), 0)
    EQ(jl.Count_range(k1, k12), 1)
    EQ(jl.Count_range(k1, k2), 1)
    EQ(jl.Count_range(k1, k23), 2)
    EQ(jl.Count_range(k1, k34), 3)
    EQ(jl.Count_range(k12, k1), 0)
    EQ(jl.Count_range(k12, k12), 0)
    EQ(jl.Count_range(k12, k2), 0)
    EQ(jl.Count_range(k12, k23), 1)
    EQ(jl.Count_range(k12, k34), 2)


    check_iterators(jl,
                    k01, k1, k12, k2, k23, k3, k34,
                    v1, v2, v3)

    EQL(jl.__reversed__(), jl.keys()[::-1])


def test_judyl_empty(klass, keys, values, to_unsigned):
    def plus_one(x):
        return to_signed(to_unsigned(x)+1)
    def minus_one(x):
        return to_signed(to_unsigned(x)-1)
    
    _keys = [(to_unsigned(k), k) for k in keys]
    _keys.sort()
    k1, k2, k3 = [x[1] for x in _keys]
    unsigned_keys = [to_unsigned(k) for k in (k1, k2, k3)]

    v1, v2, v3 = values

    jl = klass()
    if to_unsigned(k3) - to_unsigned(k1) < 10000:
        # Forward
        empties = range(to_unsigned(k1), to_unsigned(k3))
        EQL(jl.iterempty_range(k1, k3), empties)
        EQL(jl.iterempty_range(k3, k1), [])

        # Reverse
        empties = range(to_unsigned(k3), to_unsigned(k1), -1)
        EQL(jl.riterempty_range(k1, k3), [])
        EQL(jl.riterempty_range(k3, k1), empties)
    else:
        # At least make sure they do something
        it = jl.iterempty_range(k1, k3)
        it.next()
        it = jl.iterempty_range(k3, k1)
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty")
        it = jl.riterempty_range(k3, k1)
        it.next()
        it = jl.riterempty_range(k1, k3)
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty")
        
        

    jl[k1] = v1
    jl[k2] = v2
    jl[k3] = v3
    if to_unsigned(k3) - to_unsigned(k1) < 10000:
        # Forward
        empties = range(to_unsigned(k1)+1, to_unsigned(k3))
        empties.remove(to_unsigned(k2))
        EQL(jl.iterempty_range(k1, k3), empties)
        EQL(jl.iterempty_range(k3, k1), [])

        # Reverse
        empties = range(to_unsigned(k3)-1, to_unsigned(k1), -1)
        empties.remove(to_unsigned(k2))
        EQL(jl.riterempty_range(k3, k1), empties)
        EQL(jl.riterempty_range(k1, k3), [])

    # I can't test all the values for all classes but I
    # can make sure they are reasonable

    # Make sure that either k1 is not 0 or that k2 is not 1!
    it = jl.iterempty()
    if to_unsigned(k1) == 0:
        EQ(it.next(), 1)
    else:
        EQ(it.next(), 0)

    # Allow if k1 is 0, though in that case k2 must not 1
    it = jl.iterempty_to(k1)
    if to_unsigned(k1) == 0:
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty!")
    else:
        EQ(it.next(), 0)

    # Allow if k3 is MAX
    it = jl.iterempty_from(k3)
    if to_unsigned(k3) == MAX:
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty!")
    else:
        EQ(it.next(), plus_one(k3))


    ### Now, in reverse!

    it = jl.riterempty()
    if to_unsigned(k3) == MAX:
        EQ(it.next(), -2)
    else:
        EQ(it.next(), -1)

    it = jl.riterempty_to(k3)
    if to_unsigned(k3) == MAX:
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty!")
    else:
        EQ(it.next(), -1)

    it = jl.riterempty_from(k1)
    if to_unsigned(k1) == 0:
        try:
            it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError("should be empty!")
    else:
        EQ(it.next(), minus_one(k1))

# Originally I returned None when ByCount returned an error.
# That was the wrong solution because I can have None as a key.
# This tests for the correct behaviour.
def test_judyl_none_key(klass):
    arr = klass()
    must_raise_index_error(arr.ByCount, 0)
    arr[None] = 1
    EQ(arr.ByCount(1), (None, 1))
    EQ(arr.Get(None), 1)
    

def test_judyl():
    class X1: pass
    class X2: pass
    class X3: pass
    class X4: pass
    class X5: pass
    class X6: pass
    class X7: pass
    
    print "Mem test of JudyLIntInt"
    test_judyl_mem(pyjudy.JudyLIntInt)

    print "Mem test of JudyLIntObj"
    test_judyl_mem(pyjudy.JudyLIntObj)
    test_judyl_mem(pyjudy.JudyLIntObj, 654, "Hello?")
    test_judyl_mem_obj_value(pyjudy.JudyLIntObj)

    print "Mem test of JudyLObjObj"
    test_judyl_mem(pyjudy.JudyLObjObj)
    test_judyl_mem(pyjudy.JudyLObjObj, 654, "Hello?")
    test_judyl_mem(pyjudy.JudyLObjObj, "Blah!", "Hello?")
    test_judyl_mem(pyjudy.JudyLObjObj, "Blah!", "Spam.")
    test_judyl_mem_obj_value(pyjudy.JudyLObjObj)

    print "Mem test of JudyLObjInt"
    test_judyl_mem(pyjudy.JudyLObjObj, 654, 856)
    test_judyl_mem(pyjudy.JudyLObjObj, "Blah!", 929)
    
    print "judyl test of JudyLIntInt"
    test_judyl_access(pyjudy.JudyLIntInt,
                      [4, 6, 9, 11, 16, 44, 77], [5, 0, 2],
                      to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntInt,
                      [4, -6, -9, 11, -16, 1230, 14], [-5, 100, 2],
                      to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntInt,
                      [123, -6234, -2349, 23411, -9816, 24534, 4574],
                      [-5, 100, 2], to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntInt,
                      [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
                      [-34235, -100, 2342], to_unsigned_int)

    test_judyl_empty(pyjudy.JudyLIntInt, [6, 11,44], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntInt, [0, 11,-1], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntInt, [1, 11,-2], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntInt, [2, 11,-3], [5, 0, 2],
                     to_unsigned_int)
    
    print "judyl test of JudyLIntObj"
    test_judyl_access(pyjudy.JudyLIntObj,
                      [4, 6, 9, 11, 16, 44, 77], [5, 0, 2],
                      to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntObj,
                      [4, 6, 9, 11, 16, 44, 77], [X1, X2, X3],
                      to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntObj,
                      [4, -6, -9, 11, -16, 1230, 14], [-5, 100, 2],
                      to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntObj,
                      [123, -6234, -2349, 23411, -9816, 24534, 4574],
                      [-5, 100, 2], to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntObj,
                      [123, -6234, -2349, 23411, -9816, 24534, 4574],
                      ["Andrew", "was", "here"], to_unsigned_int)
                      
    test_judyl_access(pyjudy.JudyLIntObj,
               [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
               [-34235, -100, 2342], to_unsigned_int)
    test_judyl_access(pyjudy.JudyLIntObj,
               [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
               [X5, X1, X3], to_unsigned_int)

    test_judyl_empty(pyjudy.JudyLIntObj, [6, 11,44], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntObj, [0, 11,-1], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntObj, [1, 11,-2], [5, 0, 2],
                     to_unsigned_int)
    test_judyl_empty(pyjudy.JudyLIntObj, [2, 11,-3], [5, 0, 2],
                     to_unsigned_int)


    print "judyl test of JudyLObjObj"
    test_judyl_access(pyjudy.JudyLObjObj,
                      [4, 6, 9, 11, 16, 44, 77], [5, 0, 2],
                      to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [X1, X2, X3, X4, X5, X6, X7], [5, 0, 2],
                      to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [None, StopIteration, Exception, object, MAX, True, False],
                      [5, 0, 2],
                      to_unsigned_id)
    
    test_judyl_access(pyjudy.JudyLObjObj,
                      [4, 6, 9, 11, 16, 44, 77], [X1, X2, X3],
                      to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [X5, X2, X3, X1, X6, X7, X4], [X1, X2, X3],
                      to_unsigned_id)

    test_judyl_access(pyjudy.JudyLObjObj,
                      [4, -6, -9, 11, -16, 1230, 14], [-5, 100, 2],
                      to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [123, -6234, -2349, 23411, -9816, 24534, 4574],
                      [-5, 100, 2], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [1.23, -62.34, "-2349", 23411j, -9816L, 24534+3.6j, X1],
                      ["Andrew", "was", "here"], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
                      [-34235, -100, 2342], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjObj,
                      [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
                      [X5, X1, X3], to_unsigned_id)


    test_judyl_empty(pyjudy.JudyLObjObj, [6, 11,44], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjObj, [0, 11,-1], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjObj, [1, 11,-2], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjObj, [2, 11,-3], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt, [X1, X2, X3], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjObj, [None, Exception, StopIteration],
                     [5, 0, 2], to_unsigned_id)

    test_judyl_none_key(pyjudy.JudyLObjObj)

    print "judyl test of JudyLObjInt"
    test_judyl_access(pyjudy.JudyLObjInt,
                      [4, 6, 9, 11, 16, 44, 77], [5, 0, 2], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjInt,
                      [X1, X2, X3, X4, X5, X6, X7], [5, 0, 2], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjInt,
                      [None, StopIteration, Exception, object, MAX, True, False],
                      [5, 0, 2], to_unsigned_id)

    test_judyl_access(pyjudy.JudyLObjInt,
                      [4, -6, -9, 11, -16, 1230, 14], [-5, 100, 2],
                      to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjInt,
                      [123, -6234, -2349, 23411, -9816, 24534, 4574],
                      [-5, 100, 2], to_unsigned_id)
    test_judyl_access(pyjudy.JudyLObjInt,
                      [sys.maxint-1, sys.maxint-5, -2, 352, 23411, -9816, 23],
                      [-34235, -100, 2342], to_unsigned_id)

    test_judyl_empty(pyjudy.JudyLObjInt, [6, 11,44], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt, [0, 11,-1], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt, [1, 11,-2], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt, [2, 11,-3], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt, [X1, X2, X3], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_empty(pyjudy.JudyLObjInt,
                     [None, Exception, StopIteration], [5, 0, 2],
                     to_unsigned_id)
    test_judyl_none_key(pyjudy.JudyLObjInt)

############
def test_judy1_mem(klass, key = 500):
    klass_count = sys.getrefcount(klass)
    for i in range(100):
        klass()
    EQ(klass_count, sys.getrefcount(klass))

    # Make sure the key is refcounted correctly when doing sets
    arr = klass()
    IS(arr.Set(key), True)
    count = sys.getrefcount(key)
    for i in range(100):
        IS(arr.Set(key), False)
    EQ(count, sys.getrefcount(key))

    # Check refcounts on unset
    arr = klass()
    count = sys.getrefcount(key)
    for i in range(100):
        IS(arr.Set(key), True)
        IS(arr.Set(key), False)
        IS(arr.Unset(key), True)
    EQ(count, sys.getrefcount(key))

    # Check refcounts on multiple deletes
    arr = klass()
    count = sys.getrefcount(key)
    for i in range(100):
        arr.Set(key)
        IS(arr.Unset(key), True)
        IS(arr.Unset(key), False)
    EQ(count, sys.getrefcount(key))

    # Check refcounts on FreeArray
    arr = klass()
    count = sys.getrefcount(key)
    for i in range(100):
        arr.Set(key)
        arr.FreeArray()
    EQ(count, sys.getrefcount(key))
    
    # Check refcounts on iteration
    arr = klass()
    arr.Set(key)
    count = sys.getrefcount(key)
    for x in arr:
        # With object keys this is an 'IS'
        EQ(x, key)
    del x
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(arr.keys(), [key])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.iterkeys()), [key])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.iterkeys_from(key)), [key])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.iterkeys_to(key)), [])
    EQ(count, sys.getrefcount(key))

    for i in range(100):
        EQ(list(arr.riterkeys()), [key])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.riterkeys_from(key)), [key])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.riterkeys_to(key)), [])
    EQ(count, sys.getrefcount(key))
    for i in range(100):
        EQ(list(arr.__reversed__()), [key])
    EQ(count, sys.getrefcount(key))


    # Am I really deallocing the parameters correctly?
    arr = klass()
    a = 1000
    b = 2000
    a_count = sys.getrefcount(a)
    b_count = sys.getrefcount(b)
    arr.Set(1100)
    arr.Set(1200)
    for i in range(100):
        list(arr.iterkeys_range(a, b))
        list(arr.iterkeys_range(b, a))
        list(arr.riterkeys_range(a, b))
        list(arr.riterkeys_range(b, a))
    EQ(a_count, sys.getrefcount(a))
    EQ(b_count, sys.getrefcount(b))
    
def test_judy1_access(klass, k1, k2, k3, to_unsigned):
    keys = [(to_unsigned(k), k) for k in (k1, k2, k3)]
    keys.sort()
    k1, k2, k3 = [k[1] for k in keys]
    
    arr = klass()
    EQ(arr.MemUsed(), 0)
    EQ(arr.Count(), 0)
    EQ(arr.Test(k1), False)
    EQ(arr.Set(k1), True)
    EQ(arr.Count(), 1)
    EQ(arr.Test(k1), True)
    EQ(arr.Count(), 1)
    EQ(len(arr), 1)

    EQ(arr.Test(k2), False)
    EQ(arr.Test(k1), True)
    EQ(arr.Set(k2), True)
    NE(arr.MemUsed(), 0)

    EQ(arr.Count(), 2)
    EQ(len(arr), 2)
    EQ(arr.ByCount(1), k1)
    EQ(arr.ByCount(2), k2)
    try:
        arr.ByCount(3)
    except IndexError:
        pass
    else:
        raise AssertionError("raise?")

    EQ(arr.Set(k3), True)
    EQ(arr.Set(k3), False)
    EQ(arr.ByCount(1), k1)
    EQ(arr.ByCount(2), k2)
    EQ(arr.ByCount(3), k3)
    try:
        arr.ByCount(4)
    except IndexError:
        pass
    else:
        raise AssertionError("raise?")

    EQ(len(arr), 3)
    IS(arr.FreeArray(), None)
    EQ(arr.MemUsed(), 0)
    EQ(len(arr), 0)

    # Ensure 'clear' also works.
    arr.Set(k1)
    EQ(len(arr), 1)
    arr.clear()
    EQ(len(arr), 0)
    
    # Ensure 'add' works.
    arr = klass()
    IS(arr.add(k1), None)
    EQ(len(arr), 1)
    IS(arr.add(k1), None)
    EQ(len(arr), 1)
    IS(arr.add(k2), None)
    EQ(len(arr), 2)

    # Ensure 'remove' works
    arr = klass()
    arr.add(k1)
    arr.add(k2)
    EQ(len(arr), 2)
    arr.remove(k1)
    EQ(len(arr), 1)
    try:
        arr.remove(k1)
    except KeyError:
        pass
    else:
        raise AssertionError
    
    # Count in various ranges
    arr = klass()
    arr.Set(k1)
    EQ(arr.Count_from(k1), 1)
    EQ(arr.Count_to(k1), 0)
    EQ(arr.Count_range(k1, k3), 1)
    EQ(arr.Count_range(k3, k1), 0)
    EQ(arr.Count_range(k2, k3), 0)

    arr.Set(k3)
    EQ(arr.Count_from(k1), 2)
    EQ(arr.Count_to(k1), 0)
    EQ(arr.Count_from(k2), 1)
    EQ(arr.Count_to(k2), 1)
    EQ(arr.Count_from(k3), 1)
    EQ(arr.Count_to(k3), 1)
    EQ(arr.Count_range(k1, k2), 1)
    EQ(arr.Count_range(k2, k1), 0)
    EQ(arr.Count_range(k1, k3), 1)
    EQ(arr.Count_range(k3, k1), 0)

    arr.Set(k2)
    EQ(arr.Count_from(k1), 3)
    EQ(arr.Count_to(k1), 0)
    EQ(arr.Count_from(k2), 2)
    EQ(arr.Count_to(k2), 1)
    EQ(arr.Count_from(k3), 1)
    EQ(arr.Count_to(k3), 2)
    EQ(arr.Count_range(k1, k2), 1)
    EQ(arr.Count_range(k2, k1), 0)
    EQ(arr.Count_range(k1, k3), 2)
    EQ(arr.Count_range(k3, k1), 0)

    # Contains
    arr = klass()
    IS(k1 in arr, False)
    IS(k2 in arr, False)
    IS(k3 in arr, False)
    arr.Set(k3)
    IS(k1 in arr, False)
    IS(k2 in arr, False)
    IS(k3 in arr, True)
    arr.Set(k1)
    IS(k1 in arr, True)
    IS(k2 in arr, False)
    IS(k3 in arr, True)
    arr.Set(k2)
    IS(k1 in arr, True)
    IS(k2 in arr, True)
    IS(k3 in arr, True)

    # Get the keys
    EQ(arr.keys(), [k1, k2, k3])

    # Forward iteration using Judy methods
    EQ(arr.First(), k1)
    EQ(arr.First(k1), k1)
    EQ(arr.Next(k1), k2)
    EQ(arr.First(k2), k2)
    EQ(arr.Next(k2), k3)
    EQ(arr.First(k3), k3)
    try:
        arr.Next(k3)
    except StopIteration:
        pass
    else:
        raise AssertionError

    # Forward iteration using Python methods
    it = arr.iterkeys()
    EQ(it.next(), k1)
    EQ(it.next(), k2)
    EQ(it.next(), k3)
    try:
        it.next()
    except StopIteration:
        pass
    else:
        raise AssertionError
    
    it = iter(arr)
    EQ(it.next(), k1)
    EQ(it.next(), k2)
    EQ(it.next(), k3)
    try:
        it.next()
    except StopIteration:
        pass
    else:
        raise AssertionError

    # Reverse iteration using Judy methods
    EQ(arr.Last(), k3)
    EQ(arr.Last(k3), k3)
    EQ(arr.Prev(k3), k2)
    EQ(arr.Last(k2), k2)
    EQ(arr.Prev(k2), k1)
    EQ(arr.Last(k1), k1)
    try:
        arr.Prev(k1)
    except StopIteration:
        pass
    else:
        raise AssertionError

    # Reverse iteration using Python methods
    it = arr.riterkeys()
    EQ(it.next(), k3)
    EQ(it.next(), k2)
    EQ(it.next(), k1)
    try:
        it.next()
    except StopIteration:
        pass
    else:
        raise AssertionError
    
    it = arr.__reversed__()
    EQ(it.next(), k3)
    EQ(it.next(), k2)
    EQ(it.next(), k1)
    try:
        it.next()
    except StopIteration:
        pass
    else:
        raise AssertionError

    # Range searches; set k1 and k3 but not k2
    arr = klass()
    arr.Set(k1)
    arr.Set(k2)
    arr.Set(k3)
    EQ(arr.Unset(k2), True)
    EQ(arr.Unset(k2), False)
    EQ(len(arr), 2)
    EQL(arr.iterkeys_from(k1), [k1, k3])
    EQL(arr.iterkeys_from(k2), [k3])
    EQL(arr.iterkeys_from(k3), [k3])
    EQL(arr.iterkeys_to(k1), [])
    EQL(arr.iterkeys_to(k2), [k1])
    EQL(arr.iterkeys_to(k3), [k1])
    EQL(arr.iterkeys_range(k1, k2), [k1])
    EQL(arr.iterkeys_range(k1, k3), [k1])
    EQL(arr.iterkeys_range(k2, k3), [])
    EQL(arr.iterkeys_range(k2, k1), [])
    EQL(arr.iterkeys_range(k3, k1), [])

    # And in reverse
    EQL(arr.riterkeys_from(k1), [k1])
    EQL(arr.riterkeys_from(k2), [k1])
    EQL(arr.riterkeys_from(k3), [k3, k1])
    EQL(arr.riterkeys_to(k1), [k3])
    EQL(arr.riterkeys_to(k2), [k3])
    EQL(arr.riterkeys_to(k3), [])
    EQL(arr.riterkeys_range(k1, k2), [])
    EQL(arr.riterkeys_range(k1, k3), [])
    EQL(arr.riterkeys_range(k2, k3), [])
    EQL(arr.riterkeys_range(k2, k1), [])
    EQL(arr.riterkeys_range(k3, k1), [k3])

    # Check iteration with the empty set
    arr = klass()
    must_stop_iteration(arr.First)
    must_stop_iteration(arr.First, k1)
    must_stop_iteration(arr.Next, k1)
    must_stop_iteration(arr.Next, k2)
    must_stop_iteration(arr.Last)
    must_stop_iteration(arr.Last, k2)
    must_stop_iteration(arr.Prev, k2)
    must_stop_iteration(arr.Prev, k1)

    EQL(arr, [])
    EQL(arr.__reversed__(), [])
    EQL(arr.iterkeys(), [])
    EQL(arr.iterkeys_from(k1), [])
    EQL(arr.iterkeys_to(k1), [])
    EQL(arr.iterkeys_range(k1, k2), [])
    EQL(arr.riterkeys(), [])
    EQL(arr.riterkeys_from(k1), [])
    EQL(arr.riterkeys_to(k1), [])
    EQL(arr.riterkeys_range(k1, k2), [])

def test_judy1_empty(klass, k1, k2, k3, to_unsigned):
    def plus_one(x):
        return to_signed(to_unsigned(x)+1)
    def minus_one(x):
        return to_signed(to_unsigned(x)-1)

    keys = [(to_unsigned(k), k) for k in (k1, k2, k3)]
    keys.sort()
    k1, k2, k3 = [k[1] for k in keys]

    # Finally, the empties
    arr = klass()
    it = arr.iterempty()
    EQ(it.next(), 0)
    EQ(it.next(), 1)
    it = arr.riterempty()
    EQ(it.next(), -1)
    EQ(it.next(), -2)
    EQ(arr.FirstEmpty(), 0)
    EQ(arr.LastEmpty(), -1)
    EQL(arr.iterempty_range(k1, k1), [])
    EQL(arr.iterempty_range(k3, k1), [])
    EQL(arr.riterempty_range(k1, k1), [])
    EQL(arr.riterempty_range(k1, k3), [])

    EQ(arr.FirstEmpty(k1), to_signed(to_unsigned(k1)))
    EQ(arr.NextEmpty(k1), plus_one(k1))
    EQ(arr.NextEmpty(k1), plus_one(k1))
    EQ(arr.LastEmpty(k3), to_signed(to_unsigned(k3)))
    EQ(arr.PrevEmpty(k3), minus_one(k3))
    arr.Set(k1)
    EQ(arr.FirstEmpty(k1), plus_one(k1))
    EQ(arr.NextEmpty(k1), plus_one(k1))
    arr.Set(k3)
    EQ(arr.LastEmpty(k3), minus_one(k3))
    EQ(arr.PrevEmpty(k3), minus_one(k3))

    EQL(arr.iterempty_range(k1, k1), [])
    EQL(arr.iterempty_range(k3, k1), [])
    EQL(arr.riterempty_range(k1, k1), [])
    EQL(arr.riterempty_range(k1, k3), [])

    if (to_unsigned(k3) - to_unsigned(k1)) < 10000:
        x = list(arr.iterempty_range(k1, k3))
        EQ(to_signed(to_unsigned(k1)) in x, False)
        EQ(to_signed(to_unsigned(k2)) in x, True)
        EQ(to_signed(to_unsigned(k3)) in x, False)

        x = list(arr.riterempty_range(k3, k1))
        EQ(to_signed(to_unsigned(k1)) in x, False)
        EQ(to_signed(to_unsigned(k2)) in x, True)
        EQ(to_signed(to_unsigned(k3)) in x, False)

        arr.Set(k2)
        x = list(arr.iterempty_range(k1, k3))
        EQ(to_signed(to_unsigned(k2)) in x, False)
        x = list(arr.riterempty_range(k3, k1))
        EQ(to_signed(to_unsigned(k2)) in x, False)

        arr = klass()
        x = list(arr.iterempty_range(k1, k3))
        EQ(to_signed(to_unsigned(k1)) in x, True)
        EQ(to_signed(to_unsigned(k2)) in x, True)
        EQ(to_signed(to_unsigned(k3)) in x, False)

        x = list(arr.riterempty_range(k3, k1))
        EQL(x, [to_signed(i)
                     for i in xrange(to_unsigned(k3), to_unsigned(k1), -1)])
        EQ(to_signed(to_unsigned(k1)) in x, False)
        EQ(to_signed(to_unsigned(k2)) in x, True)
        EQ(to_signed(to_unsigned(k3)) in x, True)
        

    # Try iterating forward to something
    arr = klass()
    arr.Set(k1)
    arr.Set(k2)
    arr.Set(k3)
    it = arr.iterempty_to(k1)
    if to_unsigned(k1) == 0:
        try:
            x = it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError
    else:
        x = it.next()
        EQ(x, 0)

    it = arr.iterempty_to(k2)
    x = it.next()
    if to_unsigned(k1) == 0:
        EQ(x, 1)
    else:
        EQ(x, 0)

    # Try iterating forward from something
    it = arr.iterempty_from(k1)
    x = it.next()
    EQ(x, plus_one(k1))
    it = arr.iterempty_from(k3)
    if to_signed(to_unsigned(k3)) == -1:
        try:
            x = it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError
    else:
        EQ(it.next(), plus_one(k3))

    # Try iterating backward to something
    arr = klass()
    arr.Set(k1)
    arr.Set(k2)
    arr.Set(k3)
    it = arr.riterempty_to(k3)
    if to_signed(to_unsigned(k3)) == -1:
        try:
            x = it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError
    else:
        x = it.next()
        EQ(x, -1)

    it = arr.riterempty_to(k2)
    x = it.next()
    if to_signed(to_unsigned(k3)) == -1:
        EQ(x, -2)
    else:
        EQ(x, -1)

    # Try iterating backwards from something
    it = arr.riterempty_from(k3)
    x = it.next()
    EQ(x, minus_one(k3))
    it = arr.riterempty_from(k1)
    if to_signed(to_unsigned(k1)) == 0:
        try:
            x = it.next()
        except StopIteration:
            pass
        else:
            raise AssertionError
    else:
        EQ(it.next(), minus_one(k1))
    
def test_judy1():
    print "judy1 test of Judy1Int"
    test_judy1_mem(pyjudy.Judy1Int)
    test_judy1_access(pyjudy.Judy1Int, 5, 8, 15, to_unsigned_int)
    test_judy1_empty(pyjudy.Judy1Int, 5, 8, 15, to_unsigned_int)
    test_judy1_empty(pyjudy.Judy1Int, 0, -20, -1, to_unsigned_int)

    print "judy1 test of Judy1Obj"
    test_judy1_mem(pyjudy.Judy1Obj)
    test_judy1_access(pyjudy.Judy1Obj, 5, 8, 15, to_unsigned_id)
    test_judy1_empty(pyjudy.Judy1Obj, 5, 8, 15, to_unsigned_id)
    class X1: pass
    class X2: pass
    class X3: pass
    test_judy1_access(pyjudy.Judy1Obj, X1, X2, X3, to_unsigned_id)
    test_judy1_empty(pyjudy.Judy1Obj, X3, X1, X2, to_unsigned_id)
        

############

def test_judysl_mem(klass, value = 700, value2 = 800, is_object = False):
    key = "Hello"
    count = sys.getrefcount(klass)
    for i in range(100):
        klass()
    EQ(count, sys.getrefcount(klass))
    if is_object:
        CMP = IS
    else:
        CMP = EQ

    if is_object:
        # Make sure that an Insert does change the ref count of the value
        # but not the key
        arr = klass()
        key_count = sys.getrefcount(key)
        value_count = sys.getrefcount(value)
        arr.Ins(key, value)
        EQ(key_count, sys.getrefcount(key))
        EQ(value_count+1, sys.getrefcount(value))

        # Check refs upon replacement
        value2_count = sys.getrefcount(value2)
        arr.Ins(key, value2)
        EQ(key_count, sys.getrefcount(key))
        EQ(value_count, sys.getrefcount(value))
        EQ(value2_count+1, sys.getrefcount(value2))
        

    # Make sure that duplicate Inserts don't change the reference count
    arr = klass()
    arr.Ins("Hello", value)
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        IS(arr.Ins(key, value), None)
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))


    # Make sure the refcounts change on Del
    if is_object:
        arr = klass()
        key_count = sys.getrefcount(key)
        value_count = sys.getrefcount(value)
        IS(arr.Ins(key, value), None)
        EQ(value_count+1, sys.getrefcount(value))
        IS(arr.Del(key), True)
        EQ(value_count, sys.getrefcount(value))

    # Make sure the duplicate add/deletes of the same object don't
    # cause ref count problems
    arr = klass()
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr.Ins(key, value)
        EQ(arr.Del(key), True)
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    for i in range(100):
        EQ(arr.Del(key), False)
    EQ(key_count, sys.getrefcount(key))


    # A Get must not affect the ref count.
    arr = klass()
    arr.Ins(key, value)
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        CMP(arr.Get(key), value)
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))
    

    # Freeing the array shouldn't be a problem either
    for i in range(100):
        arr = klass()
        arr.FreeArray()
        del arr
    value_count = sys.getrefcount(value)
    for i in range(100):
        arr = klass()
        arr.Ins(key, value)
        arr.FreeArray()
        del arr
    EQ(value_count, sys.getrefcount(value))

    # Make sure First works w/o leaks
    arr = klass()
    arr.Ins(key, value)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.First()
        EQ(x[0], key)
        CMP(x[1], value)
    del x
    EQ(value_count, sys.getrefcount(value))

    # And first with a parameter
    arr = klass()
    arr.Ins(key, value)
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.First(key)
        EQ(x[0], key)
        CMP(x[1], value)
    del x
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Next
    arr = klass()
    arr.Ins("Andrew", value)
    arr.Ins("Dalke", value)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.Next("B")
        EQ(x[0], "Dalke")
        CMP(x[1], value)
    del x
    EQ(value_count, sys.getrefcount(value))

    # Last
    arr = klass()
    arr.Ins(key, value)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.Last()
        EQ(x[0], key)
        CMP(x[1], value)
    del x
    EQ(value_count, sys.getrefcount(value))

    # Last with a parameter
    arr = klass()
    arr.Ins(key, value)
    key_count = sys.getrefcount(key)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.Last()
        EQ(x[0], key)
        CMP(x[1], value)
    del x
    EQ(key_count, sys.getrefcount(key))
    EQ(value_count, sys.getrefcount(value))

    # Prev
    arr = klass()
    arr.Ins("Andrew", value)
    arr.Ins("Dalke", value)
    value_count = sys.getrefcount(value)
    for i in range(100):
        x = arr.Prev("B")
        EQ(x[0], "Andrew")
        CMP(x[1], value)
    del x
    EQ(value_count, sys.getrefcount(value))

def test_judysl_access(klass, v1, v2, v3, is_object):
    if is_object:
        CMP = IS
    else:
        CMP = EQ
    
    k01 = "And"
    k1 = "Andrew"
    k12 = "Ballong"
    k2 = "Bryggeri"
    k23 = "Byte"
    k3 = "Dalke"
    k34 = "Python"
    assert k01 < k1 < k12 < k2 < k23 < k3 < k34

    bad_key = "Andrew\0Dalke"

    # Ins, Del, __len__ and Count
    arr = klass()
    EQ(len(arr), 0)
    EQ(arr.Count(), 0)
    IS(arr.Ins(k1, v1), None)
    EQ(len(arr), 1)
    EQ(arr.Count(), 1)
    arr.Del(k1)
    EQ(len(arr), 0)
    EQ(arr.Count(), 0)
    IS(arr.Ins(k1, v1), None)
    IS(arr.Ins(k1, v1), None)
    EQ(len(arr), 1)
    EQ(arr.Count(), 1)

    # Do they test the input string?
    arr = klass()
    must_raise_type_error(arr.Ins, bad_key, 0)
    must_raise_type_error(arr.Del, bad_key, 0)

    # Check Get
    arr = klass()
    arr.Ins(k1, v1)
    CMP(arr.Get(k1), v1)
    CMP(arr.get(k1), v1)
    arr.Ins(k2, v2)
    CMP(arr.Get(k1), v1)
    CMP(arr.Get(k2), v2)
    CMP(arr.get(k2), v2)
    EQ(len(arr), 2)
    arr.Ins(k2, v3)
    EQ(len(arr), 2)
    CMP(arr.Get(k2), v3)

    must_raise_type_error(arr.Get, bad_key)
    must_raise_type_error(arr.get, bad_key)
    EQ(len(arr), 2)

    # FreeArray
    arr = klass()
    EQ(len(arr), 0)
    arr.FreeArray()
    EQ(len(arr), 0)
    arr.Ins(k1, v1)
    arr.Ins(k2, v2)
    arr.Ins(k3, v3)
    EQ(len(arr), 3)
    arr.FreeArray()
    EQ(len(arr), 0)

    # clear
    arr = klass()
    EQ(len(arr), 0)
    arr.clear()
    EQ(len(arr), 0)
    arr.Ins(k1, v1)
    arr.Ins(k2, v2)
    arr.Ins(k3, v3)
    EQ(len(arr), 3)
    arr.clear()
    EQ(len(arr), 0)

    # Dictionary access
    arr = klass()
    must_not_have_key(arr, k1)
    must_not_have_key(arr, k2)
    must_not_have_key(arr, k3)
    q = "hello?"
    IS(arr.get(k1, "hello?"), q)
    IS(arr.get(k2, "hello?"), q)
    arr[k1] = v1
    EQ(len(arr), 1)
    arr[k1] = v1
    EQ(len(arr), 1)
    arr[k2] = v2
    EQ(len(arr), 2)
    arr[k2] = v3
    EQ(len(arr), 2)
    must_not_have_key(arr, k3)
    arr[k3] = v3
    arr[k2] = v2
    EQ(len(arr), 3)
    CMP(arr[k1], v1)
    CMP(arr[k2], v2)
    CMP(arr[k3], v3)
    del arr[k2]
    EQ(len(arr), 2)
    CMP(arr[k1], v1)
    CMP(arr[k3], v3)
    must_not_have_key(arr, k2)

    # First
    arr = klass()
    must_stop_iteration(arr.First)
    must_stop_iteration(arr.First, "Hi")
    arr.Ins(k1, v1)
    x = arr.First(k01)
    EQ(x, (k1, v1))
    CMP(x[1], v1)
    x = arr.First(k1)
    EQ(x, (k1, v1))
    CMP(x[1], v1)
    must_stop_iteration(arr.First, k12)

    # Next
    arr = klass()
    must_stop_iteration(arr.Next, "Hi")
    arr.Ins(k1, v1)
    x = arr.Next(k01)
    EQ(x, (k1, v1))
    CMP(x[1], v1)
    must_stop_iteration(arr.Next, k1)

    # Last
    arr = klass()
    must_stop_iteration(arr.Last)
    must_stop_iteration(arr.Last, "Hi")
    arr.Ins(k3, v3)
    x = arr.Last(k34)
    EQ(x, (k3, v3))
    CMP(x[1], v3)
    x = arr.Last(k3)
    EQ(x, (k3, v3))
    CMP(x[1], v3)
    must_stop_iteration(arr.Last, k23)

    # Prev
    arr = klass()
    must_stop_iteration(arr.Prev, "Hi")
    arr.Ins(k3, v3)
    x = arr.Prev(k34)
    EQ(x, (k3, v3))
    CMP(x[1], v3)
    must_stop_iteration(arr.Prev, k3)


    # keys / values / items
    arr = klass()

    EQ(arr.keys(), [])
    EQ(arr.values(), [])
    EQ(arr.items(), [])
    arr[k1] = v1
    EQ(arr.keys(), [k1])
    EQ(arr.values(), [v1])
    CMP(arr.values()[0], v1)
    EQ(arr.items(), [(k1, v1)])
    CMP(arr.items()[0][1], v1)
    
    arr[k2] = v2
    EQ(arr.keys(), [k1, k2])
    EQ(arr.values(), [v1, v2])
    CMP(arr.values()[0], v1)
    CMP(arr.values()[1], v2)
    EQ(arr.items(), [(k1,v1), (k2,v2)])
    CMP(arr.items()[0][1], v1)
    CMP(arr.items()[1][1], v2)
    
    arr[k3] = v3
    EQ(arr.keys(), [k1, k2, k3])
    EQ(arr.values(), [v1, v2, v3])
    CMP(arr.values()[0], v1)
    CMP(arr.values()[1], v2)
    CMP(arr.values()[2], v3)
    EQ(arr.items(), [(k1,v1), (k2,v2), (k3,v3)])
    CMP(arr.items()[0][1], v1)
    CMP(arr.items()[1][1], v2)
    CMP(arr.items()[2][1], v3)


    # __contains__
    arr = klass()
    IS(k1 in arr, False)
    IS(k1 in arr, False)
    IS(k2 in arr, False)
    IS(k3 in arr, False)
    arr[k3] = v3
    IS(k1 in arr, False)
    IS(k2 in arr, False)
    IS(k3 in arr, True)
    arr[k2] = v2
    IS(k1 in arr, False)
    IS(k2 in arr, True)
    IS(k3 in arr, True)
    arr[k1] = v1
    IS(k1 in arr, True)
    IS(k2 in arr, True)
    IS(k3 in arr, True)
    del arr[k1]
    IS(k1 in arr, False)
    IS(k2 in arr, True)
    IS(k3 in arr, True)

    # iterkeys / itervalues / iteritems
    arr = klass()
    arr[k1] = v1
    arr[k2] = v2
    arr[k3] = v3
    check_iterators(arr,
                    k01, k1, k12, k2, k23, k3, k34,
                    v1, v2, v3)


def test_judysl():
    print "Testing SLInt"
    test_judysl_mem(pyjudy.JudySLInt)
    test_judysl_access(pyjudy.JudySLInt, 600, 700, 800, is_object = False)

    print "Testing SLObj"
    test_judysl_mem(pyjudy.JudySLObj, is_object = True)
    test_judysl_mem(pyjudy.JudySLObj, "blah-blah", "blarg-blarg", is_object = True)
    test_judysl_access(pyjudy.JudySLObj, 600, 700, 800, is_object = True)
    test_judysl_access(pyjudy.JudySLObj, "blah-blah", "blarg-blarg", "blue", is_object = True)

def test():
    # Don'cha just love the difference between '1' and 'l'?
    test_judy1()
    test_judyl()
    test_judysl()
    
############
    
method_call_count_table = {}

class WrappedClass:
    def __init__(self, klass_name, klass):
        self.klass_name = klass_name
        self.klass = klass
    def __call__(self):
        x = WrappedInstance(self.klass_name, self.klass())
        method_call_count_table[(self.klass_name, "__init__")] += 1
        method_call_count_table[(self.klass_name, "__new__")] += 1
        return x
class WrappedInstance:
    def __init__(self, klass_name, obj):
        self.klass_name = klass_name
        self.obj = obj
    def __getattr__(self, name):
        return WrappedMethod(self.klass_name, name, getattr(self.obj, name))

class WrappedMethod:
    def __init__(self, klass_name, name, method):
        self.klass_name = klass_name
        self.name = name
        self.method = method
    def __call__(self, *args, **kwargs):
        method_call_count_table[(self.klass_name, self.name)] += 1
        return self.method(*args, **kwargs)
    def __repr__(self):
        return "WrappedMethod(%r, %r, %r)" % (self.klass_name, self.name,
                                              self.method)

def instrument_pyjudy():
    for klass_name in ("JudyLIntInt",
                       "JudyLIntObj",
                       "JudyLObjInt",
                       "JudyLObjObj",
                       "Judy1Int",
                       "Judy1Obj",
                       "JudySLInt",
                       "JudySLObj",
                       ):
        klass = getattr(pyjudy, klass_name, None)
        if klass is None:
            print "** Skipping **", klass_name
            continue
        for name in dir(klass):
            method_call_count_table[(klass_name, name)] = 0
                
        setattr(pyjudy, klass_name,
                WrappedClass(klass_name, klass)
                )

def report():
    items = method_call_count_table.items()
    items.sort()
    ignore = ["__hash__", "__class__", "__delattr__", "__doc__",
              "__getattribute__", "__reduce__", "__reduce_ex__",
              "__repr__", "__setattr__", "__str__"]
    missing = []
    for k, v in items:
        if v == 0 and k[1] not in ignore:
            missing.append(k)
    if missing:
        print "** Missing tests for **"
        for (klass_name, name) in missing:
            print klass_name + "." + name
    else:
        print "All methods tested."

if __name__ == "__main__":
    instrument_pyjudy()
    test()
    report()
    print "All tests passed.  Amazing."
