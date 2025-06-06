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
#include <dali-toolkit/internal/text/visual-model-impl.h>

// EXTERNAL INCLUDES
#include <memory.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
VisualModelPtr VisualModel::New()
{
  return VisualModelPtr(new VisualModel());
}

void VisualModel::CreateCharacterToGlyphTable(CharacterIndex startIndex,
                                              GlyphIndex     startGlyphIndex,
                                              Length         numberOfCharacters)
{
  if(0u == numberOfCharacters)
  {
    // Nothing to do.
    return;
  }

  DALI_ASSERT_DEBUG(mGlyphsPerCharacter.Count() != 0u);

  // Get the total number of characters.
  const Length totalNumberOfCharacters = (0u == mGlyphsToCharacters.Count()) ? 0u : *(mGlyphsToCharacters.End() - 1u) + *(mCharactersPerGlyph.End() - 1u); // Index to the first character + the number of characters that form the last glyph.

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  Vector<GlyphIndex> newCharactersToGlyph;
  GlyphIndex*        charactersToGlyphBuffer = NULL;

  // 1) Reserve some space for the glyph indices to avoid reallocations.
  if(updateCurrentBuffer)
  {
    newCharactersToGlyph.Resize(numberOfCharacters);
    charactersToGlyphBuffer = newCharactersToGlyph.Begin();
  }
  else
  {
    mCharactersToGlyph.Resize(startIndex + numberOfCharacters);
    charactersToGlyphBuffer = mCharactersToGlyph.Begin() + startIndex;
  }

  const Length* const glyphsPerCharacterBuffer = mGlyphsPerCharacter.Begin();

  // 2) Traverse the glyphs and set the glyph indices per character.

  // Index to the glyph.
  GlyphIndex           glyphIndex                = startGlyphIndex;
  CharacterIndex       characterIndex            = startIndex;
  const CharacterIndex lastCharacterIndexPlusOne = startIndex + numberOfCharacters;
  for(Vector<Length>::ConstIterator it    = mCharactersPerGlyph.Begin() + glyphIndex,
                                    endIt = mCharactersPerGlyph.End();
      (it != endIt) && (characterIndex < lastCharacterIndexPlusOne);
      ++it)
  {
    const Length numberOfCharactersPerGlyph = *it;

    Length numberOfGlyphs = 0u;
    // Set the glyph indices.
    for(Length index = 0u; index < numberOfCharactersPerGlyph; ++index, ++characterIndex)
    {
      *charactersToGlyphBuffer = glyphIndex;
      numberOfGlyphs += *(glyphsPerCharacterBuffer + characterIndex);
      ++charactersToGlyphBuffer;
    }
    glyphIndex += numberOfGlyphs;
  }

  // If the character to glyph buffer is updated, it needs to be inserted in the model.
  if(updateCurrentBuffer)
  {
    // Update the indices.
    const Length numberOfGlyphs = glyphIndex - startGlyphIndex;
    for(Vector<Length>::Iterator it    = mCharactersToGlyph.Begin() + startIndex,
                                 endIt = mCharactersToGlyph.End();
        it != endIt;
        ++it)
    {
      *it += numberOfGlyphs;
    }

    mCharactersToGlyph.Insert(mCharactersToGlyph.Begin() + startIndex,
                              newCharactersToGlyph.Begin(),
                              newCharactersToGlyph.End());
  }
}

