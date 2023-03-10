#ifndef DALI_SCENE3D_LOADER_MESH_GEOMETRY_H
#define DALI_SCENE3D_LOADER_MESH_GEOMETRY_H
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
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali::Scene3D::Loader
{
struct DALI_SCENE3D_API MeshGeometry
{
  Geometry       geometry;                    ///< The array of vertices.
  Texture        blendShapeGeometry;          ///< The array of vertices of the different blend shapes encoded inside a texture with power of two dimensions.
  Vector<float>  blendShapeUnnormalizeFactor; ///< Factor used to unnormalize the geometry of the blend shape.
  unsigned int   blendShapeBufferOffset{0};   ///< Offset used to calculate the start of each blend shape.
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_MESH_GEOMETRY_H
