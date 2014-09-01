SET(PKG_NAME "dali-toolkit-unmanaged")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-unmanaged")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
 utc-Dali-AlbumLayout.cpp
 utc-Dali-BendyEffect.cpp
 utc-Dali-BlindEffect.cpp
 utc-Dali-BloomView.cpp
 utc-Dali-Builder.cpp
 utc-Dali-CarouselEffect.cpp
 utc-Dali-CheckBoxButton.cpp
 utc-Dali-Cluster.cpp
 utc-Dali-CubeTransitionEffect.cpp
 utc-Dali-DepthLayout.cpp
 utc-Dali-DisplacementEffect.cpp
 utc-Dali-DistanceFieldEffect.cpp
 utc-Dali-GaussianBlurView.cpp
 utc-Dali-JsonParser.cpp
 utc-Dali-KeyInputFocusManager.cpp
 utc-Dali-NavigationControl.cpp
 utc-Dali-NavigationLayout.cpp
 utc-Dali-OverlayEffect.cpp
 utc-Dali-Page.cpp
 utc-Dali-PageTurnEffect.cpp
 utc-Dali-PageTurnView.cpp
 utc-Dali-RollLayout.cpp
 utc-Dali-RotatingSelector.cpp
 utc-Dali-ScrollView.cpp
 utc-Dali-ShadowView.cpp
 utc-Dali-ShearEffect.cpp
 utc-Dali-Slider.cpp
 utc-Dali-SoftButtonEffect.cpp
 utc-Dali-SpiralLayout.cpp
 utc-Dali-SpotEffect.cpp
 utc-Dali-SquareDissolveEffect.cpp
 utc-Dali-TableView.cpp
 utc-Dali-ToolBar.cpp
 utc-Dali-View.cpp
 utc-Dali-WaterEffect.cpp
 utc-Dali-Button.cpp
 utc-Dali-ControlImpl.cpp
 utc-Dali-DefaultControls.cpp
 utc-Dali-ItemView.cpp
 utc-Dali-KeyboardFocusManager.cpp
 utc-Dali-ScrollViewEffect.cpp
 utc-Dali-TextInput.cpp
 utc-Dali-StyleManager.cpp
)

# Append list of test harness files (Won't get parsed for test cases)
LIST(APPEND TC_SOURCES
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-accessibility-manager.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-adaptor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-application.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-imf-manager.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-physical-keyboard.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-style-monitor.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-singleton-service.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-timer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/toolkit-clipboard-event-notifier.cpp
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
    dali
    dali-toolkit
)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -ggdb --coverage -Wall -Werror=return-type")

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(
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
