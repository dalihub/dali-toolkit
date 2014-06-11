#ifndef __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__
#define __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_TOOLKIT_ITEM_VIEW_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <utility> // std::pair
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

typedef unsigned int ItemId; ///< Unique identity for each item in the view.

typedef std::vector<ItemId> ItemIdContainer;
typedef ItemIdContainer::iterator ItemIdIter;
typedef ItemIdContainer::const_iterator ConstItemIdIter;

typedef std::pair<ItemId, Actor> Item;

typedef std::vector<Item> ItemContainer;
typedef ItemContainer::iterator ItemIter;
typedef ItemContainer::const_iterator ConstItemIter;

class ItemView;
class ItemLayout;

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_ITEM_VIEW_DECLARATIONS_H__
