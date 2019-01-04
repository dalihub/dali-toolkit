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
#include <dali/public-api/object/weak-handle.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>
#include <dali-toolkit/internal/layouting/layout-transition-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class LayoutItem::Impl
{
public:
  Impl();

public:
  WeakHandle<Handle> mOwner; ///< Control or Visual that owns this layout. Weak pointer to prevent cyclic references
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

  static const int PRIVATE_FLAG_MEASURED_DIMENSION_SET        = 0x00000001;
  static const int PRIVATE_FLAG_FORCE_LAYOUT                  = 0x00000002;
  static const int PRIVATE_FLAG_LAYOUT_REQUIRED               = 0x00000004;
  static const int PRIVATE_FLAG_IS_LAID_OUT                   = 0x00000008;
  static const int PRIVATE_FLAG_MEASURE_NEEDED_BEFORE_LAYOUT  = 0x00000010; ///< Flag indicating that a call to measure() was skipped and should be done instead when layout() is invoked.
  static const int PRIVATE_FLAG_FORCE_SET_FRAME               = 0x00000020;
  static const int PRIVATE_FLAG_USE_RESIZE_POLICY             = 0x00000040;

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

  // Custom transitions
  LayoutTransitionDataPtr mOnChildAddTransitionData;
  LayoutTransitionDataPtr mOnChildRemoveTransitionData;
  LayoutTransitionDataPtr mOnChildFocusTransitionData;
  LayoutTransitionDataPtr mOnOwnerSetTransitionData;
  LayoutTransitionDataPtr mOnLayoutChangeTransitionData;
  // Default transition
  LayoutTransitionDataPtr mDefaultTransitionData;

  // To pass layout data during perform layout
  static LayoutData* sLayoutData;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_DATA_IMPL_H_H
