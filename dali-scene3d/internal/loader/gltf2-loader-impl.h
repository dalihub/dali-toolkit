#ifndef DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
#define DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
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

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/model-loader-impl.h>
#include <dali-scene3d/public-api/api.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/threading/mutex.h>
#include <string>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace Internal
{

class Gltf2LoaderImpl : public ModelLoaderImpl
{
public:

  /**
   * @copydoc Dali::Scene3D::Loader::Internal::ModelLoaderImpl()
   */
  bool LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result) override;

private:
  /**
   * @brief Initialize glTF Loader.
   * @note This method should be called once before LoadGltfScene() is called.
   */
  void InitializeGltfLoader();
};

} // namespace Internal
} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_GLTF2_LOADER_IMPL_H
