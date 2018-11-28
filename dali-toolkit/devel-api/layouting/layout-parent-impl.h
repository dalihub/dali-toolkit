#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_PARENT_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_PARENT_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class LayoutItem;

/**
 * Interface that defines a layout Parent. Enables a layout child to access
 * methods on its parent, e.g. Remove (during unparenting)
 */
class DALI_TOOLKIT_API LayoutParent
{
public:
  /**
   * @brief Add a child to the parent
   * @param[in] item The item to add to this layout parent
   */
  virtual Toolkit::LayoutGroup::LayoutId Add( LayoutItem& item ) = 0;

  /**
   * @brief Remove a layout child from this group.
   * @param[in] childId The layout child id
   */
  virtual void Remove( Toolkit::LayoutGroup::LayoutId childId ) = 0;

  /**
   * @brief Remove a child from this parent
   * @param[in] item The item to remove from this layout parent
   */
  virtual void Remove( LayoutItem& item ) = 0;

  /**
   * @brief Insert a child to the parent
   * @param[in] target The target item
   * @param[in] item The item to insert to this layout parent
   */
  virtual Toolkit::LayoutGroup::LayoutId Insert( LayoutItem& target, LayoutItem& item ) = 0;


protected:
  virtual ~LayoutParent()
  {
  }
};



} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_PARENT_H
