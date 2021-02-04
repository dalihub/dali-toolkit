SET(PKG_NAME "dali-scene-loader-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-scene-loader")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
  utc-Dali-Gltf2Asset.cpp
  utc-Dali-Hash.cpp
  utc-Dali-JsonReader.cpp
  utc-Dali-JsonUtil.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_DIR "../dali-toolkit/dali-toolkit-test-utils")

SET(TEST_HARNESS_SOURCES
  ${TEST_HARNESS_DIR}/toolkit-adaptor.cpp
  ${TEST_HARNESS_DIR}/toolkit-application.cpp
  ${TEST_HARNESS_DIR}/toolkit-event-thread-callback.cpp
  ${TEST_HARNESS_DIR}/toolkit-environment-variable.cpp
  ${TEST_HARNESS_DIR}/toolkit-input-method-context.cpp
  ${TEST_HARNESS_DIR}/toolkit-input-method-options.cpp
  ${TEST_HARNESS_DIR}/toolkit-lifecycle-controller.cpp
  ${TEST_HARNESS_DIR}/toolkit-orientation.cpp
  ${TEST_HARNESS_DIR}/toolkit-style-monitor.cpp
  ${TEST_HARNESS_DIR}/toolkit-test-application.cpp
  ${TEST_HARNESS_DIR}/toolkit-timer.cpp
  ${TEST_HARNESS_DIR}/toolkit-trigger-event-factory.cpp
  ${TEST_HARNESS_DIR}/toolkit-window.cpp
  ${TEST_HARNESS_DIR}/toolkit-scene-holder.cpp
  ${TEST_HARNESS_DIR}/dali-test-suite-utils.cpp
  ${TEST_HARNESS_DIR}/dali-toolkit-test-suite-utils.cpp
  ${TEST_HARNESS_DIR}/dummy-control.cpp
  ${TEST_HARNESS_DIR}/mesh-builder.cpp
  ${TEST_HARNESS_DIR}/test-actor-utils.cpp
  ${TEST_HARNESS_DIR}/test-animation-data.cpp
  ${TEST_HARNESS_DIR}/test-application.cpp
  ${TEST_HARNESS_DIR}/test-button.cpp
  ${TEST_HARNESS_DIR}/test-harness.cpp
  ${TEST_HARNESS_DIR}/test-gesture-generator.cpp
  ${TEST_HARNESS_DIR}/test-gl-abstraction.cpp
  ${TEST_HARNESS_DIR}/test-gl-sync-abstraction.cpp
  ${TEST_HARNESS_DIR}/test-graphics-command-buffer.cpp
  ${TEST_HARNESS_DIR}/test-graphics-controller.cpp
  ${TEST_HARNESS_DIR}/test-graphics-texture.cpp
  ${TEST_HARNESS_DIR}/test-graphics-sampler.cpp
  ${TEST_HARNESS_DIR}/test-platform-abstraction.cpp
  ${TEST_HARNESS_DIR}/test-render-controller.cpp
  ${TEST_HARNESS_DIR}/test-trace-call-stack.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
  dali2-core
  dali2-adaptor
  dali2-toolkit
  dali2-scene-loader
)

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED)
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../../
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali-toolkit/dali-toolkit-test-utils
)

ADD_CUSTOM_COMMAND(
  COMMAND ${SCRIPT_DIR}/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT ${EXEC_NAME}.h
  COMMENT "Generating test tables"
  )

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES} ${TEST_HARNESS_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread --coverage
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
