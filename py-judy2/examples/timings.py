"""Report some performance numbers"""

import pyjudy

import time
import sys

DO_JUDYL_TIMINGS = True
DO_JUDY1_TIMINGS = True

N = 1000000
if sys.argv[1:]:
    N = int(sys.argv[1])

def make_data():
    # This is about 4 times faster than using random.randint
    import _random
    test_data = [None] * N
    R = _random.Random().random
    _int = int
    WIDTH = 5*N
    for i in xrange(N):
        test_data[i] = _int(R() * WIDTH)
    return test_data

print "Making test data set with", N, "random numbers ...",
t1 = time.time()
all_data = make_data()
t2 = time.time()
data_subset1 = all_data[::2]
data_subset2 = all_data[1::2]
print "done."
print

def report(names, results):
    header = "%40s :" + ("  %10s" * len(results))
    print header % tuple(["-------"] + names)
    for i in range(len(results[0])):
        label = results[0][i][0]
        values = [result[i][1] for result in results]
        if isinstance(values[0], float):
            values = ["%10.4f" % val for val in values]
        print header % tuple([label] + values)
    print

def time_mapping(d, all_data, subset1, subset2):
    results = []
    def A(name, dt):
        results.append( (name, dt) )

    # Load all the data
    t1 = time.time()
    for i in all_data:
        d[i] = i
    t2 = time.time()
    A("time to load mapping", t2-t1)

    # Verify that subset1 is in all_data
    t1 = time.time()
    for i in subset1:
        if i not in d:
            raise AssertionError
    t2 = time.time()
    A("time to verify subset1 in all_data", t2-t1)
            
    # Clear
    t1 = time.time()
    d.clear()
    t2 = time.time()
    A("time to clear", t2-t1)

    # Count the number of unique keys in all_data
    t1 = time.time()
    for i in all_data:
        d[i] = d.get(i, 0) + 1
    t2 = time.time()
    A("__contains__ with all hits", t2-t1)
    A("number of unique=", len(d))

    # Look up all the keys. 
    t1 = time.time()
    for i in all_data:
        d[i]
    t2 = time.time()
    A("index lookup (all elements exist)", t2-t1)

    t1 = time.time()
    for i in d:
        pass
    t2 = time.time()
    A("iter", t2-t1)

    t1 = time.time()
    for i in d.itervalues():
        pass
    t2 = time.time()
    A("itervalues", t2-t1)

    t1 = time.time()
    for i in d.iteritems():
        pass
    t2 = time.time()
    A("iteritems", t2-t1)


    d.clear()
    for i in subset1:
        d[i] = i
    t1 = time.time()
    n = 0
    for i in subset2:
        if i in d:
            n = n + 1
    t2 = time.time()
    A("__contains__ with few matches", t2-t1)
    A("% matches=", float(n) / len(subset2))

    d.clear()
    for i in all_data:
        d[i] = i
    t1 = time.time()
    for i in subset1:
        try:
            del d[i]
        except KeyError:
            # Already deleted (duplicate key in subset1)
            pass
    t2 = time.time()
    A("delete", t2-t1)
    
    return results


if DO_JUDYL_TIMINGS:
    names = []
    results = []
    for name, klass in (
        ("dict", dict),
        ("JudyLIntInt", pyjudy.JudyLIntInt),
        ("JudyLIntObj", pyjudy.JudyLIntObj),
        ("JudyLObjObj", pyjudy.JudyLObjObj),
        ):
        d = klass()
        names.append(name)
        results.append( time_mapping(d, all_data, data_subset1, data_subset2) )
    print " *** JudyL timings ***"
    report(names, results)

#############

try:
    set
except NameError:
    from sets import Set as set
    print "WARNING: using pre-2.4 set implementation"


def time_set(d, all_data, subset1, subset2):
    results = []
    def A(name, dt):
        results.append( (name, dt) )
    
    # Load all the data
    t1 = time.time()
    for i in all_data:
        d.add(i)
    t2 = time.time()
    A("time to load set", t2-t1)

    # Verify that subset1 is in all_data
    t1 = time.time()
    for i in subset1:
        if i not in d:
            raise AssertionError
    t2 = time.time()
    A("time to verify subset1 in all_data", t2-t1)
    
    # Clear
    t1 = time.time()
    d.clear()
    t2 = time.time()
    A("time to clear", t2-t1)

    # Look up all the keys again.  These will all fail. */
    t1 = time.time()
    for i in all_data:
        if i in d:
            raise AssertionError
    t2 = time.time()
    A("verify none of all_data in empty set", t2-t1)


    # Which elements of subset1 are also in subset2?
    # There should be a sparse overlap.
    d.clear()
    for i in subset1:
        d.add(i)
    t1 = time.time()
    n = 0
    for i in subset2:
        n += (i in d)
    t2 = time.time()
    A("check for overlaps (sparse __contains__)", t2-t1)
    A("number of overlaps", n)

    # Remove subset1 from all_data
    d.clear()
    for i in all_data:
        d.add(i)
    t1 = time.time()
    for i in subset1:
        try:
            d.remove(i)
        except KeyError:
            pass
    t2 = time.time()
    A("delete subset1 from all_data", t2-t1)

    return results

if DO_JUDY1_TIMINGS:
    names = []
    results = []
    for name, klass in (
        ("set", set),
        ("Judy1Int", pyjudy.Judy1Int),
        ("Judy1Obj", pyjudy.Judy1Obj),
        ):
        d = klass()
        names.append(name)
        results.append( time_set(d, all_data, data_subset1, data_subset2) )
        
    print " *** Judy1 timings ***"
    report(names, results)
