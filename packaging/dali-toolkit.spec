# This source builds DALi 3 (dali3-toolkit and its sub-packages). On Tizen
# 10.1 and 11, where applications are still moving from DALi 2.x, it also
# produces the DALi 2.x package names (dali2-toolkit, dali2-scene3d,
# dali2-physics-*, dali2-toolkit-resources_*) as alias packages: links to the
# dali3 files plus the Provides that 2.x binaries and specs depend on. On
# Tizen 10.0 and below the real DALi 2.x is installed from its own source, so
# the aliases must never be built there. --with/--without dali2_aliases
# overrides the default for a local build.
#
# Nested %if, not %elif: Tizen's rpm (4.14) predates %elif and silently
# skips such a branch.
%define legacy_tizen (0%{?tizen_version_major} < 10 || (0%{?tizen_version_major} == 10 && 0%{?tizen_version_minor} == 0))
%if %{legacy_tizen}
%bcond_with    dali2_aliases
%else
%bcond_without dali2_aliases
%endif
%if %{with dali2_aliases} && %{legacy_tizen}
%{error:Tizen <= 10.0 ships the real DALi 2.x; dali2 aliases must not be built there}
%endif

# rpm derives no Provides from a symbolic link, so the alias packages declare
# the ELF ones themselves. 64-bit ELF provides carry a suffix.
%if "%{__isa_bits}" == "64"
%define elf_suffix ()(64bit)
%else
%define elf_suffix %{nil}
%endif

Name:       dali3-toolkit
Summary:    Dali 3D engine Toolkit
Version:    3.0.1
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz


# Use USD loader only if non-riscv64 architecture and tizen_10.0
%{!?enable_usd_loader: %global enable_usd_loader 0}

%ifnarch riscv64
%if 0%{?tizen_version_major} >= 10
%define enable_usd_loader 1
%endif
%endif

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(egl)
BuildRequires:  pkgconfig(dali3-core)
BuildRequires:  pkgconfig(dali3-adaptor)
BuildRequires:  dali3-integration-devel
BuildRequires:  dali3-adaptor-integration-devel
BuildRequires:  python3

BuildRequires:  gettext
BuildRequires:  pkgconfig(libtzplatform-config)

%if 0%{?enable_usd_loader}
BuildRequires:  openusd-devel
%endif

# For ASAN test
%if "%{vd_asan}" == "1" || "%{asan}" == "1"
BuildRequires: asan-force-options
BuildRequires: asan-build-env
BuildRequires: libasan
%endif

#############################
# profile setup
#############################

%description
Dali 3D engine Toolkit - a set of controls that provide
user interface functionality.

##############################
# resource
##############################
%package resources_480x800
Summary:    default resource files for 480x800
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
Conflicts:  %{name}-resources_1920x1080_rpi
%description resources_480x800
dali-toolkit default resource files for 480x800
Contain po / sounds / common images / style / style images

%package resources_720x1280
Summary:    default resource files for 720x1280
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_1920x1080
Conflicts:  %{name}-resources_1920x1080_rpi
%description resources_720x1280
dali-toolkit default resource files for 720x1280
Contain po / sounds / common images / style / style images

%package resources_1920x1080
Summary:    default resource files for 1920x1080
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080_rpi
%description resources_1920x1080
dali-toolkit default resource files for 1920x1080
Contain po / sounds / common images / style / style images

%package resources_1920x1080_rpi
Summary:    default resource files for 1920x1080 on Raspberry Pi 4
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
%description resources_1920x1080_rpi
dali-toolkit default resource files for 1920x1080 on Raspberry Pi 4
Contain po / sounds / common images / style / style images

##############################
# devel
##############################
%package devel
Summary:    Application development package for Dali 3D engine toolkit
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
# control-impl.h includes dali/devel-api headers
Requires:   dali3-integration-devel

%description devel
Application development package for Dali 3D engine toolkit - headers and package config

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for DALi 3D engine toolkit
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-devel = %{version}-%{release}

