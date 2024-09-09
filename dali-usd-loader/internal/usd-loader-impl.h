#ifndef DALI_SCENE3D_LOADER_USD_LOADER_IMPL_H
#define DALI_SCENE3D_LOADER_USD_LOADER_IMPL_H
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
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/model-loader-impl.h>

namespace Dali::Scene3D::Loader
{
class UsdLoaderImpl : public ModelLoaderImpl
{
public:
  UsdLoaderImpl();
  ~UsdLoaderImpl();

  /**
   * @copydoc Dali::Scene3D::Loader::Internal::ModelLoaderImpl::LoadMode()
   */
  bool LoadModel(const std::string& url, Dali::Scene3D::Loader::LoadResult& result) override;

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_USD_LOADER_IMPL_H
