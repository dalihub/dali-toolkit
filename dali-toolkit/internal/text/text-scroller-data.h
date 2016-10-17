#ifndef DALI_TOOLKIT_TEXT_SCROLLER_DATA_H
#define DALI_TOOLKIT_TEXT_SCROLLER_DATA_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

extern const int MINIMUM_SCROLL_SPEED; // Speed should be set by Property system.

/**
 * @brief Stores the autoscroll data.
 */
struct ScrollerData
{
  ScrollerData();
  ~ScrollerData();

  Size               mControlSize;            ///< The size of the control to scroll within.
  Size               mOffscreenSize;          ///< The size of the sourceActor.
  int                mScrollSpeed;            ///< Speed which text should automatically scroll at.
  int                mLoopCount;              ///< Number of time the text should scroll.
  float              mWrapGap;                ///< Gap before text wraps around when scrolling.
  float              mAlignmentOffset;        ///< The alignment offset of the first line of text to be scrolled.
  CharacterDirection mAutoScrollDirectionRTL; ///< Direction of auto scrolling, true if rtl.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SCROLLER_H

