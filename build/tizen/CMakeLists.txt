CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables

SET(name "dali2-toolkit")

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 14)
PROJECT(${name})
SET(PKG_NAME ${name})

SET(GCC_COMPILER_VERSION_REQUIRED "6")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_COMPILER_VERSION_REQUIRED)
    message(FATAL_ERROR "The GCC required compiler version is " ${GCC_COMPILER_VERSION_REQUIRED})
  endif()
endif()

# API VERSION (Not DALi release version)
SET(${name}_VERSION_MAJOR 2)
SET(${name}_VERSION_MINOR 0)
SET(${name}_VERSION_PATCH 0)
SET(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_TOOLKIT_VERSION ${${name}_VERSION} )

# Define options to CMake
OPTION(ENABLE_EXPORTALL          "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG              "Enable Debug" OFF)
OPTION(ENABLE_TRACE              "Enable Trace" OFF)
OPTION(ENABLE_I18N               "Turns on internationalisation" OFF)
OPTION(ENABLE_COVERAGE           "Coverage" OFF)
OPTION(ENABLE_PKG_CONFIGURE      "Use pkgconfig" ON)
OPTION(ENABLE_LINK_TEST          "Enable the link test" ON)
OPTION(INSTALL_DOXYGEN_DOC       "Install doxygen doc" ON)
OPTION(CONFIGURE_AUTOMATED_TESTS "Configure automated tests" ON)
OPTION(USE_DEFAULT_RESOURCE_DIR  "Whether to use the default resource folders. Otherwise set environment variables for DALI_IMAGE_DIR, DALI_SOUND_DIR, DALI_STYLE_DIR, DALI_STYLE_IMAGE_DIR and DALI_DATA_READ_ONLY_DIR" ON)

IF( ENABLE_PKG_CONFIGURE )
  FIND_PACKAGE( PkgConfig REQUIRED )

  PKG_CHECK_MODULES(DALICORE REQUIRED dali2-core)
  PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali2-adaptor)
ENDIF()

IF( INSTALL_DOXYGEN_DOC )
  FIND_PACKAGE( Doxygen QUIET )
ENDIF()

IF( WIN32 ) # WIN32 includes x64 as well according to the cmake doc.
  FIND_PACKAGE( dali-windows-dependencies REQUIRED)
  FIND_PACKAGE( dali2-core REQUIRED)
  FIND_PACKAGE( dali2-adaptor REQUIRED)
ENDIF()

