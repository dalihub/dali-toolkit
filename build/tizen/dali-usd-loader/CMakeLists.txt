CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
set(name "dali2-usd-loader")

project(${name} CXX)

set(${name}_VERSION_MAJOR 2)
set(${name}_VERSION_MINOR 0)
set(${name}_VERSION_PATCH 0)
set(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_USD_LOADER_VERSION ${${name}_VERSION} )

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions("-DDEBUG_ENABLED")
endif()

add_definitions("-DBUILDING_DALI_USD_LOADER")

foreach(flag ${PKGS_CFLAGS})
    set(extra_flags "${extra_flags} ${flag}")
endforeach(flag)

# The -fPIC option used here is to generate position-independent code (PIC) suitable for use in shared libraries.
# Unlike the -fPIE option which generates position-independent executables (PIE) suitable for use in executable binaries.
set(prj_cxx_std c++17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(extra_flags "${extra_flags} -fPIC -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(extra_flags "${extra_flags} -fPIC -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(extra_flags "${extra_flags} /std:${prj_cxx_std} /vmg /D_USE_MATH_DEFINES /D_CRT_SECURE_NO_WARNINGS /MP /GS /Oi /GL /EHsc")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags} -Wno-deprecated")

set(prefix ${CMAKE_INSTALL_PREFIX})

set(repo_root_dir "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(usd_loader_dir "${repo_root_dir}/dali-usd-loader")

option(ENABLE_PKG_CONFIGURE "Use pkgconfig" ON)
option(ENABLE_COVERAGE "Coverage" OFF)

if (ENABLE_PKG_CONFIGURE)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(DALICORE REQUIRED dali2-core)
    pkg_check_modules(DALIADAPTOR REQUIRED dali2-adaptor)

    # Configure the pkg-config file
    # Requires the following variables to be setup:
    # @PREFIX@ @EXEC_PREFIX@ @DALI_VERSION@ @LIB_DIR@ @DEV_INCLUDE_PATH@
    set( LIB_DIR $ENV{libdir} )
    if( NOT LIB_DIR )
        set( LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
    endif()
    if( NOT LIB_DIR )
        set( LIB_DIR ${prefix}/lib )
    endif()

    set(PREFIX ${prefix})
    set(EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})
    set(DEV_INCLUDE_PATH ${INCLUDE_DIR})

    set(core_pkg_cfg_file dali2-usd-loader.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${core_pkg_cfg_file}.in ${core_pkg_cfg_file} @ONLY)
endif()

set(usd_loader_src_files "")
include(${usd_loader_dir}/internal/file.list)

set(prefix_include_dir "${prefix}/include")

include_directories(BEFORE
  ${repo_root_dir}
  ${USD_ROOT_DIR}/include
)

include_directories(AFTER "${prefix_include_dir}")

MESSAGE(STATUS "USD Loader sources: ${usd_loader_src_files}")

ADD_LIBRARY("${name}" SHARED ${usd_loader_src_files} )

TARGET_LINK_LIBRARIES("${name}" ${DALICORE_LDFLAGS} "-L${USD_ROOT_DIR}/lib"
  dali2-toolkit
  dali2-scene3d
  -lusd_usd -lusd_sdf -lusd_tf -lusd_usdGeom -lusd_usdShade -lusd_usdSkel -lusd_gf -lusd_vt
  ${COVERAGE})
TARGET_COMPILE_OPTIONS("${name}" PUBLIC "-I${repo_root_dir}/dali-usd-loader/include")

IF (ENABLE_PKG_CONFIGURE)
  INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file}
    DESTINATION ${LIB_DIR}/pkgconfig )
ENDIF()

IF( INSTALL_CMAKE_MODULES )
  MESSAGE(STATUS "Installing cmake modules & libs")
  SET_TARGET_PROPERTIES( ${name}
    PROPERTIES
    VERSION ${DALI_USD_LOADER_VERSION}
    SOVERSION ${${name}_VERSION_MAJOR}
    CLEAN_DIRECT_OUPUT 1
    )

  IF( ENABLE_DEBUG )
    SET( BIN_DIR "${BIN_DIR}/debug" )
    SET( LIB_DIR "${LIB_DIR}/debug" )
  ENDIF()

  # Install library
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
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake DESTINATION share/${name})

ELSE()
  MESSAGE(STATUS "Installing libs")
  INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )

ENDIF()
