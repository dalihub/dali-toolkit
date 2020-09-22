SET(PKG_NAME "dali-toolkit-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-internal")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
 utc-Dali-AddOns.cpp
 utc-Dali-Accessibility-Accessible.cpp
 utc-Dali-Accessibility-Controls.cpp
 utc-Dali-Accessibility-Controls-BridgeUp.cpp
 utc-Dali-Accessibility-Text.cpp
 utc-Dali-Accessibility-Value.cpp
 utc-Dali-BidirectionalSupport.cpp
 utc-Dali-ColorConversion.cpp
 utc-Dali-Control-internal.cpp
 utc-Dali-DebugRendering.cpp
 utc-Dali-FeedbackStyle.cpp
 utc-Dali-ItemView-internal.cpp
 utc-Dali-LogicalModel.cpp
 utc-Dali-PropertyHelper.cpp
 utc-Dali-Text-CharacterSetConversion.cpp
 utc-Dali-Text-Circular.cpp
 utc-Dali-Text-Controller.cpp
 utc-Dali-Text-Cursor.cpp
 utc-Dali-Text-Layout.cpp
 utc-Dali-Text-Markup.cpp
 utc-Dali-Text-MultiLanguage.cpp
 utc-Dali-Text-Segmentation.cpp
 utc-Dali-Text-Shaping.cpp
 utc-Dali-Text-Typesetter.cpp
 utc-Dali-Text-ViewModel.cpp
 utc-Dali-TextField-internal.cpp
 utc-Dali-TextEditor-internal.cpp
 utc-Dali-TextSelectionPopup-internal.cpp
 utc-Dali-TextureManager.cpp
 utc-Dali-Visuals-internal.cpp
 utc-Dali-VisualModel.cpp
 utc-Dali-VisualUrl.cpp
)

# Append list of test harness files (Won't get parsed for test cases)
LIST(APPEND TC_SOURCES
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-adaptor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-event-thread-callback.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-environment-variable.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-feedback-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-input-method-context.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-input-method-options.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-lifecycle-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-orientation.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-style-monitor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-timer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-tts-player.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-vector-animation-renderer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-vector-image-renderer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-window.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-scene-holder.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dali-toolkit-test-suite-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/dummy-control.cpp
   ../dali-toolkit/dali-toolkit-test-utils/mesh-builder.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-actor-utils.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-animation-data.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-button.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-harness.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-sync-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-platform-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-render-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-trace-call-stack.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-native-image.cpp
   dali-toolkit-test-utils/toolkit-text-utils.cpp
   dali-toolkit-test-utils/dummy-visual.cpp
   dali-toolkit-test-utils/test-addon-manager.cpp
   dali-toolkit-test-utils/accessibility-test-utils.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali2-core
    dali2-adaptor
    dali2-toolkit
)

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED -fPIC )
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )
ADD_DEFINITIONS(-DDALI_ADDONS_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../../
    ${${CAPI_LIB}_INCLUDE_DIRS}
    ../dali-toolkit/dali-toolkit-test-utils
    dali-toolkit-test-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread --coverage -ldl -rdynamic
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)

# build addons
MESSAGE( STATUS "BINDIR: ${CMAKE_CURRENT_BINARY_DIR}")

FILE( GLOB FILES ${CMAKE_CURRENT_SOURCE_DIR}/addons/*.cmake )

FOREACH( INFILE IN ITEMS ${FILES} )

    INCLUDE( ${INFILE} )
    MESSAGE( STATUS "Building ${INFILE}" )

    ADD_LIBRARY( ${ADDON_NAME} SHARED ${ADDON_SOURCES} )

    TARGET_INCLUDE_DIRECTORIES( ${ADDON_NAME} PUBLIC
            ../../../
            ${${CAPI_LIB}_INCLUDE_DIRS}
            ../dali-toolkit/dali-toolkit-test-utils
            dali-toolkit-test-utils)

    TARGET_LINK_LIBRARIES(${ADDON_NAME}
            ${CMAKE_CXX_LINK_FLAGS}
            ${${CAPI_LIB}_LIBRARIES}
            -lpthread -ldl --coverage
            )

    INSTALL( TARGETS ${ADDON_NAME} DESTINATION ${BIN_DIR} )

    SET( ADDON_LIST "lib${ADDON_NAME}.so
${ADDON_LIST}")
ENDFOREACH()

# store AddOns list
FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/addons.txt "${ADDON_LIST}" )
