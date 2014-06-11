Name:       dali-toolkit
Summary:    The OpenGLES Canvas Core Library Toolkit
Version:    0.9.15
Release:    1
Group:      System/Libraries
License:    Flora
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:       boost
Requires:       dali-adaptor-dali-feedback-plugin
Requires:       dali
# Do NOT put an adaptor here - it is an application choice which adaptor to use
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  boost-devel
BuildRequires:  dali-devel
BuildRequires:  dali-adaptor-devel
BuildRequires:  fribidi-devel
BuildRequires:  pkgconfig(utilX)
ExclusiveArch:  armv7l

%description
The OpenGLES Canvas Core Library Toolkit - a set of controls that provide
user interface functionality.

##############################
# devel
##############################
%package devel
Summary:    Application development package for the OpenGLES Canvas toolkit
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
Requires:   boost-devel

%description devel
Application development package for the OpenGLES Canvas toolkit - headers and package config

##############################
# Dali Base library
##############################
%package dali-toolkit-base
Summary:    The basic OpenGLES Canvas Core Library Toolkit
Group:      Development/Building

%description dali-toolkit-base
A subset of the controls provided by the main package, intended for lightweight applications.
all the controls provided by the main package.

##############################
# Preparation
##############################
%prep
%setup -q
%define dali_data_rw_dir            /opt/usr/share/dali/
%define dali_data_ro_dir            /usr/share/dali/
%define dali_toolkit_image_files    %{dali_data_ro_dir}/toolkit/images/
%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections "

%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

libtoolize --force
cd %{_builddir}/dali-toolkit-%{version}/build/slp && autoreconf --install
cd %{_builddir}/dali-toolkit-%{version}/build/slp && CXXFLAGS=$CXXFLAGS LDFLAGS=$LDFLAGS DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ./configure --prefix=$PREFIX

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/slp
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

# LICENSE
mkdir -p %{buildroot}/usr/share/license
cp -af %{_builddir}/%{name}-%{version}/LICENSE %{buildroot}/usr/share/license/%{name}

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%post dali-toolkit-base
/sbin/ldconfig
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

%postun dali-toolkit-base
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################
%files
%manifest dali-toolkit.manifest
%defattr(-,root,root,-)
%{_libdir}/lib%{name}.so*
%{dali_toolkit_image_files}/*
%{_datadir}/license/%{name}

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/%{name}/*
%{_libdir}/pkgconfig/*.pc

%files dali-toolkit-base
%defattr(-,root,root,-)
%{_libdir}/libdali-toolkit-base.so*
