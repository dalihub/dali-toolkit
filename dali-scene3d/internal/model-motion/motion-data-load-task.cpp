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
#include <dali-scene3d/internal/model-motion/motion-data-load-task.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/bvh-loader.h>
#include <dali-scene3d/public-api/loader/facial-animation-loader.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
MotionDataLoadTask::MotionDataLoadTask(const std::string& path, bool useRootTranslationOnly, const Vector3& scale, CallbackBase* callback)
: AsyncTask(callback),
  mFileUrl(path),
  mRawBuffer(nullptr),
  mRawBufferLength(0),
  mScale(scale),
  mUseRootTranslationOnly(useRootTranslationOnly),
  mAnimationDefinition{},
  mLoadMethod(MotionDataLoadTask::LoadMethod::BVH_FILE)
{
}

MotionDataLoadTask::MotionDataLoadTask(const uint8_t* rawBuffer, int rawBufferLength, bool useRootTranslationOnly, const Vector3& scale, CallbackBase* callback)
: AsyncTask(callback),
  mFileUrl(),
  mRawBuffer(nullptr),
  mRawBufferLength(rawBufferLength),
  mScale(scale),
  mUseRootTranslationOnly(useRootTranslationOnly),
  mAnimationDefinition{},
  mLoadMethod(MotionDataLoadTask::LoadMethod::BVH_BUFFER)
{
  // To keep life of raw buffer, let us copy data now.
  mRawBuffer = new uint8_t[mRawBufferLength];
  memcpy(mRawBuffer, rawBuffer, sizeof(uint8_t) * mRawBufferLength);
}

MotionDataLoadTask::MotionDataLoadTask(const std::string& url, CallbackBase* callback)
: AsyncTask(callback),
  mFileUrl(url),
  mRawBuffer(nullptr),
  mRawBufferLength(0),
  mScale(),
  mUseRootTranslationOnly(false),
  mAnimationDefinition{},
  mLoadMethod(MotionDataLoadTask::LoadMethod::FACIAL_FILE)
{
}

MotionDataLoadTask::MotionDataLoadTask(const uint8_t* rawBuffer, int rawBufferLength, CallbackBase* callback)
: AsyncTask(callback),
  mFileUrl(),
  mRawBuffer(nullptr),
  mRawBufferLength(rawBufferLength),
  mScale(),
  mUseRootTranslationOnly(false),
  mAnimationDefinition{},
  mLoadMethod(MotionDataLoadTask::LoadMethod::FACIAL_BUFFER)
{
  // To keep life of raw buffer, let us copy data now.
  mRawBuffer = new uint8_t[mRawBufferLength];
  memcpy(mRawBuffer, rawBuffer, sizeof(uint8_t) * mRawBufferLength);
}

MotionDataLoadTask::~MotionDataLoadTask()
{
  if(mRawBuffer)
  {
    delete[] mRawBuffer;
  }
}

void MotionDataLoadTask::Process()
{
  switch(mLoadMethod)
  {
    case LoadMethod::BVH_FILE:
    {
      mAnimationDefinition = std::move(Loader::LoadBvh(mFileUrl, "LoadedBvhMotionData", mUseRootTranslationOnly, mScale));
      break;
    }
    case LoadMethod::BVH_BUFFER:
    {
      mAnimationDefinition = std::move(Loader::LoadBvhFromBuffer(mRawBuffer, mRawBufferLength, "LoadedBvhMotionData", mUseRootTranslationOnly, mScale));
      break;
    }
    case LoadMethod::FACIAL_FILE:
    {
      mAnimationDefinition = std::move(Loader::LoadFacialAnimation(mFileUrl));
      break;
    }
    case LoadMethod::FACIAL_BUFFER:
    {
      mAnimationDefinition = std::move(Loader::LoadFacialAnimationFromBuffer(mRawBuffer, mRawBufferLength));
      break;
    }
  }
}

bool MotionDataLoadTask::IsReady()
{
  return true;
}

const Scene3D::Loader::AnimationDefinition& MotionDataLoadTask::GetAnimationDefinition() const
{
  return mAnimationDefinition;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali