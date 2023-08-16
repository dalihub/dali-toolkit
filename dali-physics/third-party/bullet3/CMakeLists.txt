cmake_minimum_required(VERSION 3.10.2)

unset(PROJECT_IS_TOPLEVEL)

project(bullet3
    VERSION 3.2.5
    DESCRIPTION "C++ library portion of bullet"
    LANGUAGES CXX
)

FILE (STRINGS "VERSION" BULLET_VERSION)

# PROJECT_IS_TOPLEVEL for older CMake versions
if(NOT DEFINED PROJECT_IS_TOPLEVEL)
    set(PROJECT_IS_TOPLEVEL OFF)
    if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
        set(PROJECT_IS_TOPLEVEL ON)
    endif()
endif()

# Build options
option(BULLET3_BUILD_SHARED "Build bullet3 as a shared library" ON)

set(prefix ${CMAKE_INSTALL_PREFIX})
option(ENABLE_PKG_CONFIGURE "Use pkgconfig" ON)

ADD_DEFINITIONS(-DB3_USE_CLEW)

if (WIN32)
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_DEPRECATE )
    ADD_DEFINITIONS( -D_SCL_SECURE_NO_WARNINGS )
endif(WIN32)

# Bullet is broken into sub-"libraries"; we'll automate handling of these
set(BULLET_SUBMODULES
    clew
    Bullet3Collision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Geometry
    Bullet3OpenCL
    Bullet3Serialize
    BulletCollision
    BulletDynamics
    BulletInverseDynamics
    BulletSoftBody
    LinearMath
)

set(repo_root_dir "${CMAKE_CURRENT_LIST_DIR}/../../../")
set(bullet_SOURCE_DIR "${repo_root_dir}/dali-physics/third-party/bullet3")

set(BULLET3_SOURCES
    "${bullet_SOURCE_DIR}/src/btBulletCollisionCommon.h"
    "${bullet_SOURCE_DIR}/src/btBulletDynamicsCommon.h"
)

foreach(SUB ${BULLET_SUBMODULES})
    file(GLOB_RECURSE SUBMODULE_SOURCES
        "${bullet_SOURCE_DIR}/src/${SUB}/*.h"
        "${bullet_SOURCE_DIR}/src/${SUB}/*.hpp"
        "${bullet_SOURCE_DIR}/src/${SUB}/*.cpp"
        "${bullet_SOURCE_DIR}/src/${SUB}/*.c"
    )
    list(APPEND BULLET3_SOURCES ${SUBMODULE_SOURCES})
endforeach()

# CPack support
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)

# Build the main bullet3 library
set(BULLET3_LIBRARY_TYPE "STATIC")
if(BULLET3_BUILD_SHARED)
    set(BULLET3_LIBRARY_TYPE "SHARED")
endif()
add_library(bullet3 ${BULLET3_LIBRARY_TYPE} ${BULLET3_SOURCES})
set_target_properties(bullet3 PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS 1
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
)
target_include_directories(bullet3
    SYSTEM INTERFACE
        $<BUILD_INTERFACE:${bullet_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include/bullet>
    PRIVATE
        ${bullet_SOURCE_DIR}/src
)

# Suppress the warnings in the libbullet source code
if(MSVC)
    target_compile_options(bullet3 PRIVATE /w)
else()
    target_compile_options(bullet3 PRIVATE -w)
endif()

target_compile_options(bullet3 PRIVATE -fvisibility=default)

target_link_libraries(bullet3 ${COVERAGE})

if(ENABLE_PKG_CONFIGURE)
    find_package(PkgConfig REQUIRED)

    set( LIB_DIR $ENV{libdir} )
    if( NOT LIB_DIR )
        set( LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
    endif()
    if( NOT LIB_DIR )
        set( LIB_DIR ${prefix}/lib )
    endif()
    set(DEV_INCLUDE_PATH ${prefix}/include/bullet)

    set(bullet_pkg_cfg_file bullet3.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${bullet_pkg_cfg_file}.cmake ${bullet_pkg_cfg_file} @ONLY)

    # Installation
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${bullet_pkg_cfg_file}
        DESTINATION ${LIB_DIR}/pkgconfig
    )
endif(ENABLE_PKG_CONFIGURE)

install(
    TARGETS bullet3
    DESTINATION ${LIB_DIR}
    EXPORT bullet3-targets
)

if(INSTALL_CMAKE_MODULES)
  install(
      EXPORT bullet3-targets
      NAMESPACE bullet3::
      FILE bullet3-targets.cmake
      DESTINATION share/bullet3
  )
endif()

if(BULLET3_BUILD_SHARED AND MSVC)
    install(FILES $<TARGET_PDB_FILE:bullet3> DESTINATION lib)
endif()

install(FILES
    "${bullet_SOURCE_DIR}/src/btBulletCollisionCommon.h"
    "${bullet_SOURCE_DIR}/src/btBulletDynamicsCommon.h"
    DESTINATION "include/bullet"
)
foreach(SUB ${BULLET_SUBMODULES})
    install(
        DIRECTORY "${bullet_SOURCE_DIR}/src/${SUB}"
        DESTINATION "include/bullet"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
endforeach()

if(INSTALL_CMAKE_MODULES)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/bullet3-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/bullet3-targets.cmake)
  ")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bullet3-config.cmake DESTINATION share/bullet3)
endif()
