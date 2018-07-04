#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_CHILD_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_CHILD_H

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
class LayoutParent;

/**
 * Interface that allows a layout to determine its layout parent.
 *
 * This is useful for LayoutItem to determine it's parent, without accessing
 * via LayoutGroup, which is a subclass of LayoutItem (Super classes shouldn't
 * know / care about derived classes)
 */
class DALI_TOOLKIT_API LayoutChild
{
public:
  /**
   * Set the parent of this layout.
   */
  virtual void SetParent( LayoutParent* parent ) = 0;

  /**
   * Get the parent of this layout.
   */
  virtual LayoutParent* GetParent() = 0;

protected:
  virtual ~LayoutChild()
  {
  }
};


} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_CHILD_H
