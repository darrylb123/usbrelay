CFLAGS += -O0 -Wall -g
HIDAPI = hidraw
LDFLAGS += -lhidapi-$(HIDAPI)

all: usbrelay libusbrelay.so
python: usbrelay libusbrelay.so libusbrelay_py.so

libusbrelay.so: libusbrelay.c libusbrelay.h
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

usbrelay: usbrelay.c libusbrelay.h libusbrelay.so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -lusbrelay -L./ -o $@ $<

#We build this once directly for error checking purposes, then let python do the real build
libusbrelay_py.so: libusbrelay_py.c libusbrelay.so
	$(CC) -shared -fPIC -I/usr/include/python3.5m $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -lusbrelay -L./ -o $@ $<
	python3 setup.py build

clean:
	rm -f usbrelay
	rm -f libusbrelay.so
	rm -rf build


install: usbrelay libusbrelay.so
	install -d $(DESTDIR)/usr/lib
	install -m 0755 libusbrelay.so $(DESTDIR)/usr/lib
	install -d $(DESTDIR)/usr/bin
	install -m 0755 usbrelay $(DESTDIR)/usr/bin

install_py: install libusbrelay.so libusbrelay_py.so
	python3 setup.py install

.PHONY: all clean install
