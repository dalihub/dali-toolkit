# dali2 is the same code built a second time under the dali2 name, for
# applications that still use it. To drop it, remove the "with dali2" blocks.
%bcond_without dali2

Name:       dali3-toolkit
Summary:    Dali 3D engine Toolkit
Version:    2.5.45
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
BuildRequires:  pkgconfig(dali3-core)
BuildRequires:  pkgconfig(dali3-adaptor)
BuildRequires:  pkgconfig(gles20)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(egl)
BuildRequires:  dali3-integration-devel
BuildRequires:  dali3-adaptor-integration-devel
%if %{with dali2}
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
%endif
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
%if %{with dali2}
# Both toolkits read the same resource files, so dali3 takes over the dali2 resources.
Provides:   dali2-toolkit-resources_480x800 = %{version}-%{release}
Obsoletes:  dali2-toolkit-resources_480x800 < %{version}-%{release}
%endif
%description resources_480x800
dali-toolkit default resource files for 480x800
Contain po / sounds / common images / style / style images

%package resources_720x1280
Summary:    default resource files for 720x1280
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_1920x1080
Conflicts:  %{name}-resources_1920x1080_rpi
%if %{with dali2}
# Both toolkits read the same resource files, so dali3 takes over the dali2 resources.
Provides:   dali2-toolkit-resources_720x1280 = %{version}-%{release}
Obsoletes:  dali2-toolkit-resources_720x1280 < %{version}-%{release}
%endif
%description resources_720x1280
dali-toolkit default resource files for 720x1280
Contain po / sounds / common images / style / style images

%package resources_1920x1080
Summary:    default resource files for 1920x1080
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080_rpi
%if %{with dali2}
# Both toolkits read the same resource files, so dali3 takes over the dali2 resources.
Provides:   dali2-toolkit-resources_1920x1080 = %{version}-%{release}
Obsoletes:  dali2-toolkit-resources_1920x1080 < %{version}-%{release}
%endif
%description resources_1920x1080
dali-toolkit default resource files for 1920x1080
Contain po / sounds / common images / style / style images

%package resources_1920x1080_rpi
Summary:    default resource files for 1920x1080 on Raspberry Pi 4
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
%if %{with dali2}
# Both toolkits read the same resource files, so dali3 takes over the dali2 resources.
Provides:   dali2-toolkit-resources_1920x1080_rpi = %{version}-%{release}
Obsoletes:  dali2-toolkit-resources_1920x1080_rpi < %{version}-%{release}
%endif
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

%if %{with dali2}
##############################
# dali2
##############################
%package -n dali2-toolkit
Summary:    Dali 3D engine Toolkit
Group:      System/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n dali2-toolkit
Dali 3D engine Toolkit - the same code as %{name}, under the dali2 name

%package -n dali2-toolkit-devel
Summary:    Application development package for Dali 3D engine toolkit
Group:      Development/Building
Requires:   dali2-toolkit = %{version}-%{release}
# control-impl.h includes dali/devel-api headers
Requires:   dali2-integration-devel

%description -n dali2-toolkit-devel
Application development package for Dali 3D engine toolkit - headers and package config

%package -n dali2-toolkit-integration-devel
Summary:    Integration development package for DALi 3D engine toolkit
Group:      Development/Building
Requires:   dali2-toolkit = %{version}-%{release}
Requires:   dali2-toolkit-devel = %{version}-%{release}

%description -n dali2-toolkit-integration-devel
Integration development package for DALi 3D engine toolkit - headers for integrating and features still in development

%package -n dali2-scene3d
Summary:    DLI scene loading library
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   dali2-toolkit

%description -n dali2-scene3d
Provides functionality for loading and displaying DLI format scenes. See README.md for more details.

%package -n dali2-scene3d-devel
Summary:    Development components for dali-scene3d
Group:      Development/Building
Requires:   dali2-scene3d = %{version}-%{release}

%description -n dali2-scene3d-devel
Development components for dali-scene3d.

