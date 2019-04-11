Name:       dali-toolkit
Summary:    Dali 3D engine Toolkit
Version:    1.4.14
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%if 0%{?tizen_version_major} < 4
%define disable_cxx03_build 1
%endif

BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali-core)
BuildRequires:  pkgconfig(dali-adaptor)
%if !0%{?disable_cxx03_build}
BuildRequires:  pkgconfig(dali-core-cxx03)
BuildRequires:  pkgconfig(dali-adaptor-cxx03)
%endif
BuildRequires: gettext


#need libtzplatform-config for directory if tizen version is 3.x

%if 0%{?tizen_version_major} >= 3
BuildRequires:  pkgconfig(libtzplatform-config)
%endif

#############################
# profile setup
#############################

%if "%{tizen_version_major}" == "2" && 0%{?tizen_profile_name:1}
%define profile %{tizen_profile_name}
%endif

%description
Dali 3D engine Toolkit - a set of controls that provide
user interface functionality.

##############################
# resource
##############################
%package resources_360x360
Summary:    default resource files for 360x360
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
%description resources_360x360
dali-toolkit default resource files for 360x360
Contain po / sounds / common images / style / style images

%package resources_480x800
Summary:    default resource files for 480x800
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_360x360
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
%description resources_480x800
dali-toolkit default resource files for 480x800
Contain po / sounds / common images / style / style images

%package resources_720x1280
Summary:    default resource files for 720x1280
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_360x360
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_1920x1080
%description resources_720x1280
dali-toolkit default resource files for 720x1280
Contain po / sounds / common images / style / style images

%package resources_1920x1080
Summary:    default resource files for 1920x1080
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_360x360
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
%description resources_1920x1080
dali-toolkit default resource files for 1920x1080
Contain po / sounds / common images / style / style images

%if !0%{?disable_cxx03_build}
%package cxx03
Summary:	Dali 3D engine Toolkit with cxx03
Provides:	%{name}-cxx03 = %{version}-%{release}

%description cxx03
Dali 3D engine Toolkit with cxx03
%endif

##############################
# devel
##############################
%package devel
Summary:    Application development package for Dali 3D engine toolkit
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description devel
Application development package for Dali 3D engine toolkit - headers and package config

##############################
# Preparation
##############################
%prep
%setup -q

#Use TZ_PATH when tizen version is 3.x

%if "%{tizen_version_major}" == "2"
%define dali_data_rw_dir            /opt/usr/share/dali/
%define dali_data_ro_dir            /usr/share/dali/
%else
%define dali_data_rw_dir            %TZ_SYS_SHARE/dali/
%define dali_data_ro_dir            %TZ_SYS_RO_SHARE/dali/
%endif

%define dali_toolkit_image_files    %{dali_data_ro_dir}/toolkit/images/
%define dali_toolkit_sound_files    %{dali_data_ro_dir}/toolkit/sounds/
%define dali_toolkit_style_files    %{dali_data_ro_dir}/toolkit/styles/
%define dev_include_path %{_includedir}

# PO
{
cd %{_builddir}/dali-toolkit-%{version}/dali-toolkit/po
for language in *.po
do
  language=${language%.po}
  msgfmt -o ${language}.mo ${language}.po
done
} &> /dev/null

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -Wl,-Bsymbolic-functions "

%if 0%{?enable_coverage}
CXXFLAGS+=" --coverage "
LDFLAGS+=" --coverage "
%endif

libtoolize --force
cd %{_builddir}/dali-toolkit-%{version}/build/tizen
autoreconf --install
DALI_DATA_RW_DIR="%{dali_data_rw_dir}" ; export DALI_DATA_RW_DIR
DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ; export DALI_DATA_RO_DIR

%configure --enable-profile=TIZEN \
%if 0%{?enable_debug}
           --enable-debug \
%endif
%if 0%{?enable_trace}
      --enable-trace \
%endif
           --enable-i18n=yes \
           --enable-rename-so=no

make %{?jobs:-j%jobs}

pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"
popd

pushd %{buildroot}%{_libdir}
for FILE in libdali-toolkit-cxx11.so*; do mv "$FILE" "%{_builddir}/%{name}-%{version}/build/tizen/$FILE"; done
mv pkgconfig/dali-toolkit.pc %{_builddir}/%{name}-%{version}/build/tizen/dali-toolkit.pc
popd

%if !0%{?disable_cxx03_build}
make clean

libtoolize --force
cd %{_builddir}/dali-toolkit-%{version}/build/tizen
autoreconf --install

DALI_DATA_RW_DIR="%{dali_data_rw_dir}" ; export DALI_DATA_RW_DIR
DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ; export DALI_DATA_RO_DIR

%configure --enable-profile=TIZEN \
           --enable-cxx03-abi=yes \
%if 0%{?enable_debug}
           --enable-debug \
%endif
%if 0%{?enable_trace}
      --enable-trace \
%endif
           --enable-i18n=yes \
           --enable-rename-so=no

make %{?jobs:-j%jobs}
%endif

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

for FILE in libdali-toolkit-cxx11.so*; do mv "$FILE" "%{buildroot}%{_libdir}/$FILE"; done
mv dali-toolkit.pc %{buildroot}%{_libdir}/pkgconfig/dali-toolkit.pc

