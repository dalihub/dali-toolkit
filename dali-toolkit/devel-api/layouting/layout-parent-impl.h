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

/**
 * Interface that allows a layout to determine its layout parent.
 *
 * Needed to prevent circular inheritance - most LayoutBases have a parent,
 * but parenting is provided by LayoutGroup, which is a sub-class of LayoutBase.
 */
class DALI_TOOLKIT_API LayoutParent
{
public:
  /**
   * Get the parent of this layout.
   */
  virtual LayoutParent* GetParent() = 0;

protected:
  virtual ~LayoutParent()
  {
  }
};


} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_PARENT_H
