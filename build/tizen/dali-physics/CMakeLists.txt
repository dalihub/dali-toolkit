CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
set(name "dali2-physics")

project(${name} CXX)

set(${name}_VERSION_MAJOR 1)
set(${name}_VERSION_MINOR 0)
set(${name}_VERSION_PATCH 0)
set(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_PHYSICS_VERSION ${${name}_VERSION} )

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions("-DDEBUG_ENABLED")
endif()

add_definitions("-DBUILDING_DALI_PHYSICS")

foreach(flag ${PKGS_CFLAGS})
    set(extra_flags "${extra_flags} ${flag}")
endforeach(flag)

set(prj_cxx_std c++17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(extra_flags "${extra_flags} -fPIC -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(extra_flags "${extra_flags} -fPIC -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(extra_flags "${extra_flags} /std:${prj_cxx_std} /vmg /D_USE_MATH_DEFINES /D_CRT_SECURE_NO_WARNINGS /MP /GS /Oi /GL /EHsc")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")

set(prefix ${CMAKE_INSTALL_PREFIX})

set(repo_root_dir "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(physics_dir "${repo_root_dir}/dali-physics")

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

    set(core_pkg_cfg_file_2d dali2-physics-2d.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${core_pkg_cfg_file_2d}.in ${core_pkg_cfg_file_2d} @ONLY)

    set(core_pkg_cfg_file_3d dali2-physics-3d.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${core_pkg_cfg_file_3d}.in ${core_pkg_cfg_file_3d} @ONLY)
endif()

add_subdirectory("${physics_dir}/third-party/chipmunk2d" chipmunk2d)
add_subdirectory("${physics_dir}/third-party/bullet3" bullet3)

set(physics_src_files "")
include(${physics_dir}/public-api/file.list)
include(${physics_dir}/internal/file.list)

set(prefix_include_dir "${prefix}/include")

include_directories(BEFORE
  ${repo_root_dir}
  "${prefix_include_dir}"
  "${repo_root_dir}/dali-physics/third-party/bullet3/src"
  "${repo_root_dir}/dali-physics/third-party/chipmunk2d/include"
)

MESSAGE(STATUS "2D sources: ${physics2d_src_files}")
MESSAGE(STATUS "3D sources: ${physics3d_src_files}")

ADD_LIBRARY("${name}-2d" SHARED ${physics2d_src_files} )
TARGET_LINK_LIBRARIES("${name}-2d" ${DALICORE_LDFLAGS}
  dali2-toolkit
  chipmunk
  ${COVERAGE})
TARGET_COMPILE_OPTIONS("${name}-2d" PUBLIC "-I${repo_root_dir}/dali-physics/third-party/chipmunk2d/include")

ADD_LIBRARY("${name}-3d" SHARED ${physics3d_src_files} )
TARGET_LINK_LIBRARIES("${name}-3d" ${DALICORE_LDFLAGS}
  dali2-toolkit
  bullet3
  ${COVERAGE})
TARGET_COMPILE_OPTIONS("${name}-3d" PUBLIC "-I${repo_root_dir}/dali-physics/third-party/bullet3/src")

IF (ENABLE_PKG_CONFIGURE)
  INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file_2d}
    ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file_3d}
    DESTINATION ${LIB_DIR}/pkgconfig )
ENDIF()


IF( INSTALL_CMAKE_MODULES )
  MESSAGE(STATUS "Installing cmake modules & libs")
  SET_TARGET_PROPERTIES( ${name}-2d
    PROPERTIES
    VERSION ${DALI_PHYSICS_VERSION}
    SOVERSION ${${name}_VERSION_MAJOR}
    CLEAN_DIRECT_OUPUT 1
    )

  SET_TARGET_PROPERTIES( ${name}-3d
    PROPERTIES
    VERSION ${DALI_PHYSICS_VERSION}
    SOVERSION ${${name}_VERSION_MAJOR}
    CLEAN_DIRECT_OUPUT 1
    )

  IF( ENABLE_DEBUG )
    SET( BIN_DIR "${BIN_DIR}/debug" )
    SET( LIB_DIR "${LIB_DIR}/debug" )
  ENDIF()

  # Install library
  INSTALL( TARGETS ${name}-2d
    EXPORT ${name}-2d-targets
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR}
    RUNTIME DESTINATION ${BIN_DIR}
  )

  INSTALL( TARGETS ${name}-3d
    EXPORT ${name}-3d-targets
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR}
    RUNTIME DESTINATION ${BIN_DIR}
  )

  # Install the cmake modules.
  INSTALL(
    EXPORT ${name}-2d-targets
    NAMESPACE ${name}-2d::
    FILE ${name}-2d-targets.cmake
    DESTINATION share/${name}-2d
  )

  INSTALL(
    EXPORT ${name}-3d-targets
    NAMESPACE ${name}-3d::
    FILE ${name}-3d-targets.cmake
    DESTINATION share/${name}-3d
  )

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-2d-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-2d-targets.cmake)
  ")
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-2d-config.cmake DESTINATION share/${name}-2d)

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-3d-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-3d-targets.cmake)
  ")
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-3d-config.cmake DESTINATION share/${name}-3d)

ELSE()
  MESSAGE(STATUS "Installing libs")
  INSTALL( TARGETS ${name}-2d DESTINATION ${LIB_DIR} )
  INSTALL( TARGETS ${name}-3d DESTINATION ${LIB_DIR} )

ENDIF()

# Install headers
install( FILES ${physics_public_api_header_files}
  DESTINATION "${INCLUDE_DIR}/dali-physics/public-api"
  )

install( FILES ${physics_dir}/dali-physics.h
  DESTINATION "${INCLUDE_DIR}/dali-physics"
  )