# PO
{
cd %{_builddir}/dali-toolkit-%{version}/dali-toolkit/po
for language in *.mo
do
  language=${language%.mo}
  mkdir -p %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-toolkit.mo
done
} &> /dev/null
popd

#############################
#rename
#############################
pushd  %{buildroot}%{_libdir}
rm -rf libdali-toolkit.so
rm -rf libdali-toolkit-cxx11.so
%if !0%{?disable_cxx03_build}
ln -s libdali-toolkit.so.0.0.* libdali-toolkit-cxx03.so
%endif
ln -s libdali-toolkit-cxx11.so.0.0.* libdali-toolkit.so
popd

# Remove default style and style images which are for Linux build
rm -rf %{buildroot}%{dali_toolkit_style_files}/*

# Make folder to contain style and style images
# After making folder, copy local style and style images to new folder
mkdir -p %{buildroot}%{dali_toolkit_style_files}/360x360
cp -r dali-toolkit/styles/360x360/* %{buildroot}%{dali_toolkit_style_files}/360x360
mkdir -p %{buildroot}%{dali_toolkit_style_files}/480x800
cp -r dali-toolkit/styles/480x800/* %{buildroot}%{dali_toolkit_style_files}/480x800
mkdir -p %{buildroot}%{dali_toolkit_style_files}/720x1280
cp -r dali-toolkit/styles/720x1280/* %{buildroot}%{dali_toolkit_style_files}/720x1280
mkdir -p %{buildroot}%{dali_toolkit_style_files}/1920x1080
cp -r dali-toolkit/styles/1920x1080/* %{buildroot}%{dali_toolkit_style_files}/1920x1080

# Copy default feedback theme
cp dali-toolkit/styles/default-feedback-theme.json %{buildroot}%{dali_toolkit_style_files}

##############################
# Pre Install
##############################

%pre resources_360x360
case "$1" in
  2)
    pushd %{dali_toolkit_style_files}
    rm -rf ./*
    popd
  ;;
esac

%pre resources_480x800
case "$1" in
  2)
    pushd %{dali_toolkit_style_files}
    rm -rf ./*
    popd
  ;;
esac

%pre resources_720x1280
case "$1" in
  2)
    pushd %{dali_toolkit_style_files}
    rm -rf ./*
    popd
  ;;
esac

%pre resources_1920x1080
case "$1" in
  2)
    pushd %{dali_toolkit_style_files}
    rm -rf ./*
    popd
  ;;
esac

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%post resources_360x360
pushd %{dali_toolkit_style_files}/360x360
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

%post resources_480x800
pushd %{dali_toolkit_style_files}/480x800
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

%post resources_720x1280
pushd %{dali_toolkit_style_files}/720x1280
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

%post resources_1920x1080
pushd %{dali_toolkit_style_files}/1920x1080
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

##############################
# Pre Uninstall
##############################

%preun resources_360x360
case "$1" in
  0)
    %preun resources_360x360
    pushd %{dali_toolkit_style_files}
    mv images ./360x360
    mv dali-toolkit-default-theme.json ./360x360
    popd
  ;;
esac

%preun resources_480x800
case "$1" in
  0)
    %preun resources_480x800
    pushd %{dali_toolkit_style_files}
    mv images ./480x800
    mv dali-toolkit-default-theme.json ./480x800
    popd
  ;;
esac

%preun resources_720x1280
case "$1" in
  0)
    %preun resources_720x1280
    pushd %{dali_toolkit_style_files}
    mv images ./720x1280
    mv dali-toolkit-default-theme.json ./720x1280
    popd
  ;;
esac

%preun resources_1920x1080
case "$1" in
  0)
    %preun resources_1920x1080
    pushd %{dali_toolkit_style_files}
    mv images ./1920x1080
    mv dali-toolkit-default-theme.json ./1920x1080
    popd
  ;;
esac

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

%postun resources_360x360
case "$1" in
  0)
    pushd %{dali_toolkit_style_files}
    rm -rf *
    popd
  ;;
esac

%postun resources_480x800
case "$1" in
  0)
    pushd %{dali_toolkit_style_files}
    rm -rf *
    popd
  ;;
esac

%postun resources_720x1280
case "$1" in
  0)
    pushd %{dali_toolkit_style_files}
    rm -rf *
    popd
  ;;
esac

%postun resources_1920x1080
case "$1" in
  0)
    pushd %{dali_toolkit_style_files}
    rm -rf *
    popd
  ;;
esac

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
%{_libdir}/libdali-toolkit-cxx11.so.*
%{_libdir}/libdali-toolkit.so
%license LICENSE

%if !0%{?disable_cxx03_build}
%files cxx03
%if 0%{?enable_dali_smack_rules}
%manifest dali-toolkit.manifest-smack
%else
%manifest dali-toolkit.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali-toolkit.so.*
%{_libdir}/libdali-toolkit-cxx03.so
%license LICENSE
%endif

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/*
%{_libdir}/pkgconfig/dali-toolkit.pc

%files resources_360x360
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/360x360/*
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_480x800
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/480x800/*
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_720x1280
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/720x1280/*
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_1920x1080
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/1920x1080/*
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*
