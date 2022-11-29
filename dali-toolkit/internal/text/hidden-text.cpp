/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/hidden-text.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

using namespace Dali::Toolkit;

const int      DEFAULT_SHOW_DURATION = 1000;

namespace Dali
{
namespace Toolkit
{
namespace Text
{
const char* const PROPERTY_MODE                 = "mode";
const char* const PROPERTY_SUBSTITUTE_CHARACTER = "substituteCharacter";
const char* const PROPERTY_SUBSTITUTE_COUNT     = "substituteCount";
const char* const PROPERTY_SHOW_DURATION        = "showDuration";

HiddenText::HiddenText(Observer* observer)
: mObserver(observer),
  mHideMode(static_cast<int>(Toolkit::HiddenInput::Mode::HIDE_NONE)),
  mSubstituteText(STAR),
  mDisplayDuration(DEFAULT_SHOW_DURATION),
  mSubstituteCount(0),
  mPreviousTextCount(0u),
  mIsLastCharacterShow(false)
{
  mTimer = Timer::New(mDisplayDuration);
  mTimer.TickSignal().Connect(this, &HiddenText::OnTick);
}

void HiddenText::SetProperties(const Property::Map& map)
{
  const Property::Map::SizeType count = map.Count();

  for(Property::Map::SizeType position = 0; position < count; ++position)
  {
    KeyValuePair     keyValue = map.GetKeyValue(position);
    Property::Key&   key      = keyValue.first;
    Property::Value& value    = keyValue.second;

    if(key == Toolkit::HiddenInput::Property::MODE || key == PROPERTY_MODE)
    {
      value.Get(mHideMode);
    }
    else if(key == Toolkit::HiddenInput::Property::SUBSTITUTE_CHARACTER || key == PROPERTY_SUBSTITUTE_CHARACTER)
    {
      value.Get(mSubstituteText);
    }
    else if(key == Toolkit::HiddenInput::Property::SUBSTITUTE_COUNT || key == PROPERTY_SUBSTITUTE_COUNT)
    {
      value.Get(mSubstituteCount);
    }
    else if(key == Toolkit::HiddenInput::Property::SHOW_LAST_CHARACTER_DURATION || key == PROPERTY_SHOW_DURATION)
    {
      value.Get(mDisplayDuration);
    }
  }
}

void HiddenText::GetProperties(Property::Map& map)
{
  map[Toolkit::HiddenInput::Property::MODE]                         = mHideMode;
  map[Toolkit::HiddenInput::Property::SUBSTITUTE_CHARACTER]         = mSubstituteText;
  map[Toolkit::HiddenInput::Property::SUBSTITUTE_COUNT]             = mSubstituteCount;
  map[Toolkit::HiddenInput::Property::SHOW_LAST_CHARACTER_DURATION] = mDisplayDuration;
}

void HiddenText::Substitute(const Vector<Character>& source, Vector<Character>& destination, Length cursorPos)
{
  const Length characterCount = source.Count();

  destination.Resize(characterCount);

  uint32_t* begin     = destination.Begin();
  uint32_t* end       = begin + characterCount;
  uint32_t* hideStart = NULL;
  uint32_t* hideEnd   = NULL;
  uint32_t* sourcePos = source.Begin();

  switch(mHideMode)
  {
    case Toolkit::HiddenInput::Mode::HIDE_NONE:
    {
      hideStart = NULL;
      hideEnd   = NULL;
      break;
    }
    case Toolkit::HiddenInput::Mode::HIDE_ALL:
    {
      hideStart = begin;
      hideEnd   = end;
      break;
    }
    case Toolkit::HiddenInput::Mode::HIDE_COUNT:
    {
      hideStart = begin;
      hideEnd   = begin + mSubstituteCount;
      break;
    }
    case Toolkit::HiddenInput::Mode::SHOW_COUNT:
    {
      hideStart = begin + mSubstituteCount;
      hideEnd   = end;
      break;
    }
    case Toolkit::HiddenInput::Mode::SHOW_LAST_CHARACTER:
    {
      hideStart = begin;
      hideEnd   = end;
      if(mPreviousTextCount < characterCount)
      {
        if(mDisplayDuration > 0)
        {
          mTimer.SetInterval(mDisplayDuration);
          mTimer.Start();
          mIsLastCharacterShow = true;
        }
        else
        {
          OnTick();
        }
      }
      else
      {
        mIsLastCharacterShow = false;
      }
      break;
    }
  }


  if(mHideMode == Toolkit::HiddenInput::Mode::SHOW_LAST_CHARACTER)
  {
    Length currentPos = 0u;
    for(; begin < end; ++begin)
    {
      if(begin >= hideStart && begin < hideEnd && cursorPos > 0u && currentPos != cursorPos - 1u)
      {
        *begin = static_cast<uint32_t>(mSubstituteText);
      }
      else
      {
        *begin = mIsLastCharacterShow ? *sourcePos : static_cast<uint32_t>(mSubstituteText);
      }
      sourcePos++;
      currentPos++;
    }
  }
  else
  {
    for(; begin < end; ++begin)
    {
      if(begin >= hideStart && begin < hideEnd)
      {
        *begin = static_cast<uint32_t>(mSubstituteText);
        sourcePos++;
      }
      else
      {
        *begin = *sourcePos++;
      }
    }
  }
  mPreviousTextCount = characterCount;
}

void HiddenText::InitPreviousTextCount()
{
  mPreviousTextCount = 0u;
}

int HiddenText::GetHideMode()
{
  return mHideMode;
}

bool HiddenText::OnTick()
{
  if(mObserver != NULL)
  {
    mObserver->DisplayTimeExpired();
  }
  mIsLastCharacterShow = false;
  return false;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
