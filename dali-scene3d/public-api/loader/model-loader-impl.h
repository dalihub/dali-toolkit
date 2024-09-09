#ifndef DALI_SCENE3D_LOADER_MODEL_LOADER_IMPL_H
#define DALI_SCENE3D_LOADER_MODEL_LOADER_IMPL_H
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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/model-loader.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
class ModelLoaderImpl
{
public:
  ModelLoaderImpl() = default;

  virtual ~ModelLoaderImpl() = default;

  /**
   * @brief Set InputParameter.
   * Thie method store only a pointer of InputParameter.
   * The object of InputParameter should not be deleted until it is no longer used.
   * @param[in] inputParameter Input parameters those can be used for model loading.
   */
  void SetInputParameter(Dali::Scene3D::Loader::ModelLoader::InputParameter& inputParameter)
  {
    mInputParameter = &inputParameter;
  }

  /**
   * @brief Request to load model from url.
   * @param[in] url model file url.
   * @param[out] result loaded model data.
   * @return True if model loading is successfully finished.
   */
  virtual bool LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result) = 0;

protected:
  Dali::Scene3D::Loader::ModelLoader::InputParameter* mInputParameter{nullptr};
};
} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_MODEL_LOADER_IMPL_H
