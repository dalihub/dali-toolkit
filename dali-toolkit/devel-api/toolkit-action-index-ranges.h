#ifndef DALI_TOOLKIT_ACTION_INDEX_RANGES_H
#define DALI_TOOLKIT_ACTION_INDEX_RANGES_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

/**
 * @brief Enumeration for the start and end action ranges.
 */
enum ActionRanges
{
  VISUAL_ACTION_BASE_START_INDEX = 0,                                    ///< Visual Action Base Start Index.
  VISUAL_ACTION_BASE_END_INDEX   = VISUAL_ACTION_BASE_START_INDEX + 100, ///< Visual Action Base End Index.
  VISUAL_ACTION_START_INDEX      = VISUAL_ACTION_BASE_END_INDEX + 1,     ///< Visual Action Start Index.
  VISUAL_ACTION_END_INDEX        = VISUAL_ACTION_START_INDEX + 100000,   ///< Visual Action End Index.
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ACTION_INDEX_RANGES_H
