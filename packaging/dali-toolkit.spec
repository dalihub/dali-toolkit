Name:       dali-toolkit
Summary:    The OpenGLES Canvas Core Library Toolkit
Version:    1.1.29
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-2-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali-core)

# dali-toolkit only need to know the interfaces(APIs) of dali-adaptor(the devel package).
# It doesn't need to know which adaptor will be used by applications.
# Applications or dali-addon will decide which one they will use.
BuildRequires:  dali-adaptor-devel

#############################
# profile setup
#############################

%define dali_toolkit_profile MOBILE
%define dali_style_folder 720x1280
# dali_style to be provided by build system as with dali_toolkit_profile or by passing --define 'dali_style 470x800' to the rpm build command

%if "%{?dali_style}"
  %define dali_style_folder %{dali_style}
%endif

# Further resource locations profiles can be provided here otherwise MOBILE will be used
%if "%{tizen_profile_name}" == "mobile"
  %define dali_toolkit_profile MOBILE
%endif

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

%description devel
Application development package for the OpenGLES Canvas toolkit - headers and package config

##############################
# Preparation
##############################
%prep
%setup -q
%define dali_data_rw_dir            /opt/usr/share/dali/
%define dali_data_ro_dir            /usr/share/dali/
%define dali_toolkit_image_files    %{dali_data_ro_dir}/toolkit/images/
%define dali_toolkit_sound_files    %{dali_data_ro_dir}/toolkit/sounds/
%define dali_toolkit_style_files    %{dali_data_ro_dir}/toolkit/styles/
%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -Wl,-Bsymbolic-functions "

libtoolize --force
cd %{_builddir}/dali-toolkit-%{version}/build/tizen
autoreconf --install
DALI_DATA_RW_DIR="%{dali_data_rw_dir}" ; export DALI_DATA_RW_DIR
DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ; export DALI_DATA_RO_DIR
%configure --enable-profile=%{dali_toolkit_profile} --with-style=%{dali_style_folder} --enable-i18n=yes
make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
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

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################
%files
%if 0%{?enable_dali_smack_rules}
%manifest dali-toolkit.manifest-smack
%else
%manifest dali-toolkit.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{name}.so*
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/*
%{_datadir}/license/%{name}

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/%{name}/*
%{_libdir}/pkgconfig/*.pc

