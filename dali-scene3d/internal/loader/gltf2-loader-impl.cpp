/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-scene3d/internal/loader/gltf2-loader-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/gltf2-util.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/utils.h>

using Dali::Integration::ToStdString;

namespace gt = gltf2;
namespace js = json;

namespace Dali::Scene3D::Loader::Internal
{
bool Gltf2LoaderImpl::LoadModel(const Dali::String& url, Dali::Scene3D::Loader::LoadResult& result)
{
  std::string stdUrl   = ToStdString(url);
  bool        failed   = false;
  auto        gltfText = LoadTextFile(stdUrl.c_str(), &failed);
  if(failed)
  {
    DALI_LOG_ERROR("Failed to load %s\n", stdUrl.c_str());
    return false;
  }

  json::unique_ptr root(json_parse(gltfText.CStr(), gltfText.Size()));
  if(!root)
  {
    DALI_LOG_ERROR("Failed to parse %s\n", stdUrl.c_str());
    return false;
  }

  gt::Document document;

  bool isMRendererModel(false);
  if(!Gltf2Util::GenerateDocument(root, document, isMRendererModel))
  {
    return false;
  }

  auto                         path = stdUrl.substr(0, stdUrl.rfind('/') + 1);
  Gltf2Util::ConversionContext context{result, path, INVALID_INDEX};

  Gltf2Util::ConvertGltfToContext(document, context, isMRendererModel);

  return true;
}

} // namespace Dali::Scene3D::Loader::Internal
