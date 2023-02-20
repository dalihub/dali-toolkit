#ifndef DALI_SCENE3D_LOADER_DLI_LOADER_IMPL_H
#define DALI_SCENE3D_LOADER_DLI_LOADER_IMPL_H
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
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/customization.h>
#include <dali-scene3d/public-api/loader/dli-input-parameter.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/string-callback.h>

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace Internal
{
typedef std::pair<std::string, std::string> Metadata;

// Forward declarations
struct LoadResult;

class DliLoaderImpl : public ModelLoaderImpl
{
public:
  struct LoadParams
  {
    Dali::Scene3D::Loader::DliInputParameter* input;
    Dali::Scene3D::Loader::LoadResult*        output;
  };

  DliLoaderImpl();
  ~DliLoaderImpl();

  /**
   * @brief Sets the callback that messages from non-fatal errors get posted to.
   *  Uses DefaultErrorCallback by default.
   */
  void SetErrorCallback(StringCallback onError);

  /**
   * @copydoc Dali::Scene3D::Loader::Internal::ModelLoaderImpl()
   */
  bool LoadModel(const std::string& uri, Dali::Scene3D::Loader::LoadResult& result) override;

  /**
   * @return The error string describing how the parse has failed, if any.
   */
  std::string GetParseError() const;

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

} // namespace Internal
} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_DLI_LOADER_IMPL_H