SET( VCPKG_INCLUDE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

# Define non-boolean options to CMake
SET(WITH_STYLE               "480x800" CACHE STRING "Select the style folder to use")

# from root/build/tizen, get back to root
SET(ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# Deployment folder should come from spec file or command line:
SET( PREFIX ${CMAKE_INSTALL_PREFIX})
SET( EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})

# Make sure the path is absolute
GET_FILENAME_COMPONENT(ROOT_SRC_DIR ${ROOT_SRC_DIR} ABSOLUTE)

# Set DALi style
SET( dali_style ${WITH_STYLE} )

# Style folder
SET( STYLE_BASE_DIR ${ROOT_SRC_DIR}/dali-toolkit/styles )
SET( STYLE_DIR ${STYLE_BASE_DIR}/${dali_style} )

SET( toolkit_styles_base_dir ${STYLE_BASE_DIR} )
SET( toolkit_styles_dir ${STYLE_DIR} )
SET( toolkit_style_images_dir ${STYLE_DIR}/images )

IF( DEFINED ENV{DALI_DATA_RW_DIR} )
  SET( dataReadWriteDir $ENV{DALI_DATA_RW_DIR} )
ELSE()
  SET( dataReadWriteDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

IF( DEFINED ENV{DALI_DATA_RO_DIR} )
  SET( dataReadOnlyDir $ENV{DALI_DATA_RO_DIR} )
ELSE()
  SET( dataReadOnlyDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

# Set up compiler definitions

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  SET( ENABLE_DEBUG ON )
ENDIF()

IF( ENABLE_DEBUG )
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Debug )
  ADD_DEFINITIONS( "-DDEBUG_ENABLED" )
  SET( ENABLE_EXPORTALL ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

IF( ENABLE_I18N )
  ADD_DEFINITIONS( "-DDGETTEXT_ENABLED" )
ENDIF( ENABLE_I18N )

IF( WIN32 )
  ADD_DEFINITIONS( "-DBUILDING_DALI_TOOLKIT" )

  IF( USE_DEFAULT_RESOURCE_DIR )
    SET( INSTALL_SHARE_DIR "${PREFIX}/share" )
    IF( VCPKG_TOOLKIT_BUILD )
      SET( INSTALL_SHARE_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share" )
    ENDIF()

    ADD_DEFINITIONS( -DDALI_IMAGE_DIR=\"${INSTALL_SHARE_DIR}/dali/toolkit/images/\"
                     -DDALI_SOUND_DIR=\"${INSTALL_SHARE_DIR}/dali/toolkit/sounds/\"
                     -DDALI_STYLE_DIR=\"${INSTALL_SHARE_DIR}/dali/toolkit/styles/\"
                     -DDALI_STYLE_IMAGE_DIR=\"${INSTALL_SHARE_DIR}/dali/toolkit/styles/images/\"
                     -DDALI_DATA_READ_ONLY_DIR=\"${INSTALL_SHARE_DIR}/\" )
  ENDIF()
ELSEIF( UNIX )
  ADD_DEFINITIONS(-DPIC -DSTDC_HEADERS)
  IF( USE_DEFAULT_RESOURCE_DIR )
    ADD_DEFINITIONS( -DDALI_IMAGE_DIR=\"${dataReadOnlyDir}/toolkit/images/\"
                     -DDALI_SOUND_DIR=\"${dataReadOnlyDir}/toolkit/sounds/\"
                     -DDALI_STYLE_DIR=\"${dataReadOnlyDir}/toolkit/styles/\"
                     -DDALI_STYLE_IMAGE_DIR=\"${dataReadOnlyDir}/toolkit/styles/images/\"
                     -DDALI_DATA_READ_ONLY_DIR=\"${dataReadOnlyDir}\" )
  ENDIF()

  IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
  ENDIF("${ARCH}" STREQUAL "arm")

  IF( NOT ${ENABLE_EXPORTALL} )
    ADD_DEFINITIONS( "-DHIDE_DALI_INTERNALS" )
  ENDIF( NOT ${ENABLE_EXPORTALL} )
ENDIF()

IF( NOT USE_DEFAULT_RESOURCE_DIR )
  ADD_DEFINITIONS( -DDALI_IMAGE_DIR=0
                   -DDALI_SOUND_DIR=0
                   -DDALI_STYLE_DIR=0
                   -DDALI_STYLE_IMAGE_DIR=0
                   -DDALI_DATA_READ_ONLY_DIR=0 )
ENDIF()

IF( ENABLE_TRACE )
  ADD_DEFINITIONS("-DTRACE_ENABLED")
ENDIF()

# Set paths
SET( toolkit_images_dir  ${ROOT_SRC_DIR}/dali-toolkit/styles/images-common )
SET( toolkit_sounds_dir  ${ROOT_SRC_DIR}/dali-toolkit/sounds )
SET( toolkit_src_dir     ${ROOT_SRC_DIR}/dali-toolkit/internal )
SET( public_api_src_dir  ${ROOT_SRC_DIR}/dali-toolkit/public-api )
SET( devel_api_src_dir   ${ROOT_SRC_DIR}/dali-toolkit/devel-api )
SET( third_party_src_dir ${ROOT_SRC_DIR}/dali-toolkit/third-party )
SET( package_doxy_dir    ${ROOT_SRC_DIR}/doc )

# Set up the include dir
SET( INCLUDE_DIR $ENV{includedir} )
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} )
ENDIF()
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${PREFIX}/include )
ENDIF()

# Set up the lib dir
SET( LIB_DIR $ENV{libdir} )
IF( NOT LIB_DIR )
  SET( LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
ENDIF()
IF( NOT LIB_DIR )
  SET( LIB_DIR ${PREFIX}/lib )
ENDIF()

# Set up the bin dir
SET( BIN_DIR $ENV{bindir} )
IF( NOT BIN_DIR )
  SET( BIN_DIR ${CMAKE_INSTALL_BINDIR} )
ENDIF()
IF( NOT BIN_DIR )
  SET( BIN_DIR ${PREFIX}/bin )
ENDIF()

IF( ENABLE_PKG_CONFIGURE )
  # Configure the pkg-config file
  # Requires the following variables to be setup:
  # @PREFIX@ @EXEC_PREFIX@ @DALI_VERSION@ @LIB_DIR@ @DEV_INCLUDE_PATH@
  SET( DEV_INCLUDE_PATH ${INCLUDE_DIR} )
  SET( CORE_PKG_CFG_FILE dali2-toolkit.pc )
  CONFIGURE_FILE( ${CORE_PKG_CFG_FILE}.in ${CORE_PKG_CFG_FILE} @ONLY )
ENDIF()

IF( WIN32 )
  ADD_COMPILE_OPTIONS( /FIdali-windows-dependencies.h ) # Adds missing definitions.
  ADD_COMPILE_OPTIONS( /vmg )                           # Avoids a 'reinterpret_cast' compile error while compiling signals and callbacks.
  ADD_COMPILE_OPTIONS( /wd4251 )                        # Ignores warning C4251: "'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'"
ELSE()
  # Set up compiler flags and warnings
  ADD_COMPILE_OPTIONS( -Wno-ignored-qualifiers )

  # TODO: Clang is a lot more strict with warnings, we should address
  # those issues at some point.
  IF( NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    ADD_COMPILE_OPTIONS( -Werror )
  ENDIF()

  ADD_COMPILE_OPTIONS( -Wall -Wno-unused-parameter -Wno-float-equal -Wno-class-memaccess )

  IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage )
    ADD_COMPILE_OPTIONS( --coverage )
    SET(ENABLE_COVERAGE ON)
    SET(COVERAGE --coverage)
  ENDIF()

  IF( NOT ${ENABLE_EXPORTALL} )
    ADD_COMPILE_OPTIONS( "-fvisibility=hidden" )
  ENDIF( NOT ${ENABLE_EXPORTALL} )
ENDIF()


INCLUDE_DIRECTORIES(
  ${ROOT_SRC_DIR}
  ${DALICORE_INCLUDE_DIRS}
  ${DALIADAPTOR_INCLUDE_DIRS}
  ${VCPKG_INCLUDE_DIR}
  ${INCLUDE_DIR}
)

SET(SOURCE_DIR "${ROOT_SRC_DIR}/dali")
SET(PACKAGE_DOXY_SRC_DIR "${ROOT_SRC_DIR}/doc")

SET(SOURCES "")

# could use INCLUDE to get the source lists a different way;
# would then need to specify a build folder somehow...
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/sounds/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/styles/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/styles/images-common/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/internal/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/public-api/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/devel-api/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/third-party/file.list )
INCLUDE( ${ROOT_SRC_DIR}/doc/file.list )

# Platforms with highp shader support can use vector based text
IF( NOT DEFINED ENABLE_VECTOR_BASED_TEXT_RENDERING )
  SET(ENABLE_VECTOR_BASED_TEXT_RENDERING ON)
ENDIF()

IF( ENABLE_VECTOR_BASED_TEXT_RENDERING )
  ADD_DEFINITIONS( "-DENABLE_VECTOR_BASED_TEXT_RENDERING" )
  INCLUDE( ${ROOT_SRC_DIR}/dali-toolkit/internal/text/rendering/vector-based/file.list )
ENDIF()

SET(LIBTYPE SHARED)
IF(DEFINED STATIC)
  SET(LIBTYPE STATIC)
ENDIF()

IF( WIN32 )
  SET( DALICORE_LDFLAGS
        "${DALICORE_LDFLAGS}"
        dali2-core::dali2-core )

  FIND_PACKAGE( pthreads REQUIRED )
  FIND_PACKAGE( curl REQUIRED )
  FIND_LIBRARY( GETOPT_LIBRARY NAMES getopt )
  FIND_LIBRARY( EXIF_LIBRARY NAMES libexif )

  FIND_PACKAGE( png REQUIRED )
  FIND_PACKAGE( gif REQUIRED )
  FIND_PACKAGE( jpeg REQUIRED )
  FIND_LIBRARY( TURBO_JPEG_LIBRARY NAMES turbojpeg )

  FIND_PACKAGE( unofficial-fontconfig REQUIRED )
  FIND_PACKAGE( freetype REQUIRED )
  FIND_PACKAGE( harfbuzz REQUIRED )
  FIND_LIBRARY( FRIBIDI_LIBRARY NAMES fribidi )

  FIND_PACKAGE( unofficial-angle REQUIRED )
  FIND_PACKAGE( unofficial-cairo REQUIRED )

  FIND_PACKAGE( WebP REQUIRED )

  SET( DALIADAPTOR_LDFLAGS
       "${DALIADAPTOR_LDFLAGS}"
       dali2-adaptor::dali2-adaptor )
ENDIF()


ADD_LIBRARY( ${name} ${LIBTYPE} ${SOURCES} )
TARGET_LINK_LIBRARIES( ${name}
  ${DALICORE_LDFLAGS}
  ${DALIADAPTOR_LDFLAGS}
  ${COVERAGE}
)

IF( ANDROID )
  TARGET_LINK_LIBRARIES( ${name} log )
ENDIF()

SET_TARGET_PROPERTIES( ${name}
  PROPERTIES
  VERSION ${DALI_TOOLKIT_VERSION}
  SOVERSION ${${name}_VERSION_MAJOR}
  CLEAN_DIRECT_OUPUT 1
)

IF( INSTALL_CMAKE_MODULES )
  IF( ENABLE_DEBUG )
    SET( BIN_DIR "${BIN_DIR}/debug" )
    SET( LIB_DIR "${LIB_DIR}/debug" )
  ENDIF()

  # Install the library files.
  INSTALL( TARGETS ${name}
    EXPORT ${name}-targets
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR}
    RUNTIME DESTINATION ${BIN_DIR}
  )

  # Install the cmake modules.
  INSTALL(
    EXPORT ${name}-targets
    NAMESPACE ${name}::
    FILE ${name}-targets.cmake
    DESTINATION share/${name}
  )

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-targets.cmake)
  ")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake DESTINATION share/${name})

  # Install the pdb file.
  IF( ENABLE_DEBUG )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${name}.pdb DESTINATION ${BIN_DIR} )
  ENDIF()
