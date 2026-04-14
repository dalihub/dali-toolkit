#ifndef DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H
#define DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-pair.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_item_view
 * @{
 */

typedef unsigned int ItemId; ///< Unique identity for each item in the view. @SINCE_1_0.0

typedef Dali::Vector<ItemId>           ItemIdContainer; ///< Item id container type @SINCE_1_0.0
typedef ItemIdContainer::Iterator      ItemIdIter;      ///< Item id iterator type @SINCE_1_0.0
typedef ItemIdContainer::ConstIterator ConstItemIdIter; ///< Item id const iterator type @SINCE_1_0.0

typedef Dali::Pair<ItemId, Actor> Item; ///< Item type @SINCE_1_0.0

typedef Dali::Vector<Item>           ItemContainer; ///< Item container type @SINCE_1_0.0
typedef ItemContainer::Iterator      ItemIter;      ///< Item iterator type @SINCE_1_0.0
typedef ItemContainer::ConstIterator ConstItemIter; ///< Item const iterator type @SINCE_1_0.0

class ItemView;
class ItemLayout;

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H
