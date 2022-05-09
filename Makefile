CFLAGS += -O2 -Wall
HIDAPI = hidraw
LDLIBS += -lhidapi-$(HIDAPI)

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

all: usbrelay libusbrelay.so python

install_all: install install_py

libusbrelay.so: libusbrelay.c libusbrelay.h 
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS)  $< $(LDFLAGS) -o $@ $(LDLIBS)

usbrelay: usbrelay.c libusbrelay.h libusbrelay.so
	$(CC) $(CPPFLAGS) $(CFLAGS)  $< -lusbrelay -L./ $(LDFLAGS) -o $@ $(LDLIBS)

python:
	$(MAKE) -C usbrelay_py
# Command to generate version number if running from git repo
DIR_VERSION = $(shell basename `pwd`)
GIT_VERSION = $(shell git describe --tags --match '[0-9].[0-9]*' --abbrev=10 --dirty)

# If .git/HEAD and/or .git/index exist, we generate git version with
# the command above and regenerate it whenever any of these files
# changes. If these files don't exist, we use ??? as the version.
gitversion.h: $(wildcard .git/HEAD .git/index)
	echo "#define GITVERSION \"$(if $(word 1,$^),$(GIT_VERSION),$(DIR_VERSION))\"" > $@

usbrelay.c libusbrelay.c: gitversion.h


clean:
	rm -f usbrelay
	rm -f libusbrelay.so
	rm -f gitversion.h
	$(MAKE) -C usbrelay_py clean


install: usbrelay libusbrelay.so
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0755 libusbrelay.so $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 0755 usbrelay $(DESTDIR)$(PREFIX)/bin


install_py:
	$(MAKE) -C usbrelay_py install

.PHONY: all clean install
