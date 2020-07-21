cmake_minimum_required(VERSION 3.11)

PROJECT (dali-toolkit)

ADD_DEFINITIONS(
-DDALI_ENV="../../dali-env"
-DDALI_DATA_READ_ONLY_DIR=DALI_ENV"/opt/share/dali"
-DDALI_IMAGE_DIR=DALI_ENV"/opt/share/dali/toolkit/images/"
-DDALI_STYLE_DIR=DALI_ENV"/opt/share/dali/toolkit/styles/"
-DDALI_SOUND_DIR=DALI_ENV"/opt/share/dali/toolkit/sounds/"
-DDALI_STYLE_IMAGE_DIR=DALI_ENV"/opt/share/dali/toolkit/styles/images/"
-DCURL_STATICLIB=0
/DBUILDING_DALI_TOOLKIT
/vmg
/NODEFAULTLIB:"libcmt.lib"
/FI"../windows-dependencies/ExInclude/PreprocessorDefinitions.h"
/FI"../windows-dependencies/ExInclude/ToolKitPreDefine.h"
/MP
/Gz
)

#head file path
INCLUDE_DIRECTORIES(
./
../dali-core
../dali-adaptor
../windows-dependencies/ExInclude
../dali-env/opt/include
)

#devel-api
SET( devel_api_src_dir dali-toolkit/devel-api )

SET ( SOURCES ${SOURCES}
  ${devel_api_src_dir}/accessibility-manager/accessibility-manager.cpp
  ${devel_api_src_dir}/builder/builder.cpp
  ${devel_api_src_dir}/builder/json-parser.cpp
  ${devel_api_src_dir}/builder/tree-node.cpp
  ${devel_api_src_dir}/controls/control-devel.cpp
  ${devel_api_src_dir}/controls/control-wrapper.cpp
  ${devel_api_src_dir}/controls/control-wrapper-impl.cpp
  ${devel_api_src_dir}/controls/bloom-view/bloom-view.cpp
  ${devel_api_src_dir}/controls/bubble-effect/bubble-emitter.cpp
  ${devel_api_src_dir}/controls/buttons/toggle-button.cpp
  ${devel_api_src_dir}/controls/effects-view/effects-view.cpp
  ${devel_api_src_dir}/controls/magnifier/magnifier.cpp
  ${devel_api_src_dir}/controls/navigation-view/navigation-view.cpp
  ${devel_api_src_dir}/controls/page-turn-view/page-turn-landscape-view.cpp
  ${devel_api_src_dir}/controls/page-turn-view/page-turn-portrait-view.cpp
  ${devel_api_src_dir}/controls/page-turn-view/page-turn-view.cpp
  ${devel_api_src_dir}/controls/popup/confirmation-popup.cpp
  ${devel_api_src_dir}/controls/popup/popup.cpp
  ${devel_api_src_dir}/controls/shadow-view/shadow-view.cpp
  ${devel_api_src_dir}/controls/super-blur-view/super-blur-view.cpp
  ${devel_api_src_dir}/controls/text-controls/text-editor-devel.cpp
  ${devel_api_src_dir}/controls/text-controls/text-field-devel.cpp
  ${devel_api_src_dir}/controls/text-controls/text-selection-popup.cpp
  ${devel_api_src_dir}/controls/text-controls/text-selection-toolbar.cpp
  ${devel_api_src_dir}/controls/tool-bar/tool-bar.cpp
  ${devel_api_src_dir}/focus-manager/keyinput-focus-manager.cpp
  ${devel_api_src_dir}/focus-manager/keyboard-focus-manager-devel.cpp
  ${devel_api_src_dir}/image-loader/async-image-loader-devel.cpp
  ${devel_api_src_dir}/image-loader/atlas-upload-observer.cpp
  ${devel_api_src_dir}/image-loader/image-atlas.cpp
  ${devel_api_src_dir}/image-loader/texture-manager.cpp
  ${devel_api_src_dir}/styling/style-manager-devel.cpp
  ${devel_api_src_dir}/transition-effects/cube-transition-cross-effect.cpp
  ${devel_api_src_dir}/transition-effects/cube-transition-effect.cpp
  ${devel_api_src_dir}/transition-effects/cube-transition-fold-effect.cpp
  ${devel_api_src_dir}/transition-effects/cube-transition-wave-effect.cpp
  ${devel_api_src_dir}/visual-factory/transition-data.cpp
  ${devel_api_src_dir}/visual-factory/visual-factory.cpp
  ${devel_api_src_dir}/visual-factory/visual-base.cpp
  ${devel_api_src_dir}/controls/gaussian-blur-view/gaussian-blur-view.cpp
)

