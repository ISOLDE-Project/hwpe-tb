#!python3
exec(open("code_image.py").read())
exec(open("mem_dump.py").read())

from numpy.testing import assert_allclose 
assert_allclose(mem,code_image,atol=1e-5)