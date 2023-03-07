
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

// FILE HEADER
#include <dali-scene3d/public-api/loader/blend-shape-details.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/resource-bundle.h>

namespace Dali::Scene3D::Loader
{
const std::string BlendShapes::NUMBER_OF_BLEND_SHAPES("uNumberOfBlendShapes");
const std::string BlendShapes::UNNORMALIZE_FACTOR("uBlendShapeUnnormalizeFactor");
const std::string BlendShapes::COMPONENT_SIZE("uBlendShapeComponentSize");

const std::string BlendShapes::COMPONENTS("blendShapeComponents");

const std::string BlendShapes::WEIGHTS_UNIFORM("uBlendShapeWeight");

void BlendShapes::ConfigureProperties(const std::pair<MeshDefinition, MeshGeometry>& mesh, Shader shader, Actor actor)
{
  unsigned int index = 0u;

  char        weightNameBuffer[32];
  char        unnormalizeFactorNameBuffer[64];
  char* const pWeightName = weightNameBuffer + snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s", WEIGHTS_UNIFORM.c_str());
  char* const pFactorName = unnormalizeFactorNameBuffer + snprintf(unnormalizeFactorNameBuffer, sizeof(unnormalizeFactorNameBuffer), "%s", UNNORMALIZE_FACTOR.c_str());
  for(const auto& blendShape : mesh.first.mBlendShapes)
  {
    snprintf(pWeightName, sizeof(weightNameBuffer) - (pWeightName - weightNameBuffer), "[%d]", index);
    std::string weightName{weightNameBuffer};
    actor.RegisterProperty(weightName, blendShape.weight);

    if(shader && mesh.first.mBlendShapeVersion == Version::VERSION_1_0)
    {
      snprintf(pFactorName, sizeof(unnormalizeFactorNameBuffer) - (pFactorName - unnormalizeFactorNameBuffer), "[%d]", index);
      std::string factorName{unnormalizeFactorNameBuffer};
      shader.RegisterProperty(factorName, mesh.second.blendShapeUnnormalizeFactor[index]);
    }

    ++index;
  }

  if(shader)
  {
    if(Version::VERSION_2_0 == mesh.first.mBlendShapeVersion)
    {
      shader.RegisterProperty(UNNORMALIZE_FACTOR, mesh.second.blendShapeUnnormalizeFactor[0u]);
    }

    shader.RegisterProperty(NUMBER_OF_BLEND_SHAPES, Property::Value(static_cast<float>(index)));
    shader.RegisterProperty(COMPONENT_SIZE, Property::Value(static_cast<float>(mesh.second.blendShapeBufferOffset)));

    // Create a read only property to preserve the components of the blend shape.
    int32_t components = 0x0;
    for(auto& bs : mesh.first.mBlendShapes)
    {
      components |= (bs.deltas.IsDefined() * Component::POSITIONS) |
                    (bs.normals.IsDefined() * Component::NORMALS) | (bs.tangents.IsDefined() * Component::TANGENTS);
    }
    shader.RegisterProperty(COMPONENTS, components, Property::AccessMode::READ_ONLY);
  }
}

} // namespace Dali::Scene3D::Loader