%description integration-devel
Integration development package for DALi 3D engine toolkit - headers for integrating and features still in development

##############################
# dali-scene3d
##############################
%define dali3_scene3d dali3-scene3d
%package -n %{dali3_scene3d}
Summary:    DLI scene loading library
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   dali3-toolkit

%description -n %{dali3_scene3d}
Provides functionality for loading and displaying DLI format scenes. See README.md for more details.

%package -n %{dali3_scene3d}-devel
Summary:    Development components for dali-scene3d
Group:      Development/Building
Requires:   %{dali3_scene3d} = %{version}-%{release}

%description -n %{dali3_scene3d}-devel
Development components for dali-scene3d.

%package -n %{dali3_scene3d}-integration-devel
Summary:    Integration development package for dali-scene3d
Group:      Development/Building
Requires:   %{dali3_scene3d}-devel = %{version}-%{release}

%description -n %{dali3_scene3d}-integration-devel
Integration development package for dali-scene3d - loader APIs for integrating with model loaders.

##############################
# dali-physics-2d
##############################
%define dali3_physics2d dali3-physics-2d
%package -n %{dali3_physics2d}
Summary:    Physics library 2D
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense

%description -n %{dali3_physics2d}
Provides functionality for 2D physics simulation. See README.md for more details.

%package -n %{dali3_physics2d}-devel
Summary:    Development components for dali3-physics-2d
Group:      Development/Building
Requires:   %{dali3_physics2d} = %{version}-%{release}

%description -n %{dali3_physics2d}-devel
Development components for dali3-physics-2d.

##############################
# dali-physics-3d
##############################
%define dali3_physics3d dali3-physics-3d
%package -n %{dali3_physics3d}
Summary:    Physics library 3D
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense

%description -n %{dali3_physics3d}
Provides functionality for 3D physics simulation. See README.md for more details.

%package -n %{dali3_physics3d}-devel
Summary:    Development components for dali3-physics-3d
Group:      Development/Building
Requires:   %{dali3_physics3d} = %{version}-%{release}

%description -n %{dali3_physics3d}-devel
Development components for dali3-physics-3d.

%if 0%{?enable_usd_loader}
##############################
# dali-usd-loader
##############################
%define dali3_usdloader dali3-usd-loader
%package -n %{dali3_usdloader}
Summary:    USD model loading library
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   %{dali3_scene3d}
Requires:   openusd

%description -n %{dali3_usdloader}
Provides functionality for loading and rendering USD models. See README.md for more details.

%package -n %{dali3_usdloader}-devel
Summary:    Development components for dali-usd-loader
Group:      Development/Building
Requires:   %{dali3_usdloader} = %{version}-%{release}
Requires:   %{dali3_scene3d}-devel
Requires:   %{dali3_scene3d}-integration-devel

%description -n %{dali3_usdloader}-devel
Development components for dali-usd-loader.
%endif

%if %{with dali2_aliases}
##############################
# DALi 2.x names (aliases)
##############################
%package -n dali2-toolkit
Summary:    DALi 2.x library names for the Dali 3D engine Toolkit
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   dali2-adaptor
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides:   libdali2-toolkit.so.2%{elf_suffix}

%description -n dali2-toolkit
libdali2-toolkit.so.2 and libdali2-toolkit-gles.so as links to the dali3 libraries, for binaries built against DALi 2.x. Goes away once the applications have moved to dali3.

%package -n dali2-toolkit-devel
Summary:    DALi 2.x development names for the Dali 3D engine Toolkit
Group:      Development/Building
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   %{name}-devel = %{version}-%{release}
# control-impl.h includes dali/devel-api headers
Requires:   dali2-integration-devel

%description -n dali2-toolkit-devel
include/dali-toolkit, dali2-toolkit.pc and dali-shader-generator as links to their dali3 counterparts, so that a project written for DALi 2.x builds unchanged and links against dali3.

%package -n dali2-toolkit-integration-devel
Summary:    DALi 2.x integration development name for the Dali 3D engine Toolkit
Group:      Development/Building
Requires:   dali2-toolkit-devel = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}