ELSE()
  # Install the library so file and symlinks
  INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )
ENDIF()

# Install the pkg-config file
IF( ENABLE_PKG_CONFIGURE )
  INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/${CORE_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )
ENDIF()

# Glob for resources
MACRO( COPY_RESOURCES FILE_LIST SRC_RELATIVE_DIRECTORY DESTINATION EXPLICIT_DESTINATION )
  FOREACH( PATTERN ${FILE_LIST} )
    FILE(GLOB FILES ${PATTERN} )
    FOREACH( FILE ${FILES} )
      STRING(REPLACE ${SRC_RELATIVE_DIRECTORY} ${DESTINATION} NEWFILE ${FILE} )
      GET_FILENAME_COMPONENT(NEWDIR ${NEWFILE} DIRECTORY)
      IF( "${EXPLICIT_DESTINATION}" STREQUAL "" )
        STRING( REPLACE "dali-toolkit" "toolkit" NEWDIR2 ${NEWDIR} )
        INSTALL( FILES ${FILE} DESTINATION ${NEWDIR2} )
      ELSE()
        INSTALL( FILES ${FILE} DESTINATION ${DESTINATION}/${EXPLICIT_DESTINATION} )
      ENDIF()
    ENDFOREACH()
  ENDFOREACH()
