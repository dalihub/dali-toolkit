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

// FILE HEADER
#include <dali-usd-loader/internal/utils.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#include <pxr/usd/ar/asset.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/usdShade/connectableAPI.h>

using namespace Dali;
using namespace pxr;

namespace Dali::Scene3D::Loader
{
namespace
{
#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_USD_UTILS");
#endif
} // namespace

Matrix ConvertUsdMatrix(const GfMatrix4d& gfMat)
{
  std::vector<float> matData(gfMat.data(), gfMat.data() + 16);
  return Matrix(matData.data());
}

UsdShaderContainer TraverseShaderInputs(const UsdShadeShader& shader)
{
  UsdShaderContainer matches;

  for(const auto& i : shader.GetInputs())
  {
    if(i.HasConnectedSource())
    {
      for(const auto& s : i.GetConnectedSources())
      {
        if(s)
        {
          matches.push_back(s.source);
          auto nestedMatches = TraverseShaderInputs(s.source);
          matches.insert(matches.end(), nestedMatches.begin(), nestedMatches.end());
        }
      }
    }
  }

  return matches;
}

std::string ConvertImagePath(const std::string& input)
{
  std::string result = input;

  // Find the position of '[' and ']'
  size_t startPos = result.find('[');
  size_t endPos   = result.find(']');

  if(startPos != std::string::npos && endPos != std::string::npos)
  {
    // Extract the substring between '[' and ']'
    std::string extracted = result.substr(startPos + 1, endPos - startPos - 1);

    // Find the last '/' in the extracted string between '[' and ']'
    size_t lastSlashPosInExtracted = extracted.rfind('/', endPos);
    if(lastSlashPosInExtracted != std::string::npos)
    {
      extracted.erase(0, lastSlashPosInExtracted + 1);
    }

    // Find the last '/' before '[' in the original path
    size_t lastSlashPos = result.rfind('/', startPos);
    if(lastSlashPos != std::string::npos)
    {
      result.erase(lastSlashPos + 1, endPos - lastSlashPos + 1);
      result.insert(lastSlashPos + 1, extracted);
    }
  }

  return result;
}

UsdAssetBuffer LoadAssetFileAsBuffer(const std::string resolvedAssetPath)
{
  std::shared_ptr<ArAsset> const asset = ArGetResolver().OpenAsset(ArResolvedPath(resolvedAssetPath));

  if(asset)
  {
    std::shared_ptr<const char> const buffer = asset->GetBuffer();
    if(buffer)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "LoadAssetFileAsBuffer: %s, size: %lu", ArResolvedPath(resolvedAssetPath).GetPathString().c_str(), asset->GetSize());

      // Convert the buffer to a vector of uint8_t
      return UsdAssetBuffer(buffer.get(), buffer.get() + asset->GetSize());
    }
  }

  // Return an empty vector if loading fails
  return UsdAssetBuffer();
}

} // namespace Dali::Scene3D::Loader
