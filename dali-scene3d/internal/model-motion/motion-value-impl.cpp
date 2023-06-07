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
#include <dali-scene3d/internal/model-motion/motion-value-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/animation/key-frames-devel.h>
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
  return Scene3D::MotionValue::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::MotionValue, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

MotionValuePtr MotionValue::New()
{
  MotionValuePtr motionValue = new MotionValue();

  motionValue->Initialize();

  return motionValue;
}

MotionValue::MotionValue()
{
}

MotionValue::~MotionValue()
{
}

void MotionValue::Initialize()
{
}

// Public Method

Scene3D::MotionValue::ValueType MotionValue::GetValueType() const
{
  return mType;
}

void MotionValue::SetValue(Property::Value propertyValue)
{
  mType = Scene3D::MotionValue::ValueType::PROPERTY_VALUE;

  mKeyFrames.Reset();
  mPropertyValue = propertyValue;
}

void MotionValue::SetValue(Dali::KeyFrames keyFrames)
{
  mType = Scene3D::MotionValue::ValueType::KEY_FRAMES;

  mKeyFrames     = keyFrames;
  mPropertyValue = Property::Value();
}

void MotionValue::Clear()
{
  mType = Scene3D::MotionValue::ValueType::INVALID;

  mKeyFrames.Reset();
  mPropertyValue = Property::Value();
}

Property::Value MotionValue::GetPropertyValue() const
{
  if(mType == Scene3D::MotionValue::ValueType::PROPERTY_VALUE)
  {
    return mPropertyValue;
  }
  else if(mType == Scene3D::MotionValue::ValueType::KEY_FRAMES)
  {
    // Get last value of keyframes
    auto keyFrameCount = DevelKeyFrames::GetKeyFrameCount(mKeyFrames);
    if(keyFrameCount > 0u)
    {
      float           dummyProgress;
      Property::Value propertyValue;
      DevelKeyFrames::GetKeyFrame(mKeyFrames, keyFrameCount - 1u, dummyProgress, propertyValue);

      return propertyValue;
    }
  }
  return Property::Value();
}

KeyFrames MotionValue::GetKeyFrames() const
{
  if(mType == Scene3D::MotionValue::ValueType::KEY_FRAMES)
  {
    return mKeyFrames;
  }
  else if(mType == Scene3D::MotionValue::ValueType::PROPERTY_VALUE)
  {
    // Generate stable keyframe animation here.
    // TODO : Should we check property value is animatable type or not?
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.0f, mPropertyValue);
    keyFrames.Add(1.0f, mPropertyValue);
    return keyFrames;
  }
  return Dali::KeyFrames();
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali