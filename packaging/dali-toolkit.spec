Name:       dali2-toolkit
Summary:    Dali 3D engine Toolkit
Version:    1.9.24
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  gettext
BuildRequires:  pkgconfig(libtzplatform-config)

#############################
# profile setup
#############################

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

%define dali_data_rw_dir            %TZ_SYS_SHARE/dali/
%define dali_data_ro_dir            %TZ_SYS_RO_SHARE/dali/

%define dali_toolkit_image_files    %{dali_data_ro_dir}/toolkit/images/
%define dali_toolkit_sound_files    %{dali_data_ro_dir}/toolkit/sounds/
%define dali_toolkit_style_files    %{dali_data_ro_dir}/toolkit/styles/
%define dev_include_path %{_includedir}

# PO
{
cd %{_builddir}/dali2-toolkit-%{version}/dali-toolkit/po
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
cd %{_builddir}/dali2-toolkit-%{version}/build/tizen

DALI_DATA_RW_DIR="%{dali_data_rw_dir}" ; export DALI_DATA_RW_DIR
DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ; export DALI_DATA_RO_DIR

cmake \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
%if 0%{?enable_trace}
      -DENABLE_TRACE=ON \
%endif
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} \
      -DENABLE_I18N=ON

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install

# PO
{
cd %{_builddir}/dali2-toolkit-%{version}/dali-toolkit/po
for language in *.mo
do
  language=${language%.mo}
  mkdir -p %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-toolkit.mo
done
} &> /dev/null
popd

# Remove default style and style images which are for Linux build
rm -rf %{buildroot}%{dali_toolkit_style_files}/*

# Make folder to contain style and style images
# After making folder, copy local style and style images to new folder
pushd %{_builddir}/%{name}-%{version}
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
popd

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
%{_libdir}/libdali2-toolkit.so*
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/*
%{_libdir}/pkgconfig/dali2-toolkit.pc

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
