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
#include <dali-toolkit/internal/text/text-model.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const char* DALI_ENV_MATCH_SYSTEM_LANGUAGE_DIRECTION("DALI_MATCH_SYSTEM_LANGUAGE_DIRECTION");
}

ModelPtr Model::New()
{
  return ModelPtr(new Model());
}

const Size& Model::GetControlSize() const
{
  return mVisualModel->mControlSize;
}

const Size& Model::GetLayoutSize() const
{
  return mVisualModel->GetLayoutSize();
}

const Vector2& Model::GetScrollPosition() const
{
  return mScrollPosition;
}

HorizontalAlignment::Type Model::GetHorizontalAlignment() const
{
  return mHorizontalAlignment;
}

VerticalAlignment::Type Model::GetVerticalAlignment() const
{
  return mVerticalAlignment;
}

DevelText::VerticalLineAlignment::Type Model::GetVerticalLineAlignment() const
{
  return mVerticalLineAlignment;
}

DevelText::EllipsisPosition::Type Model::GetEllipsisPosition() const
{
  return mEllipsisPosition;
}

bool Model::IsTextElideEnabled() const
{
  return mElideEnabled;
}

Length Model::GetNumberOfLines() const
{
  return mVisualModel->mLines.Count();
}

const LineRun* Model::GetLines() const
{
  return mVisualModel->mLines.Begin();
}

Length Model::GetNumberOfScripts() const
{
  return mLogicalModel->mScriptRuns.Count();
}

const ScriptRun* Model::GetScriptRuns() const
{
  return mLogicalModel->mScriptRuns.Begin();
}

Length Model::GetNumberOfGlyphs() const
{
  return mVisualModel->mGlyphs.Count();
}

GlyphIndex Model::GetStartIndexOfElidedGlyphs() const
{
  return mVisualModel->GetStartIndexOfElidedGlyphs();
}

GlyphIndex Model::GetEndIndexOfElidedGlyphs() const
{
  return mVisualModel->GetEndIndexOfElidedGlyphs();
}

GlyphIndex Model::GetFirstMiddleIndexOfElidedGlyphs() const
{
  return mVisualModel->GetFirstMiddleIndexOfElidedGlyphs();
}

GlyphIndex Model::GetSecondMiddleIndexOfElidedGlyphs() const
{
  return mVisualModel->GetSecondMiddleIndexOfElidedGlyphs();
}

const GlyphInfo* Model::GetGlyphs() const
{
  return mVisualModel->mGlyphs.Begin();
}

const Vector2* Model::GetLayout() const
{
  return mVisualModel->mGlyphPositions.Begin();
}

const Vector4* Model::GetColors() const
{
  return mVisualModel->mColors.Begin();
}

const ColorIndex* Model::GetColorIndices() const
{
  return mVisualModel->mColorIndices.Begin();
}

const Vector4* Model::GetBackgroundColors() const
{
  return mVisualModel->mBackgroundColors.Begin();
}

const ColorIndex* Model::GetBackgroundColorIndices() const
{
  return mVisualModel->mBackgroundColorIndices.Begin();
}

bool Model::IsMarkupBackgroundColorSet() const
{
  return (mVisualModel->mBackgroundColors.Count() > 0);
}

const Vector4& Model::GetDefaultColor() const
{
  return mVisualModel->mTextColor;
}

const Vector2& Model::GetShadowOffset() const
{
  return mVisualModel->mShadowOffset;
}

const Vector4& Model::GetShadowColor() const
{
  return mVisualModel->mShadowColor;
}

const float& Model::GetShadowBlurRadius() const
{
  return mVisualModel->mShadowBlurRadius;
}

const Vector4& Model::GetUnderlineColor() const
{
  return mVisualModel->GetUnderlineColor();
}

bool Model::IsUnderlineEnabled() const
{
  return mVisualModel->IsUnderlineEnabled();
}

bool Model::IsMarkupUnderlineSet() const
{
  return (mVisualModel->mUnderlineRuns.Count() > 0u);
}

float Model::GetUnderlineHeight() const
{
  return mVisualModel->GetUnderlineHeight();
}

Text::Underline::Type Model::GetUnderlineType() const
{
  return mVisualModel->GetUnderlineType();
}

float Model::GetDashedUnderlineWidth() const
{
  return mVisualModel->GetDashedUnderlineWidth();
}

float Model::GetDashedUnderlineGap() const
{
  return mVisualModel->GetDashedUnderlineGap();
}

Length Model::GetNumberOfUnderlineRuns() const
{
  return mVisualModel->GetNumberOfUnderlineRuns();
}

