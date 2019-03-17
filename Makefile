CFLAGS += -O2 -Wall
HIDAPI = hidraw
LDFLAGS += -lhidapi-$(HIDAPI)

#Default 32 bit x86, raspberry pi, etc..
LIBDIR = /usr/lib

#Catch x86_64 machines that use /usr/lib64 (RedHat)
ifneq ($(wildcard /usr/lib64/.),)
    LIBDIR = /usr/lib64
endif

#Catch debian machines with Multiarch (x64)
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/.),)
    LIBDIR = /usr/lib/x86_64-linux-gnu
endif

#Catch debian machines with Multiarch (aarch64)
ifneq ($(wildcard /usr/lib/aarch64-linux-gnu/.),)
    LIBDIR = /usr/lib/aarch64-linux-gnu
endif

#Catch debian machines with Multiarch (arm-linux-gnueabihf)
ifneq ($(wildcard /usr/lib/arm-linux-gnueabihf/.),)
    LIBDIR = /usr/lib/arm-linux-gnueabihf
endif

LDFLAGS += -L $(LIBDIR) -Wl,-rpath $(LIBDIR)

all: usbrelay libusbrelay.so
python: usbrelay libusbrelay.so libusbrelay_py.so

libusbrelay.so: libusbrelay.c libusbrelay.h
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

usbrelay: usbrelay.c libusbrelay.h libusbrelay.so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -L./ -lusbrelay -o $@ $<

#We build this once directly for error checking purposes, then let python do the real build
libusbrelay_py.so: libusbrelay_py.c libusbrelay.so
	$(CC) -shared -fPIC -I/usr/include/python3.5m $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -L./ -lusbrelay -o $@ $<
	python3 setup.py build

clean:
	rm -f usbrelay
	rm -f libusbrelay.so
	rm -f libusbrelay_py.so
	rm -rf build


install: usbrelay libusbrelay.so
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0755 libusbrelay.so $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)/usr/bin
	install -m 0755 usbrelay $(DESTDIR)/usr/bin

install_py: install libusbrelay.so libusbrelay_py.so
	python3 setup.py install

.PHONY: all clean install
