SET(PKG_NAME "dali-shader-generator")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(EXEC_NAME "tct-${PKG_NAME}-core")

SET(CAPI_LIB "dali-shader-generator")

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
  dali2-core
  dali2-adaptor
  dali2-toolkit
)

SET(TC_SOURCES
  utc-Dali-ShaderGenerator.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_DIR "../dali-toolkit/dali-toolkit-test-utils")

SET(TEST_HARNESS_SOURCES
  ${TEST_HARNESS_DIR}/toolkit-adaptor.cpp
  ${TEST_HARNESS_DIR}/toolkit-application.cpp
  ${TEST_HARNESS_DIR}/toolkit-async-task-manager.cpp
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
  ${TEST_HARNESS_DIR}/test-graphics-sync-impl.cpp
  ${TEST_HARNESS_DIR}/test-graphics-sync-object.cpp
  ${TEST_HARNESS_DIR}/test-graphics-buffer.cpp
  ${TEST_HARNESS_DIR}/test-graphics-command-buffer.cpp
  ${TEST_HARNESS_DIR}/test-graphics-controller.cpp
  ${TEST_HARNESS_DIR}/test-graphics-framebuffer.cpp
  ${TEST_HARNESS_DIR}/test-graphics-texture.cpp
  ${TEST_HARNESS_DIR}/test-graphics-sampler.cpp
  ${TEST_HARNESS_DIR}/test-graphics-program.cpp
  ${TEST_HARNESS_DIR}/test-graphics-pipeline.cpp
  ${TEST_HARNESS_DIR}/test-graphics-shader.cpp
  ${TEST_HARNESS_DIR}/test-graphics-reflection.cpp
  ${TEST_HARNESS_DIR}/test-platform-abstraction.cpp
  ${TEST_HARNESS_DIR}/test-render-controller.cpp
  ${TEST_HARNESS_DIR}/test-render-surface.cpp
  ${TEST_HARNESS_DIR}/test-trace-call-stack.cpp
)

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
  ../../../
  ${${CAPI_LIB}_INCLUDE_DIRS}
  ../dali-toolkit/dali-toolkit-test-utils
)

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED)
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES} ${TEST_HARNESS_SOURCES})

TARGET_LINK_LIBRARIES(${EXEC_NAME}
  ${${CAPI_LIB}_LIBRARIES}
  -lpthread --coverage
)

ADD_CUSTOM_COMMAND(
  COMMAND ${SCRIPT_DIR}/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT ${EXEC_NAME}.h
  COMMENT "Generating test tables"
)

INSTALL(PROGRAMS ${EXEC_NAME}
  DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

SET(SHADER_GENERATOR dali-shader-generator)
SET(GENERATED_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shader/generated)
SET(SHADER_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/shader)

# All the shader generator execution tests are below

ADD_CUSTOM_TARGET(test_help ALL COMMAND ${SHADER_GENERATOR} -h | grep "DALi Shader Generator" > /dev/null 2>&1 && echo "test_help Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_no_params ALL COMMAND ${SHADER_GENERATOR} > /dev/null 2>&1 || echo "test_no_params Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_version ALL COMMAND ${SHADER_GENERATOR} -v | wc -l | grep 1 > /dev/null 2>&1 && echo "test_version Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_invalid_option ALL COMMAND ${SHADER_GENERATOR} --undeclared > /dev/null 2>&1 || echo "test_invalid_option Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_too_many_params ALL COMMAND ${SHADER_GENERATOR} ONE TWO THREE > /dev/null 2>&1 || echo "test_too_many_params Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_invalid_indir ALL COMMAND ${SHADER_GENERATOR} ONE TWO > /dev/null 2>&1 || echo "test_invalid_indir Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(
  test_check_built_in_created
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/check_built_in
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/check_built_in/generated | grep builtin-shader | wc -l | grep 2 > /dev/null 2>&1 && echo "test_check_built_in_created Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_check_built_in_not_created
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/skip_built_in
  COMMAND ${SHADER_GENERATOR} --skip ${SHADER_FOLDER} ${GENERATED_FOLDER}/skip_built_in/generated | grep builtin-shader > /dev/null 2>&1 && exit 1 || echo "test_check_built_in_not_created Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_frag_correct
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/frag_correct
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/frag_correct/generated | grep "SHADER_FRAGMENT_SHADER_FRAG" | grep "fragment-shader-frag.h" > /dev/null 2>&1 && echo "test_frag_correct Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_vert_correct
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/vert_correct
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/vert_correct/generated | grep "SHADER_VERTEX_SHADER_VERT" | grep "vertex-shader-vert.h" > /dev/null 2>&1 && echo "test_vert_correct Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_def_correct
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/def_correct
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/def_correct/generated | grep "SHADER_SHADER_DEFINE_DEF" | grep "shader-define-def.h" > /dev/null 2>&1 && echo "test_def_correct Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_no_overwrite
  ALL
  COMMAND rm -rf ${GENERATED_FOLDER}/no_overwrite
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/no_overwrite/generated | grep "SHADER_SHADER_DEFINE_DEF" | grep "shader-define-def.h" > /dev/null 2>&1
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER}/no_overwrite/generated | grep "SHADER_SHADER_DEFINE_DEF" | grep "shader-define-def.h" > /dev/null 2>&1 && exit 1 || echo "test_no_overwrite Succeeded"
  VERBATIM)
