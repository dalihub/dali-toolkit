#ifndef DALI_DEVEL_TOOLKIT_PROPERTY_INDEX_RANGES_H
#define DALI_DEVEL_TOOLKIT_PROPERTY_INDEX_RANGES_H

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
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @brief Enumeration for the start and end property ranges.
 */
enum DevelPropertyRanges
{
  LAYOUT_GROUP_CHILD_PROPERTY_START_INDEX    = CHILD_PROPERTY_REGISTRATION_START_INDEX + 1000,   ///< Layout Group Property Start Index.
  LAYOUT_GROUP_CHILD_PROPERTY_END_INDEX      = LAYOUT_GROUP_CHILD_PROPERTY_START_INDEX + 999,    ///< Layout Group Property End Index.
  LINEAR_LAYOUT_CHILD_PROPERTY_START_INDEX   = LAYOUT_GROUP_CHILD_PROPERTY_END_INDEX + 1,        ///< Linear Layout Property Start Index.
  LINEAR_LAYOUT_CHILD_PROPERTY_END_INDEX     = LINEAR_LAYOUT_CHILD_PROPERTY_START_INDEX + 999,   ///< Linear Layout Property End Index.
  GRID_LAYOUT_CHILD_PROPERTY_START_INDEX     = LINEAR_LAYOUT_CHILD_PROPERTY_END_INDEX + 1,       ///< Grid Layout Property Start Index.
  GRID_LAYOUT_CHILD_PROPERTY_END_INDEX       = GRID_LAYOUT_CHILD_PROPERTY_START_INDEX + 999,     ///< Grid Layout Property End Index.
  FLEX_LAYOUT_CHILD_PROPERTY_START_INDEX     = GRID_LAYOUT_CHILD_PROPERTY_END_INDEX + 1,         ///< Flex Layout Property Start Index.
  FLEX_LAYOUT_CHILD_PROPERTY_END_INDEX       = FLEX_LAYOUT_CHILD_PROPERTY_START_INDEX + 999,     ///< Flex Layout Property End Index.
  ABSOLUTE_LAYOUT_CHILD_PROPERTY_START_INDEX = FLEX_LAYOUT_CHILD_PROPERTY_END_INDEX + 1,         ///< Absolute Layout Property Start Index.
  ABSOLUTE_LAYOUT_CHILD_PROPERTY_END_INDEX   = ABSOLUTE_LAYOUT_CHILD_PROPERTY_START_INDEX + 999, ///< Absolute Layout Property End Index.
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_DEVEL_TOOLKIT_PROPERTY_INDEX_RANGES_H
