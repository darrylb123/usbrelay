%global fork mefuller
%global branch rpm

Name:          usbrelay
Version:       0.8.0
Release:       1%{?dist}
Summary:       USB-connected electrical relay control, based on hidapi
License:       GPLv2
URL:           https://github.com/%{fork}/%{name}/
Source0:       %{url}archive/%{branch}.tar.gz

BuildRequires:  gcc
BuildRequires:  git
BuildRequires:  hidapi-devel
#BuildRequires:  libusb
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


%prep
%autosetup -n %{name}-%{branch}


%build
%set_build_flags
make python HIDAPI=libusb


%install
make install DESTDIR=%{buildroot}
python3 setup.py install --prefix=%{_prefix} --install-lib=%{buildroot}%{python3_sitearch}


%files common
%license LICENSE.md
%doc README.md
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so


%files python3
%{python3_sitearch}/%{name}_*.egg/


%changelog
* Thu Jan 20 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.8.0-1
- first attempt versions of spec file and packaging