void VisualModel::CreateGlyphsPerCharacterTable(CharacterIndex startIndex,
                                                GlyphIndex     startGlyphIndex,
                                                Length         numberOfCharacters)
{
  if(0u == numberOfCharacters)
  {
    // Nothing to do.
    return;
  }

  // Get the total number of characters.
  const Length totalNumberOfCharacters = (0u == mGlyphsToCharacters.Count()) ? 0u : *(mGlyphsToCharacters.End() - 1u) + *(mCharactersPerGlyph.End() - 1u); // Index to the first character + the number of characters that form the last glyph.

  // Whether the current buffer is being updated or is set from scratch.
  const bool updateCurrentBuffer = numberOfCharacters < totalNumberOfCharacters;

  Vector<Length> newGlyphsPerCharacter;
  Length*        glyphsPerCharacterBuffer = NULL;

  // 1) Reserve some space for the glyphs per character to avoid reallocations.
  if(updateCurrentBuffer)
  {
    newGlyphsPerCharacter.Resize(numberOfCharacters);
    glyphsPerCharacterBuffer = newGlyphsPerCharacter.Begin();
  }
  else
  {
    mGlyphsPerCharacter.Resize(startIndex + numberOfCharacters);
    glyphsPerCharacterBuffer = mGlyphsPerCharacter.Begin() + startIndex;
  }

  // 2) Traverse the glyphs and set the number of glyphs per character.

  Length traversedCharacters = 0;

  // The number of 'characters per glyph' equal to zero.
  Length zeroCharactersPerGlyph = 0u;

  for(Vector<Length>::ConstIterator it    = mCharactersPerGlyph.Begin() + startGlyphIndex,
                                    endIt = mCharactersPerGlyph.End();
      (it != endIt) && (traversedCharacters < numberOfCharacters);
      ++it)
  {
    const Length numberOfCharactersPerGlyph = *it;
    traversedCharacters += numberOfCharactersPerGlyph;

    // Set the glyphs per character.
    if(0u == numberOfCharactersPerGlyph)
    {
      ++zeroCharactersPerGlyph;
    }
    else
    {
      const Length numberOfZeroGlyphsPerCharacter = (numberOfCharactersPerGlyph - 1u);
      for(Length zeroIndex = 0u; zeroIndex < numberOfZeroGlyphsPerCharacter; ++zeroIndex)
      {
        *glyphsPerCharacterBuffer = 0u;

        // Point to the next position in the buffer.
        ++glyphsPerCharacterBuffer;
      }

      *glyphsPerCharacterBuffer = zeroCharactersPerGlyph + 1u;

      zeroCharactersPerGlyph = 0u;

      // Point to the next position in the buffer.
      ++glyphsPerCharacterBuffer;
    }
  }

  // If the glyphs per character buffer is updated, it needs to be inserted in the model.
  if(updateCurrentBuffer)
  {
    mGlyphsPerCharacter.Insert(mGlyphsPerCharacter.Begin() + startIndex,
                               newGlyphsPerCharacter.Begin(),
                               newGlyphsPerCharacter.End());
  }
}

void VisualModel::GetGlyphs(GlyphInfo* glyphs,
                            GlyphIndex glyphIndex,
                            Length     numberOfGlyphs) const
{
  memcpy(glyphs, mGlyphs.Begin() + glyphIndex, numberOfGlyphs * sizeof(GlyphInfo));
}

void VisualModel::GetGlyphPositions(Vector2*   glyphPositions,
                                    GlyphIndex glyphIndex,
                                    Length     numberOfGlyphs) const
{
  memcpy(glyphPositions, mGlyphPositions.Begin() + glyphIndex, numberOfGlyphs * sizeof(Vector2));
}

Length VisualModel::GetTotalNumberOfLines() const
{
  return mLines.Size();
}

void VisualModel::GetNumberOfLines(GlyphIndex glyphIndex,
                                   Length     numberOfGlyphs,
                                   LineIndex& firstLine,
                                   Length&    numberOfLines) const
{
  // Initialize the number of lines and the first line.
  firstLine           = 0u;
  numberOfLines       = 0u;
  bool firstLineFound = false;

  const GlyphIndex lastGlyphIndex = glyphIndex + numberOfGlyphs;

  // Traverse the lines and count those lines within the range of glyphs.
  for(Vector<LineRun>::ConstIterator it    = mLines.Begin(),
                                     endIt = mLines.End();
      it != endIt;
      ++it)
  {
    const LineRun& line = *it;

    if((line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs > glyphIndex) &&
       (lastGlyphIndex > line.glyphRun.glyphIndex))
    {
      firstLineFound = true;
      ++numberOfLines;
    }
    else if((line.glyphRunSecondHalf.glyphIndex + line.glyphRunSecondHalf.numberOfGlyphs > glyphIndex) &&
            (lastGlyphIndex > line.glyphRunSecondHalf.glyphIndex))
    {
      firstLineFound = true;
      ++numberOfLines;
    }
    else if(lastGlyphIndex <= line.glyphRun.glyphIndex)
    {
      // nothing else to do.
      break;
    }

    if(!firstLineFound)
    {
      ++firstLine;
    }
  }
}

LineIndex VisualModel::GetLineOfGlyph( GlyphIndex glyphIndex )
{
  const CharacterIndex characterIndex = *(mGlyphsToCharacters.Begin() + glyphIndex);
  return GetLineOfCharacter(characterIndex);
}

