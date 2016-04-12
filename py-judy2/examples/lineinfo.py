"""Use a Judy1 array to print statistics about lines in a file."""

import pyjudy

# Get a file to analyze
import inspect
filename = inspect.__file__
if filename.endswith(".pyc"):
    filename = filename[:-1]


def count_startswith(arr, word):
    # Turns "def " into ("def ", "def!")
    # Turns "class " into ("class ", "class!")
    #   because "!" is the character after " ".
    c = chr(ord(word[-1])+1)
    # JudySL arrays don't support Count so have to
    # do this manually.
    it = arr.iterkeys_range(word, word[:-1] + c)
    n = 0
    for x in it:
        n += 1
    return n

# Load the file
j1arr = pyjudy.JudySLObj()
for line in open(filename):
    j1arr[line] = j1arr.get(line, 0) + 1

print "Analyzing", repr(filename)
print "There are", len(j1arr), "unique lines"
print j1arr.get("\n", 0), "of them are newlines"
print "In sorted order the first line is", repr(j1arr.First()[0])
print " ... and the last is", repr(j1arr.Last()[0])
    
print count_startswith(j1arr, "def "),
print "lines start with a 'def' statement"
print count_startswith(j1arr, "class "),
print "lines start with a 'class' definition"
