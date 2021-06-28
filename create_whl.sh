#!/bin/bash -xe

sed -ri 's/^from distutils.core/from setuptools/' setup.py

python3 setup.py bdist_wheel

sed -ri 's/^from setuptools/from distutils.core/' setup.py
mv dist/*.whl .
rm -fr dist