void VisualModel::GetLinesOfGlyphRange(LineRun*   lines,
                                       GlyphIndex glyphIndex,
                                       Length     numberOfGlyphs) const
{
  LineIndex firstLine     = 0u;
  Length    numberOfLines = 0u;

  GetNumberOfLines(glyphIndex,
                   numberOfGlyphs,
                   firstLine,
                   numberOfLines);

  memcpy(lines, mLines.Begin() + firstLine, numberOfLines * sizeof(LineRun));
}

LineIndex VisualModel::GetLineOfCharacter(CharacterIndex characterIndex)
{
  // 1) Check line is empty or not.
  if(mLines.Empty())
  {
    return 0u;
  }

  // 2) Check in the cached line.
  const LineRun& lineRun = *(mLines.Begin() + mCachedLineIndex);
  if((lineRun.characterRun.characterIndex <= characterIndex) &&
     (characterIndex < lineRun.characterRun.characterIndex + lineRun.characterRun.numberOfCharacters))
  {
    return mCachedLineIndex;
  }

  // 3) Is not in the cached line. Check in the other lines.
  LineIndex index = characterIndex < lineRun.characterRun.characterIndex ? 0u : mCachedLineIndex + 1u;

  for(Vector<LineRun>::ConstIterator it    = mLines.Begin() + index,
                                     endIt = mLines.End();
      it != endIt;
      ++it, ++index)
  {
    const LineRun& lineRun = *it;

    if(characterIndex < lineRun.characterRun.characterIndex + lineRun.characterRun.numberOfCharacters)
    {
      mCachedLineIndex = index;
      break;
    }
  }

  return index;
}

void VisualModel::GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns,
                                   UnderlineRunIndex   index,
                                   Length              numberOfRuns) const
{
  memcpy(underlineRuns,
         mUnderlineRuns.Begin() + index,
         numberOfRuns * sizeof(UnderlinedGlyphRun));
}

void VisualModel::SetNaturalSize(const Vector2& size)
{
  mNaturalSize = size;
}

const Vector2& VisualModel::GetNaturalSize() const
{
  return mNaturalSize;
}

void VisualModel::SetLayoutSize(const Vector2& size)
{
  mLayoutSize = size;
}

const Vector2& VisualModel::GetLayoutSize() const
{
  return mLayoutSize;
}

void VisualModel::SetHeightForWidth(const Vector2& size)
{
  mHeightForWidth = size;
}

const Vector2& VisualModel::GetHeightForWidth() const
{
  return mHeightForWidth;
}

void VisualModel::SetTextColor(const Vector4& textColor)
{
  mTextColor = textColor;

  if(!mUnderlineColorSet)
  {
    mUnderlineColor = textColor;
  }
  if(!mStrikethroughColorSet)
  {
    mStrikethroughColor = textColor;
  }
}

void VisualModel::SetShadowOffset(const Vector2& shadowOffset)
{
  mShadowOffset = shadowOffset;
}

void VisualModel::SetShadowColor(const Vector4& shadowColor)
{
  mShadowColor = shadowColor;
}

void VisualModel::SetShadowBlurRadius(const float& shadowBlurRadius)
{
  mShadowBlurRadius = shadowBlurRadius;
}

void VisualModel::SetUnderlineColor(const Vector4& color)
{
  mUnderlineColor    = color;
  mUnderlineColorSet = true;
}

void VisualModel::SetOutlineOffset(const Vector2& outlineOffset)
{
  mOutlineOffset = outlineOffset;
}

void VisualModel::SetOutlineColor(const Vector4& color)
{
  mOutlineColor = color;
}

void VisualModel::SetOutlineBlurRadius(const float& outlineBlurRadius)
{
  mOutlineBlurRadius = outlineBlurRadius;
}

void VisualModel::SetUnderlineEnabled(bool enabled)
{
  mUnderlineEnabled = enabled;
}

void VisualModel::SetUnderlineHeight(float height)
{
  mUnderlineHeight = height;
}

void VisualModel::SetUnderlineType(Text::Underline::Type type)
{
  mUnderlineType = type;
}

void VisualModel::SetDashedUnderlineWidth(float width)
{
  mDashedUnderlineWidth = width;
}

void VisualModel::SetDashedUnderlineGap(float gap)
{
  mDashedUnderlineGap = gap;
}

void VisualModel::SetOutlineWidth(uint16_t width)
{
  mOutlineWidth = width;
}

void VisualModel::SetBackgroundColor(const Vector4& color)
{
  mBackgroundColor = color;
}

