#ifndef DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
#define DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/mutex.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/model-loader-impl.h>

namespace Dali::Scene3D::Loader::Internal
{
class Gltf2LoaderImpl : public ModelLoaderImpl
{
public:
  /**
   * @copydoc Dali::Scene3D::Loader::odelLoaderImpl::LoadMode()
   */
  bool LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result) override;
};

} // namespace Dali::Scene3D::Loader::Internal

#endif // DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
