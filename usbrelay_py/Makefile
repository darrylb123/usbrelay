include ../LIBVER.in
include ../Makefile.in

LDFLAGS += -lhidapi-$(HIDAPI)
PYTHON_INCLUDE=$(shell python3-config --includes)

all:  ../libusbrelay.so.$(USBLIBVER) src/libusbrelay_py.so dist/



#We build this once directly for error checking purposes, then let python do the real build

src/libusbrelay_py.so: src/libusbrelay_py.c ../libusbrelay.so.$(USBLIBVER)
	python3 -m build --wheel

clean:
	
	rm -rf build dist 
	rm -f src/libusbrelay_py.so
	


install: install 
	python3 -m pip install .

.PHONY: all clean install