void VisualModel::SetBackgroundEnabled(bool enabled)
{
  mBackgroundEnabled = enabled;
}

void VisualModel::SetMarkupProcessorEnabled(bool enabled)
{
  mMarkupProcessorEnabled = enabled;
}

void VisualModel::SetTextElideEnabled(bool enabled)
{
  mTextElideEnabled = enabled;
}

void VisualModel::SetEllipsisPosition(Toolkit::DevelText::EllipsisPosition::Type ellipsisPosition)
{
  mEllipsisPosition = ellipsisPosition;
}

void VisualModel::SetStartIndexOfElidedGlyphs(GlyphIndex startIndexOfElidedGlyphs)
{
  mStartIndexOfElidedGlyphs = startIndexOfElidedGlyphs;
}

void VisualModel::SetEndIndexOfElidedGlyphs(GlyphIndex endIndexOfElidedGlyphs)
{
  mEndIndexOfElidedGlyphs = endIndexOfElidedGlyphs;
}

void VisualModel::SetFirstMiddleIndexOfElidedGlyphs(GlyphIndex firstMiddleIndexOfElidedGlyphs)
{
  mFirstMiddleIndexOfElidedGlyphs = firstMiddleIndexOfElidedGlyphs;
}

void VisualModel::SetSecondMiddleIndexOfElidedGlyphs(GlyphIndex secondMiddleIndexOfElidedGlyphs)
{
  mSecondMiddleIndexOfElidedGlyphs = secondMiddleIndexOfElidedGlyphs;
}

void VisualModel::SetStrikethroughColor(const Vector4& color)
{
  mStrikethroughColor    = color;
  mStrikethroughColorSet = true;
}

void VisualModel::SetStrikethroughEnabled(bool enabled)
{
  mStrikethroughEnabled = enabled;
}

void VisualModel::SetStrikethroughHeight(float height)
{
  mStrikethroughHeight = height;
}

void VisualModel::SetCharacterSpacing(float characterSpacing)
{
  mCharacterSpacing = characterSpacing;
}

const Vector4& VisualModel::GetTextColor() const
{
  return mTextColor;
}

const Vector2& VisualModel::GetShadowOffset() const
{
  return mShadowOffset;
}

const Vector4& VisualModel::GetShadowColor() const
{
  return mShadowColor;
}

const float& VisualModel::GetShadowBlurRadius() const
{
  return mShadowBlurRadius;
}

const Vector4& VisualModel::GetUnderlineColor() const
{
  return mUnderlineColor;
}

const Vector2& VisualModel::GetOutlineOffset() const
{
  return mOutlineOffset;
}

const Vector4& VisualModel::GetOutlineColor() const
{
  return mOutlineColor;
}

const float& VisualModel::GetOutlineBlurRadius() const
{
  return mOutlineBlurRadius;
}

bool VisualModel::IsUnderlineEnabled() const
{
  return mUnderlineEnabled;
}

float VisualModel::GetUnderlineHeight() const
{
  return mUnderlineHeight;
}

Text::Underline::Type VisualModel::GetUnderlineType() const
{
  return mUnderlineType;
}

float VisualModel::GetDashedUnderlineWidth() const
{
  return mDashedUnderlineWidth;
}

float VisualModel::GetDashedUnderlineGap() const
{
  return mDashedUnderlineGap;
}

uint16_t VisualModel::GetOutlineWidth() const
{
  return mOutlineWidth;
}

const Vector4& VisualModel::GetBackgroundColor() const
{
  return mBackgroundColor;
}

const float VisualModel::GetCharacterSpacing() const
{
  return mCharacterSpacing;
}

bool VisualModel::IsBackgroundEnabled() const
{
  return mBackgroundEnabled;
}

bool VisualModel::IsMarkupProcessorEnabled() const
{
  return mMarkupProcessorEnabled;
}

bool VisualModel::IsTextElideEnabled() const
{
  return mTextElideEnabled;
}

Toolkit::DevelText::EllipsisPosition::Type VisualModel::GetEllipsisPosition() const
{
  return mEllipsisPosition;
}

GlyphIndex VisualModel::GetStartIndexOfElidedGlyphs() const
{
  return mStartIndexOfElidedGlyphs;
}

GlyphIndex VisualModel::GetEndIndexOfElidedGlyphs() const
{
  return mEndIndexOfElidedGlyphs;
}

GlyphIndex VisualModel::GetFirstMiddleIndexOfElidedGlyphs() const
{
  return mFirstMiddleIndexOfElidedGlyphs;
}

