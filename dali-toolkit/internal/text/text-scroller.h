#ifndef __DALI_TOOLKIT_TEXT_SCROLLER_H__
#define __DALI_TOOLKIT_TEXT_SCROLLER_H__

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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class TextScroller;
class ScrollerInterface;
struct ScrollerData;

typedef IntrusivePtr<TextScroller> TextScrollerPtr;

/**
 * @brief A helper class for scrolling text
 */
class TextScroller : public RefObject, public ConnectionTracker
{
public:
  /**
   * @brief Text Scrolling helper, used to automatically scroll text, SetParameters should be called before scrolling is needed.
   * CleanUp removes the Scrolling actors from stage whilst keeping the Scroller object alive and preserving Speed, Gap and Loop count.
   *
   * @param[in] scrollerInterface scroller interface
   */
  static TextScrollerPtr New( ScrollerInterface& scrollerInterface );

  /**
   * @brief Starts the text scrolling.
   *
   * @param[in] sourceActor source actor to be scrolled
   * @param[in] data Parameters needed to set up the text scrolling.
   *
   */
  void StartScrolling( Actor sourceActor,
                       const ScrollerData& data );

  /**
   * @brief Stops the text scrolling.
   */
  void StopScrolling();

  /**
   * @brief Get the camera used to look at source, should be added to the parent of target actor.
   * @return camera Actor
   */
  Actor GetSourceCamera() const;

  /**
   * @brief Get the resulting scrolling text actor, add to target actor which will show scrolling text
   * @return mesh Actor
   */
  Actor GetScrollingText() const;

private: // Implementation

  /**
   * Constructor
   */
  TextScroller( ScrollerInterface& scrollerInterface );

  /**
   * Destructor
   */
  ~TextScroller();

  // Undefined
  TextScroller( const TextScroller& handle );

  // Undefined
  TextScroller& operator=( const TextScroller& handle );

  /**
   * @brief Callback for end of animation
   * @param[in] animation Animation handle
   */
  void AutoScrollAnimationFinished( Dali::Animation& animation );

  /**
   * @brief When scrolling ended, the actors are cleaned up so no longer staged.
   */
  void CleanUp();

private:

  RenderTask         mRenderTask;               // Renders full text to a FrameBuffer which is then scrolled.
  CameraActor        mOffscreenCameraActor;     // Camera used by render task
  Actor              mScrollingTextActor;       // Actor used to show scrolling text
  ScrollerInterface& mScrollerInterface;        // Interface implemented by control that requires scrolling
  Property::Index    mScrollDeltaIndex;         // Property used by shader to represent distance to scroll
  Animation          mScrollAnimation;          // Animation used to update the mScrollDeltaIndex

}; // TextScroller class

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SCROLLER_H__

