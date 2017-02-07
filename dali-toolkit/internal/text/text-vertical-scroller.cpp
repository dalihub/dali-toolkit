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

// CLASS HEADER
#include <dali-toolkit/internal/text/text-vertical-scroller.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

namespace
{

const float DEFAULT_VERTICAL_SCROLL_DURATION(0.15f);     ///< Duration to complete scroll animation

} // namespace

namespace Text
{

TextVerticalScrollerPtr TextVerticalScroller::New()
{
  TextVerticalScrollerPtr textScroller( new TextVerticalScroller() );
  return textScroller;
}

TextVerticalScroller::TextVerticalScroller()
: mDuration( DEFAULT_VERTICAL_SCROLL_DURATION ),
  mScrollTo( 0.0f )
{
}

TextVerticalScroller::~TextVerticalScroller()
{
}

void TextVerticalScroller::CheckStartAnimation( Actor& sourceActor, float x, float y, float scrollAmount )
{
  if ( Equals( scrollAmount, 0.0f, Math::MACHINE_EPSILON_1 ) )
  {
    // scroll animation isn't required, set position only
    if( mScrollAnimation && mScrollAnimation.GetState() == Animation::PLAYING )
    {
      mScrollAnimation.Clear();
    }
    sourceActor.SetPosition( x, y );
    return;
  }
  float toY = y + scrollAmount;
  // Either actor or scroll area is changed, so restart animation
  if( mScrollAnimation )
  {
    mScrollAnimation.Clear();
  }
  else
  {
    // Create animation at first
    mScrollAnimation = Animation::New( mDuration );
  }
  mScrollingActor = sourceActor;
  mScrollTo = toY;

  // Set animation attribute
  sourceActor.SetPosition( x, y );
  mScrollAnimation.AnimateTo( Property(sourceActor, Actor::Property::POSITION_Y), mScrollTo, AlphaFunction::EASE_OUT_SINE );
  mScrollAnimation.Play();
}

void TextVerticalScroller::SetDuration( float duration )
{
  mDuration = duration;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
