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
#include <dali-scene3d/internal/model-motion/motion-index/motion-property-index-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES

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
  return Scene3D::MotionPropertyIndex::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::MotionPropertyIndex, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

} // namespace

MotionPropertyIndexPtr MotionPropertyIndex::New()
{
  MotionPropertyIndexPtr motionPropertyIndex = new MotionPropertyIndex();

  motionPropertyIndex->Initialize();

  return motionPropertyIndex;
}

MotionPropertyIndex::MotionPropertyIndex()
{
}

MotionPropertyIndex::~MotionPropertyIndex()
{
}

void MotionPropertyIndex::Initialize()
{
}

void MotionPropertyIndex::SetPropertyId(Property::Key propertyId)
{
  mPropertyId = propertyId;
}

Property::Key MotionPropertyIndex::GetPropertyId() const
{
  return mPropertyId;
}

std::string MotionPropertyIndex::GetPropertyName(Scene3D::ModelNode node)
{
  if(mPropertyId.type == Property::Key::Type::STRING)
  {
    return mPropertyId.stringKey;
  }
  return "";
}

Property::Index MotionPropertyIndex::GetPropertyIndex(Scene3D::ModelNode node)
{
  if(mPropertyId.type == Property::Key::Type::INDEX)
  {
    return mPropertyId.indexKey;
  }
  return Property::INVALID_INDEX;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali