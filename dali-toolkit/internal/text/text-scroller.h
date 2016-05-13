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
   * @brief Set parameters relating to source required for scrolling
   *
   * @param[in] sourceActor source actor to be scrolled
   * @param[in] controlSize size of the control to scroll within
   * @param[in] offScreenSize size of the sourceActor
   * @param[in] direction text direction true for right to left text
   * @param[in] alignmentOffset alignment of source text
   *
   */
  void SetParameters( Actor sourceActor, const Size& controlSize, const Size& offScreenSize, CharacterDirection direction, float alignmentOffset );

  /**
   * @brief Set the gap distance to elapse before the text wraps around
   * @param[in] gap distance to elapse
   */
  void SetGap( int gap );

  /**
   * @brief Get the distance before scrolling wraps
   * @return gap distance to elapse
   */
  int GetGap() const;

  /**
   * @brief Set speed the text should scroll
   * @param[in] scrollSpeed pixels per second
   */
  void SetSpeed( int scrollSpeed );

  /**
   * @brief Get the speed of text scrolling
   * @return speed in pixels per second
   */
  int GetSpeed() const;

  /**
   * @brief Set the number of times the text scrolling should loop, can stop current scrolling by passing in 0;
   * @param[in] loopCount number of times the scrolled text should loop, 0 to stop scrolling
   */
  void SetLoopCount( int loopCount );

  /**
   * @brief Get the number of loops
   * @return int number of loops
   */
  int GetLoopCount() const;

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
   * @brief variables required to set up scrolling animation
   * @param[in] scrollAmount distance to animate text for the given duration
   * @param[in] scrollDuration duration of aninmation
   * @param[in] loopCount number of times to loop the scrolling text
   */
  void StartScrolling( float scrollAmount, float scrollDuration, int loopCount );

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

  int mScrollSpeed;            ///< Speed which text should automatically scroll at
  int mLoopCount;              ///< Number of time the text should scroll
  int mWrapGap;                ///< Gap before text wraps around when scrolling

}; // TextScroller class

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SCROLLER_H__

