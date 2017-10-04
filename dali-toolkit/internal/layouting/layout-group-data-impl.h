#ifndef DALI_TOOLKIT_INTERNAL_LAYOUT_GROUP_DATA_IMPL_H_H
#define DALI_TOOLKIT_INTERNAL_LAYOUT_GROUP_DATA_IMPL_H_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <vector>

#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/internal/layouting/layout-base-data-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class LayoutGroup::Impl : public Internal::LayoutBase::Impl
{
public:
  Impl( LayoutGroup& layoutGroup );


public:
  /**
   * The leading padding in pixels, that is the distance in pixels between the
   * beginning edge of this view and the beginning edge of its content.
   */
  int mPaddingBegin;

  /**
   * The end padding in pixels, that is the distance in pixels between the
   * end edge of this view and the end edge of its content.
   */
  int mPaddingEnd;

  /**
   * The top padding in pixels, that is the distance in pixels between the
   * top edge of this view and the top edge of its content.
   */
  int mPaddingTop;

  /**
   * The bottom padding in pixels, that is the distance in pixels between the
   * bottom edge of this view and the bottom edge of its content.
   */
  int mPaddingBottom;

public:
  struct ChildLayout
  {
    LayoutBasePtr child;
    Toolkit::LayoutGroup::LayoutId layoutId;
  };

  std::vector<ChildLayout> mChildren;
  Toolkit::LayoutGroup::LayoutId mNextLayoutId;
};


} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUT_GROUP_DATA_IMPL_H_H
