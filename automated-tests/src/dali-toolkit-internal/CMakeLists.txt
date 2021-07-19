SET(PKG_NAME "dali-toolkit-internal")

SET(EXEC_NAME "tct-${PKG_NAME}-core")
SET(RPM_NAME "core-${PKG_NAME}-tests")

SET(CAPI_LIB "dali-toolkit-internal")

# List of test case sources (Only these get parsed for test cases)
SET(TC_SOURCES
 utc-Dali-AddOns.cpp
 utc-Dali-BidirectionalSupport.cpp
 utc-Dali-ColorConversion.cpp
 utc-Dali-Control-internal.cpp
 utc-Dali-DebugRendering.cpp
 utc-Dali-Dictionary.cpp
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
 utc-Dali-TextLabel-internal.cpp
 utc-Dali-TextSelectionPopup-internal.cpp
 utc-Dali-TextureManager.cpp
 utc-Dali-Visuals-internal.cpp
 utc-Dali-VisualModel.cpp
 utc-Dali-VisualUrl.cpp
 utc-Dali-Text-Hyphen-Wrapping.cpp
 utc-Dali-Text-Ellipsis.cpp
)

IF(ELDBUS_AVAILABLE)
  LIST(APPEND TC_SOURCES
    utc-Dali-Accessibility-Accessible.cpp
    utc-Dali-Accessibility-Controls.cpp
    utc-Dali-Accessibility-Controls-BridgeUp.cpp
    utc-Dali-Accessibility-Text.cpp
    utc-Dali-Accessibility-Value.cpp
    )
ENDIF()

# List of test harness files (Won't get parsed for test cases)
SET(TEST_HARNESS_SOURCES
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
   ../dali-toolkit/dali-toolkit-test-utils/test-encoded-image-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-harness.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-gl-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-impl.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sync-object.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-command-buffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-framebuffer.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-texture.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-program.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-pipeline.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-reflection.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-sampler.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-graphics-shader.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-platform-abstraction.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-render-controller.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-trace-call-stack.cpp
   ../dali-toolkit/dali-toolkit-test-utils/test-native-image.cpp
   dali-toolkit-test-utils/toolkit-text-utils.cpp
   dali-toolkit-test-utils/dummy-visual.cpp
   dali-toolkit-test-utils/test-addon-manager.cpp
)

IF(ELDBUS_AVAILABLE)
  LIST(APPEND TEST_HARNESS_SOURCES
   dali-toolkit-test-utils/accessibility-test-utils.cpp
   dali-toolkit-test-utils/dbus-wrapper.cpp
    )
ENDIF()


PKG_CHECK_MODULES(${CAPI_LIB} REQUIRED
    dali2-core
    dali2-adaptor
    dali2-toolkit
    eldbus
    ecore-input
    eina
)

MESSAGE("Libraries to link with:>${${CAPI_LIB}_LIBRARIES}")

ADD_COMPILE_OPTIONS( -O0 -ggdb --coverage -Wall -Werror -DDEBUG_ENABLED -fPIC )
ADD_COMPILE_OPTIONS( ${${CAPI_LIB}_CFLAGS_OTHER} )

ADD_DEFINITIONS(-DTEST_RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../resources\" )
ADD_DEFINITIONS(-DDALI_ADDONS_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\")

IF(ELDBUS_AVAILABLE)
  ADD_COMPILE_OPTIONS(-Wno-error=attributes)
  ADD_DEFINITIONS(-DELDBUS_ENABLED)
ENDIF()

FOREACH(directory ${${CAPI_LIB}_LIBRARY_DIRS})
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${directory}")
ENDFOREACH(directory ${CAPI_LIB_LIBRARY_DIRS})

STRING(STRIP ${CMAKE_CXX_LINK_FLAGS} CMAKE_CXX_LINK_FLAGS)

INCLUDE_DIRECTORIES(
  ../../../
  ${${CAPI_LIB}_INCLUDE_DIRS}
  ../dali-toolkit/dali-toolkit-test-utils
  dali-toolkit-test-utils
)

ADD_CUSTOM_COMMAND(
  COMMAND ../../scripts/tcheadgen.sh ${EXEC_NAME}.h ${TC_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT  ${EXEC_NAME}.h
  COMMENT "Generating test tables"
  )

# Add explicit dependency on auto-generated header.
ADD_EXECUTABLE(${EXEC_NAME} ${EXEC_NAME}.h ${EXEC_NAME}.cpp ${TC_SOURCES} ${TEST_HARNESS_SOURCES})
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
