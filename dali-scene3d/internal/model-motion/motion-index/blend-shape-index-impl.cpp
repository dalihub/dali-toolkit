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

// CLASS HEADER
#include <dali-scene3d/internal/model-motion/motion-index/blend-shape-index-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/blend-shape-details.h> ///< For BlendShapes::WEIGHTS_UNIFORM

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Scene3D::BlendShapeIndex::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::BlendShapeIndex, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

std::string GetBlendShapePropertyNameFromIndex(int index)
{
  if(index >= 0)
  {
    char weightNameBuffer[32];

    // Get return value of snprintf to avoid SVACE.
    [[maybe_unused]] auto prefixSize = snprintf(weightNameBuffer, sizeof(weightNameBuffer), "%s[%d]", Loader::BlendShapes::WEIGHTS_UNIFORM, index);

    return weightNameBuffer;
  }
  return "";
}

} // namespace

BlendShapeIndexPtr BlendShapeIndex::New()
{
  BlendShapeIndexPtr blendShapeIndex = new BlendShapeIndex();

  blendShapeIndex->Initialize();

  return blendShapeIndex;
}

BlendShapeIndex::BlendShapeIndex()
{
}

BlendShapeIndex::~BlendShapeIndex()
{
}

void BlendShapeIndex::Initialize()
{
}

void BlendShapeIndex::SetBlendShapeId(Property::Key blendShapeId)
{
  mBlendShapeId = blendShapeId;
}

Property::Key BlendShapeIndex::GetBlendShapeId() const
{
  return mBlendShapeId;
}

std::string BlendShapeIndex::GetPropertyName(Scene3D::ModelNode node)
{
  if(mBlendShapeId.type == Property::Key::Type::INDEX)
  {
    if(mBlendShapeId.indexKey != Property::INVALID_INDEX)
    {
      return GetBlendShapePropertyNameFromIndex(mBlendShapeId.indexKey);
    }
  }
  else if(mBlendShapeId.type == Property::Key::Type::STRING)
  {
    if(node)
    {
      auto index = node.GetBlendShapeIndexByName(mBlendShapeId.stringKey);
      if(index != Scene3D::Loader::BlendShapes::INVALID_INDEX)
      {
        return GetBlendShapePropertyNameFromIndex(index);
      }
    }
  }
  return "";
}

Property::Index BlendShapeIndex::GetPropertyIndex(Scene3D::ModelNode node)
{
  // Not surport now
  return Property::INVALID_INDEX;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali