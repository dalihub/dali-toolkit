#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H

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

#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class LayoutParent;


class LayoutItem::Impl
{
public:
  Impl();

public:
  BaseObject* mOwner; ///< Control or Visual that owns this layout. Raw pointer to prevent cyclic references
  LayoutParent* mLayoutParent; ///< The containing layout parent.

  MeasureSpec mOldWidthMeasureSpec;
  MeasureSpec mOldHeightMeasureSpec;
  LayoutSize mMinimumSize;

  MeasuredSize mMeasuredWidth;
  MeasuredSize mMeasuredHeight;

  LayoutLength mLeft;
  LayoutLength mRight;
  LayoutLength mTop;
  LayoutLength mBottom;

  /**
   * This view is visible.
   * Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
   * android:visibility}.
   */
  static const int VISIBLE = 0x00000000;

  /**
   * This view is invisible, but it still takes up space for layout purposes.
   * Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
   * android:visibility}.
   */
  static const int INVISIBLE = 0x00000004;

  /**
   * This view is invisible, and it doesn't take any space for layout
   * purposes. Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
   * android:visibility}.
   */
  static const int GONE = 0x00000008;

  /**
   * Mask for use with setFlags indicating bits used for visibility.
   * {@hide}
   */
  static const int VISIBILITY_MASK = 0x0000000C;

  static const int PRIVATE_FLAG_MEASURED_DIMENSION_SET        = 0x00000001;
  static const int PRIVATE_FLAG_FORCE_LAYOUT                  = 0x00000002;
  static const int PRIVATE_FLAG_LAYOUT_REQUIRED               = 0x00000004;
  static const int PRIVATE_FLAG_IS_LAID_OUT                   = 0x00000008;
  static const int PRIVATE_FLAG_MEASURE_NEEDED_BEFORE_LAYOUT  = 0x00000010; ///< Flag indicating that a call to measure() was skipped and should be done instead when layout() is invoked.
  static const int PRIVATE_FLAG_HAS_BOUNDS                    = 0x00000020;

  int mViewFlags;
  int mPrivateFlags;

  bool mAnimated;

  inline void ClearPrivateFlag( int flag )
  {
    mPrivateFlags &= ~flag;
  }

  inline void SetPrivateFlag( int flag )
  {
    mPrivateFlags |= flag;
  }

  inline bool GetPrivateFlag( int flag )
  {
    return ( mPrivateFlags & flag ) != 0;
  }

  static bool sUseZeroUnspecifiedMeasureSpec;

};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H
