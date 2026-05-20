#ifndef DALI_SCENE3D_H
#define DALI_SCENE3D_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>
#include <dali-scene3d/public-api/algorithm/path-finder.h>

#include <dali-scene3d/public-api/common/environment-map.h>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/panel/panel.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

#include <dali-scene3d/public-api/light/light.h>

#include <dali-scene3d/public-api/common/blend-shape-types.h>

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