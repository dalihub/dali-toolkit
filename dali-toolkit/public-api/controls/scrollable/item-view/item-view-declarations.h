#ifndef __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__
#define __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <utility> // std::pair
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_item_view
 * @{
 */

typedef unsigned int ItemId; ///< Unique identity for each item in the view. @SINCE_1_0.0

typedef std::vector<ItemId> ItemIdContainer;  ///< Item id container type @SINCE_1_0.0
typedef ItemIdContainer::iterator ItemIdIter; ///< Item id iterator type @SINCE_1_0.0
typedef ItemIdContainer::const_iterator ConstItemIdIter;  ///< Item id const iterator type @SINCE_1_0.0

typedef std::pair<ItemId, Actor> Item;  ///< Item type @SINCE_1_0.0

typedef std::vector<Item> ItemContainer;  ///< Item container type @SINCE_1_0.0
typedef ItemContainer::iterator ItemIter; ///< Item iterator type @SINCE_1_0.0
typedef ItemContainer::const_iterator ConstItemIter;  ///< Item const iterator type @SINCE_1_0.0

class ItemView;
class ItemLayout;

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__