%package -n dali2-scene3d-integration-devel
Summary:    Integration development package for dali-scene3d
Group:      Development/Building
Requires:   dali2-scene3d-devel = %{version}-%{release}

%description -n dali2-scene3d-integration-devel
Integration development package for dali-scene3d - loader APIs for integrating with model loaders.

%package -n dali2-physics-2d
Summary:    Physics library 2D
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense

%description -n dali2-physics-2d
Provides functionality for 2D physics simulation. See README.md for more details.

%package -n dali2-physics-2d-devel
Summary:    Development components for dali2-physics-2d
Group:      Development/Building
Requires:   dali2-physics-2d = %{version}-%{release}

%description -n dali2-physics-2d-devel
Development components for dali2-physics-2d.

%package -n dali2-physics-3d
Summary:    Physics library 3D
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense

%description -n dali2-physics-3d
Provides functionality for 3D physics simulation. See README.md for more details.

%package -n dali2-physics-3d-devel
Summary:    Development components for dali2-physics-3d
Group:      Development/Building
Requires:   dali2-physics-3d = %{version}-%{release}

%description -n dali2-physics-3d-devel
Development components for dali2-physics-3d.

%if 0%{?enable_usd_loader}
%package -n dali2-usd-loader
Summary:    USD model loading library
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT and Zlib and Unlicense
Requires:   dali2-scene3d
Requires:   openusd

%description -n dali2-usd-loader
Provides functionality for loading and rendering USD models. See README.md for more details.

%package -n dali2-usd-loader-devel
Summary:    Development components for dali-usd-loader
Group:      Development/Building
Requires:   dali2-usd-loader = %{version}-%{release}
Requires:   dali2-scene3d-devel
Requires:   dali2-scene3d-integration-devel

%description -n dali2-usd-loader-devel
Development components for dali-usd-loader.
%endif
%endif

%define dali_data_rw_dir            %TZ_SYS_SHARE/dali/
%define dali_data_ro_dir            %TZ_SYS_RO_SHARE/dali/

%define dali_toolkit_image_files    %{dali_data_ro_dir}/toolkit/images/
%define dali_toolkit_sound_files    %{dali_data_ro_dir}/toolkit/sounds/
%define dali_toolkit_style_files    %{dali_data_ro_dir}/toolkit/styles/
%define dev_include_path %{_includedir}

%define dali_xml_file_dir     %TZ_SYS_RO_PACKAGES

%if %{with dali2}
%define dali2_build_dir %{_builddir}/%{name}-%{version}/_build/dali2
%endif

##############################
# Preparation
##############################
%prep
%setup -q

##############################
# Build
##############################
%build
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

DALI_DATA_RW_DIR="%{dali_data_rw_dir}" ; export DALI_DATA_RW_DIR
DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ; export DALI_DATA_RO_DIR

cmake \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
%if 0%{?enable_gpu_memory_profile}
      -DENABLE_GPU_MEMORY_PROFILE=ON \
%endif
      -DENABLE_TRACE=ON \
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} \
      -DENABLE_I18N=ON \
      -DCP_USE_DOUBLES=ON \
      .

make %{?jobs:-j%jobs}

%if %{with dali2}
# Keep the options the same as dali3 above; only DALI_API_MAJOR differs.
# -S and -B are required: a plain source path would reuse the dali3 in-source cache.
cmake -S . -B %{dali2_build_dir} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
%if 0%{?enable_gpu_memory_profile}
      -DENABLE_GPU_MEMORY_PROFILE=ON \
%endif
      -DDALI_API_MAJOR=2 \
      -DENABLE_TRACE=ON \
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} \
      -DENABLE_I18N=ON \
      -DCP_USE_DOUBLES=ON

make -C %{dali2_build_dir} %{?jobs:-j%jobs}

# The dali2 build rewrites the generated shader sources in the source tree,
# so bring the dali3 build up to date again here rather than at install time.
make %{?jobs:-j%jobs}
%endif

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

_saved_dir=$(pwd)
cd %{_builddir}/%{name}-%{version}/build/tizen
%make_install

%if %{with dali2}
cd %{dali2_build_dir}
%make_install
%endif

