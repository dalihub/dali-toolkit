cmake_minimum_required(VERSION 3.7)

project(chipmunk)

# to change the prefix, run cmake with the parameter:
#   -D CMAKE_INSTALL_PREFIX=/my/prefix

# to change the build type, run cmake with the parameter:
#   -D CMAKE_BUILD_TYPE=<build-type>
# run "cmake --help-variable CMAKE_BUILD_TYPE" for details
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# to manually select install locations of libraries and executables
#   -D LIB_INSTALL_DIR mylib
#   -D BIN_INSTALL_DIR newbin
set(LIB_INSTALL_DIR ${LIB_DIR} CACHE STRING "Install location of libraries")
set(BIN_INSTALL_DIR bin CACHE STRING "Install location of executables")

# other options for the build, you can i.e. activate the shared library by passing
#   -D BUILD_SHARED=ON
# to cmake. Other options analog
if(ANDROID)
  option(BUILD_SHARED "Build and install the shared library" ON)
  option(BUILD_STATIC "Build as static library" ON)
  option(INSTALL_STATIC "Install the static library" OFF)
else()
  option(BUILD_SHARED "Build and install the shared library" ON)
  option(BUILD_STATIC "Build as static library" ON)
  option(INSTALL_STATIC "Install the static library" ON)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  option(FORCE_CLANG_BLOCKS "Force enable Clang blocks" YES)
endif()

if(INSTALL_STATIC)
  set(BUILD_STATIC ON FORCE)
endif()

if(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99") # always use gnu99
  include(CheckCCompilerFlag)
  check_c_compiler_flag(-Wno-alloc-size-larger-than HAVE_NO_ALLOC_SIZE_LARGER_THAN)
  if(HAVE_NO_ALLOC_SIZE_LARGER_THAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-alloc-size-larger-than")
  endif()
  if(FORCE_CLANG_BLOCKS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fblocks")
  endif()
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math") # extend release-profile with fast-math
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall") # extend debug-profile with -Wall
endif()

add_subdirectory(src)
