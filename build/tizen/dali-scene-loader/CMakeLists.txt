cmake_minimum_required(VERSION 2.6)
set(name "dali2-scene-loader")

project(${name} CXX)

set(${name}_VERSION_MAJOR 2)
set(${name}_VERSION_MINOR 0)
set(${name}_VERSION_PATCH 0)
set(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH} )

SET(DALI_SCENE_LOADER_VERSION ${${name}_VERSION} )

if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions("-DDEBUG_ENABLED")
endif()

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
set(scene_loader_dir "${repo_root_dir}/dali-scene-loader")

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
	set(DEV_INCLUDE_PATH ${repo_root_dir})

	set(core_pkg_cfg_file dali2-scene-loader.pc)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/${core_pkg_cfg_file}.in ${core_pkg_cfg_file} @ONLY)
endif()

if (ENABLE_COVERAGE)
	find_program(lcov_bin "lcov")
	if (${lcov_bin})
		set(cov_dir ".cov")
		set(cov_output_dir "doc/coverage")

		execute_process(COMMAND bash -c "${lcov_bin} --version | cut -d' ' -f4" OUTPUT_VARIABLE lcov_version)
		string(REPLACE "." ";" lcov_vlist ${lcov_version})
		if (NOT $<VERSION_LESS:${lcov_version},"1.10"> )
			set(lcov_opts --rc lcov_branch_coverage=1)
		endif()

		add_custom_target(${prefix}rename_cov_data ./rename-cov-data)

		add_custom_target(${prefix}cov_data
			${lcov_bin} ${lcov_opts} --base-directory . --directory . -c -o dali.info
			COMMAND ${lcov_bin} ${lcov_opts} --remove dali.info \"*/dali-env/*\" \"/usr/include/*\" \"/usr/local/include/*\" \"*/dali-env/*\" \"*/dali-scene-loader/third-party/*\" -o dali.info
		)

		add_custom_target(${prefix}coverage genhtml ${lcov_opts} -o ${cov_output_dir} dali.info)

		add_custom_target(${prefix}reset_coverage @${lcov_bin} -\ --direwctory `pwd`)

		add_custom_target(${prefix}distclean @echo cleaning for source distribution)
		add_custom_command(
			DEPENDS ${prefix}clean
			COMMENT "distribution clean"
			COMMAND find
			ARGS .
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
	endif()
endif()

set(scene_loader_src_files "")
include("${scene_loader_dir}/internal/file.list")
include("${scene_loader_dir}/public-api/file.list")

set(prefix_include_dir "${prefix}/include")
include_directories(${repo_root_dir}
	${prefix_include_dir}
)

add_library(${name} SHARED ${scene_loader_src_files})

target_link_libraries(${name} ${DALICORE_LDFLAGS} ${DALIADAPTOR_LDFLAGS}
	dali2-toolkit
	${COVERAGE})

if( ANDROID )
	target_link_libraries(${name} log)
endif()

install(TARGETS ${name} DESTINATION ${prefix}/lib)

file(GLOB scene_loader_include_files ${scene_loader_public_api_dir})
install(DIRECTORY ${scene_loader_include_files}
	DESTINATION "${prefix_include_dir}/dali-scene-loader"
	FILES_MATCHING PATTERN "*.h"
)

if (ENABLE_PKG_CONFIGURE)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${core_pkg_cfg_file}
		DESTINATION ${LIB_DIR}/pkgconfig
	)
endif()
