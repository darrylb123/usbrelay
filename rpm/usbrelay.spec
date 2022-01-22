%global fork mefuller
%global branch rpm

Name:          usbrelay
Version:       0.9.0
Release:       1%{?dist}
Summary:       USB-connected electrical relay control, based on hidapi
License:       GPLv2
URL:           https://github.com/%{fork}/%{name}/
Source0:       %{url}archive/%{branch}.tar.gz

BuildRequires:  gcc
BuildRequires:  git
BuildRequires:  hidapi-devel
BuildRequires:  make
BuildRequires:  python3
BuildRequires:  python3-devel
BuildRequires:  python3-setuptools


%global common_description %{expand: \
 This package includes programs to operate some USB connected electrical relays.
 Supported relays USB ID:
 - 16c0:05df
 - 0519:2018}

%description
%{common_description}


%package common
Summary: Common files needed for all usbrelay interfaces
%description common
%{common_description}


%package python3
Requires: %{name}-common%{_isa} = %{version}-%{release}
Summary: Python 3 user interface for usbrelay
%description python3
%{common_description}
 .
 This package includes the usbrelay Python 3 module.


%package mqtt
Requires: %{name}-common%{_isa} = %{version}-%{release}
Requires: %{name}-python3%{_isa} = %{version}-%{release}
Summary: Support for Home Assistant or nodered with usbrelay
%description mqtt
%{common_description}
 .
 This package provides the MQTT support for using usbrelay with Home Assistant
 or nodered.


%prep
%autosetup -n %{name}-%{branch}


%build
%set_build_flags
make python HIDAPI=libusb
%py3_build


%install
make install DESTDIR=%{buildroot}

%py3_install

mkdir -p %{buildroot}%{_prefix}/lib/udev/rules.d/
cp 50-usbrelay.rules %{buildroot}%{_prefix}/lib/udev/rules.d/
mkdir %{buildroot}%{_sbindir}
cp usbrelayd %{buildroot}%{_sbindir}
mkdir -p %{buildroot}%{_sysconfdir}/systemd/system
cp usbrelayd.service %{buildroot}/etc/systemd/system/


%files common
%license LICENSE.md
%doc README.md
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so


%files python3
%{python3_sitearch}/%{name}_*.egg-info
%{python3_sitearch}/%{name}_py*.so


%files mqtt
%{_prefix}/lib/udev/rules.d/50-usbrelay.rules
%{_sbindir}/usbrelayd
%{_sysconfdir}/systemd/system/usbrelayd.service


%changelog
* Sat Jan 22 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.9.0-1
- bump version 

* Thu Jan 20 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.8.0-1
- first attempt versions of spec file and packaging