ENDMACRO()

# macro for installing headers by replacing prefix. (TODO, investigate
# if there is a CMAKE way of doing this automatically)
MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST STRIP_PREFIX REPLACE_PREFIX)
  GET_FILENAME_COMPONENT( SPREFIX ${STRIP_PREFIX} ABSOLUTE )
  FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/]" DIR ${HEADER})
    STRING(REPLACE ${SPREFIX} ${REPLACE_PREFIX} NEWDIR ${DIR})
    INSTALL(FILES ${HEADER} DESTINATION ${INCLUDE_DIR}/${NEWDIR})
  ENDFOREACH(HEADER)
ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

# Install headers using lists defined by ADD_SUBDIRECTORY
INSTALL_HEADERS_WITH_DIRECTORY(PUBLIC_API_HEADERS ${ROOT_SRC_DIR}/dali-toolkit "dali-toolkit")
INSTALL_HEADERS_WITH_DIRECTORY(DEVEL_API_HEADERS ${ROOT_SRC_DIR}/dali-toolkit "dali-toolkit")
INSTALL_HEADERS_WITH_DIRECTORY(INTEGRATION_API_HEADERS ${ROOT_SRC_DIR}/dali-toolkit "dali-toolkit")
INSTALL_HEADERS_WITH_DIRECTORY(PACKAGE_DOXY_HEADERS ${PACKAGE_DOXY_SRC_DIR} "dali-toolkit/doc" )

