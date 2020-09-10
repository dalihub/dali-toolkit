/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>

namespace
{
#define TOKEN_STRING(x) #x

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
const std::string AssetManager::GetDaliImagePath()
{
  /**
   * @note DALI_IMAGE_DIR is a macro that can be defined either with a file system path or zero.
   *       If it's defined as zero then the value is retrieved from an environment variable
   *       named DALI_IMAGE_DIR.
   */
  return (nullptr == DALI_IMAGE_DIR) ? EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_IMAGE_DIR)) : DALI_IMAGE_DIR;
}

const std::string AssetManager::GetDaliSoundPath()
{
  /**
   * @note DALI_SOUND_DIR is a macro that can be defined either with a file system path or zero.
   *       If it's defined as zero then the value is retrieved from an environment variable
   *       named DALI_SOUND_DIR.
   */
  return (nullptr == DALI_SOUND_DIR) ? EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_SOUND_DIR)) : DALI_SOUND_DIR;
}

const std::string AssetManager::GetDaliStylePath()
{
  /**
   * @note DALI_STYLE_DIR is a macro that can be defined either with a file system path or zero.
   *       If it's defined as zero then the value is retrieved from an environment variable
   *       named DALI_STYLE_DIR.
   */
  return (nullptr == DALI_STYLE_DIR) ? EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_STYLE_DIR)) : DALI_STYLE_DIR;
}

const std::string AssetManager::GetDaliStyleImagePath()
{
  /**
   * @note DALI_STYLE_IMAGE_DIR is a macro that can be defined either with a file system path or zero.
   *       If it's defined as zero then the value is retrieved from an environment variable
   *       named DALI_STYLE_IMAGE_DIR.
   */
  return (nullptr == DALI_STYLE_IMAGE_DIR) ? EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_STYLE_IMAGE_DIR)) : DALI_STYLE_IMAGE_DIR;
}

const std::string AssetManager::GetDaliDataReadOnlyPath()
{
  /**
   * @note DALI_DATA_READ_ONLY_DIR is a macro that can be defined either with a file system path or zero.
   *       If it's defined as zero then the value is retrieved from an environment variable
   *       named DALI_DATA_READ_ONLY_DIR.
   */
  return (nullptr == DALI_DATA_READ_ONLY_DIR) ? EnvironmentVariable::GetEnvironmentVariable(TOKEN_STRING(DALI_DATA_READ_ONLY_DIR)) : DALI_DATA_READ_ONLY_DIR;
}

} // namespace Toolkit

} // namespace Dali
