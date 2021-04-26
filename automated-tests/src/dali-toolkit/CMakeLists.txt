SET(PKG_NAME "dali-toolkit")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
  utc-Dali-Alignment.cpp
  utc-Dali-AnimatedImageVisual.cpp
  utc-Dali-AnimatedVectorImageVisual.cpp
  utc-Dali-ArcVisual.cpp
  utc-Dali-BloomView.cpp
  utc-Dali-BubbleEmitter.cpp
  utc-Dali-Builder.cpp
  utc-Dali-CanvasView.cpp
  utc-Dali-CheckBoxButton.cpp
  utc-Dali-ConfirmationPopup.cpp
  utc-Dali-CubeTransitionEffect.cpp
  utc-Dali-EffectsView.cpp
  utc-Dali-FlexContainer.cpp
  utc-Dali-FlexNode.cpp
  utc-Dali-GaussianBlurView.cpp
  utc-Dali-Image.cpp
  utc-Dali-ImageView.cpp
  utc-Dali-ImageVisual.cpp
  utc-Dali-JsonParser.cpp
  utc-Dali-KeyInputFocusManager.cpp
  utc-Dali-PageTurnView.cpp
  utc-Dali-Scene3dView.cpp
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
  utc-Dali-TextSelectionPopupMirroringLTR.cpp
  utc-Dali-TextSelectionPopupMirroringRTL.cpp
  utc-Dali-TextureManager.cpp
  utc-Dali-ToolBar.cpp
  utc-Dali-Tooltip.cpp
  utc-Dali-Transition.cpp
  utc-Dali-TransitionData.cpp
  utc-Dali-Button.cpp
  utc-Dali-CameraView.cpp
  utc-Dali-Control.cpp
  utc-Dali-ControlImpl.cpp
  utc-Dali-ItemLayout.cpp
  utc-Dali-ItemView.cpp
  utc-Dali-KeyboardFocusManager.cpp
  utc-Dali-Magnifier.cpp
  utc-Dali-NavigationView.cpp
  utc-Dali-Popup.cpp
  utc-Dali-ProgressBar.cpp
  utc-Dali-PushButton.cpp
  utc-Dali-RadioButton.cpp
  utc-Dali-ToggleButton.cpp
  utc-Dali-ScrollViewEffect.cpp
  utc-Dali-SuperBlurView.cpp
  utc-Dali-Toolkit.cpp
  utc-Dali-Model3dView.cpp
  utc-Dali-Visual.cpp
  utc-Dali-VisualFactory.cpp
  utc-Dali-ImageAtlas.cpp
  utc-Dali-VideoView.cpp
  utc-Dali-WebView.cpp
  utc-Dali-AsyncImageLoader.cpp
  utc-Dali-SyncImageLoader.cpp
  utc-Dali-ControlWrapper.cpp
  utc-Dali-DragAndDropDetector.cpp
  utc-Dali-NPatchUtilities.cpp
)

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_SOURCES
  dali-toolkit-test-utils/toolkit-adaptor.cpp
  dali-toolkit-test-utils/toolkit-application.cpp
  dali-toolkit-test-utils/toolkit-canvas-renderer.cpp
  dali-toolkit-test-utils/toolkit-clipboard.cpp
  dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
  dali-toolkit-test-utils/toolkit-event-thread-callback.cpp
  dali-toolkit-test-utils/toolkit-environment-variable.cpp
  dali-toolkit-test-utils/toolkit-input-method-context.cpp
  dali-toolkit-test-utils/toolkit-input-method-options.cpp
  dali-toolkit-test-utils/toolkit-lifecycle-controller.cpp
  dali-toolkit-test-utils/toolkit-orientation.cpp
  dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
  dali-toolkit-test-utils/toolkit-style-monitor.cpp
  dali-toolkit-test-utils/toolkit-test-application.cpp
  dali-toolkit-test-utils/toolkit-timer.cpp
  dali-toolkit-test-utils/toolkit-trigger-event-factory.cpp
  dali-toolkit-test-utils/toolkit-tts-player.cpp
  dali-toolkit-test-utils/toolkit-native-image-source.cpp
  dali-toolkit-test-utils/toolkit-vector-animation-renderer.cpp
  dali-toolkit-test-utils/toolkit-vector-image-renderer.cpp
  dali-toolkit-test-utils/toolkit-video-player.cpp
  dali-toolkit-test-utils/toolkit-web-engine.cpp
  dali-toolkit-test-utils/toolkit-window.cpp
  dali-toolkit-test-utils/toolkit-scene-holder.cpp
  dali-toolkit-test-utils/dali-test-suite-utils.cpp
  dali-toolkit-test-utils/dali-toolkit-test-suite-utils.cpp
  dali-toolkit-test-utils/dummy-control.cpp
  dali-toolkit-test-utils/mesh-builder.cpp
  dali-toolkit-test-utils/test-actor-utils.cpp
  dali-toolkit-test-utils/test-animation-data.cpp
  dali-toolkit-test-utils/test-application.cpp
  dali-toolkit-test-utils/test-button.cpp
  dali-toolkit-test-utils/test-harness.cpp
  dali-toolkit-test-utils/test-gesture-generator.cpp
  dali-toolkit-test-utils/test-gl-abstraction.cpp
  dali-toolkit-test-utils/test-gl-sync-abstraction.cpp
  dali-toolkit-test-utils/test-graphics-buffer.cpp
  dali-toolkit-test-utils/test-graphics-command-buffer.cpp
  dali-toolkit-test-utils/test-graphics-controller.cpp
  dali-toolkit-test-utils/test-graphics-texture.cpp
  dali-toolkit-test-utils/test-graphics-pipeline.cpp
  dali-toolkit-test-utils/test-graphics-program.cpp
  dali-toolkit-test-utils/test-graphics-reflection.cpp
  dali-toolkit-test-utils/test-graphics-sampler.cpp
  dali-toolkit-test-utils/test-graphics-shader.cpp
  dali-toolkit-test-utils/test-platform-abstraction.cpp
  dali-toolkit-test-utils/test-render-controller.cpp
  dali-toolkit-test-utils/test-trace-call-stack.cpp
  dali-toolkit-test-utils/test-native-image.cpp
)

PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
  dali2-core
  dali2-adaptor
  dali2-toolkit
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
    dali-toolkit-test-utils
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
