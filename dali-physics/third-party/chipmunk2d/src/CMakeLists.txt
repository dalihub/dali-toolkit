file(GLOB chipmunk_source_files "*.c")
file(GLOB chipmunk_public_header "${chipmunk_SOURCE_DIR}/include/chipmunk/*.h")

include_directories(BEFORE ${chipmunk_SOURCE_DIR}/include)

# Chipmunk2D 7.0.3
set(CHIPMUNK_VERSION_MAJOR 7)
set(CHIPMUNK_VERSION_MINOR 0)
set(CHIPMUNK_VERSION_PATCH 3)
set(CHIPMUNK_VERSION "${CHIPMUNK_VERSION_MAJOR}.${CHIPMUNK_VERSION_MINOR}.${CHIPMUNK_VERSION_PATCH}")
message("Configuring Chipmunk2D version ${CHIPMUNK_VERSION}")

set(prefix ${CMAKE_INSTALL_PREFIX})
option(ENABLE_PKG_CONFIGURE "Use pkgconfig" ON)

# Installed names. DALi passes DALI_THIRD_PARTY_PREFIX ("dali3-" for API
# major 3, empty for 2) and INCLUDE_DIR so that a chipmunk bundled with one
# DALi major never shares a file with the one bundled with another:
# lib<prefix>chipmunk.so, <prefix>chipmunk2d.pc, <includedir>/chipmunk.
if(NOT DEFINED DALI_THIRD_PARTY_PREFIX)
    set(DALI_THIRD_PARTY_PREFIX "")
endif()
set(CHIPMUNK_LIB_NAME ${DALI_THIRD_PARTY_PREFIX}chipmunk)
set(CHIPMUNK_PC_NAME ${DALI_THIRD_PARTY_PREFIX}chipmunk2d)
if(INCLUDE_DIR)
    set(CHIPMUNK_INCLUDE_INSTALL_DIR ${INCLUDE_DIR}/chipmunk)
else()
    set(CHIPMUNK_INCLUDE_INSTALL_DIR include/chipmunk)
endif()

if(ENABLE_PKG_CONFIGURE)
    find_package(PkgConfig REQUIRED)

    # Configure the pkg-config file
    # Requires the following variables to be setup:
    # @PREFIX@ @EXEC_PREFIX@ @CHIPMUNK_VERSION@ @LIB_DIR@ @DEV_INCLUDE_PATH@
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

    set(chipmunk_pkg_cfg_file ${CHIPMUNK_PC_NAME}.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/chipmunk2d.pc.in ${chipmunk_pkg_cfg_file} @ONLY)
endif(ENABLE_PKG_CONFIGURE)

if(ANDROID)
  FIND_LIBRARY(LOGLIB log)
endif(ANDROID)

# If you want to reduce the size of the library slightly, uncomment this section
#
#if(NOT ENABLE_DEBUG)
#  add_definitions( "-DCP_SPACE_DISABLE_DEBUG_API")
#endif()

if(BUILD_SHARED)
  add_library(chipmunk SHARED
    ${chipmunk_source_files}
  )
  # Tell MSVC to compile the code as C++.
  if(MSVC)
    set_source_files_properties(${chipmunk_source_files} PROPERTIES LANGUAGE CXX)
    set_target_properties(chipmunk PROPERTIES LINKER_LANGUAGE CXX)
  endif(MSVC)
  # set the lib's version number
  # But avoid on Android because symlinks to version numbered .so's don't work with Android's Java-side loadLibrary.
  set_target_properties(chipmunk PROPERTIES OUTPUT_NAME ${CHIPMUNK_LIB_NAME})
  if(NOT ANDROID)
    set_target_properties(chipmunk PROPERTIES
      SOVERSION ${CHIPMUNK_VERSION_MAJOR}
      VERSION ${CHIPMUNK_VERSION})
  endif(NOT ANDROID)
  target_compile_options(chipmunk PRIVATE -fvisibility=default)
  if(ANDROID OR UNIX)
    # need to explicitly link to the math library because the CMake/Android toolchains may not do it automatically
    target_link_libraries(chipmunk m ${COVERAGE})
  endif(ANDROID OR UNIX)
  if(ANDROID)
    target_link_libraries(chipmunk ${LOGLIB})
  endif(ANDROID)
  install(TARGETS chipmunk EXPORT chipmunk-targets
                           RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                           LIBRARY DESTINATION ${LIB_INSTALL_DIR}
                           ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
                           )
  # DALi 2.x shipped this library as libchipmunk with chipmunk2d.pc and
  # include/chipmunk; DALI_LEGACY_ALIASES=ON adds those names as links.
  if(COMMAND dali_legacy_library_alias)
    if(ANDROID)
      dali_legacy_library_alias(chipmunk chipmunk)
    else()
      dali_legacy_library_alias(chipmunk chipmunk ${CHIPMUNK_VERSION_MAJOR})
    endif()
    dali_legacy_include_alias(chipmunk)
    if(ENABLE_PKG_CONFIGURE)
      dali_legacy_pkgconfig_alias(${CMAKE_CURRENT_BINARY_DIR}/${chipmunk_pkg_cfg_file} chipmunk2d)
    endif()
  endif()
endif(BUILD_SHARED)

if(BUILD_STATIC)
  add_library(chipmunk_static STATIC
    ${chipmunk_source_files}
  )
  # Tell MSVC to compile the code as C++.
  if(MSVC)
    set_source_files_properties(${chipmunk_source_files} PROPERTIES LANGUAGE CXX)
    set_target_properties(chipmunk_static PROPERTIES LINKER_LANGUAGE CXX)
  endif(MSVC)
  # Sets chipmunk_static to output "lib<prefix>chipmunk.a" not "libchipmunk_static.a"
  set_target_properties(chipmunk_static PROPERTIES OUTPUT_NAME ${CHIPMUNK_LIB_NAME})
  target_compile_options(chipmunk PRIVATE -fvisibility=default)
  target_link_libraries(chipmunk m ${COVERAGE})
  if(ANDROID)
    target_link_libraries(chipmunk ${LOGLIB})
  endif(ANDROID)
  if(INSTALL_STATIC)
    install(TARGETS chipmunk_static EXPORT chipmunk-targets ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
  endif(INSTALL_STATIC)
endif(BUILD_STATIC)

if(BUILD_SHARED OR INSTALL_STATIC)
  # FIXME: change to PUBLIC_HEADER to allow building frameworks
  install(FILES ${chipmunk_public_header} DESTINATION ${CHIPMUNK_INCLUDE_INSTALL_DIR})
  install(FILES ${chipmunk_constraint_header} DESTINATION ${CHIPMUNK_INCLUDE_INSTALL_DIR}/constraints)

  if(INSTALL_CMAKE_MODULES)
    install(
      EXPORT chipmunk-targets
      NAMESPACE chipmunk::
      FILE ${CHIPMUNK_LIB_NAME}-targets.cmake
      DESTINATION share/${CHIPMUNK_LIB_NAME}
    )

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CHIPMUNK_LIB_NAME}-config.cmake "
      include(CMakeFindDependencyMacro)
      include(\${CMAKE_CURRENT_LIST_DIR}/${CHIPMUNK_LIB_NAME}-targets.cmake)
    ")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CHIPMUNK_LIB_NAME}-config.cmake DESTINATION share/${CHIPMUNK_LIB_NAME})
  endif()
endif(BUILD_SHARED OR INSTALL_STATIC)

if (ENABLE_PKG_CONFIGURE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${chipmunk_pkg_cfg_file}
        DESTINATION ${LIB_DIR}/pkgconfig
    )
endif()
