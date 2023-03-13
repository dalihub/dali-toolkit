#ifndef DALI_SCENE3D_LOADER_LIGHT_PARAMETERS_H
#define DALI_SCENE3D_LOADER_LIGHT_PARAMETERS_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
struct DALI_SCENE3D_API LightParameters
{
  Matrix transform;

  Vector3  color;
  float    intensity;
  float    shadowIntensity;
  uint32_t shadowMapSize;
  float    orthographicSize;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_LIGHT_PARAMETERS_H
