#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

m4_define([dali_version],[0.1.0])
AC_INIT([dali], [dali_version])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

DALI_TOOLKIT_VERSION=dali_version
AC_SUBST(DALI_TOOLKIT_VERSION)

PKG_CHECK_MODULES(DALICORE, dali-core)
PKG_CHECK_MODULES(DALI, dali)
PKG_CHECK_MODULES(FRIBIDI, fribidi)

DALI_TOOLKIT_CFLAGS=-DPLATFORM_TIZEN

AC_ARG_ENABLE(exportall,
              [AC_HELP_STRING([--enable-exportall],
                              [enables the exporting of all the symbols in the library])],
              [enable_exportall=yes],
              [enable_exportall=no])

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug],
                              [Turns on debugging])],
              [enable_debug=$enableval],
              [enable_debug=no])

# option to build JavaScript plugin
# configure settings and output
# --enable-javascript        // enable_javascript = yes
# --enable-javascript=yes    // enable_javascript = yes
# --enable-javascript=no     // enable_javascript = no
# --disable-javascript       // enable_javascript = no
#  no setting                // enable_javascript = automatic ( enable if v8 present)
AC_ARG_ENABLE([javascript],
              [AC_HELP_STRING([--enable-javascript],
               [Enable JavaScript plugin])] ,
               [enable_javascript=$enableval],
               [enable_javascript=automatic])


if test "x$enable_debug" = "xyes"; then
  DALI_TOOLKIT_CFLAGS="$DALI_TOOLKIT_CFLAGS -DDEBUG_ENABLED"
fi

if test "x$enable_debug" = "xno" -a "x$enable_exportall" = "xno"; then
  DALI_TOOLKIT_CFLAGS="$DALI_TOOLKIT_CFLAGS -fvisibility=hidden -DHIDE_DALI_INTERNALS"
fi




# Tizen Profile options
AC_ARG_ENABLE([profile],
              [AC_HELP_STRING([--enable-profile=COMMON,MOBILE,WEARABLE,TV],
                            [Select the variant of tizen])],
              [dali_profile=$enableval],
              [dali_profile=COMMON])

if test x$DALI_DATA_RW_DIR != x; then
  dataReadWriteDir=${DALI_DATA_RW_DIR}/
else
  dataReadWriteDir=${prefix}/share/dali/
fi

if test x$DALI_DATA_RO_DIR != x; then
  dataReadOnlyDir=${DALI_DATA_RO_DIR}/
else
  dataReadOnlyDir=${prefix}/share/dali/
fi

# v8 version 4+ requires c++11
PKG_CHECK_MODULES(V8, v8 = 3.32.7, [ pkg_check_v8=yes ],  [ pkg_check_v8=no  ] )

# Rules for building JavaScript plugin
# If enable_javascript=yes and v8 installed = build javascript
# If enable_javascript=automatic and v8 installed = build javascript
# If enable_javascript=yes and v8 not installed = throw an error
build_javascript_plugin=no
if test x$enable_javascript = xyes; then
  if test x$pkg_check_v8 = xno; then
    [build_javascript_plugin=no]
    AC_MSG_ERROR("V8 not found or incorrect version installed")
    AC_MSG_NOTICE("To disable building of JavaScript plugin use --disable-javascript")
  else
    [build_javascript_plugin=yes]
    AC_MSG_NOTICE(V8 library found. Building DALi JavaScript plugin)
  fi
fi
if test x$enable_javascript = xautomatic && test x$pkg_check_v8 = xyes; then
  [build_javascript_plugin=yes]
  AC_MSG_NOTICE( V8 library found. Automatic building of JavaScript plugin. Use  use --disable-javascript to disable)
fi


#set a variable for the makefile to force compile the JAvaSplugin
AM_CONDITIONAL([ENABLE_JAVASCRIPT_PLUGIN], [test x$build_javascript_plugin = xyes])

AC_SUBST(dataReadWriteDir)
AC_SUBST(dataReadOnlyDir)
AC_SUBST(DALI_TOOLKIT_CFLAGS)


# Specify the include directory for development headers
#devincludepath=${includedir}/dali/internal
devincludepath=${includedir}
AC_SUBST(devincludepath)

# Doxygen paths
DOXYGEN_DOCS_DIR=../../../docs
DOXYGEN_ROOT_DIR=../../..
AC_SUBST(DOXYGEN_DOCS_DIR)
AC_SUBST(DOXYGEN_ROOT_DIR)

AC_CONFIG_FILES([
 Makefile
 dali-toolkit/Makefile
 plugins/Makefile
 dali-toolkit.pc
 docs/Makefile
 docs/dali.doxy
])

AC_OUTPUT

echo "
Configuration
-------------
  Prefix:                           $prefix
  Debug Build:                      $enable_debug
  JavaScript support (V8 required)  $build_javascript_plugin
  Profile:                          $dali_profile
  Data Dir (Read/Write):            $dataReadWriteDir
  Data Dir (Read Only):             $dataReadOnlyDir
"
