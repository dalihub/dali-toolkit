#ifndef DALI_SCENE3D_LOADER_VIEW_PROJECTION_H_
#define DALI_SCENE3D_LOADER_VIEW_PROJECTION_H_
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

// EXTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Contains view and projection matrices, also caching the view-projection
 *      and inverse projection matrices.
 */
class ViewProjection
{
public:
  Matrix& GetView()
  {
    return mView;
  }
  Matrix& GetProjection()
  {
    return mProjection;
  }

  /*
   * @brief Updates the cached view projection and inverse projection matrices.
   */
  void Update();

  const Matrix& GetView() const
  {
    return mView;
  }
  const Matrix& GetProjection() const
  {
    return mProjection;
  }
  const Matrix& GetInverseProjection() const
  {
    return mInvProjection;
  }
  const Matrix& GetViewProjection() const
  {
    return mViewProjection;
  }

private:
  Matrix mView;
  Matrix mProjection;
  Matrix mInvProjection;
  Matrix mViewProjection;
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_VIEW_PROJECTION_H_
