
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
const char* BlendShapes::NUMBER_OF_BLEND_SHAPES("uNumberOfBlendShapes");
const char* BlendShapes::UNNORMALIZE_FACTOR("uBlendShapeUnnormalizeFactor");
const char* BlendShapes::COMPONENT_SIZE("uBlendShapeComponentSize");

const char* BlendShapes::COMPONENTS("blendShapeComponents");

const char* BlendShapes::WEIGHTS_UNIFORM("uBlendShapeWeight");

void BlendShapes::ConfigureProperties(const BlendShapeData& data, Renderer renderer)
{
  unsigned int index = 0u;

  char        weightNameBuffer[32];
  char        unnormalizeFactorNameBuffer[64];
  char* const pWeightName = weightNameBuffer + snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s", WEIGHTS_UNIFORM);
  char* const pFactorName = unnormalizeFactorNameBuffer + snprintf(unnormalizeFactorNameBuffer, sizeof(unnormalizeFactorNameBuffer), "%s", UNNORMALIZE_FACTOR);
  for(const auto& weight : data.weights)
  {
    snprintf(pWeightName, sizeof(weightNameBuffer) - (pWeightName - weightNameBuffer), "[%d]", index);
    std::string weightName{weightNameBuffer};
    Dali::Actor actor = data.mActor.GetHandle();
    if(actor)
    {
      actor.RegisterProperty(weightName, weight);
    }

    if(renderer && data.version == Version::VERSION_1_0)
    {
      snprintf(pFactorName, sizeof(unnormalizeFactorNameBuffer) - (pFactorName - unnormalizeFactorNameBuffer), "[%d]", index);
      std::string factorName{unnormalizeFactorNameBuffer};
      renderer.RegisterProperty(factorName, data.unnormalizeFactors[index]);
    }

    ++index;
  }

  if(renderer)
  {
    if(Version::VERSION_2_0 == data.version)
    {
      renderer.RegisterProperty(UNNORMALIZE_FACTOR, data.unnormalizeFactors[0u]);
    }

    renderer.RegisterProperty(NUMBER_OF_BLEND_SHAPES, Property::Value(static_cast<float>(index)));
    renderer.RegisterProperty(COMPONENT_SIZE, Property::Value(static_cast<float>(data.bufferOffset)));

    // Create a read only property to preserve the components of the blend shape.
    renderer.RegisterProperty(COMPONENTS, data.components, Property::AccessMode::READ_ONLY);
  }
}

} // namespace Dali::Scene3D::Loader