#internal
SET(internal_src_dir dali-toolkit/internal )

SET( SOURCES ${SOURCES}
   ${internal_src_dir}/builder/builder-animations.cpp
   ${internal_src_dir}/builder/builder-impl.cpp
   ${internal_src_dir}/builder/builder-impl-debug.cpp
   ${internal_src_dir}/builder/builder-set-property.cpp
   ${internal_src_dir}/builder/builder-signals.cpp
   ${internal_src_dir}/builder/json-parser-state.cpp
   ${internal_src_dir}/builder/json-parser-impl.cpp
   ${internal_src_dir}/builder/style.cpp
   ${internal_src_dir}/builder/tree-node-manipulator.cpp
   ${internal_src_dir}/builder/replacement.cpp
   ${internal_src_dir}/visuals/animated-image/animated-image-visual.cpp
   ${internal_src_dir}/visuals/animated-image/image-cache.cpp
   ${internal_src_dir}/visuals/animated-image/fixed-image-cache.cpp
   ${internal_src_dir}/visuals/animated-image/rolling-image-cache.cpp
   ${internal_src_dir}/visuals/animated-image/rolling-gif-image-cache.cpp
   ${internal_src_dir}/visuals/border/border-visual.cpp
   ${internal_src_dir}/visuals/color/color-visual.cpp
   ${internal_src_dir}/visuals/gradient/gradient-visual.cpp
   ${internal_src_dir}/visuals/gradient/gradient.cpp
   ${internal_src_dir}/visuals/gradient/linear-gradient.cpp
   ${internal_src_dir}/visuals/gradient/radial-gradient.cpp
   ${internal_src_dir}/visuals/animated-gradient/animated-gradient-visual.cpp
   ${internal_src_dir}/visuals/image-atlas-manager.cpp
   ${internal_src_dir}/visuals/image/image-visual.cpp
   ${internal_src_dir}/visuals/mesh/mesh-visual.cpp
   ${internal_src_dir}/visuals/npatch-loader.cpp
   ${internal_src_dir}/visuals/npatch/npatch-visual.cpp
   ${internal_src_dir}/visuals/primitive/primitive-visual.cpp
   ${internal_src_dir}/visuals/svg/svg-rasterize-thread.cpp
   ${internal_src_dir}/visuals/svg/svg-visual.cpp
   ${internal_src_dir}/visuals/text/text-visual.cpp
   ${internal_src_dir}/visuals/transition-data-impl.cpp
   ${internal_src_dir}/visuals/texture-manager-impl.cpp
   ${internal_src_dir}/visuals/texture-upload-observer.cpp
   ${internal_src_dir}/visuals/image-visual-shader-factory.cpp
   ${internal_src_dir}/visuals/visual-base-data-impl.cpp
   ${internal_src_dir}/visuals/visual-base-impl.cpp
   ${internal_src_dir}/visuals/visual-factory-cache.cpp
   ${internal_src_dir}/visuals/visual-factory-impl.cpp
   ${internal_src_dir}/visuals/visual-string-constants.cpp
   ${internal_src_dir}/visuals/visual-url.cpp
   ${internal_src_dir}/visuals/wireframe/wireframe-visual.cpp
   ${internal_src_dir}/controls/alignment/alignment-impl.cpp
   ${internal_src_dir}/controls/bloom-view/bloom-view-impl.cpp
   ${internal_src_dir}/controls/bubble-effect/bubble-emitter-impl.cpp
   ${internal_src_dir}/controls/bubble-effect/bubble-renderer.cpp
   ${internal_src_dir}/controls/buttons/button-impl.cpp
   ${internal_src_dir}/controls/buttons/check-box-button-impl.cpp
   ${internal_src_dir}/controls/buttons/push-button-impl.cpp
   ${internal_src_dir}/controls/buttons/radio-button-impl.cpp
   ${internal_src_dir}/controls/buttons/toggle-button-impl.cpp
   ${internal_src_dir}/controls/control/control-data-impl.cpp
   ${internal_src_dir}/controls/control/control-debug.cpp
   ${internal_src_dir}/controls/effects-view/effects-view-impl.cpp
   ${internal_src_dir}/controls/flex-container/flex-container-impl.cpp
   ${internal_src_dir}/controls/gaussian-blur-view/gaussian-blur-view-impl.cpp
   ${internal_src_dir}/controls/image-view/image-view-impl.cpp
   ${internal_src_dir}/controls/magnifier/magnifier-impl.cpp
   ${internal_src_dir}/controls/navigation-view/navigation-view-impl.cpp
   ${internal_src_dir}/controls/popup/confirmation-popup-impl.cpp
   ${internal_src_dir}/controls/model3d-view/model3d-view-impl.cpp
   ${internal_src_dir}/controls/model3d-view/obj-loader.cpp
   ${internal_src_dir}/controls/popup/popup-impl.cpp
   ${internal_src_dir}/controls/page-turn-view/page-turn-portrait-view-impl.cpp
   ${internal_src_dir}/controls/page-turn-view/page-turn-effect.cpp
   ${internal_src_dir}/controls/page-turn-view/page-turn-landscape-view-impl.cpp
   ${internal_src_dir}/controls/page-turn-view/page-turn-view-impl.cpp
   ${internal_src_dir}/controls/progress-bar/progress-bar-impl.cpp
   ${internal_src_dir}/controls/scroll-bar/scroll-bar-impl.cpp
   ${internal_src_dir}/controls/scrollable/bouncing-effect-actor.cpp
   ${internal_src_dir}/controls/scrollable/item-view/depth-layout.cpp
   ${internal_src_dir}/controls/scrollable/item-view/grid-layout.cpp
   ${internal_src_dir}/controls/scrollable/item-view/item-view-impl.cpp
   ${internal_src_dir}/controls/scrollable/item-view/spiral-layout.cpp
   ${internal_src_dir}/controls/scrollable/scrollable-impl.cpp
   ${internal_src_dir}/controls/scrollable/scroll-view/scroll-base-impl.cpp
   ${internal_src_dir}/controls/scrollable/scroll-view/scroll-overshoot-indicator-impl.cpp
   ${internal_src_dir}/controls/scrollable/scroll-view/scroll-view-effect-impl.cpp
   ${internal_src_dir}/controls/scrollable/scroll-view/scroll-view-impl.cpp
   ${internal_src_dir}/controls/scrollable/scroll-view/scroll-view-page-path-effect-impl.cpp
   ${internal_src_dir}/controls/shadow-view/shadow-view-impl.cpp
   ${internal_src_dir}/controls/slider/slider-impl.cpp
   ${internal_src_dir}/controls/super-blur-view/super-blur-view-impl.cpp
   ${internal_src_dir}/controls/table-view/table-view-impl.cpp
   ${internal_src_dir}/controls/text-controls/text-editor-impl.cpp
   ${internal_src_dir}/controls/text-controls/text-field-impl.cpp
   ${internal_src_dir}/controls/text-controls/text-label-impl.cpp
   ${internal_src_dir}/controls/text-controls/text-selection-popup-impl.cpp
   ${internal_src_dir}/controls/text-controls/text-selection-toolbar-impl.cpp
   ${internal_src_dir}/controls/tool-bar/tool-bar-impl.cpp
   ${internal_src_dir}/controls/tooltip/tooltip.cpp
   ${internal_src_dir}/controls/video-view/video-view-impl.cpp
   ${internal_src_dir}/accessibility-manager/accessibility-manager-impl.cpp
   ${internal_src_dir}/feedback/feedback-style.cpp
   ${internal_src_dir}/focus-manager/keyboard-focus-manager-impl.cpp
   ${internal_src_dir}/focus-manager/keyinput-focus-manager-impl.cpp
   ${internal_src_dir}/helpers/color-conversion.cpp
   ${internal_src_dir}/helpers/property-helper.cpp
   ${internal_src_dir}/filters/blur-two-pass-filter.cpp
   ${internal_src_dir}/filters/emboss-filter.cpp
   ${internal_src_dir}/filters/image-filter.cpp
   ${internal_src_dir}/filters/spread-filter.cpp
   ${internal_src_dir}/image-loader/async-image-loader-impl.cpp
   ${internal_src_dir}/image-loader/atlas-packer.cpp
   ${internal_src_dir}/image-loader/image-atlas-impl.cpp
   ${internal_src_dir}/image-loader/image-load-thread.cpp
   ${internal_src_dir}/styling/style-manager-impl.cpp
   ${internal_src_dir}/text/bidirectional-support.cpp
   ${internal_src_dir}/text/character-set-conversion.cpp
   ${internal_src_dir}/text/color-segmentation.cpp
   ${internal_src_dir}/text/cursor-helper-functions.cpp
   ${internal_src_dir}/text/glyph-metrics-helper.cpp
   ${internal_src_dir}/text/logical-model-impl.cpp
   ${internal_src_dir}/text/markup-processor.cpp
   ${internal_src_dir}/text/markup-processor-color.cpp
   ${internal_src_dir}/text/markup-processor-font.cpp
   ${internal_src_dir}/text/markup-processor-helper-functions.cpp
   ${internal_src_dir}/text/multi-language-support.cpp
   ${internal_src_dir}/text/hidden-text.cpp
   ${internal_src_dir}/text/property-string-parser.cpp
   ${internal_src_dir}/text/segmentation.cpp
   ${internal_src_dir}/text/shaper.cpp
   ${internal_src_dir}/text/text-enumerations-impl.cpp
   ${internal_src_dir}/text/text-controller.cpp
   ${internal_src_dir}/text/text-controller-impl.cpp
   ${internal_src_dir}/text/text-effects-style.cpp
   ${internal_src_dir}/text/text-font-style.cpp
   ${internal_src_dir}/text/text-io.cpp
   ${internal_src_dir}/text/text-model.cpp
   ${internal_src_dir}/text/text-scroller.cpp
   ${internal_src_dir}/text/text-vertical-scroller.cpp
   ${internal_src_dir}/text/text-view.cpp
   ${internal_src_dir}/text/text-view-interface.cpp
   ${internal_src_dir}/text/visual-model-impl.cpp
   ${internal_src_dir}/text/decorator/text-decorator.cpp
   ${internal_src_dir}/text/layouts/layout-engine.cpp
   ${internal_src_dir}/text/multi-language-helper-functions.cpp
   ${internal_src_dir}/text/multi-language-support-impl.cpp
   ${internal_src_dir}/text/rendering/text-backend.cpp
   ${internal_src_dir}/text/rendering/text-renderer.cpp
   ${internal_src_dir}/text/rendering/atlas/text-atlas-renderer.cpp
   ${internal_src_dir}/text/rendering/atlas/atlas-glyph-manager.cpp
   ${internal_src_dir}/text/rendering/atlas/atlas-glyph-manager-impl.cpp
   ${internal_src_dir}/text/rendering/atlas/atlas-manager.cpp
   ${internal_src_dir}/text/rendering/atlas/atlas-manager-impl.cpp
   ${internal_src_dir}/text/rendering/atlas/atlas-mesh-factory.cpp
   ${internal_src_dir}/text/rendering/text-backend-impl.cpp
   ${internal_src_dir}/text/rendering/text-typesetter.cpp
   ${internal_src_dir}/text/rendering/view-model.cpp
   ${internal_src_dir}/transition-effects/cube-transition-effect-impl.cpp
   ${internal_src_dir}/transition-effects/cube-transition-cross-effect-impl.cpp
   ${internal_src_dir}/transition-effects/cube-transition-fold-effect-impl.cpp
   ${internal_src_dir}/transition-effects/cube-transition-wave-effect-impl.cpp
   ${internal_src_dir}/text/xhtml-entities.cpp
)