%description -n dali2-toolkit-integration-devel
Name-only package: the headers are reached through the include/dali-toolkit link of dali2-toolkit-devel.

%package -n dali2-scene3d
Summary:    DALi 2.x library name for dali-scene3d
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   %{dali3_scene3d} = %{version}-%{release}
Requires:   dali2-toolkit = %{version}-%{release}
Provides:   libdali2-scene3d.so%{elf_suffix}

%description -n dali2-scene3d
libdali2-scene3d.so as a link to the dali3 library.

%package -n dali2-scene3d-devel
Summary:    DALi 2.x development names for dali-scene3d
Group:      Development/Building
Requires:   dali2-scene3d = %{version}-%{release}
Requires:   %{dali3_scene3d}-devel = %{version}-%{release}

%description -n dali2-scene3d-devel
include/dali-scene3d and dali2-scene3d.pc as links to their dali3 counterparts.

%package -n dali2-scene3d-integration-devel
Summary:    DALi 2.x integration development name for dali-scene3d
Group:      Development/Building
Requires:   dali2-scene3d-devel = %{version}-%{release}
Requires:   %{dali3_scene3d}-integration-devel = %{version}-%{release}

%description -n dali2-scene3d-integration-devel
dali2-scene3d-integration.pc as a link to its dali3 counterpart.

%package -n dali2-physics-2d
Summary:    DALi 2.x library names for dali-physics-2d
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   %{dali3_physics2d} = %{version}-%{release}
Provides:   libdali2-physics-2d.so%{elf_suffix}
Provides:   libchipmunk.so.7%{elf_suffix}

%description -n dali2-physics-2d
libdali2-physics-2d.so, libdali2-physics-2d-gles.so and libchipmunk.so.7 as links to the dali3 libraries.

%package -n dali2-physics-2d-devel
Summary:    DALi 2.x development names for dali-physics-2d
Group:      Development/Building
Requires:   dali2-physics-2d = %{version}-%{release}
Requires:   %{dali3_physics2d}-devel = %{version}-%{release}

%description -n dali2-physics-2d-devel
include/dali-physics, include/chipmunk, dali2-physics-2d.pc and chipmunk2d.pc as links to their dali3 counterparts.

%package -n dali2-physics-3d
Summary:    DALi 2.x library names for dali-physics-3d
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   %{dali3_physics3d} = %{version}-%{release}
Provides:   libdali2-physics-3d.so%{elf_suffix}
Provides:   libbullet3.so.3.2.5%{elf_suffix}

%description -n dali2-physics-3d
libdali2-physics-3d.so, libdali2-physics-3d-gles.so and libbullet3.so.3.2.5 as links to the dali3 libraries.

%package -n dali2-physics-3d-devel
Summary:    DALi 2.x development names for dali-physics-3d
Group:      Development/Building
Requires:   dali2-physics-3d = %{version}-%{release}
Requires:   %{dali3_physics3d}-devel = %{version}-%{release}

%description -n dali2-physics-3d-devel
include/dali-physics, include/bullet, dali2-physics-3d.pc and bullet3.pc as links to their dali3 counterparts.

%if 0%{?enable_usd_loader}
%package -n dali2-usd-loader
Summary:    DALi 2.x library name for dali-usd-loader
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   %{dali3_usdloader} = %{version}-%{release}
Requires:   dali2-scene3d = %{version}-%{release}
Provides:   libdali2-usd-loader.so%{elf_suffix}

%description -n dali2-usd-loader
libdali2-usd-loader.so as a link to the dali3 library.

%package -n dali2-usd-loader-devel
Summary:    DALi 2.x development name for dali-usd-loader
Group:      Development/Building
Requires:   dali2-usd-loader = %{version}-%{release}
Requires:   %{dali3_usdloader}-devel = %{version}-%{release}

%description -n dali2-usd-loader-devel
dali2-usd-loader.pc as a link to its dali3 counterpart.
%endif

