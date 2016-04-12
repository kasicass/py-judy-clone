import os, re, sys
from distutils.core import setup, Extension


JUDY_BASE_DIR = "/usr/local"
JUDY_LIB_DIR = os.path.join(JUDY_BASE_DIR, "lib")
JUDY_INC_DIR = os.path.join(JUDY_BASE_DIR, "include")

# Give some advice if it looks like Judy isn't installed.
if not os.path.exists(os.path.join(JUDY_INC_DIR, "Judy.h")):
    print >>sys.stderr, "WARNING: Judy.h not found in", repr(JUDY_INC_DIR)
    print >>sys.stderr, "Judy source is available from http://judy.sourceforge.net/"
    print >>sys.stderr, "Edit this setup.py to change the default location."
    # But keep on going since there are other reasons to build 

# Some code to rebuild 'pyjudy.c' when its dependencies change.
# This won't run in a distribution since I provide the right pyjudy.c.

def mtime(filename):
    try:
        return os.path.getmtime(filename)
    except OSError:
        return 0

deps = ["pyjudy1_code_template.c",
        "pyjudy1_iter_template.c",
        "pyjudyl_code_template.c",
        "pyjudyl_iter_template.c",
        "pyjudysl_code_template.c",
        "pyjudysl_iter_template.c",
        "pyjudy_common.c",
        "pyjudy.h",
        "pyjudy_gen.py",
        "template.py",
        ]

pyjudy_mtime = mtime("pyjudy.c")
for dep_filename in deps:
    if mtime(dep_filename) < pyjudy_mtime:
        import pyjudy_gen
        pyjudy_gen.main()
        break

setup(name = "pyjudy", version = "1.0",
      author = "Andrew Dalke",
      author_email = "dalke@dalkescientific.com",
      url = "http://www.dalkescientific.com/Python/PyJudy.html",
      description = "A python wrapper to the Judy sparse dynamic array library",
      download_url = "http://www.dalkescientific.com/Python/pyjudy-1.0.tar.gz",
      classifiers = ["Development Status :: 5 - Production/Stable",
                     "Intended Audience :: Developers",
                     "License :: OSI Approved :: MIT License",
                     "Programming Language :: Python",
                     "Topic :: Software Development :: Libraries :: Python Modules",
                     ],                     

      
      ext_modules = [Extension("pyjudy",
         ["pyjudy.c"],
         include_dirs = [JUDY_INC_DIR],
         library_dirs = [JUDY_LIB_DIR],
         libraries = ["Judy"],
         )]
)
