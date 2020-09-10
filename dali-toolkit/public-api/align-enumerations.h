#ifndef DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMERATIONS_H
#define DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMERATIONS_H

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

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

/**
 * @brief Enumerations for the alignment of Visuals
 * @SINCE_1_2.60
 */
namespace Align
{
/**
 * @brief Describes anchor point and parent origin of visuals
 * @SINCE_1_2.60
 */
enum Type
{
  TOP_BEGIN = 0, ///< @SINCE_1_2.60
  TOP_CENTER,    ///< @SINCE_1_2.60
  TOP_END,       ///< @SINCE_1_2.60
  CENTER_BEGIN,  ///< @SINCE_1_2.60
  CENTER,        ///< @SINCE_1_2.60
  CENTER_END,    ///< @SINCE_1_2.60
  BOTTOM_BEGIN,  ///< @SINCE_1_2.60
  BOTTOM_CENTER, ///< @SINCE_1_2.60
  BOTTOM_END     ///< @SINCE_1_2.60
};
} // namespace Align

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_ALIGN_ENUMERATIONS_H
