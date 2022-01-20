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


%global common_description %{expand: \
 usbrelay...}

%description
%{common_description}


%package common
Summary: Common files needed for all usbrelay interfaces
%description common
%{common_description}
 .
 This package includes programs for parsing and converting chemical
 mechanisms, a set of common mechanism files, and several sample problems.


%prep
%autosetup -n %{name}-%{branch}


%build
%set_build_flags
make python HIDAPI=libusb


%install
make install DESTDIR=%{buildroot}


%files common
%license LICENSE.md
%doc README.md
%{_bindir}/usbrelay
%{_libdir}/libusbrelay.so


%changelog
* Thu Jan 20 2022 Mark E. Fuller <mark.e.fuller@gmx.de> - 0.8.0-1
- first attempt versions of spec file and packaging
