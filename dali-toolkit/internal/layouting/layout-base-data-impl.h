#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H

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

#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/internal/layouting/child-layout-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class LayoutParent;


class LayoutBase::Impl
{
public:
  Impl( LayoutBase& layoutBase );

public:
  Toolkit::LayoutController mLayoutController;

  BaseObject* mOwner; ///< Control or Visual that owns this layout. Raw pointer to avoid cyclic references
  LayoutParent* mLayoutParent; ///< The containing layout parent.
  ChildLayoutDataPtr mLayoutData; ///< The layout data for this object ( within it's parent layouter )

  MeasureSpec mOldWidthMeasureSpec;
  MeasureSpec mOldHeightMeasureSpec;
  Uint16Pair mMinimumSize;
  Uint16Pair mMeasuredSize;

  Extents mMargin; ///< Distances in pixels from the edges of this view to this view's parent.
  Extents mPadding; ///< Distances in pixels from the edges of this view to this view's content.


  uint16_t mLeft;
  uint16_t mRight;
  uint16_t mTop;
  uint16_t mBottom;

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

  //static final int[] VISIBILITY_FLAGS = {VISIBLE, INVISIBLE, GONE};

  static const int PFLAG_MEASURED_DIMENSION_SET        = 0x00000001;
  static const int PFLAG_FORCE_LAYOUT                  = 0x00000002;
  static const int PFLAG_LAYOUT_REQUIRED               = 0x00000004;
  static const int PFLAG_IS_LAID_OUT                   = 0x00000008;
  static const int PFLAG_MEASURE_NEEDED_BEFORE_LAYOUT  = 0x00000010; ///< Flag indicating that a call to measure() was skipped and should be done instead when layout() is invoked.
  static const int PFLAG_HAS_BOUNDS                    = 0x00000020;

  int mViewFlags;
  int mPrivateFlags;

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