# Install dali-toolkit.h
INSTALL( FILES ${ROOT_SRC_DIR}/dali-toolkit/dali-toolkit.h DESTINATION ${INCLUDE_DIR}/dali-toolkit )

# package doxygen file (contains doxygen grouping information)
INSTALL( FILES ${package_doxy_files} DESTINATION ${DEV_INCLUDE_PATH}/dali-toolkit/doc )

# On some platforms we want to install the data files to different location compare to
# the data location used runtime. For example, on Android we want the toolkit library to load
# the files from /data/data/com.sec.dali_demo/files but to install the files into ${PREFIX}/files
# for SDK to pick them up for the archive.
IF( DEFINED ENV{DALI_DATA_RW_INSTALL_DIR} )
  SET( dataReadWriteInstallDir $ENV{DALI_DATA_RW_INSTALL_DIR} )
ELSE()
  SET( dataReadWriteInstallDir ${dataReadWriteDir} )
ENDIF()

IF( DEFINED ENV{DALI_DATA_RO_INSTALL_DIR} )
  SET( dataReadOnlyInstallDir $ENV{DALI_DATA_RO_INSTALL_DIR} )
ELSE()
  SET( dataReadOnlyInstallDir ${dataReadOnlyDir} )
ENDIF()

COPY_RESOURCES( "${dali_toolkit_style_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyInstallDir}" "./toolkit/styles" )
COPY_RESOURCES( "${dali_toolkit_image_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyInstallDir}" "./toolkit/images" )
COPY_RESOURCES( "${dali_toolkit_sound_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyInstallDir}" "./toolkit/sounds" )
COPY_RESOURCES( "${dali_toolkit_style_images}" "${ROOT_SRC_DIR}" "${dataReadOnlyInstallDir}" "./toolkit/styles/images" )

