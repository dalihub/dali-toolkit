/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-scroller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/text/text-scroller-interface.h>

namespace Dali
{
namespace Toolkit
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_SCROLLING");
#endif

const int MINIMUM_SCROLL_SPEED = 1; // Speed should be set by Property system.

/**
 * @brief How the text should be aligned horizontally when scrolling the text.
 *
 * -0.5f aligns the text to the left, 0.0f aligns the text to the center, 0.5f aligns the text to the right.
 * The final alignment depends on two factors:
 *   1) The alignment value of the text label (Use Text::HorizontalAlignment enumerations).
 *   2) The text direction, i.e. whether it's LTR or RTL (0 = LTR, 1 = RTL).
 */
const float HORIZONTAL_ALIGNMENT_TABLE[Text::HorizontalAlignment::END + 1][2] =
  {
    // HorizontalAlignment::BEGIN
    {
      -0.5f, // LTR
      0.5f   // RTL
    },

    // HorizontalAlignment::CENTER
    {
      0.0f, // LTR
      0.0f  // RTL
    },

    // HorizontalAlignment::END
    {
      0.5f, // LTR
      -0.5f // RTL
    }};

/**
 * @brief How the text should be aligned vertically when scrolling the text.
 *
 * -0.5f aligns the text to the top, 0.0f aligns the text to the center, 0.5f aligns the text to the bottom.
 * The alignment depends on the alignment value of the text label (Use Text::VerticalAlignment enumerations).
 */
const float VERTICAL_ALIGNMENT_TABLE[Text::VerticalAlignment::BOTTOM + 1] =
  {
    -0.5f, // VerticalAlignment::TOP
    0.0f,  // VerticalAlignment::CENTER
    0.5f   // VerticalAlignment::BOTTOM
};

} // namespace

namespace Text
{
TextScrollerPtr TextScroller::New(ScrollerInterface& scrollerInterface)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::New\n");

  TextScrollerPtr textScroller(new TextScroller(scrollerInterface));
  return textScroller;
}

void TextScroller::SetGap(int gap)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetGap gap[%d]\n", gap);
  mWrapGap = static_cast<float>(gap);
}

int TextScroller::GetGap() const
{
  return static_cast<int>(mWrapGap);
}

void TextScroller::SetSpeed(int scrollSpeed)
{
  mScrollSpeed = std::max(MINIMUM_SCROLL_SPEED, scrollSpeed);
}

int TextScroller::GetSpeed() const
{
  return mScrollSpeed;
}

void TextScroller::SetLoopCount(int loopCount)
{
  if(loopCount >= 0)
  {
    mLoopCount = loopCount;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetLoopCount [%d] Status[%s]\n", mLoopCount, (loopCount) ? "looping" : "stop");
}

int TextScroller::GetLoopCount() const
{
  return mLoopCount;
}

void TextScroller::SetLoopDelay(float delay)
{
  mLoopDelay = delay;
}

float TextScroller::GetLoopDelay() const
{
  return mLoopDelay;
}

void TextScroller::SetStopMode(TextLabel::AutoScrollStopMode::Type stopMode)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetAutoScrollStopMode [%s]\n", (stopMode == TextLabel::AutoScrollStopMode::IMMEDIATE) ? "IMMEDIATE" : "FINISH_LOOP");
  mStopMode = stopMode;
}

void TextScroller::StopScrolling()
{
  if(IsScrolling())
  {
    switch(mStopMode)
    {
      case TextLabel::AutoScrollStopMode::IMMEDIATE:
      {
        mIsStop = true;
        mScrollAnimation.Stop();
        break;
      }
      case TextLabel::AutoScrollStopMode::FINISH_LOOP:
      {
        mIsStop = true;
        mScrollAnimation.SetLoopCount(1); // As animation already playing this allows the current animation to finish instead of trying to stop mid-way
        break;
      }
      default:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Undifined AutoScrollStopMode\n");
      }
    }
  }
  else
  {
    mScrollerInterface.ScrollingFinished();
  }
}

bool TextScroller::IsStop()
{
  return mIsStop;
}

bool TextScroller::IsScrolling()
{
  return (mScrollAnimation && mScrollAnimation.GetState() == Animation::PLAYING);
}

TextLabel::AutoScrollStopMode::Type TextScroller::GetStopMode() const
{
  return mStopMode;
}

TextScroller::TextScroller(ScrollerInterface& scrollerInterface)
: mScrollerInterface(scrollerInterface),
  mScrollDeltaIndex(Property::INVALID_INDEX),
  mScrollSpeed(MINIMUM_SCROLL_SPEED),
  mLoopCount(1),
  mLoopDelay(0.0f),
  mWrapGap(0.0f),
  mStopMode(TextLabel::AutoScrollStopMode::FINISH_LOOP),
  mIsStop(false)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller Default Constructor\n");
}

