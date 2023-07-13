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

target_link_libraries(bullet3 ${COVERAGE})

# Installation
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_DESTINATION "lib${LIB_SUFFIX}" CACHE STRING "Library directory name")
## the following are directories where stuff will be installed to
set(INCLUDE_INSTALL_DIR "include/bullet/" CACHE PATH "The subdirectory to the header prefix")
set(PKGCONFIG_INSTALL_PREFIX "lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")
if(NOT MSVC)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/bullet3.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/bullet3.pc @ONLY)
install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/bullet3.pc
  DESTINATION
  ${PKGCONFIG_INSTALL_PREFIX})
endif(NOT MSVC)

install(
    TARGETS bullet3
    DESTINATION ${LIB_DIR}
    EXPORT Bullet3Targets
)

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
        FILES_MATCHING PATTERN "*.h"
    )
endforeach()
