SET(PKG_NAME "dali-toolkit-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-internal")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
 utc-Dali-PushButton.cpp
 utc-Dali-Text-CharacterSetConversion.cpp
 utc-Dali-Text-Segmentation.cpp
 utc-Dali-Text-MultiLanguage.cpp
 utc-Dali-LogicalModel.cpp
 utc-Dali-BidirectionalSupport.cpp
 utc-Dali-Text-Shaping.cpp
 utc-Dali-VisualModel.cpp
 utc-Dali-Text-Layout.cpp
)

# Append list of test harness files (Won't get parsed for test cases)
LIST(APPEND TC_SOURCES
   ../dali-toolkit/dali-toolkit-test-utils/test-harness.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-accessibility-adaptor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-imf-manager.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-orientation.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-style-monitor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-singleton-service.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-timer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-text-model.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dummy-control.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-platform-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gesture-manager.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-sync-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-render-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-trace-call-stack.cpp
)


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali-core
    dali-adaptor
    dali-toolkit
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -Wall -Werror")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../../
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali-toolkit/dali-toolkit-test-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
