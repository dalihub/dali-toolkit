SET(PKG_NAME "dali-toolkit")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
   utc-Dali-Alignment.cpp
   utc-Dali-BloomView.cpp
   utc-Dali-BubbleEmitter.cpp
   utc-Dali-Builder.cpp
   utc-Dali-CheckBoxButton.cpp
   utc-Dali-ConfirmationPopup.cpp
   utc-Dali-CubeTransitionEffect.cpp
   utc-Dali-EffectsView.cpp
   utc-Dali-GaussianBlurView.cpp
   utc-Dali-ImageView.cpp
   utc-Dali-JsonParser.cpp
   utc-Dali-KeyInputFocusManager.cpp
   utc-Dali-PageTurnView.cpp
   utc-Dali-Script.cpp
   utc-Dali-ScrollBar.cpp
   utc-Dali-ScrollView.cpp
   utc-Dali-ShaderEffects.cpp
   utc-Dali-ShadowView.cpp
   utc-Dali-Slider.cpp
   utc-Dali-TableView.cpp
   utc-Dali-TextEditor.cpp
   utc-Dali-TextField.cpp
   utc-Dali-TextLabel.cpp
   utc-Dali-TextSelectionPopup.cpp
   utc-Dali-ToolBar.cpp
   utc-Dali-Button.cpp
   utc-Dali-Control.cpp
   utc-Dali-ControlImpl.cpp
   utc-Dali-AccessibilityManager.cpp
   utc-Dali-ItemLayout.cpp
   utc-Dali-ItemView.cpp
   utc-Dali-KeyboardFocusManager.cpp
   utc-Dali-Magnifier.cpp
   utc-Dali-Popup.cpp
   utc-Dali-PushButton.cpp
   utc-Dali-RadioButton.cpp
   utc-Dali-ScrollViewEffect.cpp
   utc-Dali-StyleManager.cpp
   utc-Dali-SuperBlurView.cpp
   utc-Dali-Toolkit.cpp
   utc-Dali-Model3dView.cpp
   utc-Dali-ControlRenderer.cpp
   utc-Dali-RendererFactory.cpp
   utc-Dali-DebugRenderer.cpp
   utc-Dali-ImageAtlas.cpp
)

# Append list of test harness files (Won't get parsed for test cases)
LIST(APPEND TC_SOURCES
   dali-toolkit-test-utils/toolkit-accessibility-adaptor.cpp
   dali-toolkit-test-utils/toolkit-application.cpp
   dali-toolkit-test-utils/toolkit-clipboard.cpp
   dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
   dali-toolkit-test-utils/toolkit-event-thread-callback.cpp
   dali-toolkit-test-utils/toolkit-environment-variable.cpp
   dali-toolkit-test-utils/toolkit-imf-manager.cpp
   dali-toolkit-test-utils/toolkit-orientation.cpp
   dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   dali-toolkit-test-utils/toolkit-style-monitor.cpp
   dali-toolkit-test-utils/toolkit-singleton-service.cpp
   dali-toolkit-test-utils/toolkit-text-model.cpp
   dali-toolkit-test-utils/toolkit-timer.cpp
   dali-toolkit-test-utils/toolkit-tts-player.cpp
   dali-toolkit-test-utils/dali-test-suite-utils.cpp
   dali-toolkit-test-utils/dummy-control.cpp
   dali-toolkit-test-utils/mesh-builder.cpp
   dali-toolkit-test-utils/test-actor-utils.cpp
   dali-toolkit-test-utils/test-animation-data.cpp
   dali-toolkit-test-utils/test-application.cpp
   dali-toolkit-test-utils/test-button.cpp
   dali-toolkit-test-utils/test-harness.cpp
   dali-toolkit-test-utils/test-gesture-manager.cpp
   dali-toolkit-test-utils/test-gl-abstraction.cpp
   dali-toolkit-test-utils/test-gl-sync-abstraction.cpp
   dali-toolkit-test-utils/test-platform-abstraction.cpp
   dali-toolkit-test-utils/test-render-controller.cpp
   dali-toolkit-test-utils/test-trace-call-stack.cpp
   dali-toolkit-test-utils/test-native-image.cpp
)


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali-core
    dali-adaptor
    dali-toolkit
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -Wall -Werror")

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
    ../../../
    ${${CAPI_LIB}_INCLUDE_DIRS}
    dali-toolkit-test-utils
)

ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.cpp ${TC_SOURCES})
TARGET_LINK_LIBRARIES(${EXEC_NAME}
    ${${CAPI_LIB}_LIBRARIES}
    -lpthread
)

INSTALL(PROGRAMS ${EXEC_NAME}
    DESTINATION ${BIN_DIR}/${EXEC_NAME}
)