#public-api
SET( public_api_src_dir dali-toolkit/public-api )

SET( SOURCES ${SOURCES}
  ${public_api_src_dir}/controls/control-impl.cpp
  ${public_api_src_dir}/controls/control.cpp
  ${public_api_src_dir}/controls/alignment/alignment.cpp
  ${public_api_src_dir}/controls/buttons/button.cpp
  ${public_api_src_dir}/controls/buttons/check-box-button.cpp
  ${public_api_src_dir}/controls/buttons/push-button.cpp
  ${public_api_src_dir}/controls/buttons/radio-button.cpp
  ${public_api_src_dir}/controls/flex-container/flex-container.cpp
  ${public_api_src_dir}/controls/image-view/image-view.cpp
  ${public_api_src_dir}/controls/model3d-view/model3d-view.cpp
  ${public_api_src_dir}/controls/progress-bar/progress-bar.cpp
  ${public_api_src_dir}/controls/scroll-bar/scroll-bar.cpp
  ${public_api_src_dir}/controls/scrollable/item-view/default-item-layout.cpp
  ${public_api_src_dir}/controls/scrollable/item-view/item-layout.cpp
  ${public_api_src_dir}/controls/scrollable/item-view/item-view.cpp
  ${public_api_src_dir}/controls/scrollable/scroll-view/scroll-view-constraints.cpp
  ${public_api_src_dir}/controls/scrollable/scroll-view/scroll-view-effect.cpp
  ${public_api_src_dir}/controls/scrollable/scroll-view/scroll-view-page-path-effect.cpp
  ${public_api_src_dir}/controls/scrollable/scroll-view/scroll-view.cpp
  ${public_api_src_dir}/controls/scrollable/scrollable.cpp
  ${public_api_src_dir}/controls/slider/slider.cpp
  ${public_api_src_dir}/controls/table-view/table-view.cpp
  ${public_api_src_dir}/controls/text-controls/text-editor.cpp
  ${public_api_src_dir}/controls/text-controls/text-label.cpp
  ${public_api_src_dir}/controls/text-controls/text-field.cpp
  ${public_api_src_dir}/controls/video-view/video-view.cpp
  ${public_api_src_dir}/image-loader/async-image-loader.cpp
  ${public_api_src_dir}/image-loader/sync-image-loader.cpp
  ${public_api_src_dir}/styling/style-manager.cpp
  ${public_api_src_dir}/focus-manager/keyboard-focus-manager.cpp
  ${public_api_src_dir}/dali-toolkit-version.cpp
  ${public_api_src_dir}/enums.cpp
)

