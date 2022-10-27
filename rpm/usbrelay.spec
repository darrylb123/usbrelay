Name:          usbrelay
Version:       1.1
Release:       %autorelease
Summary:       USB-connected electrical relay control, based on hidapi
License:       GPL-2.0-or-later
URL:           https://github.com/darrylb123/%{name}
Source0:       %{url}/archive/%{version}/%{name}-%{version}.tar.gz



BuildRequires:  gcc
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

Requires: hidapi
Requires: systemd-udev
Summary: A library and command line tool for controlling USB-connected relays


%package devel
Requires: hidapi-devel
Requires: python3-devel
Requires: %{name}%{_isa} = %{version}-%{release}
Summary: Package for developing against libusbrelay
%description devel
%{common_description}
.
Headers for developing against libusbrelay

%package -n python3-%{name}
Requires: %{name}%{_isa} = %{version}-%{release}
Summary: Python 3 user interface for usbrelay
%description -n python3-%{name}
%{common_description}
 .
 This package includes the usbrelay Python 3 module.


%package mqtt
Requires: %{name}%{_isa} = %{version}-%{release}
Requires: python3-%{name}%{_isa} = %{version}-%{release}
Requires: python3-paho-mqtt
Summary: Support for Home Assistant or nodered with usbrelay
%description mqtt
%{common_description}
 .
 This package provides the MQTT support for using usbrelay with Home Assistant
 or Node Red.


%prep
%autosetup -n %{name}-%{version}
%generate_buildrequires
%py3_shebang_fix .

cd usbrelay_py
%pyproject_buildrequires

%build
%set_build_flags
make
cd usbrelay_py
%pyproject_wheel

%install
# Install binaries
%{make_install}
# manual copy/install operations from README
install -d %{buildroot}%{_udevrulesdir}/
cp --preserve=timestamps 50-usbrelay.rules %{buildroot}%{_udevrulesdir}/
install -d %{buildroot}%{_sbindir}
install --preserve-timestamps usbrelayd %{buildroot}%{_sbindir}

install -d %{buildroot}%{_unitdir}/
cp --preserve=timestamps usbrelayd.service %{buildroot}%{_unitdir}/
install -d %{buildroot}%{_sysconfdir}/
cp --preserve=timestamps usbrelayd.conf %{buildroot}%{_sysconfdir}/
install -d %{buildroot}%{_mandir}/man8
install -d %{buildroot}%{_mandir}/man1
cp --preserve=timestamps usbrelay.1 %{buildroot}%{_mandir}/man1
cp --preserve=timestamps usbrelayd.8 %{buildroot}%{_mandir}/man8
install -d %{buildroot}%{_includedir}
cp --preserve=timestamps libusbrelay.h %{buildroot}%{_includedir}
# Create the dynamic users/groups
install -p -D -m 0644 rpm/usbrelay.sysusers %{buildroot}%{_sysusersdir}/usbrelay.conf
# Create an empty pid file to be marked as a ghost file below.
mkdir -p %{buildroot}%{_rundir}/usbrelay
touch %{buildroot}%{_rundir}/usbrelay/usbrelayd.pid

# Install Python
cd usbrelay_py
%pyproject_install
cd ..
# install test function (since users need to test relay boards)
install -d %{buildroot}%{python3_sitearch}/%{name}
cp --preserve=timestamps usbrelay_py/tests/usbrelay_test.py %{buildroot}%{python3_sitearch}/%{name}/




%check


%pre
%sysusers_create_compat rpm/usbrelay.sysusers


%preun -n %{name}-mqtt
%systemd_preun usbrelayd.service


%post -n %{name}-mqtt
%systemd_post usbrelayd.service




%postun -n %{name}-mqtt 
%systemd_postun_with_restart usbrelayd.service

%files
%license LICENSE.md
%doc README.md
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so.?.?
%ghost %{_libdir}/libusbrelay.so.?
%{_udevrulesdir}/50-usbrelay.rules
%{_sysusersdir}/usbrelay.conf
%{_mandir}/man1/usbrelay.1*

%files -n python3-%{name}
%{python3_sitearch}/%{name}_py*.so
%{python3_sitearch}/%{name}_py*.dist-info
%{python3_sitearch}/%{name}



%files mqtt
%{_sbindir}/usbrelayd
%{_unitdir}/usbrelayd.service
%{_sysconfdir}/usbrelayd.conf %config(noreplace)
%attr(0755,root,usbrelay) %ghost %dir %{_rundir}/usbrelay/
%attr(0644,root,usbrelay) %ghost %{_rundir}/usbrelay/usbrelayd.pid
%{_mandir}/man8/usbrelayd.8*

%files devel
%{_includedir}/libusbrelay.h
%{_libdir}/libusbrelay.so

%changelog
%autochangelog
