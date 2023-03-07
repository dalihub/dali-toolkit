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
#include <dali-scene3d/public-api/loader/view-projection.h>

// INTERNAL INCLDUES
#include <dali-scene3d/public-api/loader/utils.h>

namespace Dali::Scene3D::Loader
{
void ViewProjection::Update()
{
  Matrix::Multiply(mViewProjection, mView, mProjection);

  mInvProjection = mProjection;
  if(!mInvProjection.Invert())
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to find inverse of projection matrix " << mProjection << ".";
  }
}

} // namespace Dali::Scene3D::Loader
