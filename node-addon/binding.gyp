{
  "variables": {
    'DALI_JS_DIR%':'../plugins/dali-script-v8/src/'
  },
  "targets": [
    {
      'target_name': 'dali',
      'include_dirs': ['<(DALI_JS_DIR)/',
                       '<(DALI_JS_DIR)/utils'],
      'sources': [ 'dali-addon.cpp',
      'javascript-application-options.cpp',
      '<(DALI_JS_DIR)/utils/v8-utils.cpp',
      '<(DALI_JS_DIR)/dali-wrapper.cpp',
      '<(DALI_JS_DIR)/shared/base-wrapped-object.cpp',
      '<(DALI_JS_DIR)/shared/object-template-helper.cpp',
      '<(DALI_JS_DIR)/actors/actor-wrapper.cpp',
      '<(DALI_JS_DIR)/actors/actor-api.cpp',
      '<(DALI_JS_DIR)/actors/layer-api.cpp',
      '<(DALI_JS_DIR)/actors/camera-actor-api.cpp',
      '<(DALI_JS_DIR)/constants/constants-wrapper.cpp',
      '<(DALI_JS_DIR)/controls/control-wrapper.cpp',
      '<(DALI_JS_DIR)/controls/item-factory-wrapper.cpp',
      '<(DALI_JS_DIR)/controls/item-view-api.cpp',
      '<(DALI_JS_DIR)/animation/animation-api.cpp',
      '<(DALI_JS_DIR)/animation/animation-wrapper.cpp',
      '<(DALI_JS_DIR)/animation/constrainer-api.cpp',
      '<(DALI_JS_DIR)/animation/linear-constrainer-wrapper.cpp',
      '<(DALI_JS_DIR)/animation/path-api.cpp',
      '<(DALI_JS_DIR)/animation/path-constrainer-wrapper.cpp',
      '<(DALI_JS_DIR)/animation/path-wrapper.cpp',
      '<(DALI_JS_DIR)/stage/stage-wrapper.cpp',
      '<(DALI_JS_DIR)/events/event-object-generator.cpp',
      '<(DALI_JS_DIR)/events/pan-gesture-detector-api.cpp',
      '<(DALI_JS_DIR)/events/pan-gesture-detector-wrapper.cpp',
      '<(DALI_JS_DIR)/stage/stage-api.cpp',
      '<(DALI_JS_DIR)/image/image-wrapper.cpp',
      '<(DALI_JS_DIR)/image/image-api.cpp',
      '<(DALI_JS_DIR)/image/buffer-image-api.cpp',
      '<(DALI_JS_DIR)/image/frame-buffer-image-api.cpp',
      '<(DALI_JS_DIR)/image/resource-image-api.cpp',
      '<(DALI_JS_DIR)/object/handle-wrapper.cpp',
      '<(DALI_JS_DIR)/object/property-buffer-api.cpp',
      '<(DALI_JS_DIR)/object/property-buffer-wrapper.cpp',
      '<(DALI_JS_DIR)/object/property-value-wrapper.cpp',
      '<(DALI_JS_DIR)/signals/signal-manager.cpp',
      '<(DALI_JS_DIR)/render-tasks/render-task-list-api.cpp',
      '<(DALI_JS_DIR)/render-tasks/render-task-list-wrapper.cpp',
      '<(DALI_JS_DIR)/render-tasks/render-task-api.cpp',
      '<(DALI_JS_DIR)/render-tasks/render-task-wrapper.cpp',
      '<(DALI_JS_DIR)/rendering/geometry-api.cpp',
      '<(DALI_JS_DIR)/rendering/geometry-wrapper.cpp',
      '<(DALI_JS_DIR)/rendering/material-api.cpp',
      '<(DALI_JS_DIR)/rendering/material-wrapper.cpp',
      '<(DALI_JS_DIR)/rendering/renderer-api.cpp',
      '<(DALI_JS_DIR)/rendering/renderer-wrapper.cpp',
      '<(DALI_JS_DIR)/rendering/sampler-api.cpp',
      '<(DALI_JS_DIR)/rendering/sampler-wrapper.cpp',
      '<(DALI_JS_DIR)/rendering/shader-api.cpp',
      '<(DALI_JS_DIR)/rendering/shader-wrapper.cpp',
      '<(DALI_JS_DIR)/toolkit/builder/builder-api.cpp',
      '<(DALI_JS_DIR)/toolkit/builder/builder-wrapper.cpp',
      '<(DALI_JS_DIR)/toolkit/focus-manager/keyboard-focus-manager-api.cpp',
      '<(DALI_JS_DIR)/toolkit/focus-manager/keyboard-focus-manager-wrapper.cpp',
      '<(DALI_JS_DIR)/signals/dali-any-javascript-converter.cpp',
      '<(DALI_JS_DIR)/garbage-collector/garbage-collector.cpp',
      '<(DALI_JS_DIR)/module-loader/module.cpp',
      '<(DALI_JS_DIR)/module-loader/module-loader.cpp'
     ],
      'cflags': [
            '-fPIC',
            '-frtti',
            '<!@(pkg-config --cflags dali-core dali-adaptor-uv dali-toolkit)'
        ],

    'cflags_cc': [
            '-fexceptions', # Enable exception handler
            '-frtti'    # needed for typeinfo with dali-any
        ],
      'ldflags': [
            '<!@(pkg-config --libs-only-L --libs-only-other dali-core dali-adaptor-uv dali-toolkit)'
        ],
      'libraries': [
        '<!@(pkg-config --libs-only-l dali-core dali-adaptor-uv dali-toolkit)'
      ]
    }]
}
