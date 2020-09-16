#ifndef DALI_TOOLKIT_ASSET_MANAGER_DEVEL_H
#define DALI_TOOLKIT_ASSET_MANAGER_DEVEL_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Retrieves the file system path of the assets.
 */
class AssetManager
{
public:
  /**
   * @return The file system path of the images.
   */
  static const std::string GetDaliImagePath();

  /**
   * @return The file system path of the sounds.
   */
  static const std::string GetDaliSoundPath();

  /**
   * @return The file system path of the styles.
   */
  static const std::string GetDaliStylePath();

  /**
   * @return The file system path of the images in the styles.
   */
  static const std::string GetDaliStyleImagePath();

  /**
   * @return The file system path of the DALi's read only data.
   */
  static const std::string GetDaliDataReadOnlyPath();
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ASSET_MANAGER_DEVEL_H
