#ifndef DALI_SCENE3D_H
#define DALI_SCENE3D_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>

#include <dali-scene3d/public-api/common/environment-map.h>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

#include <dali-scene3d/public-api/loader/alpha-function-helper.h>
#include <dali-scene3d/public-api/loader/animated-property.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/buffer-definition.h>
#include <dali-scene3d/public-api/loader/bvh-loader.h>
#include <dali-scene3d/public-api/loader/camera-parameters.h>
#include <dali-scene3d/public-api/loader/customization.h>
#include <dali-scene3d/public-api/loader/dli-input-parameter.h>
#include <dali-scene3d/public-api/loader/environment-definition.h>
#include <dali-scene3d/public-api/loader/environment-map-data.h>
#include <dali-scene3d/public-api/loader/environment-map-loader.h>
#include <dali-scene3d/public-api/loader/facial-animation-loader.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/loader/ktx-loader.h>
#include <dali-scene3d/public-api/loader/light-parameters.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/load-scene-metadata.h>
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-scene3d/public-api/loader/matrix-stack.h>
#include <dali-scene3d/public-api/loader/mesh-definition.h>
#include <dali-scene3d/public-api/loader/mesh-geometry.h>
#include <dali-scene3d/public-api/loader/model-loader.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/parse-renderer-state.h>
#include <dali-scene3d/public-api/loader/renderer-state.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>
#include <dali-scene3d/public-api/loader/shader-definition.h>
#include <dali-scene3d/public-api/loader/skeleton-definition.h>
#include <dali-scene3d/public-api/loader/skinning-details.h>
#include <dali-scene3d/public-api/loader/string-callback.h>
#include <dali-scene3d/public-api/loader/utils.h>
#include <dali-scene3d/public-api/loader/view-projection.h>

#include <dali-scene3d/public-api/model-components/material.h>
#include <dali-scene3d/public-api/model-components/model-node.h>
#include <dali-scene3d/public-api/model-components/model-primitive.h>

#include <dali-scene3d/public-api/model-motion/motion-data.h>
#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-transform-index.h>
#include <dali-scene3d/public-api/model-motion/motion-value.h>

#endif // DALI_SCENE3D_H