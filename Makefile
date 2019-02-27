CFLAGS += -O0 -Wall -g
HIDAPI = hidraw
LDFLAGS += -lhidapi-$(HIDAPI)

all: usbrelay

libusbrelay.so: libusbrelay.c libusbrelay.h
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

usbrelay: usbrelay.c libusbrelay.h libusbrelay.so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -lusbrelay -L./ -o $@ $<

clean:
	rm -f usbrelay

install: usbrelay
	install -d $(DESTDIR)/usr/lib
	install -m 0755 libusbrelay.so $(DESTDIR)/usr/lib
	install -d $(DESTDIR)/usr/bin
	install -m 0755 usbrelay $(DESTDIR)/usr/bin


.PHONY: all clean install
