Name:          usbrelay
Version:       1.0.1
Release:       %autorelease
Summary:       USB-connected electrical relay control, based on hidapi
License:       GPLv2
URL:           https://github.com/darrylb123/%{name}/
Source0:       %{url}archive/refs/tags/usbrelay-%{version}.tar.gz



BuildRequires:  gcc
BuildRequires:  git
BuildRequires:  hidapi-devel
BuildRequires:  make
BuildRequires:  python3
BuildRequires:  python3-devel
BuildRequires:  python3-wheel
BuildRequires:  python3-tox-current-env
BuildRequires:  systemd-rpm-macros


%global common_description %{expand: \
 This package includes programs to operate some USB connected electrical relays.
 Supported relays USB ID:
 - 16c0:05df
 - 0519:2018}

%description
%{common_description}


%package common
Requires: hidapi
Summary: Common files needed for all usbrelay interfaces
# Provides: libusbrelay.so()(64bit)
%description common
%{common_description}


%package -n python3-%{name}
Requires: %{name}-common%{_isa} = %{version}-%{release}
Summary: Python 3 user interface for usbrelay
%description -n python3-%{name}
%{common_description}
 .
 This package includes the usbrelay Python 3 module.


%package mqtt
Requires: %{name}-common%{_isa} = %{version}-%{release}
Requires: python3-%{name}%{_isa} = %{version}-%{release}
Summary: Support for Home Assistant or nodered with usbrelay
%description mqtt
%{common_description}
 .
 This package provides the MQTT support for using usbrelay with Home Assistant
 or nodered.


%prep
%autosetup -n %{name}-%{version}

%generate_buildrequires
cd usbrelay_py
%pyproject_buildrequires

%build
%set_build_flags
make
cd usbrelay_py
%pyproject_wheel
# make python HIDAPI=libusb

%install
# make install DESTDIR=%{buildroot}
cd usbrelay_py
%pyproject_install
cd ..
# Install binaries
install -d %{buildroot}%{_bindir}
install usbrelay %{buildroot}%{_bindir}
install -d %{buildroot}%{_libdir}
install libusbrelay.so.?.? %{buildroot}%{_libdir}
# manual copy/install operations from README
install -d %{buildroot}%{_udevrulesdir}/
install 50-usbrelay.rules %{buildroot}%{_udevrulesdir}/
install -d %{buildroot}%{_sbindir}
install usbrelayd %{buildroot}%{_sbindir}
install -d %{buildroot}%{_unitdir}/
install usbrelayd.service %{buildroot}%{_unitdir}/
install -d %{buildroot}%{_sysconfdir}/
install usbrelayd.conf %{buildroot}%{_sysconfdir}/

# install test function (since users need to test relay boards)
install -d %{buildroot}%{python3_sitearch}/%{name}
install usbrelay_py/tests/usbrelay_test.py %{buildroot}%{python3_sitearch}/%{name}/

# Create the dynamic users/groups
install -p -D -m 0644 rpm/usbrelay.sysusers %{buildroot}%{_sysusersdir}/usbrelay.conf


# Create and empty key file and pid file to be marked as a ghost file below.
mkdir -p %{buildroot}%{_rundir}/usbrelay
touch %{buildroot}%{_rundir}/usbrelay/usbrelayd.pid



%check
# verify that Python module imports
# can't test here as this required hardware(?)


%pre
%sysusers_create_compat rpm/usbrelay.sysusers


%preun
%systemd_preun usbrelayd.service


%post
%systemd_post usbrelayd.service
/usr/sbin/ldconfig




%postun
%systemd_postun_with_restart usbrelayd.service
/usr/sbin/ldconfig
cd %{_libdir}
ln -s libusbrelay.so.?.? libusbrelay.so

%files common
%license LICENSE.md
%doc README.md
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so.?.?
%ghost %{_libdir}/libusbrelay.so.?
%ghost %{_libdir}/libusbrelay.so
%{_udevrulesdir}/50-usbrelay.rules
%{_sysusersdir}/usbrelay.conf

%files -n python3-%{name}
%{python3_sitearch}/%{name}_py*.so
%{python3_sitearch}/%{name}_py*.dist-info
%{python3_sitearch}/%{name}/*
%ghost %{_libdir}/libusbrelay.so


%files mqtt
%{_sbindir}/usbrelayd
%{_unitdir}/usbrelayd.service
%{_sysconfdir}/usbrelayd.conf
%attr(0755,usbrelay,usbrelay) %ghost %dir %{_rundir}/usbrelay/
%attr(0644,usbrelay,usbrelay) %ghost %{_rundir}/usbrelay/usbrelayd.pid


%changelog
%autochangelog
