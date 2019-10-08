CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables

SET(name "dali-toolkit")

PROJECT(${name})
SET(PKG_NAME ${name})

FIND_PACKAGE( PkgConfig REQUIRED )
FIND_PACKAGE( Doxygen QUIET )

PKG_CHECK_MODULES(DALICORE REQUIRED dali-core)
PKG_CHECK_MODULES(DALIADAPTOR REQUIRED dali-adaptor)

# API VERSION (Not DALi release version)
SET(${name}_VERSION_MAJOR 0)
SET(${name}_VERSION_MINOR 0)
SET(${name}_VERSION_PATCH 0)
SET(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_TOOLKIT_VERSION ${${name}_VERSION} )

# Define options to CMake
OPTION(ENABLE_EXPORTALL      "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG          "Enable Debug" OFF)
OPTION(ENABLE_TRACE          "Enable Trace" OFF)
OPTION(ENABLE_I18N           "Turns on internationalisation" OFF)
OPTION(ENABLE_COVERAGE       "Coverage" OFF)

# Define non-boolean options to CMake
SET(WITH_STYLE               "480x800" CACHE STRING "Select the style folder to use")

# from root/build/tizen, get back to root
SET(ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

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

IF( $ENV{DALI_DATA_RW_DIR} )
  SET( dataReadWriteDir $ENV{DALI_DATA_RW_DIR}/ )
ELSE()
  SET( dataReadWriteDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

IF( $ENV{DALI_DATA_RO_DIR} )
  SET( dataReadOnlyDir $ENV{DALI_DATA_RO_DIR}/ )
ELSE()
  SET( dataReadOnlyDir ${CMAKE_INSTALL_PREFIX}/share/dali/ )
ENDIF()

# Set up compiler definitions
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Debug )
  SET( ENABLE_DEBUG ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

ADD_DEFINITIONS(-DPIC -DSTDC_HEADERS)
ADD_DEFINITIONS(-DDALI_IMAGE_DIR=\"${dataReadOnlyDir}/toolkit/images/\"
                -DDALI_SOUND_DIR=\"${dataReadOnlyDir}/toolkit/sounds/\"
                -DDALI_STYLE_DIR=\"${dataReadOnlyDir}/toolkit/styles/\"
                -DDALI_STYLE_IMAGE_DIR=\"${dataReadOnlyDir}/toolkit/styles/images/\"
                -DDALI_DATA_READ_ONLY_DIR=\"${dataReadOnlyDir}\" )

IF("${ARCH}" STREQUAL "arm")
  ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

IF( ENABLE_DEBUG )
  ADD_DEFINITIONS( "-DDEBUG_ENABLED" )
  SET( ENABLE_EXPORTALL ON )
ENDIF( ENABLE_DEBUG )

IF( NOT ${ENABLE_EXPORTALL} )
  ADD_DEFINITIONS( "-fvisibility=hidden -DHIDE_DALI_INTERNALS" )
ENDIF( NOT ${ENABLE_EXPORTALL} )

IF( ENABLE_I18N )
  ADD_DEFINITIONS( "-DDGETTEXT_ENABLED" )
ENDIF( ENABLE_I18N )

# Platforms with highp shader support can use vector based text
IF( "${PROFILE}" STREQUAL "UBUNTU" )
  SET( ENABLE_VECTOR_BASED_TEXT_RENDERING 1 )
ENDIF()

# Deployment folder should come from spec file or command line:
SET( PREFIX ${CMAKE_INSTALL_PREFIX})
SET( EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})

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

# Configure the pkg-config file
# Requires the following variables to be setup:
# @PREFIX@ @EXEC_PREFIX@ @DALI_VERSION@ @LIB_DIR@ @DEV_INCLUDE_PATH@
SET( DEV_INCLUDE_PATH ${INCLUDE_DIR} )
SET( CORE_PKG_CFG_FILE dali-toolkit.pc )
CONFIGURE_FILE( ${CORE_PKG_CFG_FILE}.in ${CORE_PKG_CFG_FILE} @ONLY )

# Set up compiler flags and warnings
ADD_COMPILE_OPTIONS( -std=c++11 )
ADD_COMPILE_OPTIONS( -Wno-ignored-qualifiers )

# TODO: Clang is a lot more strict with warnings, we should address
# those issues at some point.
IF( NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  ADD_COMPILE_OPTIONS( -Werror )
ENDIF()

ADD_COMPILE_OPTIONS( -Wall -Wno-unused-parameter -Wno-float-equal )

IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage )
  ADD_COMPILE_OPTIONS( --coverage )
  SET(ENABLE_COVERAGE ON)
  SET(COVERAGE --coverage)
ENDIF()

IF( ENABLE_TRACE )
  ADD_DEFINITIONS("-DTRACE_ENABLED")
ENDIF()

INCLUDE_DIRECTORIES(
  ${ROOT_SRC_DIR}
  ${DALICORE_INCLUDE_DIRS}
  ${DALIADAPTOR_INCLUDE_DIRS}
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

SET(LIBTYPE SHARED)
IF(DEFINED STATIC)
  SET(LIBTYPE STATIC)
ENDIF()

ADD_LIBRARY( ${name} ${LIBTYPE} ${SOURCES} )

TARGET_LINK_LIBRARIES( ${name}
  ${DALICORE_LDFLAGS}
  ${DALIADAPTOR_LDFLAGS}
  ${COVERAGE}
)

SET_TARGET_PROPERTIES( ${name}
  PROPERTIES
  VERSION ${DALI_TOOLKIT_VERSION}
  SOVERSION ${${name}_VERSION_MAJOR}
  CLEAN_DIRECT_OUPUT 1
)

# Install the library so file and symlinks
INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )

# Install the pkg-config file
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/${CORE_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )

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
INSTALL( FILES ${ROOT_SRC_DIR}/dali-toolkit/dali-toolkit.h DESTINATION ${DEV_INCLUDE_PATH}/dali-toolkit )

# package doxygen file (contains doxygen grouping information)
INSTALL( FILES ${package_doxy_files} DESTINATION ${DEV_INCLUDE_PATH}/dali-toolkit/doc )

COPY_RESOURCES( "${dali_toolkit_style_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyDir}" "./toolkit/styles" )
COPY_RESOURCES( "${dali_toolkit_image_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyDir}" "./toolkit/images" )
COPY_RESOURCES( "${dali_toolkit_sound_files}" "${ROOT_SRC_DIR}" "${dataReadOnlyDir}" "./toolkit/sounds" )
COPY_RESOURCES( "${dali_toolkit_style_images}" "${ROOT_SRC_DIR}" "${dataReadOnlyDir}" "./toolkit/styles/images" )

# The DALI_TOOLKIT_PREFIX must be set if this CMakeLists.txt is executed
# from the top-level CMake script using ADD_SUBDIRECTORY() to avoid
# target names duplication with other DALi modules.
IF( UNIX )
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
ENDIF( UNIX )

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

# Configure automated tests
CONFIGURE_FILE( ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt.in
                ${ROOT_SRC_DIR}/automated-tests/CMakeLists.txt @ONLY )

# Configuration Messages
MESSAGE( STATUS "Configuration:\n" )
MESSAGE( STATUS "Prefix:                " ${PREFIX} )
MESSAGE( STATUS "Lib Dir:               " ${LIB_DIR} )
MESSAGE( STATUS "Include Dir:           " ${INCLUDE_DIR} )
MESSAGE( STATUS "Debug build:           " ${ENABLE_DEBUG} )
MESSAGE( STATUS "Export all symbols:    " ${ENABLE_EXPORTALL} )
MESSAGE( STATUS "Coverage:              " ${ENABLE_COVERAGE} )
MESSAGE( STATUS "Trace:                 " ${ENABLE_TRACE} )
MESSAGE( STATUS "Doxygen:               " ${doxygenEnabled} )
MESSAGE( STATUS "Data Dir (Read/Write): " ${dataReadWriteDir} )
MESSAGE( STATUS "Data Dir (Read Only):  " ${dataReadOnlyDir} )
MESSAGE( STATUS "Style Dir:             " ${STYLE_DIR} )
MESSAGE( STATUS "Style:                 " ${dali_style} )
MESSAGE( STATUS "i18n:                  " ${ENABLE_I18N} )
MESSAGE( STATUS "CXXFLAGS:              " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:               " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )
