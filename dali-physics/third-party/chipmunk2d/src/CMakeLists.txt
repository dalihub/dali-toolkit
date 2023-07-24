file(GLOB chipmunk_source_files "*.c")
file(GLOB chipmunk_public_header "${chipmunk_SOURCE_DIR}/include/chipmunk/*.h")

include_directories(${chipmunk_SOURCE_DIR}/include)

# Chipmunk2D 7.0.3
set(CHIPMUNK_VERSION_MAJOR 7)
set(CHIPMUNK_VERSION_MINOR 0)
set(CHIPMUNK_VERSION_PATCH 3)
set(CHIPMUNK_VERSION "${CHIPMUNK_VERSION_MAJOR}.${CHIPMUNK_VERSION_MINOR}.${CHIPMUNK_VERSION_PATCH}")
message("Configuring Chipmunk2D version ${CHIPMUNK_VERSION}")

set(prefix ${CMAKE_INSTALL_PREFIX})
option(ENABLE_PKG_CONFIGURE "Use pkgconfig" ON)

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

    set(chipmunk_pkg_cfg_file chipmunk2d.pc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${chipmunk_pkg_cfg_file}.in ${chipmunk_pkg_cfg_file} @ONLY)
endif(ENABLE_PKG_CONFIGURE)

if(ANDROID)
  FIND_LIBRARY(LOGLIB log)
endif(ANDROID)

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
  # Sets chipmunk_static to output "libchipmunk.a" not "libchipmunk_static.a"
  set_target_properties(chipmunk_static PROPERTIES OUTPUT_NAME chipmunk)
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
  install(FILES ${chipmunk_public_header} DESTINATION include/chipmunk)
  install(FILES ${chipmunk_constraint_header} DESTINATION include/chipmunk/constraints)

  if(INSTALL_CMAKE_MODULES)
    install(
      EXPORT chipmunk-targets
      NAMESPACE chipmunk::
      FILE chipmunk-targets.cmake
      DESTINATION share/chipmunk
    )

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/chipmunk-config.cmake "
      include(CMakeFindDependencyMacro)
      include(\${CMAKE_CURRENT_LIST_DIR}/chipmunk-targets.cmake)
    ")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/chipmunk-config.cmake DESTINATION share/chipmunk)
  endif()
endif(BUILD_SHARED OR INSTALL_STATIC)

if (ENABLE_PKG_CONFIGURE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${chipmunk_pkg_cfg_file}
        DESTINATION ${LIB_DIR}/pkgconfig
    )
endif()

