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
#include <dali-toolkit/internal/text/spannable/spans/font-span-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/spans/font-span.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
namespace
{
const float PIXEL_FORMAT_64_FACTOR = 64.f; ///< 64.f is used to convert from point size to 26.6 pixel format.
}

struct FontSpan::Impl
{
  std::string mFamilyName; ///< The font's family name.
  Dali::TextAbstraction::FontWeight::Type mWeight;     ///< The font's weight.
  Dali::TextAbstraction::FontWidth::Type  mWidth;      ///< The font's width.
  Dali::TextAbstraction::FontSlant::Type  mSlant;      ///< The font's slant.
  float       mSize;       ///< The font's size.

  bool mFamilyNameDefined : 1; ///< Whether the font's family is defined.
  bool mWeightDefined : 1;     ///< Whether the font's weight is defined.
  bool mWidthDefined : 1;      ///< Whether the font's width is defined.
  bool mSlantDefined : 1;      ///< Whether the font's slant is defined.
  bool mSizeDefined : 1;       ///< Whether the font's size is defined.
};

FontSpan::FontSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::FONT)
{
  mImpl = std::make_unique<Impl>();
}

FontSpan ::~FontSpan()
{
}

Dali::Toolkit::Text::FontSpan FontSpan::New(const std::string&                             familyName,
                                            const float&                                   sizeInPoints,
                                            const Dali::TextAbstraction::FontWeight::Type& weight,
                                            const Dali::TextAbstraction::FontWidth::Type&  width,
                                            const Dali::TextAbstraction::FontSlant::Type&  slant)
{
  FontSpanPtr object = new FontSpan();
  object->SetFamilyName(familyName);
  object->SetSize(sizeInPoints);
  object->SetWeight(weight);
  object->SetWidth(width);
  object->SetSlant(slant);

  Dali::Toolkit::Text::FontSpan handle = Dali::Toolkit::Text::FontSpan(object.Get());

  return handle;
}

//Methods
const std::string FontSpan::GetFamilyName() const
{
  return mImpl->mFamilyName;
}

bool FontSpan::IsFamilyNameDefined() const
{
  return mImpl->mFamilyNameDefined;
}

void FontSpan::SetFamilyName(const std::string& familyName)
{
  mImpl->mFamilyName        = familyName;
  mImpl->mFamilyNameDefined = true;
}

Dali::TextAbstraction::FontWeight::Type FontSpan::GetWeight() const
{
  return mImpl->mWeight;
}

bool FontSpan::IsWeightDefined() const
{
  return mImpl->mWeightDefined;
}

void FontSpan::SetWeight(const Dali::TextAbstraction::FontWeight::Type& weight)
{
  mImpl->mWeight        = weight;
  mImpl->mWeightDefined = true;
}

Dali::TextAbstraction::FontWidth::Type FontSpan::GetWidth() const
{
  return mImpl->mWidth;
}

bool FontSpan::IsWidthDefined() const
{
  return mImpl->mWidthDefined;
}

void FontSpan::SetWidth(const Dali::TextAbstraction::FontWidth::Type& width)
{
  mImpl->mWidth        = width;
  mImpl->mWidthDefined = true;
}

Dali::TextAbstraction::FontSlant::Type FontSpan::GetSlant() const
{
  return mImpl->mSlant;
}

bool FontSpan::IsSlantDefined() const
{
  return mImpl->mSlantDefined;
}

void FontSpan::SetSlant(const Dali::TextAbstraction::FontSlant::Type& slant)
{
  mImpl->mSlant        = slant;
  mImpl->mSlantDefined = true;
}

float FontSpan::GetSize() const
{
  return mImpl->mSize;
}

bool FontSpan::IsSizeDefined() const
{
  return mImpl->mSizeDefined;
}

void FontSpan::SetSize(const float& size)
{
  mImpl->mSize        = size;
  mImpl->mSizeDefined = true;
}

void FontSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  FontDescriptionRun fontRun;
  fontRun.characterRun.characterIndex     = range.GetStartIndex();
  fontRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();

  InitializeFontRun(fontRun);
  ProcessFontFamily(fontRun);
  ProcessFontSize(fontRun);
  ProcessFontWeight(fontRun);
  ProcessFontWidth(fontRun);
  ProcessFontSlant(fontRun);

  logicalModel->mFontDescriptionRuns.PushBack(fontRun);
}

void FontSpan::InitializeFontRun(FontDescriptionRun& fontRun) const
{
  fontRun.familyName   = NULL;
  fontRun.familyLength = 0u;
  fontRun.weight       = TextAbstraction::FontWeight::NORMAL;
  fontRun.width        = TextAbstraction::FontWidth::NORMAL;
  fontRun.slant        = TextAbstraction::FontSlant::NORMAL;
  fontRun.size         = 0u;

  fontRun.familyDefined = false;
  fontRun.weightDefined = false;
  fontRun.widthDefined  = false;
  fontRun.slantDefined  = false;
  fontRun.sizeDefined   = false;
}

void FontSpan::ProcessFontFamily(FontDescriptionRun& fontRun) const
{
  if(IsFamilyNameDefined())
  {
    const std::string& familyName = GetFamilyName();
    fontRun.familyDefined         = true;
    fontRun.familyLength          = familyName.length();
    fontRun.familyName            = new char[fontRun.familyLength];
    memcpy(fontRun.familyName, familyName.c_str(), fontRun.familyLength);
    // The memory is freed when the font run is removed from the logical model.
  }
}

void FontSpan::ProcessFontSize(FontDescriptionRun& fontRun) const
{
  if(IsSizeDefined())
  {
    // 64.f is used to convert from point size to 26.6 pixel format.
    fontRun.size        = static_cast<PointSize26Dot6>(GetSize() * PIXEL_FORMAT_64_FACTOR);
    fontRun.sizeDefined = true;
  }
}

void FontSpan::ProcessFontWeight(FontDescriptionRun& fontRun) const
{
  if(IsWeightDefined())
  {
    fontRun.weight        = GetWeight();
    fontRun.weightDefined = true;
  }
}

void FontSpan::ProcessFontWidth(FontDescriptionRun& fontRun) const
{
  if(IsWidthDefined())
  {
    fontRun.width        = GetWidth();
    fontRun.widthDefined = true;
  }
}

void FontSpan::ProcessFontSlant(FontDescriptionRun& fontRun) const
{
  if(IsSlantDefined())
  {
    fontRun.slant        = GetSlant();
    fontRun.slantDefined = true;
  }
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
