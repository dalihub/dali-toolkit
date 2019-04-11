#ifndef DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H
#define DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct BidirectionalLineInfoRun;

namespace Layout
{

/**
 * @brief Struct used to pass parameters.
 */
struct Parameters
{
  /**
   * Constructor with the needed parameters to layout the text.
   *
   * @param[in] boundingBox The size of the box containing the text.
   * @param[in] textBuffer The text buffer.
   * @param[in] lineBreakInfoBuffer The line break info.
   * @param[in] wordBreakInfoBuffer The word break info.
   * @param[in] characterDirectionBuffer Vector with the direction of each character.
   * @param[in] glyphsBuffer Vector with glyphs.
   * @param[in] glyphsToCharactersBuffer Vector with indices pointing the first character of each glyph.
   * @param[in] charactersPerGlyphBuffer Vector with the number of characters that forms each glyph.
   * @param[in] charactersToGlyphsBuffer Vector with indices pointing the first glyph of each character.
   * @param[in] glyphsPerCharacterBuffer Vector with the number of glyphs shaped from the character.
   * @param[in] totalNumberOfGlyphs The number of glyphs.
   * @param[in] horizontalAlignment The horizontal alignment.
   * @param[in] lineWrapMode The text wrap mode.
   * @param[in] outlineWidth The outline width.
   * @param[in] ignoreSpaceAfterText Whether ignoring spaces after text or not.
   * @param[in] matchSystemLanguageDirection Whether match align for system language direction or not..
   */
  Parameters( const Vector2& boundingBox,
              const Character* const textBuffer,
              const LineBreakInfo* const lineBreakInfoBuffer,
              const WordBreakInfo* const wordBreakInfoBuffer,
              const CharacterDirection* const characterDirectionBuffer,
              const GlyphInfo* const glyphsBuffer,
              const CharacterIndex* const glyphsToCharactersBuffer,
              const Length* const charactersPerGlyphBuffer,
              const GlyphIndex* const charactersToGlyphsBuffer,
              const Length* const glyphsPerCharacterBuffer,
              Length totalNumberOfGlyphs,
              Text::HorizontalAlignment::Type horizontalAlignment,
              Text::LineWrap::Mode lineWrapMode,
              float outlineWidth,
              bool ignoreSpaceAfterText,
              bool matchSystemLanguageDirection )
  : boundingBox( boundingBox ),
    textBuffer( textBuffer ),
    lineBreakInfoBuffer( lineBreakInfoBuffer ),
    wordBreakInfoBuffer( wordBreakInfoBuffer ),
    characterDirectionBuffer( characterDirectionBuffer ),
    glyphsBuffer( glyphsBuffer ),
    glyphsToCharactersBuffer( glyphsToCharactersBuffer ),
    charactersPerGlyphBuffer( charactersPerGlyphBuffer ),
    charactersToGlyphsBuffer( charactersToGlyphsBuffer ),
    glyphsPerCharacterBuffer( glyphsPerCharacterBuffer ),
    lineBidirectionalInfoRunsBuffer( NULL ),
    numberOfBidirectionalInfoRuns( 0u ),
    startGlyphIndex( 0u ),
    numberOfGlyphs( 0u ),
    totalNumberOfGlyphs( totalNumberOfGlyphs ),
    horizontalAlignment( horizontalAlignment ),
    startLineIndex( 0u ),
    estimatedNumberOfLines( 0u ),
    lineWrapMode( lineWrapMode ),
    outlineWidth( outlineWidth ),
    ignoreSpaceAfterText( ignoreSpaceAfterText ),
    matchSystemLanguageDirection ( matchSystemLanguageDirection ),
    interGlyphExtraAdvance( 0.f ),
    isLastNewParagraph( false )
  {}

  Vector2                         boundingBox;                     ///< The size of the box containing the text.
  const Character* const          textBuffer;                      ///< The text buffer.
  const LineBreakInfo* const      lineBreakInfoBuffer;             ///< The line break info.
  const WordBreakInfo* const      wordBreakInfoBuffer;             ///< The word break info.
  const CharacterDirection* const characterDirectionBuffer;        ///< Vector with the direction of each character.
  const GlyphInfo* const          glyphsBuffer;                    ///< Vector with glyphs.
  const CharacterIndex* const     glyphsToCharactersBuffer;        ///< Vector with indices pointing the first character of each glyph.
  const Length* const             charactersPerGlyphBuffer;        ///< Vector with the number of characters that forms each glyph.
  const GlyphIndex* const         charactersToGlyphsBuffer;        ///< Vector with indices pointing the first glyph of each character.
  const Length* const             glyphsPerCharacterBuffer;        ///< Vector with the number of glyphs shaped from the character.
  BidirectionalLineInfoRun*       lineBidirectionalInfoRunsBuffer; ///< Bidirectional conversion tables per line.
  Length                          numberOfBidirectionalInfoRuns;   ///< The number of lines with bidirectional info.
  GlyphIndex                      startGlyphIndex;                 ///< Index to the first glyph to layout.
  Length                          numberOfGlyphs;                  ///< The number of glyphs to layout.
  Length                          totalNumberOfGlyphs;             ///< The number of glyphs.
  HorizontalAlignment::Type       horizontalAlignment;             ///< The horizontal alignment.
  LineIndex                       startLineIndex;                  ///< The line index where to insert the new lines.
  Length                          estimatedNumberOfLines;          ///< The estimated number of lines.
  Text::LineWrap::Mode            lineWrapMode;                    ///< The line wrap mode for moving to next line.
  float                           outlineWidth;                    ///< The outline width.
  bool                            ignoreSpaceAfterText:1;          ///< Whether ignoring spaces after text or not. Default is true.
  bool                            matchSystemLanguageDirection:1;  ///< Whether match align for system language direction or not. Default is false.
  float                           interGlyphExtraAdvance;          ///< Extra advance added to each glyph.
  bool                            isLastNewParagraph:1;            ///< Whether the last character is a new paragraph character.
};

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LAYOUT_PARAMETERS_H
