CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
set(name "dali2-scene3d")

project(${name} CXX)

set(${name}_VERSION_MAJOR 2)
set(${name}_VERSION_MINOR 0)
set(${name}_VERSION_PATCH 0)
set(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_SCENE3D_VERSION ${${name}_VERSION} )

if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions("-DDEBUG_ENABLED")
endif()

ADD_DEFINITIONS( "-DBUILDING_DALI_SCENE3D" )

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
set(scene3d_dir "${repo_root_dir}/dali-scene3d")

option(ENABLE_PKG_CONFIGURE "Use pkgconfig" ON)
option(ENABLE_COVERAGE "Coverage" OFF)

IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage )
  ADD_COMPILE_OPTIONS( --coverage )
  SET(ENABLE_COVERAGE ON)
  SET(COVERAGE --coverage)
ENDIF()

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

	set(core_pkg_cfg_file dali2-scene3d.pc)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/${core_pkg_cfg_file}.in ${core_pkg_cfg_file} @ONLY)
endif()

set(scene3d_src_files "")
include("${scene3d_dir}/internal/file.list")
include("${scene3d_dir}/public-api/file.list")

set(prefix_include_dir "${prefix}/include")
include_directories(${repo_root_dir}
	${prefix_include_dir}
)

# Generate source files for shaders
SET(SHADER_SOURCE_DIR "${scene3d_dir}/internal/graphics/shaders/")
SET(SHADER_GENERATED_DIR "${scene3d_dir}/internal/graphics/generated")

SET(GENERATED_SHADER_DIR ${scene3d_dir}/internal/graphics/)
SET_PROPERTY(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
             "${GENERATED_SHADER_DIR}/generated/"
             "${GENERATED_SHADER_DIR}/builtin-shader-extern-gen.h")

IF( WIN32)
  # When Using VCPKG, the default is always set to Debug if CMAKE_BUILD_TYPE is not set
  IF( NOT CMAKE_BUILD_TYPE )
    SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/../Debug/${SHADER_GENERATOR_NAME})
  ELSE()
    SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/../${CMAKE_BUILD_TYPE}/${SHADER_GENERATOR_NAME})
  ENDIF()
ELSE()
  SET(SHADER_GENERATOR_BINARY ${CMAKE_CURRENT_BINARY_DIR}/../${SHADER_GENERATOR_NAME})
ENDIF()

FILE(GLOB SHADERS_SRC "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")
SET( BUILT_IN_SHADER_GEN_CPP "${GENERATED_SHADER_DIR}/generated/builtin-shader-gen.cpp")
ADD_CUSTOM_COMMAND(OUTPUT ${BUILT_IN_SHADER_GEN_CPP}
                   DEPENDS ${SHADER_GENERATOR_BINARY} ${SHADERS_SRC}
                   COMMAND ${SHADER_GENERATOR_BINARY} ${SHADER_SOURCE_DIR} ${SHADER_GENERATED_DIR})

SET( scene3d_src_files ${scene3d_src_files} ${BUILT_IN_SHADER_GEN_CPP} )

add_library(${name} SHARED ${scene3d_src_files})

target_link_libraries(${name} ${DALICORE_LDFLAGS} ${DALIADAPTOR_LDFLAGS}
	dali2-toolkit
	${COVERAGE})

if( ANDROID )
	target_link_libraries(${name} log)
endif()

IF( INSTALL_CMAKE_MODULES )
	SET_TARGET_PROPERTIES( ${name}
		PROPERTIES
		VERSION ${DALI_SCENE3D_VERSION}
		SOVERSION ${${name}_VERSION_MAJOR}
		CLEAN_DIRECT_OUPUT 1
	)

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
	IF( ENABLE_DEBUG AND WIN32 )
		install( FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${name}.pdb DESTINATION ${BIN_DIR} )
	ENDIF()
ELSE()
	# Install the library so file and symlinks
	INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )
ENDIF()

file(GLOB scene3d_include_files ${scene3d_public_api_dir})

install(DIRECTORY ${scene3d_include_files}
	DESTINATION "${INCLUDE_DIR}/dali-scene3d"
	FILES_MATCHING PATTERN "*.h"
)

# Install dali-scene3d.h
INSTALL( FILES ${scene3d_dir}/dali-scene3d.h DESTINATION ${INCLUDE_DIR}/dali-scene3d )

if (ENABLE_PKG_CONFIGURE)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file}
		DESTINATION ${LIB_DIR}/pkgconfig
	)
endif()