link_directories(
../windows-dependencies/ExLib
../bin
)

SET( SOURCES ${SOURCES}
  dali-toolkit/third-party/nanosvg/nanosvg.cc
  dali-toolkit/third-party/nanosvg/nanosvgrast.cc
  dali-toolkit/third-party/yoga/Yoga.cpp
  dali-toolkit/third-party/yoga/YGStyle.cpp
  dali-toolkit/third-party/yoga/YGNodePrint.cpp
  dali-toolkit/third-party/yoga/YGNode.cpp
  dali-toolkit/third-party/yoga/YGLayout.cpp
  dali-toolkit/third-party/yoga/YGFloatOptional.cpp
  dali-toolkit/third-party/yoga/YGEnums.cpp
  dali-toolkit/third-party/yoga/YGConfig.cpp
  dali-toolkit/third-party/yoga/Utils.cpp
)
set(LIBRARY_OUTPUT_PATH ../../../bin)

ADD_LIBRARY(dali-toolkit SHARED ${SOURCES})

target_link_libraries(dali-toolkit dali-core.lib)
target_link_libraries(dali-toolkit dali-adaptor.lib)
target_link_libraries(dali-toolkit dlfcn.lib)
target_link_libraries(dali-toolkit WindowsPlatform.lib)

add_dependencies(dali-toolkit dali-core)
add_dependencies(dali-toolkit dali-adaptor)

#add link library
#TARGET_LINK_LIBRARIES(${FS_BUILD_BINARY_PREFIX}sqrt ${LIBRARIES})