TextScroller::~TextScroller()
{
}

void TextScroller::SetParameters(Actor scrollingTextActor, Renderer renderer, TextureSet textureSet, const Size& controlSize, const Size& textureSize, const float wrapGap, CharacterDirection direction, HorizontalAlignment::Type horizontalAlignment, VerticalAlignment::Type verticalAlignment)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters controlSize[%f,%f] textureSize[%f,%f] direction[%d]\n", controlSize.x, controlSize.y, textureSize.x, textureSize.y, direction);
  mRenderer = renderer;

  float animationProgress = 0.0f;
  int   remainedLoop      = mLoopCount;
  if(mScrollAnimation)
  {
    if(mScrollAnimation.GetState() == Animation::PLAYING)
    {
      animationProgress = mScrollAnimation.GetCurrentProgress();

      if(mLoopCount > 0) // If not a ininity loop, then calculate remained loop
      {
        remainedLoop = mLoopCount - (mScrollAnimation.GetCurrentLoop());
        remainedLoop = mIsStop ? 1 : (remainedLoop <= 0 ? 1 : remainedLoop);
      }
    }
    mScrollAnimation.Clear();

    // Reset to the original shader and texture before scrolling
    mRenderer.SetShader(mShader);
    if(mTextureSet)
    {
      mRenderer.SetTextures(mTextureSet);
    }
  }

  mShader     = mRenderer.GetShader();
  mTextureSet = mRenderer.GetTextures();

  // Set the shader and texture for scrolling
  Shader shader = Shader::New(SHADER_TEXT_SCROLLER_SHADER_VERT, SHADER_TEXT_SCROLLER_SHADER_FRAG, Shader::Hint::NONE, "TEXT_SCROLLER");
  mRenderer.SetShader(shader);
  mRenderer.SetTextures(textureSet);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters wrapGap[%f]\n", wrapGap);

  float horizontalAlign;

  if(textureSize.x > controlSize.x)
  {
    // if Text is elided, scroll should start at the begin of text.
    horizontalAlign = HORIZONTAL_ALIGNMENT_TABLE[HorizontalAlignment::BEGIN][direction];
  }
  else
  {
    horizontalAlign = HORIZONTAL_ALIGNMENT_TABLE[horizontalAlignment][direction];
  }

  const float verticalAlign = VERTICAL_ALIGNMENT_TABLE[verticalAlignment];

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters horizontalAlign[%f], verticalAlign[%f]\n", horizontalAlign, verticalAlign);

  shader.RegisterProperty("uTextureSize", textureSize);
  shader.RegisterProperty("uHorizontalAlign", horizontalAlign);
  shader.RegisterProperty("uVerticalAlign", verticalAlign);
  shader.RegisterProperty("uGap", wrapGap);
  mScrollDeltaIndex = shader.RegisterProperty("uDelta", 0.0f);

  float scrollAmount   = std::max(textureSize.width, controlSize.width);
  float scrollDuration = scrollAmount / mScrollSpeed;

  if(direction)
  {
    scrollAmount = -scrollAmount; // reverse direction of scrolling
  }

  StartScrolling(scrollingTextActor, scrollAmount, scrollDuration, remainedLoop);
  mScrollAnimation.SetCurrentProgress(animationProgress);
}

void TextScroller::AutoScrollAnimationFinished(Dali::Animation& animation)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::AutoScrollAnimationFinished\n");
  mIsStop = false;
  mScrollerInterface.ScrollingFinished();

  // Revert to the original shader and texture after scrolling
  mRenderer.SetShader(mShader);
  if(mTextureSet)
  {
    mRenderer.SetTextures(mTextureSet);
  }
}

void TextScroller::StartScrolling(Actor scrollingTextActor, float scrollAmount, float scrollDuration, int loopCount)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::StartScrolling scrollAmount[%f] scrollDuration[%f], loop[%d] speed[%d]\n", scrollAmount, scrollDuration, loopCount, mScrollSpeed);
  Shader shader    = mRenderer.GetShader();
  mScrollAnimation = Animation::New(scrollDuration);
  mScrollAnimation.AnimateTo(Property(shader, mScrollDeltaIndex), scrollAmount, TimePeriod(mLoopDelay, scrollDuration));
  mScrollAnimation.SetEndAction(Animation::DISCARD);
  mScrollAnimation.SetLoopCount(loopCount);
  mScrollAnimation.FinishedSignal().Connect(this, &TextScroller::AutoScrollAnimationFinished);
  mScrollAnimation.Play();

  mIsStop = false;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