void Model::GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const
{
  mVisualModel->GetUnderlineRuns(underlineRuns, index, numberOfRuns);
}

const Vector4& Model::GetOutlineColor() const
{
  return mVisualModel->GetOutlineColor();
}

uint16_t Model::GetOutlineWidth() const
{
  return mVisualModel->GetOutlineWidth();
}

const Vector4& Model::GetBackgroundColor() const
{
  return mVisualModel->GetBackgroundColor();
}

bool Model::IsBackgroundEnabled() const
{
  return mVisualModel->IsBackgroundEnabled();
}

bool Model::IsMarkupProcessorEnabled() const
{
  return mVisualModel->IsMarkupProcessorEnabled();
}

bool Model::IsSpannedTextPlaced() const
{
  return mLogicalModel->mSpannedTextPlaced;
}

const GlyphInfo* Model::GetHyphens() const
{
  return mVisualModel->mHyphen.glyph.Begin();
}

const Length* Model::GetHyphenIndices() const
{
  return mVisualModel->mHyphen.index.Begin();
}

Length Model::GetHyphensCount() const
{
  return mVisualModel->mHyphen.glyph.Size();
}
const Vector4& Model::GetStrikethroughColor() const
{
  return mVisualModel->GetStrikethroughColor();
}

bool Model::IsStrikethroughEnabled() const
{
  return mVisualModel->IsStrikethroughEnabled();
}

bool Model::IsMarkupStrikethroughSet() const
{
  return (mVisualModel->mStrikethroughRuns.Count() > 0u);
}

float Model::GetStrikethroughHeight() const
{
  return mVisualModel->GetStrikethroughHeight();
}

Length Model::GetNumberOfStrikethroughRuns() const
{
  return mVisualModel->GetNumberOfStrikethroughRuns();
}

Length Model::GetNumberOfBoundedParagraphRuns() const
{
  return mLogicalModel->GetNumberOfBoundedParagraphRuns();
}

const Vector<BoundedParagraphRun>& Model::GetBoundedParagraphRuns() const
{
  return mLogicalModel->GetBoundedParagraphRuns();
}

void Model::GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index, Length numberOfRuns) const
{
  mVisualModel->GetStrikethroughRuns(strikethroughRuns, index, numberOfRuns);
}

Length Model::GetNumberOfCharacterSpacingGlyphRuns() const
{
  return mVisualModel->GetNumberOfCharacterSpacingGlyphRuns();
}

const Vector<CharacterSpacingGlyphRun>& Model::GetCharacterSpacingGlyphRuns() const
{
  return mVisualModel->GetCharacterSpacingGlyphRuns();
}

float Model::GetCharacterSpacing() const
{
  return mVisualModel->GetCharacterSpacing();
}

const Character* Model::GetTextBuffer() const
{
  return mLogicalModel->mText.Begin();
}

const Vector<CharacterIndex>& Model::GetGlyphsToCharacters() const
{
  return mVisualModel->mGlyphsToCharacters;
}

const Vector<FontRun>& Model::GetFontRuns() const
{
  return mLogicalModel->mFontRuns;
}

const Vector<FontDescriptionRun>& Model::GetFontDescriptionRuns() const
{
  return mLogicalModel->mFontDescriptionRuns;
}

Model::Model()
: mLogicalModel(),
  mVisualModel(),
  mScrollPosition(),
  mScrollPositionLast(),
  mHorizontalAlignment(Text::HorizontalAlignment::BEGIN),
  mVerticalAlignment(Text::VerticalAlignment::TOP),
  mVerticalLineAlignment(DevelText::VerticalLineAlignment::TOP),
  mLineWrapMode(Text::LineWrap::WORD),
  mAlignmentOffset(0.0f),
  mElideEnabled(false),
  mIgnoreSpacesAfterText(true),
  mMatchLayoutDirection(DevelText::MatchLayoutDirection::INHERIT),
  mEllipsisPosition(DevelText::EllipsisPosition::END)
{
  mLogicalModel = LogicalModel::New();
  mVisualModel  = VisualModel::New();

  // Check environment variable for DALI_MATCH_SYSTEM_LANGUAGE_DIRECTION
  auto match = Dali::EnvironmentVariable::GetEnvironmentVariable(DALI_ENV_MATCH_SYSTEM_LANGUAGE_DIRECTION);
  if(match && (std::atoi(match) == 0))
  {
    mMatchLayoutDirection = DevelText::MatchLayoutDirection::CONTENTS;
  }
}

Model::~Model()
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
