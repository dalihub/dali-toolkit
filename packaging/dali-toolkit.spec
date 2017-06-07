Name:       dali-toolkit
Summary:    The OpenGLES Canvas Core Library Toolkit
Version:    1.2.41
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-toolkit.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dali-core)
BuildRequires: gettext

# dali-toolkit only need to know the interfaces(APIs) of dali-adaptor(the devel package).
# It doesn't need to know which adaptor will be used by applications.
# Applications or dali-addon will decide which one they will use.
BuildRequires:  dali-adaptor-devel

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

%if "%{profile}" == "tv"
%define dali_toolkit_profile TV
%define dali_style_folder 1920x1080
%else
%define dali_toolkit_profile MOBILE
%define dali_style_folder 720x1280
# dali_style to be provided by build system as with dali_toolkit_profile or by passing --define 'dali_style 470x800' to the rpm build command
%endif

%if "%{?dali_style}"
  %define dali_style_folder %{dali_style}
%endif

%description
The OpenGLES Canvas Core Library Toolkit - a set of controls that provide
user interface functionality.

##############################
# resource
##############################
%package resources_480x800
Summary:    default resource files for 480x800
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_720x1280
Conflicts:  %{name}-resources_1920x1080
%description resources_480x800
dali-toolkit default resource files for 480x800
Contain po / sounds / common images / style / style images

%package resources_720x1280
Summary:    default resource files for 720x1280
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_1920x1080
%description resources_720x1280
dali-toolkit default resource files for 720x1280
Contain po / sounds / common images / style / style images

%package resources_1920x1080
Summary:    default resource files for 1920x1080
Requires:   %{name} = %{version}-%{release}
Conflicts:  %{name}-resources_480x800
Conflicts:  %{name}-resources_720x1280
%description resources_1920x1080
dali-toolkit default resource files for 1920x1080
Contain po / sounds / common images / style / style images

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

%configure --enable-profile=%{dali_toolkit_profile} \
           --with-style=%{dali_style_folder} \
%if 0%{?enable_debug}
           --enable-debug \
%endif
           --enable-i18n=yes

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
pushd build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

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

# Remove default style and style images which are for Linux build
rm -rf %{buildroot}%{dali_toolkit_style_files}/*

# Make folder to contain style and style images
# After making folder, copy local style and style images to new folder
mkdir -p %{buildroot}%{dali_toolkit_style_files}/480x800
cp -r dali-toolkit/styles/480x800/* %{buildroot}%{dali_toolkit_style_files}/480x800
mkdir -p %{buildroot}%{dali_toolkit_style_files}/720x1280
cp -r dali-toolkit/styles/720x1280/* %{buildroot}%{dali_toolkit_style_files}/720x1280
mkdir -p %{buildroot}%{dali_toolkit_style_files}/1920x1080
cp -r dali-toolkit/styles/1920x1080/* %{buildroot}%{dali_toolkit_style_files}/1920x1080

# Copy default feedback theme
cp dali-toolkit/styles/default-feedback-theme.json %{buildroot}%{dali_toolkit_style_files}

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%post resources_480x800
pushd %{dali_toolkit_style_files}/480x800
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

%post resources_720x1280
pushd %{dali_toolkit_style_files}/720x1280
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
rm -rf ./*
popd

%post resources_1920x1080
pushd %{dali_toolkit_style_files}/1920x1080
for FILE in *; do mv ./"${FILE}" ../"${FILE}"; done
popd

##############################
# Pre Uninstall
##############################

%preun resources_480x800
pushd %{dali_toolkit_style_files}
mv images ./480x800
mv *.json ./480x800
popd

%preun resources_720x1280
pushd %{dali_toolkit_style_files}
mv images ./720x1280
mv *.json ./720x1280
popd

%preun resources_1920x1080
pushd %{dali_toolkit_style_files}
mv images ./1920x1080
mv *.json ./1920x1080
popd

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

%postun resources_480x800
pushd %{dali_toolkit_style_files}
rm -rf 480x800
popd

%postun resources_720x1280
pushd %{dali_toolkit_style_files}
rm -rf 720x1280
popd

%postun resources_1920x1080
pushd %{dali_toolkit_style_files}
rm -rf 1920x1080
popd

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
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{dev_include_path}/%{name}/*
%{_libdir}/pkgconfig/*.pc

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
