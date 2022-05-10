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

# Commands to generate version number in various situations
# - Release tarballs have version.mk
REL_VERSION = $(if $(wildcard version.mk),$(shell cat version.mk))
# - Use version reported by git
GIT_VERSION = $(if $(wildcard .git/HEAD .git/index),$(shell git describe --tags --match '[0-9].[0-9]*' --abbrev=10 --dirty))
# - Fall back to version derived from directory name
DIR_VERSION = $(shell echo $(notdir $(CURDIR)) | sed -nEe 's/usbrelay-([0-9].*)/\1/p')

# Use first version, which is not empty
VERSION=$(or $(REL_VERSION),$(GIT_VERSION),$(DIR_VERSION),unknown)

# If .git/HEAD and/or .git/index exist, we generate git version with
# the command above and regenerate it whenever any of these files
# changes. If these files don't exist, we use ??? as the version.
gitversion.h: $(wildcard .git/HEAD .git/index)
	echo "#define GITVERSION \"$(VERSION)\"" > $@

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

# Release target for maintainers
#
# The goal is to generate a commit which contains version.mk with the
# version number, but not have this file in subsequent commits.
release:
	@if [ -z "$(VER)" ]; then echo "Please specify version to release, e.g., 'make release VER=1.2.3'."; exit 1; fi
	@if [ -n "$$(git tag -l '$(VER)')" ]; then echo ''Tag $(VER) already exists.''; exit 1; fi
	git diff --stat --exit-code HEAD # Check that there are no uncommitted changes
	echo $(VER) > version.mk
	git add version.mk
	git commit -m "Release $(VER)"
	git tag -m "Release $(VER)" --sign $(VER)
	git reset --keep HEAD^
	git merge --strategy ours -m 'Remove version.mk after release' $(VER)
	@echo
	@echo "Now run 'git push --follow-tags' and create the release on GitHub."
	@echo "Then, you can sign the release archives by 'make sign-release VER=$(VER)'"

# Sign the release tarball after its creation on GitHub. This is based
# on https://wiki.debian.org/Creating%20signed%20GitHub%20releases. In
# addition to that, we use the 'gh' tool to automate downloading of
# the tarball and uploading of the signature.
sign-release:
	@if [ -z "$(VER)" ]; then echo "Please specify the release to sign, e.g., 'make sign-release VER=1.2.3'."; exit 1; fi
	git archive --prefix='usbrelay-$(VER)/' -o 'usbrelay-$(VER).tar.gz' '$(VER)'
	gpg --armor --detach-sign 'usbrelay-$(VER).tar.gz'
# Verify that github has the same tarball
	rm -f 'usbrelay-$(VER).tar.gz'
	gh release download '$(VER)' --archive=tar.gz
	gpg --verify 'usbrelay-$(VER).tar.gz.asc'
	gh release upload '$(VER)' 'usbrelay-$(VER).tar.gz.asc'

.PHONY: all clean install release sign-release