# The DALI_TOOLKIT_PREFIX must be set if this CMakeLists.txt is executed
# from the top-level CMake script using ADD_SUBDIRECTORY() to avoid
# target names duplication with other DALi modules.
IF( ENABLE_COVERAGE )
  FIND_PROGRAM( LCOV_BIN "lcov" )
  IF( LCOV_BIN )

    # Define custom rules for coverage
    SET(COVERAGE_DIR .cov)
    SET(COVERAGE_OUTPUT_DIR doc/coverage)

    # lcov prior to 1.10 doesn't have -rc option; this and subsequent version don't output
    # branch coverage. Determine the lcov version, and enable branch coverage accordingly.
    EXECUTE_PROCESS( COMMAND bash -c "${LCOV_BIN} --version | cut -d' ' -f4" OUTPUT_VARIABLE LCOV_VERSION )
    STRING( REPLACE "." ";" LCOV_VLIST ${LCOV_VERSION})
    IF( NOT $<VERSION_LESS:${LCOV_VERSION},"1.10"> )
      SET(LCOV_OPTS --rc lcov_branch_coverage=1)
    ENDIF()

    ADD_CUSTOM_TARGET( ${DALI_TOOLKIT_PREFIX}rename_cov_data ./rename-cov-data )

    ADD_CUSTOM_TARGET( ${DALI_TOOLKIT_PREFIX}cov_data ${LCOV_BIN} ${LCOV_OPTS} --base-directory . --directory . -c -o dali.info
      COMMAND ${LCOV_BIN} ${LCOV_OPTS} --remove dali.info \"*/dali-env/*\" \"/usr/include/*\" "*/dali-env/*" "*solid-color-actor*" "*/dali-toolkit/third-party/*" -o dali.info )

    ADD_CUSTOM_TARGET( ${DALI_TOOLKIT_PREFIX}coverage genhtml ${LCOV_OPTS} -o ${COVERAGE_OUTPUT_DIR} dali.info
      DEPENDS cov_data )

    ADD_CUSTOM_TARGET( ${DALI_TOOLKIT_PREFIX}reset_coverage @${LCOV_BIN} -z --directory `pwd` )

    # Define custom rule for distclean
    ADD_CUSTOM_TARGET( ${DALI_TOOLKIT_PREFIX}distclean @echo cleaning for source distribution )
    ADD_CUSTOM_COMMAND(
          DEPENDS ${DALI_TOOLKIT_PREFIX}clean
          COMMENT "distribution clean"
          COMMAND find
          ARGS    .
          -not -name config.cmake -and \(
          -name tester.c -or
          -name Testing -or
          -name CMakeFiles -or
          -name doc -or
          -name cmake.depends -or
          -name cmake.check_depends -or
          -name CMakeCache.txt -or
          -name cmake.check_cache -or
          -name *.cmake -or
          -name Makefile -or
          -name core -or
          -name core.* -or
          -name gmon.out -or
          -name install_manifest.txt -or
          -name *.pc -or
          -name *.gcov -or
          -name *.gcno -or
          -name *.gcda -or
          -name *~ -or
          -name libdali*.so* \)
          | grep -v TC | xargs rm -rf
          TARGET  ${DALI_TOOLKIT_PREFIX}distclean
          VERBATIM
          )

  ENDIF( LCOV_BIN )
ENDIF()

# Build documentation if doxygen tool is available
SET( doxygenEnabled OFF )
IF( DOXYGEN_FOUND )
  SET( doxygenEnabled ON )
  # 'prefix' is used by doxygen in-files.
  SET( prefix ${PREFIX} )
  SET( DOXYGEN_DOCS_DIR ${ROOT_SRC_DIR}/docs )
  SET( DOXYGEN_ROOT_DIR ${ROOT_SRC_DIR} )
  SET( DOXYGEN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs )
  SET( DOXYGEN_INTERNAL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs-internal )
  CONFIGURE_FILE( ${DOXYGEN_SRC_DIR}/dali.doxy.in ${DOXYGEN_SRC_DIR}/dali.doxy @ONLY )
  CONFIGURE_FILE( ${DOXYGEN_INTERNAL_SRC_DIR}/dali-internal.doxy.in ${DOXYGEN_INTERNAL_SRC_DIR}/dali-internal.doxy @ONLY )
  ADD_CUSTOM_TARGET( doc_doxygen ALL
                     DEPENDS ${name}
                     COMMAND ${DOXYGEN_EXECUTABLE} -u ${DOXYGEN_SRC_DIR}/dali.doxy
                     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_SRC_DIR}/dali.doxy
                     COMMAND ${CMAKE_COMMAND} -P ${DOXYGEN_SRC_DIR}/check_for_errors.cmake
                     COMMENT "Generating API documentation with Doxygen"
                     WORKING_DIRECTORY ${DOXYGEN_SRC_DIR}
                     VERBATIM )
  ADD_CUSTOM_TARGET( doc_doxygen_internal
                     COMMAND ${DOXYGEN_EXECUTABLE} -u ${DOXYGEN_INTERNAL_SRC_DIR}/dali-internal.doxy
                     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INTERNAL_SRC_DIR}/dali-internal.doxy
                     COMMENT "Generating Internal API documentation with Doxygen"
                     VERBATIM )

ENDIF()

IF( CONFIGURE_AUTOMATED_TESTS )
  # Configure automated tests
  CONFIGURE_FILE( ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt.in
                  ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt @ONLY )
ENDIF()

# Configuration Messages
MESSAGE( STATUS "Configuration:\n" )
MESSAGE( STATUS "Prefix:                        " ${PREFIX} )
MESSAGE( STATUS "Lib Dir:                       " ${LIB_DIR} )
MESSAGE( STATUS "Include Dir:                   " ${INCLUDE_DIR} )
MESSAGE( STATUS "Debug build:                   " ${ENABLE_DEBUG} )
MESSAGE( STATUS "Export all symbols:            " ${ENABLE_EXPORTALL} )
MESSAGE( STATUS "Coverage:                      " ${ENABLE_COVERAGE} )
MESSAGE( STATUS "Trace:                         " ${ENABLE_TRACE} )
MESSAGE( STATUS "Doxygen:                       " ${doxygenEnabled} )
MESSAGE( STATUS "Data Dir (Read/Write):         " ${dataReadWriteDir} )
MESSAGE( STATUS "Data Dir (Read Only):          " ${dataReadOnlyDir} )
MESSAGE( STATUS "Data Install Dir (Read/Write): " ${dataReadWriteInstallDir} )
MESSAGE( STATUS "Data Install Dir (Read Only):  " ${dataReadOnlyInstallDir} )
MESSAGE( STATUS "Style Dir:                     " ${STYLE_DIR} )
MESSAGE( STATUS "Style:                         " ${dali_style} )
MESSAGE( STATUS "i18n:                          " ${ENABLE_I18N} )
MESSAGE( STATUS "Use pkg configure:             " ${ENABLE_PKG_CONFIGURE} )
MESSAGE( STATUS "Vector Based Text Rendering:   " ${ENABLE_VECTOR_BASED_TEXT_RENDERING} )
MESSAGE( STATUS "Enable link test:              " ${ENABLE_LINK_TEST} )
MESSAGE( STATUS "Configure automated tests:     " ${CONFIGURE_AUTOMATED_TESTS} )
MESSAGE( STATUS "CXXFLAGS:                      " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:                       " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )

MESSAGE( " Folder       INSTALL_SHARE_DIR : [" ${INSTALL_SHARE_DIR} "]" )
MESSAGE( " Folder          DALI_IMAGE_DIR : [" ${DALI_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_STYLE_DIR : [" ${DALI_STYLE_DIR} "]" )
MESSAGE( " Folder    DALI_STYLE_IMAGE_DIR : [" ${DALI_STYLE_IMAGE_DIR} "]" )
MESSAGE( " Folder          DALI_SOUND_DIR : [" ${DALI_SOUND_DIR} "]" )
MESSAGE( " Folder DALI_DATA_READ_ONLY_DIR : [" ${DALI_DATA_READ_ONLY_DIR} "]" )
