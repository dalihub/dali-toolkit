SET(PKG_NAME "dali-shader-generator")

SET(EXEC_NAME "tct-${PKG_NAME}-core")

PKG_CHECK_MODULES(${EXEC_NAME} REQUIRED
  dali2-toolkit
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

SET(SHADER_GENERATOR dali-shader-generator)
SET(GENERATED_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/shader/generated)
SET(SHADER_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/shader)

ADD_CUSTOM_TARGET(test_help ALL COMMAND ${SHADER_GENERATOR} -h | grep "DALi Shader Generator" > /dev/null 2>&1 && echo "test_help Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_no_params ALL COMMAND ${SHADER_GENERATOR} > /dev/null 2>&1 || echo "test_no_params Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_version ALL COMMAND ${SHADER_GENERATOR} -v | wc -l | grep 1 > /dev/null 2>&1 && echo "test_version Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_invalid_option ALL COMMAND ${SHADER_GENERATOR} --undeclared > /dev/null 2>&1 || echo "test_invalid_option Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_too_many_params ALL COMMAND ${SHADER_GENERATOR} ONE TWO THREE > /dev/null 2>&1 || echo "test_too_many_params Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(test_invalid_indir ALL COMMAND ${SHADER_GENERATOR} ONE TWO > /dev/null 2>&1 || echo "test_invalid_indir Succeeded" VERBATIM)
ADD_CUSTOM_TARGET(
  test_check_built_in_created
  ALL
  COMMAND ${SHADER_GENERATOR} . ${GENERATED_FOLDER} | grep builtin-shader | wc -l | grep 2 > /dev/null 2>&1 && echo "test_check_built_in_created Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_check_built_in_not_created
  ALL
  COMMAND ${SHADER_GENERATOR} --skip . ${GENERATED_FOLDER} | grep builtin-shader > /dev/null 2>&1 || echo "test_check_built_in_not_created Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_frag_correct
  ALL
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER} | grep "SHADER_FRAGMENT_SHADER_FRAG" | grep "fragment-shader-frag.h" > /dev/null 2>&1 && echo "test_frag_correct Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_vert_correct
  ALL
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER} | grep "SHADER_VERTEX_SHADER_VERT" | grep "vertex-shader-vert.h" > /dev/null 2>&1 && echo "test_vert_correct Succeeded"
  VERBATIM)
ADD_CUSTOM_TARGET(
  test_def_correct
  ALL
  COMMAND ${SHADER_GENERATOR} ${SHADER_FOLDER} ${GENERATED_FOLDER} | grep "SHADER_SHADER_DEFINE_DEF" | grep "shader-define-def.h" > /dev/null 2>&1 && echo "test_def_correct Succeeded"
  VERBATIM)
