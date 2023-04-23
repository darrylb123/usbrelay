include ../LIBVER.in

all: ../libusbrelay.so.$(USBLIBVER)
	python3 setup.py bdist_wheel --universal
#	python3 -m build --wheel

clean:
	rm -rf build dist *.egg-info

install: install
	python3 -m pip install .

.PHONY: all clean install
