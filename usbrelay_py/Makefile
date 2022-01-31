CFLAGS += -O2 -Wall
HIDAPI = hidraw
LDFLAGS += -lhidapi-$(HIDAPI)
PYTHON_INCLUDE=$(shell python3-config --includes)

PREFIX=/usr

#Default 32 bit x86, raspberry pi, etc..
LIBDIR = $(PREFIX)/lib

#Catch x86_64 machines that use /usr/lib64 (RedHat)
ifneq ($(wildcard $(PREFIX)/lib64/.),)
    LIBDIR = $(PREFIX)/lib64
endif

#Catch debian machines
DEB_HOST_MULTIARCH=$(shell dpkg-architecture -qDEB_HOST_MULTIARCH 2>/dev/null)
ifneq ($(DEB_HOST_MULTIARCH),)
  ifneq ($(wildcard $(PREFIX)/lib/$(DEB_HOST_MULTIARCH)/.),)
    LIBDIR = $(PREFIX)/lib/$(DEB_HOST_MULTIARCH)
  endif
endif


python:  ../libusbrelay.so src/libusbrelay_py.so



#We build this once directly for error checking purposes, then let python do the real build

src/libusbrelay_py.so: src/libusbrelay_py.c ../libusbrelay.so
	$(CC) -shared -fPIC $(PYTHON_INCLUDE) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -I./.. -L./.. -lusbrelay -o $@ $<
	python3 -m build

clean:
	
	rm -rf build dist 
	rm -f src/libusbrelay_py.so
	


install: 

install_py: install libusbrelay.so libusbrelay_py.so
	python3 -m install

.PHONY: all clean install
