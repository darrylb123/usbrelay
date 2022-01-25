%global fork mefuller
%global branch rpm

Name:          usbrelay
Version:       0.9
Release:       2%{?dist}
Summary:       USB-connected electrical relay control, based on hidapi
License:       GPLv2
URL:           https://github.com/%{fork}/%{name}/
Source0:       %{url}archive/%{branch}.tar.gz
Source1:       ./%{name}.sysusers

BuildRequires:  gcc
BuildRequires:  git
BuildRequires:  hidapi-devel
BuildRequires:  make
BuildRequires:  python3
BuildRequires:  python3-devel
BuildRequires:  python3-setuptools
BuildRequires:  systemd-rpm-macros


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
%autosetup -n %{name}-%{branch}


%build
%set_build_flags
make python HIDAPI=libusb
%py3_build


%install
make install DESTDIR=%{buildroot}

%py3_install

install -d %{buildroot}%{_prefix}/lib/udev/rules.d/
install 50-usbrelay.rules %{buildroot}%{_prefix}/lib/udev/rules.d/
install -d %{buildroot}%{_sbindir}
install usbrelayd %{buildroot}%{_sbindir}
install -d %{buildroot}%{_sysconfdir}/systemd/system
install usbrelayd.service %{buildroot}%{_sysconfdir}/systemd/system/
install usbrelayd.conf %{buildroot}%{_sysconfdir}/
install -d %{_buildroot}/share/man/man1/
install usbrelay.1 %{_buildroot}/share/man/man1/


%pre
%sysusers_create_compat %{SOURCE1}


%files common
%license LICENSE.md
%doc README.md
%doc %{_prefix}/share/man/man1/usbrelay.1
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so
%{_prefix}/lib/udev/rules.d/50-usbrelay.rules


%files -n python3-%{name}
%{python3_sitearch}/%{name}_*.egg-info
%{python3_sitearch}/%{name}_py*.so
%{python3_sitearch}/%{name}_test.py


%files mqtt
%{_sbindir}/usbrelayd
%{_sysconfdir}/systemd/system/usbrelayd.service
%{_sysconfdir}/usbrelayd.conf


%changelog
* Tue Jan 25 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.9.0-2
- continued spec development and upstream improvements 

* Sat Jan 22 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.9.0-1
- bump version 

* Thu Jan 20 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.8.0-1
- first attempt versions of spec file and packaging
