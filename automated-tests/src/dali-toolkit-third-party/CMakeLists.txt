SET(PKG_NAME "dali-toolkit-third-party")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-third-party")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
 utc-Dali-Flexbox-Layout.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_SOURCES
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-event-thread-callback.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-environment-variable.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-input-method-context.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-orientation.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-style-monitor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-timer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-tts-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/mesh-builder.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-actor-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-animation-data.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-button.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-harness.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-impl.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-object.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-command-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-framebuffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-texture.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sampler.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-pipeline.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-program.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-shader.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-reflection.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-platform-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-render-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-trace-call-stack.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-native-image.cpp
)


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali2-core
    dali2-adaptor
    dali2-toolkit
)

# Locate GTest
CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
FIND_PACKAGE(GTest REQUIRED)

#ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror )
# ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )
# This works for a homogenous C++ project, but not for mixed C++/C project
# On CMake >= 3.3, could use
# ADD_COMPILE_OPTIONS( "$<$<COMPILE_LANGUAGE:CXX>:${${CAPI_LIB}_CFLAGS_OTHER}>" )
# However, we need to support CMake 2.8, so instead, fall back to setting the CXX_FLAGS directly:
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -Wall -Werror" )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${CAPI_LIB}_CFLAGS_OTHER}" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

STRING(STRIP ${CMAKE_CXX_LINK_FLAGS} CMAKE_CXX_LINK_FLAGS)

INCLUDE_DIRECTORIES(
    ../../../
    ../../../dali-toolkit/third-party
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali-toolkit/dali-toolkit-test-utils
    ${GTEST_INCLUDE_DIRS}
)

ADD_CUSTOM_COMMAND(
  COMMAND ../../scripts/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT  ${EXEC_NAME}.h
  COMMENT "Generating test tables"
  )

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES} ${TEST_HARNESS_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${CMAKE_CXX_LINK_FLAGS}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread --coverage
    ${GTEST_LIBRARIES}
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
