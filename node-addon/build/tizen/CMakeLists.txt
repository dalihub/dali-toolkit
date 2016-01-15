# Guide to build for GBS
# Build dali-toolkit with gbs, the dali-addon will be built automatically
#
# To build for Desktop
# option 1 (easy):  in node-addon folder type
#
# $ node-gyp build
#
# option 2: use cmake
# This relies on Node source being downloaded ( don't forget the dot)
#
# cmake -DDesktopNodePath:String=/home/Downloads/node-v0.12.4 .


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(dali_addon)
set(configuration "Default")
enable_language(ASM)
set(builddir "${CMAKE_BINARY_DIR}")
set(obj "${builddir}/obj")

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

option(GBSBuild "GBSBuild" 0)
option(DesktopNodePath "DesktopNodePath" "")

IF( ${GBSBuild} )
  set( NODE_HEADERS "/usr/include/node" )
  set( V8_HEADERS "/usr/include/node" )
  MESSAGE("Performing a GBS build, ensure Node JS devel RPM is installed")
  MESSAGE("node.h and v8.h should exist in ${NODE_HEADERS}")
ELSE()
  MESSAGE("Performing Desktop build node path =${DesktopNodePath}")
  IF( NOT DesktopNodePath )
    MESSAGE(FATAL_ERROR "\n Node JS Source code path empty, please specify,e.g. \n cmake -DDesktopNodePath:String=~/Downloads/node-v0.12.4/  .")
  ENDIF( NOT DesktopNodePath )
  set( NODE_HEADERS "${DesktopNodePath}/src" )
  set( V8_HEADERS   "${DesktopNodePath}/deps/v8/include" )
  MESSAGE( "-- Node include header location = ${NODE_HEADERS} ")
  MESSAGE( "-- V8 include header location = ${V8_HEADERS} ")
ENDIF( ${GBSBuild})

