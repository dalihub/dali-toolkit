/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/internal/common/environment-map-load-task.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/cube-map-loader.h>


namespace Dali
{
namespace Scene3D
{
namespace Internal
{

EnvironmentMapLoadTask::EnvironmentMapLoadTask(const std::string& environmentMapUrl, CallbackBase* callback)
: AsyncTask(callback),
  mEnvironmentMapUrl(environmentMapUrl),
  mIsReady(true),
  mHasSucceeded(false)
{
}

EnvironmentMapLoadTask::~EnvironmentMapLoadTask()
{
}

void EnvironmentMapLoadTask::Process()
{
  mHasSucceeded = Scene3D::Loader::LoadCubeMapData(mEnvironmentMapUrl, mEnvironmentMapPixelData);
}

bool EnvironmentMapLoadTask::IsReady()
{
  return mIsReady;
}

bool EnvironmentMapLoadTask::HasSucceeded() const
{
  return mHasSucceeded;
}

Dali::Scene3D::Loader::CubeData EnvironmentMapLoadTask::GetEnvironmentMap() const
{
  Dali::Scene3D::Loader::CubeData environmentMapPixelData;
  if(mIsReady && mHasSucceeded && !mEnvironmentMapPixelData.data.empty())
  {
    environmentMapPixelData = mEnvironmentMapPixelData;
  }
  return environmentMapPixelData;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
