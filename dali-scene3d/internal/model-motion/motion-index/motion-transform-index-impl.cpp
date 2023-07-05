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
#include <dali-scene3d/internal/model-motion/motion-index/motion-transform-index-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
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
  return Scene3D::MotionTransformIndex::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::MotionTransformIndex, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()
} // namespace

MotionTransformIndexPtr MotionTransformIndex::New()
{
  MotionTransformIndexPtr motionTransformIndex = new MotionTransformIndex();

  motionTransformIndex->Initialize();

  return motionTransformIndex;
}

MotionTransformIndex::MotionTransformIndex()
{
}

MotionTransformIndex::~MotionTransformIndex()
{
}

void MotionTransformIndex::Initialize()
{
}

void MotionTransformIndex::SetTransformType(Scene3D::MotionTransformIndex::TransformType type)
{
  mTransformType = type;
}

Scene3D::MotionTransformIndex::TransformType MotionTransformIndex::GetTransformType() const
{
  return mTransformType;
}

std::string MotionTransformIndex::GetPropertyName(Scene3D::ModelNode node)
{
  using TransformType = Scene3D::MotionTransformIndex::TransformType;

  switch(mTransformType)
  {
    case TransformType::POSITION:
    {
      return "position";
    }
    case TransformType::POSITION_X:
    {
      return "positionX";
    }
    case TransformType::POSITION_Y:
    {
      return "positionY";
    }
    case TransformType::POSITION_Z:
    {
      return "positionZ";
    }
    case TransformType::ORIENTATION:
    {
      return "orientation";
    }
    case TransformType::SCALE:
    {
      return "scale";
    }
    case TransformType::SCALE_X:
    {
      return "scaleX";
    }
    case TransformType::SCALE_Y:
    {
      return "scaleY";
    }
    case TransformType::SCALE_Z:
    {
      return "scaleZ";
    }
    default:
    {
      DALI_LOG_ERROR("Invalid transform type used\n");
      return "";
    }
  }
}

Property::Index MotionTransformIndex::GetPropertyIndex(Scene3D::ModelNode node)
{
  using TransformType = Scene3D::MotionTransformIndex::TransformType;

  switch(mTransformType)
  {
    case TransformType::POSITION:
    {
      return Dali::Actor::Property::POSITION;
    }
    case TransformType::POSITION_X:
    {
      return Dali::Actor::Property::POSITION_X;
    }
    case TransformType::POSITION_Y:
    {
      return Dali::Actor::Property::POSITION_Y;
    }
    case TransformType::POSITION_Z:
    {
      return Dali::Actor::Property::POSITION_Z;
    }
    case TransformType::ORIENTATION:
    {
      return Dali::Actor::Property::ORIENTATION;
    }
    case TransformType::SCALE:
    {
      return Dali::Actor::Property::SCALE;
    }
    case TransformType::SCALE_X:
    {
      return Dali::Actor::Property::SCALE_X;
    }
    case TransformType::SCALE_Y:
    {
      return Dali::Actor::Property::SCALE_Y;
    }
    case TransformType::SCALE_Z:
    {
      return Dali::Actor::Property::SCALE_Z;
    }
    default:
    {
      DALI_LOG_ERROR("Invalid transform type used\n");
      return Property::INVALID_INDEX;
    }
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali