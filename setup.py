from setuptools import setup
from setuptools.extension import Extension

cpy_extension = Extension('_eval_tracer', sources=['_eval_tracer.c'], 
        extra_compile_args=[])

setup(
    name='eval_tracer',
    packages=["eval_tracer"],
    ext_modules=[cpy_extension],
    author='Tony Simpson',
    author_email='agjasimpson@gmail.com',
    license='MIT',
)
