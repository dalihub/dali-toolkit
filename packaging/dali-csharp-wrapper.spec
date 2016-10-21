Name:       NDalic
Summary:    dali wrapper
Version:    1.2.11
Release:    1
Group:      uifw/graphic
License:    TO_BE_FILLED_IN
Source0:    %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: swig
#BuildRequires: ruby
#BuildRequires: ruby-devel

#BuildRequires: mono-core
#BuildRequires: mono-compat
#BuildRequires: mono-data
#BuildRequires: mono-compiler
#BuildRequires: mono-wcf
#BuildRequires: mono-web

BuildRequires:  pkgconfig

BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(dali-core)
BuildRequires: pkgconfig(dali-adaptor)
BuildRequires: pkgconfig(dali-toolkit)

BuildRequires: dali-devel
BuildRequires: dali-integration-devel
BuildRequires: dali-adaptor-devel
BuildRequires: dali-adaptor-integration-devel
BuildRequires: dali-toolkit-devel


%description
dali wrapper

%package dali-wrapper

Summary: Development dali-wrapper
Group: Development/Libraries/Other
Requires: %{name} = %{version}

%description dali-wrapper
Development .so for dali wrapper
%define daliSwigDir %{_builddir}/%{name}-%{version}/build/tizen
%define daliToolkitDir %{_builddir}/dali-toolkit-%{version}/build/tizen
%define daliPluginsDir %{_builddir}/%{name}-%{version}/build/tizen/plugins/csharp

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
# Preparation
##############################
%prep
%setup -q

#Use TZ_PATH when tizen version is 3.x

%if "%{tizen_version_major}" == "2"
%define ndali_data_rw_dir            /opt/usr/share/dali/
%define ndali_data_ro_dir            /usr/share/dali/
%else
%define ndali_data_rw_dir            %TZ_SYS_SHARE/dali/
%define ndali_data_ro_dir            %TZ_SYS_RO_SHARE/dali/
%endif
##############################
# Build
##############################
%build
echo "----------------" %{_builddir}
echo "----------------" %{daliSwigDir}
echo "----------------" %{_libdir}
echo "----------------" %{daliPluginsDir}

PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -Wl,-Bsymbolic-functions "

%if 0%{?enable_coverage}
CXXFLAGS+=" --coverage "
LDFLAGS+=" --coverage "
%endif


##############################
# Build dali-wrap
##############################

libtoolize --force
cd %{daliSwigDir}
autoreconf --install
NDALI_DATA_RW_DIR="%{ndali_data_rw_dir}" ; export NDALI_DATA_RW_DIR
NDALI_DATA_RO_DIR="%{ndali_data_ro_dir}" ; export NDALI_DATA_RO_DIR
%configure --enable-profile=%{dali_toolkit_profile} \
           --with-style=%{dali_style_folder} \
           --enable-csharp=yes \
%if 0%{?enable_debug}
           --enable-debug \
%endif
           --enable-i18n=yes
cd %{daliPluginsDir}
make gbs-local

##############################
# Installation
##############################
echo "----------------" %{_buildrootdir}
%install
cd "%{daliPluginsDir}"
make DESTDIR=%{_buildrootdir}/%{name}-%{version}-%{release}.arm/usr/lib/ gbs-install

#mv %{_builddir}/%{name}-%{version}/plugins/dali-swig/NDali.dll %{_buildrootdir}/%{name}-%{version}-%{release}.arm/usr/lib/

##############################
# Post Install
##############################
%post


##############################
# Files in Binary Packages
##############################
%files
%manifest dali-csharp-wrapper.manifest
%defattr(-,root,root,-)
%{_libdir}/lib*
%{_libdir}/NDali.dll