##############################
# DALi 2.x resource names (aliases)
##############################
# Name-only packages: the files are reached through the share/dali link of
# dali2-adaptor. Each requires the dali3 package of the same resolution.
%package -n dali2-toolkit-resources_480x800
Summary:    DALi 2.x name for the default resource files for 480x800
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   %{name}-resources_480x800 = %{version}-%{release}
Conflicts:  dali2-toolkit-resources_720x1280
Conflicts:  dali2-toolkit-resources_1920x1080
Conflicts:  dali2-toolkit-resources_1920x1080_rpi
%description -n dali2-toolkit-resources_480x800
Name-only package for the DALi 2.x resource package name; pulls in the dali3 resources.

%package -n dali2-toolkit-resources_720x1280
Summary:    DALi 2.x name for the default resource files for 720x1280
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   %{name}-resources_720x1280 = %{version}-%{release}
Conflicts:  dali2-toolkit-resources_480x800
Conflicts:  dali2-toolkit-resources_1920x1080
Conflicts:  dali2-toolkit-resources_1920x1080_rpi
%description -n dali2-toolkit-resources_720x1280
Name-only package for the DALi 2.x resource package name; pulls in the dali3 resources.

%package -n dali2-toolkit-resources_1920x1080
Summary:    DALi 2.x name for the default resource files for 1920x1080
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   %{name}-resources_1920x1080 = %{version}-%{release}
Conflicts:  dali2-toolkit-resources_480x800
Conflicts:  dali2-toolkit-resources_720x1280
Conflicts:  dali2-toolkit-resources_1920x1080_rpi
%description -n dali2-toolkit-resources_1920x1080
Name-only package for the DALi 2.x resource package name; pulls in the dali3 resources.

%package -n dali2-toolkit-resources_1920x1080_rpi
Summary:    DALi 2.x name for the default resource files for 1920x1080 on Raspberry Pi 4
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   %{name}-resources_1920x1080_rpi = %{version}-%{release}
Conflicts:  dali2-toolkit-resources_480x800
Conflicts:  dali2-toolkit-resources_720x1280
Conflicts:  dali2-toolkit-resources_1920x1080
%description -n dali2-toolkit-resources_1920x1080_rpi
Name-only package for the DALi 2.x resource package name; pulls in the dali3 resources.
%endif

%define dali3_data_rw_dir           %TZ_SYS_SHARE/dali3/
%define dali3_data_ro_dir           %TZ_SYS_RO_SHARE/dali3/

%define dali3_toolkit_image_files   %{dali3_data_ro_dir}/toolkit/images/
%define dali3_toolkit_sound_files   %{dali3_data_ro_dir}/toolkit/sounds/
%define dali3_toolkit_style_files   %{dali3_data_ro_dir}/toolkit/styles/

%define dali_xml_file_dir     %TZ_SYS_RO_PACKAGES

##############################
# Preparation
##############################
%prep
%setup -q

##############################
# Build
##############################
%build
# Keep shadow diagnostics enabled independently of the build server policy.
export CFLAGS="${CFLAGS:-%optflags} -Werror=shadow"
export CXXFLAGS="${CXXFLAGS:-%optflags} -Werror=shadow"

# PO
{
cd %{_builddir}/%{name}-%{version}/dali-toolkit/po
for language in *.po
do
  language=${language%.po}
  msgfmt -o ${language}.mo ${language}.po
done
} > /dev/null 2>&1

PREFIX="/usr"
CXXFLAGS="$CXXFLAGS -Wall -g -Os -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS="$LDFLAGS -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -Wl,-Bsymbolic-functions "

%if 0%{?enable_coverage}
CXXFLAGS="$CXXFLAGS --coverage "
LDFLAGS="$LDFLAGS --coverage "
%endif

%ifarch %{arm}
CXXFLAGS="$CXXFLAGS -D_ARCH_ARM_"
%endif

%if "%{vd_asan}" == "1" || "%{asan}" == "1"
CFLAGS="$CFLAGS -fsanitize=address"
CXXFLAGS="$CXXFLAGS -fsanitize=address"
LDFLAGS="$LDFLAGS -fsanitize=address"
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/tizen