set(TARGET "dali_addon")
set(TOOLSET "target")
set(PLUGING_SRC_DIR "../../../plugins/dali-script-v8/src")
set(NODE_ADDON_SRC "../..")
add_library(dali_addon SHARED
  ${NODE_ADDON_SRC}/dali-addon.cpp
  ${NODE_ADDON_SRC}/javascript-application-options.cpp
  ${PLUGING_SRC_DIR}/utils/v8-utils.cpp
  ${PLUGING_SRC_DIR}/dali-wrapper.cpp
  ${PLUGING_SRC_DIR}/shared/base-wrapped-object.cpp
  ${PLUGING_SRC_DIR}/shared/object-template-helper.cpp
  ${PLUGING_SRC_DIR}/actors/actor-wrapper.cpp
  ${PLUGING_SRC_DIR}/actors/actor-api.cpp
  ${PLUGING_SRC_DIR}/actors/layer-api.cpp
  ${PLUGING_SRC_DIR}/actors/camera-actor-api.cpp
  ${PLUGING_SRC_DIR}/constants/constants-wrapper.cpp
  ${PLUGING_SRC_DIR}/animation/animation-api.cpp
  ${PLUGING_SRC_DIR}/animation/animation-wrapper.cpp
  ${PLUGING_SRC_DIR}/animation/constrainer-api.cpp
  ${PLUGING_SRC_DIR}/animation/linear-constrainer-wrapper.cpp
  ${PLUGING_SRC_DIR}/animation/path-api.cpp
  ${PLUGING_SRC_DIR}/animation/path-constrainer-wrapper.cpp
  ${PLUGING_SRC_DIR}/animation/path-wrapper.cpp
  ${PLUGING_SRC_DIR}/stage/stage-wrapper.cpp
  ${PLUGING_SRC_DIR}/events/event-object-generator.cpp
  ${PLUGING_SRC_DIR}/events/pan-gesture-detector-api.cpp
  ${PLUGING_SRC_DIR}/events/pan-gesture-detector-wrapper.cpp
  ${PLUGING_SRC_DIR}/stage/stage-api.cpp
  ${PLUGING_SRC_DIR}/image/image-wrapper.cpp
  ${PLUGING_SRC_DIR}/image/image-api.cpp
  ${PLUGING_SRC_DIR}/image/buffer-image-api.cpp
  ${PLUGING_SRC_DIR}/image/frame-buffer-image-api.cpp
  ${PLUGING_SRC_DIR}/image/resource-image-api.cpp
  ${PLUGING_SRC_DIR}/object/handle-wrapper.cpp
  ${PLUGING_SRC_DIR}/object/property-buffer-api.cpp
  ${PLUGING_SRC_DIR}/object/property-buffer-wrapper.cpp
  ${PLUGING_SRC_DIR}/object/property-value-wrapper.cpp
  ${PLUGING_SRC_DIR}/signals/signal-manager.cpp
  ${PLUGING_SRC_DIR}/render-tasks/render-task-list-api.cpp
  ${PLUGING_SRC_DIR}/render-tasks/render-task-list-wrapper.cpp
  ${PLUGING_SRC_DIR}/render-tasks/render-task-api.cpp
  ${PLUGING_SRC_DIR}/render-tasks/render-task-wrapper.cpp
  ${PLUGING_SRC_DIR}/rendering/geometry-api.cpp
  ${PLUGING_SRC_DIR}/rendering/geometry-wrapper.cpp
  ${PLUGING_SRC_DIR}/rendering/material-api.cpp
  ${PLUGING_SRC_DIR}/rendering/material-wrapper.cpp
  ${PLUGING_SRC_DIR}/rendering/renderer-api.cpp
  ${PLUGING_SRC_DIR}/rendering/renderer-wrapper.cpp
  ${PLUGING_SRC_DIR}/rendering/sampler-api.cpp
  ${PLUGING_SRC_DIR}/rendering/sampler-wrapper.cpp
  ${PLUGING_SRC_DIR}/rendering/shader-api.cpp
  ${PLUGING_SRC_DIR}/rendering/shader-wrapper.cpp
  ${PLUGING_SRC_DIR}/toolkit/builder/builder-api.cpp
  ${PLUGING_SRC_DIR}/toolkit/builder/builder-wrapper.cpp
  ${PLUGING_SRC_DIR}/toolkit/focus-manager/keyboard-focus-manager-api.cpp
  ${PLUGING_SRC_DIR}/toolkit/focus-manager/keyboard-focus-manager-wrapper.cpp
  ${PLUGING_SRC_DIR}/signals/dali-any-javascript-converter.cpp
  ${PLUGING_SRC_DIR}/garbage-collector/garbage-collector.cpp
  ${PLUGING_SRC_DIR}/module-loader/module.cpp
  ${PLUGING_SRC_DIR}/module-loader/module-loader.cpp
)
set_target_properties(dali_addon PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${builddir}/lib.${TOOLSET}")
set_target_properties(dali_addon PROPERTIES PREFIX "")
set_target_properties(dali_addon PROPERTIES LIBRARY_OUTPUT_NAME "dali")
set_target_properties(dali_addon PROPERTIES SUFFIX ".node")
set_source_files_properties(${builddir}/lib.${TOOLSET}/dali.node PROPERTIES GENERATED "TRUE")
set_target_properties(dali_addon PROPERTIES EXCLUDE_FROM_ALL "FALSE")


SET(PKG_LIST dali
             dali-toolkit)

INCLUDE(FindPkgConfig)
pkg_check_modules(REQUIRED_PKGS REQUIRED ${PKG_LIST})

# This is just pulling out dali / dali-toolkit header locations from pkg-config
FOREACH(flag ${REQUIRED_PKGS_CFLAGS})
        SET(REQUIRED_CFLAGS "${REQUIRED_CFLAGS} ${flag}")
ENDFOREACH(flag)

# Set the include paths to include dali, dali-core, node, v8 and headers for dali-addon
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${REQUIRED_CFLAGS} -I${PLUGING_SRC_DIR}/utils -I${PLUGING_SRC_DIR}  -I${NODE_HEADERS} -I ${V8_HEADERS}")

set_target_properties(dali_addon PROPERTIES COMPILE_FLAGS "-fPIC -frtti -fexceptions")

# This sets up the linker flags for dali-core/ adaptor and toolkit
# we don't need any flags for node / v8 as we're running inside node and v8 is statically linked to it
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${REQUIRED_PKGS_LDFLAGS})

# ${CMAKE_INSTALL_PREFIX}
#################################### install #####################################
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} )

unset(TOOLSET)
unset(TARGET)
