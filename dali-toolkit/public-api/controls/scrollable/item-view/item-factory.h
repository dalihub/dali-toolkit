#ifndef __DALI_TOOLKIT_ITEM_FACTORY_H__
#define __DALI_TOOLKIT_ITEM_FACTORY_H__

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
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Toolkit
{
/**
 * @addtogroup dali-toolkit-controls-item-view
 * @{
 */

/**
 * @brief ItemFactory is for providing actors to ItemView.
 * Each actor is identified by a unique ID, and has a linear order from 0 to GetNumberOfItems()-1.
 */
class ItemFactory
{
public:

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Virtual destructor.
   */
  DALI_EXPORT_API virtual ~ItemFactory() {};

  /**
   * @brief Query the number of items available from the factory.
   *
   * The maximum available item has an ID of GetNumberOfItems() - 1.
   * @return the number of items
   */
  virtual unsigned int GetNumberOfItems() = 0;

  /**
   * @brief Create an Actor to represent a visible item.
   *
   * @param[in] itemId The ID of the newly visible item.
   * @return An actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewItem(unsigned int itemId) = 0;

  /**
   * @brief Notify the factory the actor representing the item is removed from ItemView.
   *
   * @param[in] itemId The ID of the released item.
   * @param[in] actor The actor that represents the released item.
   */
  virtual void ItemReleased(unsigned int itemId, Actor actor) {};

  /**
   * Retrieve the extension for this control
   *
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetExtension()
  {
    return NULL;
  }
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ITEM_FACTORY_H__