# The data directories go in as cache variables, not the environment, so
# that a re-run of cmake inside the build directory keeps them.
cmake -DDALI_DATA_RW_DIR="%{dali3_data_rw_dir}" \
      -DDALI_DATA_RO_DIR="%{dali3_data_ro_dir}" \
      -DDALI_LEGACY_ALIASES=%{?with_dali2_aliases:ON}%{!?with_dali2_aliases:OFF} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DENABLE_TRACE=ON \
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} \
      -DENABLE_I18N=ON \
      -DCP_USE_DOUBLES=ON \
      .

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install

# Translations and the per-resolution style folders are not part of the CMake
# install. The gettext domain and the data directory carry the API major (see
# build/tizen/CMakeLists.txt).
install_resources() {
  local domain=$1
  local styledir=$2
  local src=%{_builddir}/%{name}-%{version}/dali-toolkit
  local mo lang res

  # PO
  for mo in ${src}/po/*.mo
  do
    [ -e "$mo" ] || continue
    lang=$(basename "$mo" .mo)
    mkdir -p %{buildroot}%{_datadir}/locale/${lang}/LC_MESSAGES/
    cp "$mo" %{buildroot}%{_datadir}/locale/${lang}/LC_MESSAGES/${domain}.mo
  done

  # Remove default style and style images which are for Linux build
  rm -rf %{buildroot}${styledir}/*

  # Make folder to contain style and style images
  # After making folder, copy local style and style images to new folder
  for res in 480x800 720x1280 1920x1080 1920x1080_rpi
  do
    mkdir -p %{buildroot}${styledir}/${res}
    cp -r ${src}/styles/${res}/* %{buildroot}${styledir}/${res}
  done

  # Copy default feedback theme
  cp ${src}/styles/default-feedback-theme.json %{buildroot}${styledir}

  # Copy default debug image visual shader script
  cp ${src}/styles/debug-image-visual-shader-script.json %{buildroot}${styledir}
}

install_resources dali3-toolkit %{dali3_toolkit_style_files}

##############################
# Pre Install
##############################
%pre resources_480x800
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_720x1280
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_1920x1080
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_1920x1080_rpi
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%post resources_480x800
cd %{dali3_toolkit_style_files}/480x800 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_720x1280
cd %{dali3_toolkit_style_files}/720x1280 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_1920x1080
cd %{dali3_toolkit_style_files}/1920x1080 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_1920x1080_rpi
cd %{dali3_toolkit_style_files}/1920x1080_rpi || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%if 0%{?enable_usd_loader}
%post -n %{dali3_usdloader}
/sbin/ldconfig
exit 0
%endif

%if %{with dali2_aliases}
%post -n dali2-toolkit
/sbin/ldconfig
exit 0

%postun -n dali2-toolkit
/sbin/ldconfig
exit 0
%endif

##############################
# Pre Uninstall
##############################
%preun resources_480x800
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    mv images ./480x800
    mv dali-toolkit-default-theme.json ./480x800
  ;;
esac

%preun resources_720x1280
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    mv images ./720x1280
    mv dali-toolkit-default-theme.json ./720x1280
  ;;
esac

%preun resources_1920x1080
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    mv images ./1920x1080
    mv dali-toolkit-default-theme.json ./1920x1080
  ;;
esac

%preun resources_1920x1080_rpi
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    mv images ./1920x1080_rpi
    mv dali-toolkit-default-theme.json ./1920x1080_rpi
  ;;
esac

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

%postun resources_480x800
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_720x1280
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_1920x1080
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_1920x1080_rpi
case "$1" in
  0)
    cd %{dali3_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%if 0%{?enable_usd_loader}
%postun -n %{dali3_usdloader}
/sbin/ldconfig
exit 0
%endif

##############################
# Files in Binary Packages
##############################
# The Lua scriptlets run up to the next section, so nothing but another
# scriptlet or %files may follow them. Every earlier package (DALi 2.x and
# dali3 3.0.1 alike) installed these include directories as real
# directories; rpm cannot replace a directory with a link, so the alias
# packages clear them before the transaction unpacks.
%if %{with dali2_aliases}
%pretrans -n dali2-toolkit-devel -p <lua>
local path = "%{_includedir}/dali-toolkit"
if posix.stat(path, "type") == "directory" then
  os.execute("rm -rf '" .. path .. "'")
end
%pretrans -n dali2-scene3d-devel -p <lua>
local path = "%{_includedir}/dali-scene3d"
if posix.stat(path, "type") == "directory" then
  os.execute("rm -rf '" .. path .. "'")
end
%pretrans -n dali2-physics-2d-devel -p <lua>
for _, path in ipairs({"%{_includedir}/dali-physics", "%{_includedir}/chipmunk"}) do
  if posix.stat(path, "type") == "directory" then
    os.execute("rm -rf '" .. path .. "'")
  end
end
%pretrans -n dali2-physics-3d-devel -p <lua>
for _, path in ipairs({"%{_includedir}/dali-physics", "%{_includedir}/bullet"}) do
  if posix.stat(path, "type") == "directory" then
    os.execute("rm -rf '" .. path .. "'")
  end
end
%endif
%files
%if 0%{?enable_dali_smack_rules}
%manifest dali-toolkit.manifest-smack
%else
%manifest dali-toolkit.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali3-toolkit.so*
%{_libdir}/libdali3-toolkit-gles.so*
%license LICENSE
%license LICENSE.MIT

%files devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-toolkit/doc/*
%{_includedir}/dali3/dali-toolkit/public-api/*
%{_includedir}/dali3/dali-toolkit/dali-toolkit.h
%{_libdir}/pkgconfig/dali3-toolkit.pc
%{_bindir}/dali3-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-toolkit/devel-api/*

%files resources_480x800
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali3_toolkit_image_files}/*
%{dali3_toolkit_sound_files}/*
%{dali3_toolkit_style_files}/480x800/*
%{dali3_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali3_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/dali3-toolkit.mo

%files resources_720x1280
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali3_toolkit_image_files}/*
%{dali3_toolkit_sound_files}/*
%{dali3_toolkit_style_files}/720x1280/*
%{dali3_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali3_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/dali3-toolkit.mo

%files resources_1920x1080
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali3_toolkit_image_files}/*
%{dali3_toolkit_sound_files}/*
%{dali3_toolkit_style_files}/1920x1080/*
%{dali3_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali3_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/dali3-toolkit.mo

%files resources_1920x1080_rpi
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali3_toolkit_image_files}/*
%{dali3_toolkit_sound_files}/*
%{dali3_toolkit_style_files}/1920x1080_rpi/*
%{dali3_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali3_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/dali3-toolkit.mo

%files -n %{dali3_scene3d}
%if 0%{?enable_dali_smack_rules}
%manifest dali-scene3d.manifest-smack
%else
%manifest dali-scene3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{dali3_scene3d}.so
%license LICENSE
%license LICENSE.Unlicense

%files -n %{dali3_scene3d}-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-scene3d/public-api/*
%{_includedir}/dali3/dali-scene3d/dali-scene3d.h
%{_libdir}/pkgconfig/dali3-scene3d.pc

%files -n %{dali3_scene3d}-integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-scene3d/integration-api/*
%{_libdir}/pkgconfig/dali3-scene3d-integration.pc

%files -n %{dali3_physics2d}
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-2d.manifest-smack
%else
%manifest dali-physics-2d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali3-chipmunk.so*
%{_libdir}/libdali3-physics-2d.so*
%{_libdir}/libdali3-physics-2d-gles.so*
%license LICENSE
%license LICENSE.MIT

%files -n %{dali3_physics2d}-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-physics/integration-api/*
%{_includedir}/dali3/dali-physics/public-api/*
%{_includedir}/dali3/dali-physics/dali-physics.h
%{_includedir}/dali3/chipmunk/*
%{_libdir}/pkgconfig/dali3-chipmunk2d.pc
%{_libdir}/pkgconfig/dali3-physics-2d.pc

%files -n %{dali3_physics3d}
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-3d.manifest-smack
%else
%manifest dali-physics-3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali3-bullet3.so*
%{_libdir}/libdali3-physics-3d.so*
%{_libdir}/libdali3-physics-3d-gles.so*
%license LICENSE
%license LICENSE.Zlib

%files -n %{dali3_physics3d}-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali-physics/integration-api/*
%{_includedir}/dali3/dali-physics/public-api/*
%{_includedir}/dali3/dali-physics/dali-physics.h
%{_includedir}/dali3/bullet/*
%{_libdir}/pkgconfig/dali3-physics-3d.pc
%{_libdir}/pkgconfig/dali3-bullet3.pc

%if 0%{?enable_usd_loader}
%files -n %{dali3_usdloader}
%if 0%{?enable_dali_smack_rules}
%manifest dali-usd-loader.manifest-smack
%else
%manifest dali-usd-loader.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/lib%{dali3_usdloader}.so
%license LICENSE

%files -n %{dali3_usdloader}-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali3-usd-loader.pc
%endif

%if %{with dali2_aliases}
%files -n dali2-toolkit
%if 0%{?enable_dali_smack_rules}
%manifest dali-toolkit.manifest-smack
%else
%manifest dali-toolkit.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-toolkit.so.2
%{_libdir}/libdali2-toolkit-gles.so
%license LICENSE

%files -n dali2-toolkit-devel
%defattr(-,root,root,-)
%{_includedir}/dali-toolkit
%{_libdir}/libdali2-toolkit.so
%{_libdir}/pkgconfig/dali2-toolkit.pc
%{_bindir}/dali-shader-generator

%files -n dali2-toolkit-integration-devel
%defattr(-,root,root,-)

%files -n dali2-scene3d
%if 0%{?enable_dali_smack_rules}
%manifest dali-scene3d.manifest-smack
%else
%manifest dali-scene3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-scene3d.so
%license LICENSE

%files -n dali2-scene3d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-scene3d
%{_libdir}/pkgconfig/dali2-scene3d.pc

%files -n dali2-scene3d-integration-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-scene3d-integration.pc

%files -n dali2-physics-2d
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-2d.manifest-smack
%else
%manifest dali-physics-2d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libchipmunk.so.7
%{_libdir}/libdali2-physics-2d.so
%{_libdir}/libdali2-physics-2d-gles.so
%license LICENSE

%files -n dali2-physics-2d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics
%{_includedir}/chipmunk
%{_libdir}/libchipmunk.so
%{_libdir}/pkgconfig/chipmunk2d.pc
%{_libdir}/pkgconfig/dali2-physics-2d.pc

%files -n dali2-physics-3d
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-3d.manifest-smack
%else
%manifest dali-physics-3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libbullet3.so.3.2.5
%{_libdir}/libdali2-physics-3d.so
%{_libdir}/libdali2-physics-3d-gles.so
%license LICENSE

%files -n dali2-physics-3d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics
%{_includedir}/bullet
%{_libdir}/libbullet3.so
%{_libdir}/pkgconfig/dali2-physics-3d.pc
%{_libdir}/pkgconfig/bullet3.pc

%if 0%{?enable_usd_loader}
%files -n dali2-usd-loader
%if 0%{?enable_dali_smack_rules}
%manifest dali-usd-loader.manifest-smack
%else
%manifest dali-usd-loader.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-usd-loader.so
%license LICENSE

%files -n dali2-usd-loader-devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali2-usd-loader.pc
%endif

%files -n dali2-toolkit-resources_480x800
%defattr(-,root,root,-)

%files -n dali2-toolkit-resources_720x1280
%defattr(-,root,root,-)

%files -n dali2-toolkit-resources_1920x1080
%defattr(-,root,root,-)

%files -n dali2-toolkit-resources_1920x1080_rpi
%defattr(-,root,root,-)
%endif