GlyphIndex VisualModel::GetSecondMiddleIndexOfElidedGlyphs() const
{
  return mSecondMiddleIndexOfElidedGlyphs;
}

Length VisualModel::GetNumberOfUnderlineRuns() const
{
  return mUnderlineRuns.Count();
}

const Vector4& VisualModel::GetStrikethroughColor() const
{
  return mStrikethroughColor;
}

bool VisualModel::IsStrikethroughEnabled() const
{
  return mStrikethroughEnabled;
}

float VisualModel::GetStrikethroughHeight() const
{
  return mStrikethroughHeight;
}

void VisualModel::GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns,
                                       StrikethroughRunIndex  index,
                                       Length                 numberOfRuns) const
{
  memcpy(strikethroughRuns,
         mStrikethroughRuns.Begin() + index,
         numberOfRuns * sizeof(StrikethroughGlyphRun));
}

Length VisualModel::GetNumberOfStrikethroughRuns() const
{
  return mStrikethroughRuns.Count();
}

Length VisualModel::GetNumberOfCharacterSpacingGlyphRuns() const
{
  return mCharacterSpacingRuns.Count();
}

const Vector<CharacterSpacingGlyphRun>& VisualModel::GetCharacterSpacingGlyphRuns() const
{
  return mCharacterSpacingRuns;
}

void VisualModel::ClearCaches()
{
  mCachedLineIndex = 0u;
}

const Vector<CharacterIndex>& VisualModel::GetGlyphsToCharacters() const
{
  return mGlyphsToCharacters;
}

void VisualModel::SetCutoutEnabled(bool enable)
{
  mCutoutEnabled = enable;
}

bool VisualModel::IsCutoutEnabled() const
{
  return mCutoutEnabled;
}

void VisualModel::SetBackgroundWithCutoutEnabled(bool enable)
{
  mBackgroundWithCutoutEnabled = enable;
}

bool VisualModel::IsBackgroundWithCutoutEnabled() const
{
  return mBackgroundWithCutoutEnabled;
}

void VisualModel::SetBackgroundColorWithCutout(const Vector4& color)
{
  mBackgroundColorWithCutout = color;
}

const Vector4& VisualModel::GetBackgroundColorWithCutout() const
{
  return mBackgroundColorWithCutout;
}

void VisualModel::SetOffsetWithCutout(const Vector2& offset)
{
  mOffsetWithCutout = offset;
}

const Vector2& VisualModel::GetOffsetWithCutout() const
{
  return mOffsetWithCutout;
}

VisualModel::~VisualModel()
{
}

VisualModel::VisualModel()
: mGlyphs(),
  mGlyphsToCharacters(),
  mCharactersToGlyph(),
  mCharactersPerGlyph(),
  mGlyphsPerCharacter(),
  mGlyphPositions(),
  mLines(),
  mTextColor(Color::BLACK),
  mShadowColor(Color::BLACK),
  mUnderlineColor(Color::BLACK),
  mOutlineColor(Color::WHITE),
  mBackgroundColor(Color::TRANSPARENT),
  mStrikethroughColor(Color::BLACK),
  mControlSize(),
  mShadowOffset(),
  mOutlineOffset(),
  mUnderlineHeight(0.0f),
  mStrikethroughHeight(0.0f),
  mUnderlineType(Text::Underline::SOLID),
  mDashedUnderlineWidth(2.0f),
  mDashedUnderlineGap(1.0f),
  mShadowBlurRadius(0.0f),
  mOutlineBlurRadius(0.0f),
  mOutlineWidth(0u),
  mNaturalSize(),
  mLayoutSize(),
  mHeightForWidth(0.0f, 0.0f),
  mCachedLineIndex(0u),
  mEllipsisPosition(DevelText::EllipsisPosition::END),
  mStartIndexOfElidedGlyphs(0u),
  mEndIndexOfElidedGlyphs(0u),
  mFirstMiddleIndexOfElidedGlyphs(0u),
  mSecondMiddleIndexOfElidedGlyphs(0u),
  mTextElideEnabled(false),
  mUnderlineEnabled(false),
  mUnderlineColorSet(false),
  mBackgroundEnabled(false),
  mMarkupProcessorEnabled(false),
  mStrikethroughEnabled(false),
  mStrikethroughColorSet(false),
  mCharacterSpacing(0.0f),
  mCutoutEnabled(false),
  mBackgroundWithCutoutEnabled(false)
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
