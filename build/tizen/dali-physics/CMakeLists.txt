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

set(repo_root_dir "${CMAKE_CURRENT_LIST_DIR}/../../../")
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

if (ENABLE_PKG_CONFIGURE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file_2d} ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file_3d}
        DESTINATION ${LIB_DIR}/pkgconfig
    )
endif()

if( INSTALL_CMAKE_MODULES )

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-2d-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-2d-targets.cmake)
  ")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-2d-config.cmake DESTINATION share/${name}-2d)

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}-3d-config.cmake "
    include(CMakeFindDependencyMacro)
    include(\${CMAKE_CURRENT_LIST_DIR}/${name}-3d-targets.cmake)
  ")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}-3d-config.cmake DESTINATION share/${name}-3d)
endif()


