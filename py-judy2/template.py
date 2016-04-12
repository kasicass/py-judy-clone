# Read the various template files.  The forward-iterator code is also
# a template to make the template for the reverse-iterator.  This
# module handles that conversion.

import os

## For {int,obj}->{int,obj} mappings 

def get_pyjudyl_iter():
    s = open("pyjudyl_iter_template.c").read()
    forward = s.replace(
        "START", "0").replace(
        "END", "-1")
    reverse = s.replace(
        "forward iteration", "reverse iteration").replace(
        "_iter", "_riter").replace(
        "<P>Iter", "<P>RIter").replace(
        "START", "-1").replace(
        "END", "0")
    return forward + reverse

def get_pyjudyl_code():
    return open("pyjudyl_code_template.c").read()

def get_pyjudyl_template():
    return get_pyjudyl_iter() + get_pyjudyl_code()


## For {int,obj}->bit mappings 
def get_pyjudy1_iter():
    s = open("pyjudy1_iter_template.c").read()
    forward = s.replace(
        "START", "0").replace(
        "END", "-1")
    reverse = s.replace(
        "forward iteration", "reverse iteration").replace(
        "_iter", "_riter").replace(
        "<P>Iter", "<P>RIter").replace(
        "EmptyIter", "EmptyRIter").replace(
        "START", "-1").replace(
        "END", "0")
    return forward + reverse

def get_pyjudy1_code():
    return open("pyjudy1_code_template.c").read()

def get_pyjudy1_template():
    return get_pyjudy1_iter() + get_pyjudy1_code()

## For string->{int,obj} mappings 

def get_pyjudysl_iter():
    s = open("pyjudysl_iter_template.c").read()
    forward = s.replace(
        "START", "0").replace(
        "END", "-1")
    reverse = s.replace(
        "forward iteration", "reverse iteration").replace(
        "_iter", "_riter").replace(
        "<P>Iter", "<P>RIter").replace(
        "START", "-1").replace(
        "END", "0")
    return forward + reverse

def get_pyjudysl_code():
    return open("pyjudysl_code_template.c").read()

def get_pyjudysl_template():
    return get_pyjudysl_iter() + get_pyjudysl_code()