# PO
{
cd %{_builddir}/%{name}-%{version}/dali-toolkit/po
for language in *.mo
do
  language=${language%.mo}
  mkdir -p %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/
  cp ${language}.mo %{buildroot}/%{_datadir}/locale/${language}/LC_MESSAGES/dali-toolkit.mo
done
} > /dev/null 2>&1
cd "$_saved_dir"

# Remove default style and style images which are for Linux build
rm -rf %{buildroot}%{dali_toolkit_style_files}/*

# Make folder to contain style and style images
# After making folder, copy local style and style images to new folder
_saved_dir=$(pwd)
cd %{_builddir}/%{name}-%{version}
mkdir -p %{buildroot}%{dali_toolkit_style_files}/480x800
cp -r dali-toolkit/styles/480x800/* %{buildroot}%{dali_toolkit_style_files}/480x800
mkdir -p %{buildroot}%{dali_toolkit_style_files}/720x1280
cp -r dali-toolkit/styles/720x1280/* %{buildroot}%{dali_toolkit_style_files}/720x1280
mkdir -p %{buildroot}%{dali_toolkit_style_files}/1920x1080
cp -r dali-toolkit/styles/1920x1080/* %{buildroot}%{dali_toolkit_style_files}/1920x1080
mkdir -p %{buildroot}%{dali_toolkit_style_files}/1920x1080_rpi
cp -r dali-toolkit/styles/1920x1080_rpi/* %{buildroot}%{dali_toolkit_style_files}/1920x1080_rpi

# Copy default feedback theme
cp dali-toolkit/styles/default-feedback-theme.json %{buildroot}%{dali_toolkit_style_files}

# Copy default debug image visual shader script
cp dali-toolkit/styles/debug-image-visual-shader-script.json %{buildroot}%{dali_toolkit_style_files}
cd "$_saved_dir"

##############################
# Pre Install
##############################

%pre resources_480x800
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_720x1280
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_1920x1080
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
    cd "$_saved_dir"
  ;;
esac

%pre resources_1920x1080_rpi
case "$1" in
  2)
    _saved_dir=$(pwd)
    cd %{dali_toolkit_style_files} || exit 0
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
cd %{dali_toolkit_style_files}/480x800 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_720x1280
cd %{dali_toolkit_style_files}/720x1280 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_1920x1080
cd %{dali_toolkit_style_files}/1920x1080 || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%post resources_1920x1080_rpi
cd %{dali_toolkit_style_files}/1920x1080_rpi || exit 0
for FILE in *; do rm -rf ../"${FILE}"; mv ./"${FILE}" ../"${FILE}"; done

%if 0%{?enable_usd_loader}
%post -n %{dali3_usdloader}
/sbin/ldconfig
exit 0
%endif

%if %{with dali2}
%post -n dali2-toolkit
/sbin/ldconfig
exit 0

%if 0%{?enable_usd_loader}
%post -n dali2-usd-loader
/sbin/ldconfig
exit 0
%endif
%endif

##############################
# Pre Uninstall
##############################

%preun resources_480x800
case "$1" in
  0)
    %preun resources_480x800
    cd %{dali_toolkit_style_files} || exit 0
    mv images ./480x800
    mv dali-toolkit-default-theme.json ./480x800
  ;;
esac

%preun resources_720x1280
case "$1" in
  0)
    %preun resources_720x1280
    cd %{dali_toolkit_style_files} || exit 0
    mv images ./720x1280
    mv dali-toolkit-default-theme.json ./720x1280
  ;;
esac

%preun resources_1920x1080
case "$1" in
  0)
    %preun resources_1920x1080
    cd %{dali_toolkit_style_files} || exit 0
    mv images ./1920x1080
    mv dali-toolkit-default-theme.json ./1920x1080
  ;;
esac

%preun resources_1920x1080_rpi
case "$1" in
  0)
    %preun resources_1920x1080_rpi
    cd %{dali_toolkit_style_files} || exit 0
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
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_720x1280
case "$1" in
  0)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_1920x1080
case "$1" in
  0)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%postun resources_1920x1080_rpi
case "$1" in
  0)
    cd %{dali_toolkit_style_files} || exit 0
    rm -rf ./*
  ;;
esac

%if %{with dali2}
##############################
# Replacing the dali2 resources
##############################
# A dali2 resource package erased because this package replaces it runs its
# uninstall scripts, which clear the style folder. Keep the folder aside while
# it is erased.
%triggerun resources_480x800 -- dali2-toolkit-resources_480x800
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles" ]; then mv "$styles" "$styles.dali3-keep"; fi
exit 0

%triggerpostun resources_480x800 -- dali2-toolkit-resources_480x800
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles.dali3-keep" ]; then rm -rf "$styles"; mv "$styles.dali3-keep" "$styles"; fi
exit 0

%triggerun resources_720x1280 -- dali2-toolkit-resources_720x1280
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles" ]; then mv "$styles" "$styles.dali3-keep"; fi
exit 0

%triggerpostun resources_720x1280 -- dali2-toolkit-resources_720x1280
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles.dali3-keep" ]; then rm -rf "$styles"; mv "$styles.dali3-keep" "$styles"; fi
exit 0

%triggerun resources_1920x1080 -- dali2-toolkit-resources_1920x1080
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles" ]; then mv "$styles" "$styles.dali3-keep"; fi
exit 0

%triggerpostun resources_1920x1080 -- dali2-toolkit-resources_1920x1080
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles.dali3-keep" ]; then rm -rf "$styles"; mv "$styles.dali3-keep" "$styles"; fi
exit 0

%triggerun resources_1920x1080_rpi -- dali2-toolkit-resources_1920x1080_rpi
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles" ]; then mv "$styles" "$styles.dali3-keep"; fi
exit 0

%triggerpostun resources_1920x1080_rpi -- dali2-toolkit-resources_1920x1080_rpi
styles=%{dali_toolkit_style_files}
styles=${styles%/}
if [ "$2" = 0 ] && [ -d "$styles.dali3-keep" ]; then rm -rf "$styles"; mv "$styles.dali3-keep" "$styles"; fi
exit 0
%endif

%if 0%{?enable_usd_loader}
%postun -n %{dali3_usdloader}
/sbin/ldconfig
exit 0
%endif

%if %{with dali2}
%postun -n dali2-toolkit
/sbin/ldconfig
exit 0

%if 0%{?enable_usd_loader}
%postun -n dali2-usd-loader
/sbin/ldconfig
exit 0
%endif
%endif

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
%{_libdir}/libdali3-toolkit.so*
%{_libdir}/libdali3-toolkit-gles.so*
%license LICENSE
%license LICENSE.MIT

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/doc/*
%{dev_include_path}/dali-toolkit/public-api/*
%{dev_include_path}/dali-toolkit/dali-toolkit.h
%{_libdir}/pkgconfig/dali3-toolkit.pc
%{_bindir}/dali-shader-generator

%files integration-devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/devel-api/*

%files resources_480x800
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/480x800/*
%{dali_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_720x1280
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/720x1280/*
%{dali_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_1920x1080
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/1920x1080/*
%{dali_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

%files resources_1920x1080_rpi
%manifest dali-toolkit-resources.manifest
%defattr(-,root,root,-)
%{dali_toolkit_image_files}/*
%{dali_toolkit_sound_files}/*
%{dali_toolkit_style_files}/1920x1080_rpi/*
%{dali_toolkit_style_files}/debug-image-visual-shader-script.json
%{dali_toolkit_style_files}/default-feedback-theme.json
%{_datadir}/locale/*/LC_MESSAGES/*

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
%{_includedir}/dali-scene3d/public-api/*
%{_includedir}/dali-scene3d/dali-scene3d.h
%{_libdir}/pkgconfig/dali3-scene3d.pc

%files -n %{dali3_scene3d}-integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali-scene3d/integration-api/*
%{_libdir}/pkgconfig/dali3-scene3d-integration.pc

%files -n %{dali3_physics2d}
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-2d.manifest-smack
%else
%manifest dali-physics-2d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libchipmunk.so*
%{_libdir}/libdali3-physics-2d.so*
%{_libdir}/libdali3-physics-2d-gles.so*
%license LICENSE
%license LICENSE.MIT

%files -n %{dali3_physics2d}-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics/integration-api/*
%{_includedir}/dali-physics/public-api/*
%{_includedir}/dali-physics/dali-physics.h
%{_includedir}/chipmunk/*
%{_libdir}/pkgconfig/chipmunk2d.pc
%{_libdir}/pkgconfig/dali3-physics-2d.pc

%files -n %{dali3_physics3d}
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-3d.manifest-smack
%else
%manifest dali-physics-3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libbullet3.so*
%{_libdir}/libdali3-physics-3d.so*
%{_libdir}/libdali3-physics-3d-gles.so*
%license LICENSE
%license LICENSE.Zlib

%files -n %{dali3_physics3d}-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics/integration-api/*
%{_includedir}/dali-physics/public-api/*
%{_includedir}/dali-physics/dali-physics.h
%{_includedir}/bullet/*
%{_libdir}/pkgconfig/dali3-physics-3d.pc
%{_libdir}/pkgconfig/bullet3.pc

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

%if %{with dali2}
# Both builds install the same headers, tools and third-party libraries, so
# dali2 shares them with dali3. The resource packages are shared as well.
%files -n dali2-toolkit
%if 0%{?enable_dali_smack_rules}
%manifest dali-toolkit.manifest-smack
%else
%manifest dali-toolkit.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-toolkit.so*
%{_libdir}/libdali2-toolkit-gles.so*
%license LICENSE
%license LICENSE.MIT

%files -n dali2-toolkit-devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/doc/*
%{dev_include_path}/dali-toolkit/public-api/*
%{dev_include_path}/dali-toolkit/dali-toolkit.h
%{_libdir}/pkgconfig/dali2-toolkit.pc
%{_bindir}/dali-shader-generator

%files -n dali2-toolkit-integration-devel
%defattr(-,root,root,-)
%{dev_include_path}/dali-toolkit/devel-api/*

%files -n dali2-scene3d
%if 0%{?enable_dali_smack_rules}
%manifest dali-scene3d.manifest-smack
%else
%manifest dali-scene3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-scene3d.so
%license LICENSE
%license LICENSE.Unlicense

%files -n dali2-scene3d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-scene3d/public-api/*
%{_includedir}/dali-scene3d/dali-scene3d.h
%{_libdir}/pkgconfig/dali2-scene3d.pc

%files -n dali2-scene3d-integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali-scene3d/integration-api/*
%{_libdir}/pkgconfig/dali2-scene3d-integration.pc

%files -n dali2-physics-2d
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-2d.manifest-smack
%else
%manifest dali-physics-2d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libchipmunk.so*
%{_libdir}/libdali2-physics-2d.so*
%{_libdir}/libdali2-physics-2d-gles.so*
%license LICENSE
%license LICENSE.MIT

%files -n dali2-physics-2d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics/integration-api/*
%{_includedir}/dali-physics/public-api/*
%{_includedir}/dali-physics/dali-physics.h
%{_includedir}/chipmunk/*
%{_libdir}/pkgconfig/chipmunk2d.pc
%{_libdir}/pkgconfig/dali2-physics-2d.pc

%files -n dali2-physics-3d
%if 0%{?enable_dali_smack_rules}
%manifest dali-physics-3d.manifest-smack
%else
%manifest dali-physics-3d.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libbullet3.so*
%{_libdir}/libdali2-physics-3d.so*
%{_libdir}/libdali2-physics-3d-gles.so*
%license LICENSE
%license LICENSE.Zlib

%files -n dali2-physics-3d-devel
%defattr(-,root,root,-)
%{_includedir}/dali-physics/integration-api/*
%{_includedir}/dali-physics/public-api/*
%{_includedir}/dali-physics/dali-physics.h
%{_includedir}/bullet/*
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
%endif
