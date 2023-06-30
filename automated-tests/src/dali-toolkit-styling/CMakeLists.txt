SET(PKG_NAME "dali-toolkit-styling")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-styling")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
   utc-Dali-StyleManager.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_SOURCES
   ../dali-toolkit/dali-toolkit-test-utils/test-harness.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-adaptor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-color-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-event-thread-callback.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-environment-variable.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-feedback-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-input-method-context.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-input-method-options.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-lifecycle-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-native-image-source.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-orientation.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-style-monitor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-sound-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-text-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-timer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-tts-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-video-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-virtual-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-window.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-scene-holder.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dummy-control.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-toolkit-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-animation-data.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-button.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-platform-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-impl.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-object.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-command-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-framebuffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-texture.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-pipeline.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-program.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sampler.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-shader.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-reflection.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-render-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-trace-call-stack.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-native-image.cpp
)


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
  dali2-core
  dali2-adaptor
  dali2-toolkit
)

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror )
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../../
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ${DALI_ADAPTOR_INCLUDE_DIRS}
    ../dali-toolkit/dali-toolkit-test-utils
)

ADD_CUSTOM_COMMAND(
  COMMAND ../../scripts/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT  ${EXEC_NAME}.h
  COMMENT "Generating test tables"
  )

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES} ${TEST_HARNESS_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
  ${${CAPI_LIB}_LIBRARIES}
  -lpthread -ldl --coverage
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
