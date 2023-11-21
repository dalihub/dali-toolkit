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
#include <dali-scene3d/internal/model-motion/motion-data-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/model-motion/motion-index/motion-property-index.h>

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
  return Scene3D::MotionData::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::MotionData, Dali::BaseHandle, Create);
DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

MotionDataPtr MotionData::New()
{
  MotionDataPtr motionData = new MotionData();

  motionData->Initialize();

  return motionData;
}

MotionData::MotionData()
{
}

MotionData::~MotionData()
{
  if(Dali::Adaptor::IsAvailable())
  {
    CancelMotionDataLoad();
  }
}

void MotionData::Initialize()
{
}

uint32_t MotionData::GetMotionCount() const
{
  return static_cast<uint32_t>(mMotions.size());
}

Scene3D::MotionIndex MotionData::GetIndex(uint32_t index) const
{
  if(index < mMotions.size())
  {
    return mMotions[index].first;
  }
  return Scene3D::MotionIndex();
}

Scene3D::MotionValue MotionData::GetValue(uint32_t index) const
{
  if(index < mMotions.size())
  {
    return mMotions[index].second;
  }
  return Scene3D::MotionValue();
}

void MotionData::Add(Scene3D::MotionIndex index, Scene3D::MotionValue value)
{
  mMotions.emplace_back(index, value);
}

void MotionData::Clear()
{
  mMotions.clear();
}

void MotionData::SetDuration(float durationSeconds)
{
  mDurationSeconds = durationSeconds;
}

float MotionData::GetDuration() const
{
  return mDurationSeconds;
}

void MotionData::LoadBvh(const std::string& path, bool useRootTranslationOnly, const Vector3& scale, bool synchronousLoad)
{
  CancelMotionDataLoad();
  mMotionDataLoadTask = new MotionDataLoadTask(path, useRootTranslationOnly, scale, MakeCallback(this, &MotionData::OnLoadCompleted));
  RequestMotionDataLoad(synchronousLoad);
}

void MotionData::LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool useRootTranslationOnly, const Vector3& scale, bool synchronousLoad)
{
  CancelMotionDataLoad();
  mMotionDataLoadTask = new MotionDataLoadTask(rawBuffer, rawBufferLength, useRootTranslationOnly, scale, MakeCallback(this, &MotionData::OnLoadCompleted));
  RequestMotionDataLoad(synchronousLoad);
}

void MotionData::LoadFacialAnimation(const std::string& url, bool synchronousLoad)
{
  CancelMotionDataLoad();
  mMotionDataLoadTask = new MotionDataLoadTask(url, MakeCallback(this, &MotionData::OnLoadCompleted));
  RequestMotionDataLoad(synchronousLoad);
}

void MotionData::LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool synchronousLoad)
{
  CancelMotionDataLoad();
  mMotionDataLoadTask = new MotionDataLoadTask(rawBuffer, rawBufferLength, MakeCallback(this, &MotionData::OnLoadCompleted));
  RequestMotionDataLoad(synchronousLoad);
}

// Private method
void MotionData::RequestMotionDataLoad(bool synchronousLoad)
{
  if(mMotionDataLoadTask)
  {
    if(synchronousLoad)
    {
      mMotionDataLoadTask->Process();
      OnLoadCompleted(mMotionDataLoadTask);
    }
    else
    {
      Dali::AsyncTaskManager::Get().AddTask(mMotionDataLoadTask);
    }
  }
}

void MotionData::CancelMotionDataLoad()
{
  if(mMotionDataLoadTask)
  {
    Dali::AsyncTaskManager::Get().RemoveTask(mMotionDataLoadTask);
    mMotionDataLoadTask.Reset();
  }
}

// Called from MotionDataLoadTask
void MotionData::OnLoadCompleted(MotionDataLoadTaskPtr task)
{
  if(mMotionDataLoadTask == task)
  {
    const Scene3D::Loader::AnimationDefinition& animationDefinition = mMotionDataLoadTask->GetAnimationDefinition();

    mDurationSeconds = animationDefinition.GetDuration();

    auto animatedPropertyCount = animationDefinition.GetPropertyCount();

    mMotions.clear();
    mMotions.reserve(animatedPropertyCount);

    for(uint32_t i = 0u; i < animatedPropertyCount; ++i)
    {
      const auto& animatedProperty = animationDefinition.GetPropertyAt(i);

      // TODO : Currently, we only support KeyFrames without alpha function and time period now.
      if(animatedProperty.mKeyFrames)
      {
        Add(Scene3D::MotionPropertyIndex::New(animatedProperty.mNodeName, animatedProperty.mPropertyName), Scene3D::MotionValue::New(animatedProperty.mKeyFrames));
      }
    }

    // Reset task before emit load competed signal.
    mMotionDataLoadTask.Reset();
    {
      Scene3D::MotionData handle(this); ///< Keep handle for lifecycle.
      LoadCompletedSignal().Emit(handle);
    }
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali