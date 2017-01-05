#ifndef DALI_TOOLKIT_TEXT_VERTICAL_SCROLLER_H
#define DALI_TOOLKIT_TEXT_VERTICAL_SCROLLER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class TextVerticalScroller;

typedef IntrusivePtr<TextVerticalScroller> TextVerticalScrollerPtr;

/**
 * @brief A helper class for scrolling text vertically
 */
class TextVerticalScroller : public RefObject, public ConnectionTracker
{
public:

  /**
   * @brief Text Scrolling helper, used to automatically scroll text, StartScroll should be called when scrolling is needed.
   *
   */
  static TextVerticalScrollerPtr New();

  /**
   * @brief variables required to set up scrolling animation
   * @param[in] sourceActor actor to be animated
   * @param[in] x The new x position
   * @param[in] y The new y position
   * @param[in] scrollAmount The distance to destination y position for actor to be animated
   */
  void CheckStartAnimation( Actor& sourceActor, float x, float y, float scrollAmount );

  /**
   * @brief Set duration the text should scroll
   * @param[in] duration The duration in seconds
   */
  void SetDuration( float duration );

private: // Implementation
  /**
   * Constructor
   */
  TextVerticalScroller();

  /**
   * @brief Virtual Destructor.
   */
  virtual ~TextVerticalScroller();

private:

  Animation          mScrollAnimation;          // Animation used to update the actor's position
  Actor              mScrollingActor;           // The actor being animated
  float              mDuration;                 // The duration of text scrolling
  float              mScrollTo;                 // The destination y position
}; // TextVerticalScroller class

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VERTICAL_SCROLLER_H
