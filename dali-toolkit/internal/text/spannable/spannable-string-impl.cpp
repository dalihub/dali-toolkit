/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/spannable/spannable-string-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
struct SpannableString::Impl
{
  std::string       mText;       ///< The text (utf8).
  Vector<Character> mCharacters; ///< The container of characters (utf32).
  SpanRangesContainer mSpansContainer; ///< The container of spans and ranges
};

SpannableString::SpannableString(const std::string& text)
{
  mImpl                   = std::make_unique<Impl>();
  mImpl->mText            = text;
  Length         textSize = 0u;
  const uint8_t* utf8     = NULL;

  textSize = text.size();
  utf8     = reinterpret_cast<const uint8_t*>(text.c_str());

  //  Convert text into UTF-32
  Vector<Character>& utf32Characters = mImpl->mCharacters;
  utf32Characters.Resize(textSize);

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  Length characterCount = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
  utf32Characters.Resize(characterCount);

  DALI_ASSERT_DEBUG(textSize >= characterCount && "Invalid UTF32 conversion length");
}

SpannableString::~SpannableString()
{
}

Dali::Toolkit::Text::SpannableString SpannableString::New(const std::string& text)
{
  SpannableStringPtr object = new SpannableString(text);

  Dali::Toolkit::Text::SpannableString handle = Dali::Toolkit::Text::SpannableString(object.Get());

  return handle;
}

//Methods from CharacterSequence

Vector<uint32_t> SpannableString::GetCharacters() const
{
  return mImpl->mCharacters;
}

uint32_t SpannableString::GetNumberOfCharacters() const
{
  return mImpl->mCharacters.Count();
}

std::string SpannableString::ToString() const
{
  return mImpl->mText;
}

//Methods from Spannable
bool SpannableString::AttachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan, const Dali::Toolkit::Text::Range& range)
{
  if(range.GetStartIndex() < GetNumberOfCharacters() && range.GetEndIndex() < GetNumberOfCharacters())
  {
    mImpl->mSpansContainer.AddSpan(styleSpan, range);
    return true;
  }

  return false;
}

bool SpannableString::DetachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan)
{
  if(mImpl->mSpansContainer.Contains(styleSpan))
  {
    mImpl->mSpansContainer.RemoveSpan(styleSpan);

    return true;
  }

  return false;
}

//Methods from Spanned

std::vector<Dali::Toolkit::Text::BaseSpan> SpannableString::GetAllSpans() const
{
  std::vector<Dali::Toolkit::Text::BaseSpan> listOfSpans;
  mImpl->mSpansContainer.GetSpans(listOfSpans);
  return listOfSpans;
}

void SpannableString::RetrieveAllSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const
{
  mImpl->mSpansContainer.GetSpansAndRanges(spans, ranges);
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
