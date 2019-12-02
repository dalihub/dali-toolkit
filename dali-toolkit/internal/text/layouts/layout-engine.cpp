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
#include <dali-toolkit/internal/text/layouts/layout-engine.h>

// EXTERNAL INCLUDES
#include <limits>
#include <cmath>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Layout
{

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_LAYOUT");
#endif

const float MAX_FLOAT = std::numeric_limits<float>::max();
const bool RTL = true;
const float LINE_SPACING= 0.f;

inline bool isEmptyLineAtLast( const Vector<LineRun>& lines, const Vector<LineRun>::Iterator& line )
{
  return ( (*line).characterRun.numberOfCharacters == 0 && line + 1u == lines.End() );
}

} //namespace

/**
 * @brief Stores temporary layout info of the line.
 */
struct LineLayout
{
  LineLayout()
  : glyphIndex( 0u ),
    characterIndex( 0u ),
    numberOfGlyphs( 0u ),
    numberOfCharacters( 0u ),
    ascender( -MAX_FLOAT ),
    descender( MAX_FLOAT ),
    lineSpacing( 0.f ),
    penX( 0.f ),
    previousAdvance( 0.f ),
    length( 0.f ),
    wsLengthEndOfLine( 0.f )
  {}

  ~LineLayout()
  {}

  void Clear()
  {
    glyphIndex = 0u;
    characterIndex = 0u;
    numberOfGlyphs = 0u;
    numberOfCharacters = 0u;
    ascender = 0.f;
    descender = MAX_FLOAT;
  }

  GlyphIndex     glyphIndex;         ///< Index of the first glyph to be laid-out.
  CharacterIndex characterIndex;     ///< Index of the first character to be laid-out.
  Length         numberOfGlyphs;     ///< The number of glyph which fit in one line.
  Length         numberOfCharacters; ///< The number of characters which fit in one line.
  float          ascender;           ///< The maximum ascender of all fonts in the line.
  float          descender;          ///< The minimum descender of all fonts in the line.
  float          lineSpacing;        ///< The line spacing
  float          penX;               ///< The origin of the current glyph ( is the start point plus the accumulation of all advances ).
  float          previousAdvance;    ///< The advance of the previous glyph.
  float          length;             ///< The current length of the line.
  float          wsLengthEndOfLine;  ///< The length of the white spaces at the end of the line.
};

struct Engine::Impl
{
  Impl()
  : mLayout( Layout::Engine::SINGLE_LINE_BOX ),
    mCursorWidth( 0.f ),
    mDefaultLineSpacing( LINE_SPACING )
  {
  }

  /**
   * @brief Updates the line ascender and descender with the metrics of a new font.
   *
   * @param[in] glyphMetrics The metrics of the new font.
   * @param[in,out] lineLayout The line layout.
   */
  void UpdateLineHeight( const GlyphMetrics& glyphMetrics, LineLayout& lineLayout )
  {
    Text::FontMetrics fontMetrics;
    if( 0u != glyphMetrics.fontId )
    {
      mMetrics->GetFontMetrics( glyphMetrics.fontId, fontMetrics );
    }
    else
    {
      fontMetrics.ascender = glyphMetrics.fontHeight;
      fontMetrics.descender = 0.f;
      fontMetrics.height = fontMetrics.ascender;
      fontMetrics.underlinePosition = 0.f;
      fontMetrics.underlineThickness = 1.f;
    }

    // Sets the maximum ascender.
    lineLayout.ascender = std::max( lineLayout.ascender, fontMetrics.ascender );

    // Sets the minimum descender.
    lineLayout.descender = std::min( lineLayout.descender, fontMetrics.descender );

    // set the line spacing
    lineLayout.lineSpacing = mDefaultLineSpacing;
  }

  /**
   * @brief Merges a temporary line layout into the line layout.
   *
   * @param[in,out] lineLayout The line layout.
   * @param[in] tmpLineLayout A temporary line layout.
   */
  void MergeLineLayout( LineLayout& lineLayout,
                        const LineLayout& tmpLineLayout )
  {
    lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
    lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;

    lineLayout.penX = tmpLineLayout.penX;
    lineLayout.previousAdvance = tmpLineLayout.previousAdvance;

    lineLayout.length = tmpLineLayout.length;
    lineLayout.wsLengthEndOfLine = tmpLineLayout.wsLengthEndOfLine;

    // Sets the maximum ascender.
    lineLayout.ascender = std::max( lineLayout.ascender, tmpLineLayout.ascender );

    // Sets the minimum descender.
    lineLayout.descender = std::min( lineLayout.descender, tmpLineLayout.descender );
  }

  /**
   * Retrieves the line layout for a given box width.
   *
   * @note This method lais out text as it were left to right. At this point is not possible to reorder the line
   *       because the number of characters of the line is not known (one of the responsabilities of this method
   *       is calculate that). Due to glyph's 'x' bearing, width and advance, when right to left or mixed right to left
   *       and left to right text is laid-out, it can be small differences in the line length. One solution is to
   *       reorder and re-lay out the text after this method and add or remove one extra glyph if needed. However,
   *       this method calculates which are the first and last glyphs of the line (the ones that causes the
   *       differences). This is a good point to check if there is problems with the text exceeding the boundaries
   *       of the control when there is right to left text.
   *
   * @param[in] parameters The layout parameters.
   * @param[out] lineLayout The line layout.
   * @param[in,out] paragraphDirection in: the current paragraph's direction, out: the next paragraph's direction. Is set after a must break.
   * @param[in] completelyFill Whether to completely fill the line ( even if the last word exceeds the boundaries ).
   */
  void GetLineLayoutForBox( const Parameters& parameters,
                            LineLayout& lineLayout,
                            CharacterDirection& paragraphDirection,
                            bool completelyFill )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->GetLineLayoutForBox\n" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  initial glyph index : %d\n", lineLayout.glyphIndex );

    const bool isMultiline = mLayout == MULTI_LINE_BOX;
    const bool isWordLaidOut = parameters.lineWrapMode == Text::LineWrap::WORD;

    // The last glyph to be laid-out.
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;

    // If the first glyph has a negative bearing its absolute value needs to be added to the line length.
    // In the case the line starts with a right to left character, if the width is longer than the advance,
    // the difference needs to be added to the line length.

    // Check whether the first glyph comes from a character that is shaped in multiple glyphs.
    const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup( lineLayout.glyphIndex,
                                                                   lastGlyphOfParagraphPlusOne,
                                                                   parameters.charactersPerGlyphBuffer );

    GlyphMetrics glyphMetrics;
    GetGlyphsMetrics( lineLayout.glyphIndex,
                      numberOfGLyphsInGroup,
                      glyphMetrics,
                      parameters.glyphsBuffer,
                      mMetrics );

    // Set the direction of the first character of the line.
    lineLayout.characterIndex = *( parameters.glyphsToCharactersBuffer + lineLayout.glyphIndex );

    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;

    // Initialize the start point.

    // The initial start point is zero. However it needs a correction according the 'x' bearing of the first glyph.
    // i.e. if the bearing of the first glyph is negative it may exceed the boundaries of the text area.
    // It needs to add as well space for the cursor if the text is in edit mode and extra space in case the text is outlined.
    tmpLineLayout.penX = -glyphMetrics.xBearing + mCursorWidth + parameters.outlineWidth;

    // Initialize the advance of the previous glyph.
    tmpLineLayout.previousAdvance = 0.f;

    // Calculate the line height if there is no characters.
    FontId lastFontId = glyphMetrics.fontId;
    UpdateLineHeight( glyphMetrics, tmpLineLayout );

    bool oneWordLaidOut = false;

    for( GlyphIndex glyphIndex = lineLayout.glyphIndex;
         glyphIndex < lastGlyphOfParagraphPlusOne; )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  glyph index : %d\n", glyphIndex );

      // Check whether this glyph comes from a character that is shaped in multiple glyphs.
      const Length numberOfGLyphsInGroup = GetNumberOfGlyphsOfGroup( glyphIndex,
                                                                     lastGlyphOfParagraphPlusOne,
                                                                     parameters.charactersPerGlyphBuffer );

      GlyphMetrics glyphMetrics;
      GetGlyphsMetrics( glyphIndex,
                        numberOfGLyphsInGroup,
                        glyphMetrics,
                        parameters.glyphsBuffer,
                        mMetrics );

      const bool isLastGlyph = glyphIndex + numberOfGLyphsInGroup  == parameters.totalNumberOfGlyphs;

      // Check if the font of the current glyph is the same of the previous one.
      // If it's different the ascender and descender need to be updated.
      if( lastFontId != glyphMetrics.fontId )
      {
        UpdateLineHeight( glyphMetrics, tmpLineLayout );
        lastFontId = glyphMetrics.fontId;
      }

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length charactersPerGlyph = *( parameters.charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u );
      const bool hasCharacters = charactersPerGlyph > 0u;
      const CharacterIndex characterFirstIndex = *( parameters.glyphsToCharactersBuffer + glyphIndex );
      const CharacterIndex characterLastIndex = characterFirstIndex + ( hasCharacters ? charactersPerGlyph - 1u : 0u );

      // Get the line break info for the current character.
      const LineBreakInfo lineBreakInfo = hasCharacters ? *( parameters.lineBreakInfoBuffer + characterLastIndex ) : TextAbstraction::LINE_NO_BREAK;

      // Increase the number of characters.
      tmpLineLayout.numberOfCharacters += charactersPerGlyph;

      // Increase the number of glyphs.
      tmpLineLayout.numberOfGlyphs += numberOfGLyphsInGroup;

      // Check whether is a white space.
      const Character character = *( parameters.textBuffer + characterFirstIndex );
      const bool isWhiteSpace = TextAbstraction::IsWhiteSpace( character );

      // Calculate the length of the line.

      // Used to restore the temporal line layout when a single word does not fit in the control's width and is split by character.
      const float previousTmpPenX = tmpLineLayout.penX;
      const float previousTmpAdvance = tmpLineLayout.previousAdvance;
      const float previousTmpLength = tmpLineLayout.length;
      const float previousTmpWsLengthEndOfLine = tmpLineLayout.wsLengthEndOfLine;

      if( isWhiteSpace )
      {
        // Add the length to the length of white spaces at the end of the line.
        tmpLineLayout.wsLengthEndOfLine += glyphMetrics.advance; // The advance is used as the width is always zero for the white spaces.
      }
      else
      {
        tmpLineLayout.penX += tmpLineLayout.previousAdvance + tmpLineLayout.wsLengthEndOfLine;
        tmpLineLayout.previousAdvance = ( glyphMetrics.advance + parameters.interGlyphExtraAdvance );
        tmpLineLayout.length = tmpLineLayout.penX + glyphMetrics.xBearing + glyphMetrics.width;

        // Clear the white space length at the end of the line.
        tmpLineLayout.wsLengthEndOfLine = 0.f;
      }

      // Check if the accumulated length fits in the width of the box.
      if( ( completelyFill || isMultiline ) && !isWhiteSpace &&
          ( tmpLineLayout.length > parameters.boundingBox.width ) )
      {
        // Current word does not fit in the box's width.
        if( !oneWordLaidOut || completelyFill )
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Break the word by character\n" );

          // The word doesn't fit in the control's width. It needs to be split by character.
          if( tmpLineLayout.numberOfGlyphs > 0u )
          {
            tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
            tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;

            tmpLineLayout.penX = previousTmpPenX;
            tmpLineLayout.previousAdvance = previousTmpAdvance;
            tmpLineLayout.length = previousTmpLength;
            tmpLineLayout.wsLengthEndOfLine = previousTmpWsLengthEndOfLine;
          }

          // Add part of the word to the line layout.
          MergeLineLayout( lineLayout, tmpLineLayout );
        }
        else
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Current word does not fit.\n" );
        }

        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox.\n" );

        return;
      }

      if( ( isMultiline || isLastGlyph ) &&
          ( TextAbstraction::LINE_MUST_BREAK == lineBreakInfo ) )
      {
        // Must break the line. Update the line layout and return.
        MergeLineLayout( lineLayout, tmpLineLayout );

        // Set the next paragraph's direction.
        if( !isLastGlyph &&
            ( NULL != parameters.characterDirectionBuffer ) )
        {
          paragraphDirection = *( parameters.characterDirectionBuffer + 1u + characterLastIndex );
        }

        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Must break\n" );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n" );

        return;
      }

      if( isMultiline &&
          ( TextAbstraction::LINE_ALLOW_BREAK == lineBreakInfo ) )
      {
        oneWordLaidOut = isWordLaidOut;
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  One word laid-out\n" );

        // Current glyph is the last one of the current word.
        // Add the temporal layout to the current one.
        MergeLineLayout( lineLayout, tmpLineLayout );

        tmpLineLayout.Clear();
      }

      glyphIndex += numberOfGLyphsInGroup;
    }

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n" );
  }

  void SetGlyphPositions( const GlyphInfo* const glyphsBuffer,
                          Length numberOfGlyphs,
                          float outlineWidth,
                          float interGlyphExtraAdvance,
                          Vector2* glyphPositionsBuffer )
  {
    // Traverse the glyphs and set the positions.

    // Check if the x bearing of the first character is negative.
    // If it has a negative x bearing, it will exceed the boundaries of the actor,
    // so the penX position needs to be moved to the right.

    const GlyphInfo& glyph = *glyphsBuffer;
    float penX = -glyph.xBearing + mCursorWidth + outlineWidth;

    for( GlyphIndex i = 0u; i < numberOfGlyphs; ++i )
    {
      const GlyphInfo& glyph = *( glyphsBuffer + i );
      Vector2& position = *( glyphPositionsBuffer + i );

      position.x = std::roundf( penX + glyph.xBearing );
      position.y = -glyph.yBearing;

      penX += ( glyph.advance + interGlyphExtraAdvance );
    }
  }

  /**
   * @brief Resizes the line buffer.
   *
   * @param[in,out] lines The vector of lines. Used when the layout is created from scratch.
   * @param[in,out] newLines The vector of lines used instead of @p lines when the layout is updated.
   * @param[in,out] linesCapacity The capacity of the vector (either lines or newLines).
   * @param[in] updateCurrentBuffer Whether the layout is updated.
   *
   * @return Pointer to either lines or newLines.
   */
  LineRun* ResizeLinesBuffer( Vector<LineRun>& lines,
                              Vector<LineRun>& newLines,
                              Length& linesCapacity,
                              bool updateCurrentBuffer )
  {
    LineRun* linesBuffer = NULL;
    // Reserve more space for the next lines.
    linesCapacity *= 2u;
    if( updateCurrentBuffer )
    {
      newLines.Resize( linesCapacity );
      linesBuffer = newLines.Begin();
    }
    else
    {
      lines.Resize( linesCapacity );
      linesBuffer = lines.Begin();
    }

    return linesBuffer;
  }

  /**
   * Ellipsis a line if it exceeds the width's of the bounding box.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] glyphPositionsBuffer Pointer to the position's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] penY The vertical layout position.
   * @param[in] currentParagraphDirection The current paragraph's direction.
   * @param[in,out] isAutoScrollEnabled If the isAutoScrollEnabled is true and the height of the text exceeds the boundaries of the control the text is elided and the isAutoScrollEnabled is set to false to disable the autoscroll
   *
   * return Whether the line is ellipsized.
   */
  bool EllipsisLine( const Parameters& layoutParameters,
                     const LineLayout& layout,
                     Size& layoutSize,
                     LineRun* linesBuffer,
                     Vector2* glyphPositionsBuffer,
                     Length& numberOfLines,
                     float penY,
                     CharacterDirection currentParagraphDirection,
                     bool& isAutoScrollEnabled )
  {
    const bool ellipsis = isAutoScrollEnabled ? ( penY - layout.descender > layoutParameters.boundingBox.height ) :
                                                ( ( penY - layout.descender > layoutParameters.boundingBox.height ) ||
                                                  ( ( mLayout == SINGLE_LINE_BOX ) &&
                                                    ( layout.length > layoutParameters.boundingBox.width ) ) );

    if( ellipsis )
    {
      isAutoScrollEnabled = false;
      // Do not layout more lines if ellipsis is enabled.

      // The last line needs to be completely filled with characters.
      // Part of a word may be used.

      LineRun* lineRun = NULL;
      LineLayout ellipsisLayout;
      if( 0u != numberOfLines )
      {
        // Get the last line and layout it again with the 'completelyFill' flag to true.
        lineRun = linesBuffer + ( numberOfLines - 1u );

        penY -= layout.ascender - lineRun->descender + lineRun->lineSpacing;

        ellipsisLayout.glyphIndex = lineRun->glyphRun.glyphIndex;
      }
      else
      {
        // At least there is space reserved for one line.
        lineRun = linesBuffer;

        lineRun->glyphRun.glyphIndex = 0u;
        ellipsisLayout.glyphIndex = 0u;

        ++numberOfLines;
      }

      GetLineLayoutForBox( layoutParameters,
                           ellipsisLayout,
                           currentParagraphDirection,
                           true );

      lineRun->glyphRun.numberOfGlyphs = ellipsisLayout.numberOfGlyphs;
      lineRun->characterRun.characterIndex = ellipsisLayout.characterIndex;
      lineRun->characterRun.numberOfCharacters = ellipsisLayout.numberOfCharacters;
      lineRun->width = ellipsisLayout.length;
      lineRun->extraLength = std::ceil( ellipsisLayout.wsLengthEndOfLine );
      lineRun->ascender = ellipsisLayout.ascender;
      lineRun->descender = ellipsisLayout.descender;
      lineRun->direction = !RTL;
      lineRun->ellipsis = true;

      layoutSize.width = layoutParameters.boundingBox.width;
      if( layoutSize.height < Math::MACHINE_EPSILON_1000 )
      {
        layoutSize.height += ( lineRun->ascender + -lineRun->descender ) + lineRun->lineSpacing;
      }

      SetGlyphPositions( layoutParameters.glyphsBuffer + lineRun->glyphRun.glyphIndex,
                         ellipsisLayout.numberOfGlyphs,
                         layoutParameters.outlineWidth,
                         layoutParameters.interGlyphExtraAdvance,
                         glyphPositionsBuffer + lineRun->glyphRun.glyphIndex - layoutParameters.startGlyphIndex );
    }

    return ellipsis;
  }

  /**
   * @brief Updates the text layout with a new laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in] index Index to the vector of glyphs.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] isLastLine Whether the laid-out line is the last one.
   */
  void UpdateTextLayout( const Parameters& layoutParameters,
                         const LineLayout& layout,
                         Size& layoutSize,
                         LineRun* linesBuffer,
                         GlyphIndex index,
                         Length& numberOfLines,
                         bool isLastLine )
  {
    LineRun& lineRun = *( linesBuffer + numberOfLines );
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex = index;
    lineRun.glyphRun.numberOfGlyphs = layout.numberOfGlyphs;
    lineRun.characterRun.characterIndex = layout.characterIndex;
    lineRun.characterRun.numberOfCharacters = layout.numberOfCharacters;
    lineRun.lineSpacing = mDefaultLineSpacing;

    if( isLastLine && !layoutParameters.isLastNewParagraph )
    {
      const float width = layout.length + layout.wsLengthEndOfLine;
      if( MULTI_LINE_BOX == mLayout )
      {
        lineRun.width = ( width > layoutParameters.boundingBox.width ) ? layoutParameters.boundingBox.width : width;
      }
      else
      {
        lineRun.width = width;
      }

      lineRun.extraLength = 0.f;
    }
    else
    {
      lineRun.width = layout.length;
      lineRun.extraLength = std::ceil( layout.wsLengthEndOfLine );
    }

    // Rounds upward to avoid a non integer size.
    lineRun.width = std::ceil( lineRun.width );

    lineRun.ascender = layout.ascender;
    lineRun.descender = layout.descender;
    lineRun.direction = !RTL;
    lineRun.ellipsis = false;

    // Update the actual size.
    if( lineRun.width > layoutSize.width )
    {
      layoutSize.width = lineRun.width;
    }

    layoutSize.height += ( lineRun.ascender + -lineRun.descender ) + lineRun.lineSpacing;
  }

  /**
   * @brief Updates the text layout with the last laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] characterIndex The character index of the line.
   * @param[in] glyphIndex The glyph index of the line.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   */
  void UpdateTextLayout( const Parameters& layoutParameters,
                         CharacterIndex characterIndex,
                         GlyphIndex glyphIndex,
                         Size& layoutSize,
                         LineRun* linesBuffer,
                         Length& numberOfLines )
  {
    // Need to add a new line with no characters but with height to increase the layoutSize.height
    const GlyphInfo& glyphInfo = *( layoutParameters.glyphsBuffer + layoutParameters.totalNumberOfGlyphs - 1u );

    Text::FontMetrics fontMetrics;
    if( 0u != glyphInfo.fontId )
    {
      mMetrics->GetFontMetrics( glyphInfo.fontId, fontMetrics );
    }

    LineRun& lineRun = *( linesBuffer + numberOfLines );
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex = glyphIndex;
    lineRun.glyphRun.numberOfGlyphs = 0u;
    lineRun.characterRun.characterIndex = characterIndex;
    lineRun.characterRun.numberOfCharacters = 0u;
    lineRun.width = 0.f;
    lineRun.ascender = fontMetrics.ascender;
    lineRun.descender = fontMetrics.descender;
    lineRun.extraLength = 0.f;
    lineRun.alignmentOffset = 0.f;
    lineRun.direction = !RTL;
    lineRun.ellipsis = false;
    lineRun.lineSpacing = mDefaultLineSpacing;

    layoutSize.height += ( lineRun.ascender + -lineRun.descender ) + lineRun.lineSpacing;
  }

  /**
   * @brief Updates the text's layout size adding the size of the previously laid-out lines.
   *
   * @param[in] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in,out] layoutSize The text's layout size.
   */
  void UpdateLayoutSize( const Vector<LineRun>& lines,
                         Size& layoutSize )
  {
    for( Vector<LineRun>::ConstIterator it = lines.Begin(),
           endIt = lines.End();
         it != endIt;
         ++it )
    {
      const LineRun& line = *it;

      if( line.width > layoutSize.width )
      {
        layoutSize.width = line.width;
      }

      layoutSize.height += ( line.ascender + -line.descender ) + line.lineSpacing;
    }
  }

  /**
   * @brief Updates the indices of the character and glyph runs of the lines before the new lines are inserted.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in,out] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in] characterOffset The offset to be added to the runs of characters.
   * @param[in] glyphOffset The offset to be added to the runs of glyphs.
   */
  void UpdateLineIndexOffsets( const Parameters& layoutParameters,
                               Vector<LineRun>& lines,
                               Length characterOffset,
                               Length glyphOffset )
  {
    // Update the glyph and character runs.
    for( Vector<LineRun>::Iterator it = lines.Begin() + layoutParameters.startLineIndex,
           endIt = lines.End();
         it != endIt;
         ++it )
    {
      LineRun& line = *it;

      line.glyphRun.glyphIndex = glyphOffset;
      line.characterRun.characterIndex = characterOffset;

      glyphOffset += line.glyphRun.numberOfGlyphs;
      characterOffset += line.characterRun.numberOfCharacters;
    }
  }

  bool LayoutText( const Parameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& layoutSize,
                   bool elideTextEnabled,
                   bool& isAutoScrollEnabled )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "-->LayoutText\n" );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  box size %f, %f\n", layoutParameters.boundingBox.width, layoutParameters.boundingBox.height );

    if( 0u == layoutParameters.numberOfGlyphs )
    {
      // Add an extra line if the last character is a new paragraph character and the last line doesn't have zero characters.
      if( layoutParameters.isLastNewParagraph )
      {
        Length numberOfLines = lines.Count();
        if( 0u != numberOfLines )
        {
          const LineRun& lastLine = *( lines.End() - 1u );

          if( 0u != lastLine.characterRun.numberOfCharacters )
          {
            // Need to add a new line with no characters but with height to increase the layoutSize.height
            LineRun newLine;
            Initialize( newLine );
            lines.PushBack( newLine );

            UpdateTextLayout( layoutParameters,
                              lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters,
                              lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs,
                              layoutSize,
                              lines.Begin(),
                              numberOfLines );
          }
        }
      }

      // Calculates the layout size.
      UpdateLayoutSize( lines,
                        layoutSize );

      // Rounds upward to avoid a non integer size.
      layoutSize.height = std::ceil( layoutSize.height );

      // Nothing else do if there are no glyphs to layout.
      return false;
    }

    const GlyphIndex lastGlyphPlusOne = layoutParameters.startGlyphIndex + layoutParameters.numberOfGlyphs;

    // In a previous layout, an extra line with no characters may have been added if the text ended with a new paragraph character.
    // This extra line needs to be removed.
    if( 0u != lines.Count() )
    {
      Vector<LineRun>::Iterator lastLine = lines.End() - 1u;

      if( ( 0u == lastLine->characterRun.numberOfCharacters ) &&
          ( lastGlyphPlusOne == layoutParameters.totalNumberOfGlyphs ) )
      {
        lines.Remove( lastLine );
      }
    }

    // Set the first paragraph's direction.
    CharacterDirection paragraphDirection = ( NULL != layoutParameters.characterDirectionBuffer ) ? *layoutParameters.characterDirectionBuffer : !RTL;

    // Whether the layout is being updated or set from scratch.
    const bool updateCurrentBuffer = layoutParameters.numberOfGlyphs < layoutParameters.totalNumberOfGlyphs;

    Vector2* glyphPositionsBuffer = NULL;
    Vector<Vector2> newGlyphPositions;

    LineRun* linesBuffer = NULL;
    Vector<LineRun> newLines;

    // Estimate the number of lines.
    Length linesCapacity = std::max( 1u, layoutParameters.estimatedNumberOfLines );
    Length numberOfLines = 0u;

    if( updateCurrentBuffer )
    {
      newGlyphPositions.Resize( layoutParameters.numberOfGlyphs );
      glyphPositionsBuffer = newGlyphPositions.Begin();

      newLines.Resize( linesCapacity );
      linesBuffer = newLines.Begin();
    }
    else
    {
      glyphPositionsBuffer = glyphPositions.Begin();

      lines.Resize( linesCapacity );
      linesBuffer = lines.Begin();
    }

    float penY = CalculateLineOffset( lines,
                                      layoutParameters.startLineIndex );

    for( GlyphIndex index = layoutParameters.startGlyphIndex; index < lastGlyphPlusOne; )
    {
      CharacterDirection currentParagraphDirection = paragraphDirection;

      // Get the layout for the line.
      LineLayout layout;
      layout.glyphIndex = index;
      GetLineLayoutForBox( layoutParameters,
                           layout,
                           paragraphDirection,
                           false );

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "           glyph index %d\n", layout.glyphIndex );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "       character index %d\n", layout.characterIndex );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "      number of glyphs %d\n", layout.numberOfGlyphs );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  number of characters %d\n", layout.numberOfCharacters );
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "                length %f\n", layout.length );

      if( 0u == layout.numberOfGlyphs )
      {
        // The width is too small and no characters are laid-out.
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--LayoutText width too small!\n\n" );

        lines.Resize( numberOfLines );

        // Rounds upward to avoid a non integer size.
        layoutSize.height = std::ceil( layoutSize.height );

        return false;
      }

      // Set the line position. Discard if ellipsis is enabled and the position exceeds the boundaries
      // of the box.
      penY += layout.ascender;

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  pen y %f\n", penY );

      bool ellipsis = false;
      if( elideTextEnabled )
      {
        // Does the ellipsis of the last line.
        ellipsis = EllipsisLine( layoutParameters,
                                 layout,
                                 layoutSize,
                                 linesBuffer,
                                 glyphPositionsBuffer,
                                 numberOfLines,
                                 penY,
                                 currentParagraphDirection,
                                 isAutoScrollEnabled );
      }

      if( ellipsis )
      {
        // No more lines to layout.
        break;
      }
      else
      {
        // Whether the last line has been laid-out.
        const bool isLastLine = index + layout.numberOfGlyphs == layoutParameters.totalNumberOfGlyphs;

        if( numberOfLines == linesCapacity )
        {
          // Reserve more space for the next lines.
          linesBuffer = ResizeLinesBuffer( lines,
                                           newLines,
                                           linesCapacity,
                                           updateCurrentBuffer );
        }

        // Updates the current text's layout with the line's layout.
        UpdateTextLayout( layoutParameters,
                          layout,
                          layoutSize,
                          linesBuffer,
                          index,
                          numberOfLines,
                          isLastLine );

        const GlyphIndex nextIndex = index + layout.numberOfGlyphs;

        if( ( nextIndex == layoutParameters.totalNumberOfGlyphs ) &&
            layoutParameters.isLastNewParagraph &&
            ( mLayout == MULTI_LINE_BOX ) )
        {
          // The last character of the text is a new paragraph character.
          // An extra line with no characters is added to increase the text's height
          // in order to place the cursor.

          if( numberOfLines == linesCapacity )
          {
            // Reserve more space for the next lines.
            linesBuffer = ResizeLinesBuffer( lines,
                                             newLines,
                                             linesCapacity,
                                             updateCurrentBuffer );
          }

          UpdateTextLayout( layoutParameters,
                            layout.characterIndex + layout.numberOfCharacters,
                            index + layout.numberOfGlyphs,
                            layoutSize,
                            linesBuffer,
                            numberOfLines );
        } // whether to add a last line.

        // Sets the positions of the glyphs.
        SetGlyphPositions( layoutParameters.glyphsBuffer + index,
                           layout.numberOfGlyphs,
                           layoutParameters.outlineWidth,
                           layoutParameters.interGlyphExtraAdvance,
                           glyphPositionsBuffer + index - layoutParameters.startGlyphIndex );

        // Updates the vertical pen's position.
        penY += -layout.descender + layout.lineSpacing + mDefaultLineSpacing;

        // Increase the glyph index.
        index = nextIndex;
      } // no ellipsis
    } // end for() traversing glyphs.

    if( updateCurrentBuffer )
    {
      glyphPositions.Insert( glyphPositions.Begin() + layoutParameters.startGlyphIndex,
                             newGlyphPositions.Begin(),
                             newGlyphPositions.End() );
      glyphPositions.Resize( layoutParameters.totalNumberOfGlyphs );

      newLines.Resize( numberOfLines );

      // Current text's layout size adds only the newly laid-out lines.
      // Updates the layout size with the previously laid-out lines.
      UpdateLayoutSize( lines,
                        layoutSize );

      if( 0u != newLines.Count() )
      {
        const LineRun& lastLine = *( newLines.End() - 1u );

        const Length characterOffset = lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters;
        const Length glyphOffset = lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs;

        // Update the indices of the runs before the new laid-out lines are inserted.
        UpdateLineIndexOffsets( layoutParameters,
                                lines,
                                characterOffset,
                                glyphOffset );

        // Insert the lines.
        lines.Insert( lines.Begin() + layoutParameters.startLineIndex,
                      newLines.Begin(),
                      newLines.End() );
      }
    }
    else
    {
      lines.Resize( numberOfLines );
    }

    // Rounds upward to avoid a non integer size.
    layoutSize.height = std::ceil( layoutSize.height );

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "<--LayoutText\n\n" );

    return true;
  }

  void ReLayoutRightToLeftLines( const Parameters& layoutParameters,
                                 CharacterIndex startIndex,
                                 Length numberOfCharacters,
                                 Vector<Vector2>& glyphPositions )
  {
    const CharacterIndex lastCharacterIndex = startIndex + numberOfCharacters;

    // Traverses the paragraphs with right to left characters.
    for( LineIndex lineIndex = 0u; lineIndex < layoutParameters.numberOfBidirectionalInfoRuns; ++lineIndex )
    {
      const BidirectionalLineInfoRun& bidiLine = *( layoutParameters.lineBidirectionalInfoRunsBuffer + lineIndex );

      if( startIndex >= bidiLine.characterRun.characterIndex + bidiLine.characterRun.numberOfCharacters )
      {
        // Do not reorder the line if it has been already reordered.
        continue;
      }

      if( bidiLine.characterRun.characterIndex >= lastCharacterIndex )
      {
        // Do not reorder the lines after the last requested character.
        break;
      }

      const CharacterIndex characterVisualIndex = bidiLine.characterRun.characterIndex + *bidiLine.visualToLogicalMap;
      const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + *( layoutParameters.charactersToGlyphsBuffer + characterVisualIndex ) );

      float penX = -glyph.xBearing + layoutParameters.outlineWidth + mCursorWidth;

      Vector2* glyphPositionsBuffer = glyphPositions.Begin();

      // Traverses the characters of the right to left paragraph.
      for( CharacterIndex characterLogicalIndex = 0u;
           characterLogicalIndex < bidiLine.characterRun.numberOfCharacters;
           ++characterLogicalIndex )
      {
        // Convert the character in the logical order into the character in the visual order.
        const CharacterIndex characterVisualIndex = bidiLine.characterRun.characterIndex + *( bidiLine.visualToLogicalMap + characterLogicalIndex );

        // Get the number of glyphs of the character.
        const Length numberOfGlyphs = *( layoutParameters.glyphsPerCharacterBuffer + characterVisualIndex );

        for( GlyphIndex index = 0u; index < numberOfGlyphs; ++index )
        {
          // Convert the character in the visual order into the glyph in the visual order.
          const GlyphIndex glyphIndex = *( layoutParameters.charactersToGlyphsBuffer + characterVisualIndex ) + index;

          DALI_ASSERT_DEBUG( 0u <= glyphIndex && glyphIndex < layoutParameters.totalNumberOfGlyphs );

          const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + glyphIndex );
          Vector2& position = *( glyphPositionsBuffer + glyphIndex );

          position.x = std::round( penX + glyph.xBearing );
          penX += ( glyph.advance + layoutParameters.interGlyphExtraAdvance );
        }
      }
    }
  }

  void Align( const Size& size,
              CharacterIndex startIndex,
              Length numberOfCharacters,
              Text::HorizontalAlignment::Type horizontalAlignment,
              Vector<LineRun>& lines,
              float& alignmentOffset,
              Dali::LayoutDirection::Type layoutDirection,
              bool matchSystemLanguageDirection )
  {
    const CharacterIndex lastCharacterPlusOne = startIndex + numberOfCharacters;

    alignmentOffset = MAX_FLOAT;
    // Traverse all lines and align the glyphs.
    for( Vector<LineRun>::Iterator it = lines.Begin(), endIt = lines.End();
         it != endIt;
         ++it )
    {
      LineRun& line = *it;

      if( line.characterRun.characterIndex < startIndex )
      {
        // Do not align lines which have already been aligned.
        continue;
      }

      if( line.characterRun.characterIndex > lastCharacterPlusOne )
      {
        // Do not align lines beyond the last laid-out character.
        break;
      }

      if( line.characterRun.characterIndex == lastCharacterPlusOne && !isEmptyLineAtLast( lines, it ) )
      {
        // Do not align lines beyond the last laid-out character unless the line is last and empty.
        break;
      }

      // Calculate the line's alignment offset accordingly with the align option,
      // the box width, line length, and the paragraph's direction.
      CalculateHorizontalAlignment( size.width,
                                    horizontalAlignment,
                                    line,
                                    layoutDirection,
                                    matchSystemLanguageDirection );

      // Updates the alignment offset.
      alignmentOffset = std::min( alignmentOffset, line.alignmentOffset );
    }
  }

  void CalculateHorizontalAlignment( float boxWidth,
                                     HorizontalAlignment::Type horizontalAlignment,
                                     LineRun& line,
                                     Dali::LayoutDirection::Type layoutDirection,
                                     bool matchSystemLanguageDirection )
  {
    line.alignmentOffset = 0.f;
    const bool isLineRTL = RTL == line.direction;
    // Whether to swap the alignment.
    // Swap if the line is RTL and is not required to match the direction of the system's language or if it's required to match the direction of the system's language and it's RTL.
    bool isLayoutRTL = isLineRTL;
    float lineLength = line.width;

    // match align for system language direction
    if( matchSystemLanguageDirection )
    {
      // Swap the alignment type if the line is right to left.
      isLayoutRTL = layoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }
    // Calculate the horizontal line offset.
    switch( horizontalAlignment )
    {
      case HorizontalAlignment::BEGIN:
      {
        if( isLayoutRTL )
        {
          if( isLineRTL )
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        else
        {
          line.alignmentOffset = 0.f;

          if( isLineRTL )
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        break;
      }
      case HorizontalAlignment::CENTER:
      {
        line.alignmentOffset = 0.5f * ( boxWidth - lineLength );

        if( isLineRTL )
        {
          line.alignmentOffset -= line.extraLength;
        }

        line.alignmentOffset = std::floor( line.alignmentOffset ); // floor() avoids pixel alignment issues.
        break;
      }
      case HorizontalAlignment::END:
      {
        if( isLayoutRTL )
        {
          line.alignmentOffset = 0.f;

          if( isLineRTL )
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        else
        {
          if( isLineRTL )
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        break;
      }
    }
  }

  void Initialize( LineRun& line )
  {
    line.glyphRun.glyphIndex = 0u;
    line.glyphRun.numberOfGlyphs = 0u;
    line.characterRun.characterIndex = 0u;
    line.characterRun.numberOfCharacters = 0u;
    line.width = 0.f;
    line.ascender = 0.f;
    line.descender = 0.f;
    line.extraLength = 0.f;
    line.alignmentOffset = 0.f;
    line.direction = !RTL;
    line.ellipsis = false;
    line.lineSpacing = mDefaultLineSpacing;
  }

  Type mLayout;
  float mCursorWidth;
  float mDefaultLineSpacing;

  IntrusivePtr<Metrics> mMetrics;
};

Engine::Engine()
: mImpl( NULL )
{
  mImpl = new Engine::Impl();
}

Engine::~Engine()
{
  delete mImpl;
}

void Engine::SetMetrics( MetricsPtr& metrics )
{
  mImpl->mMetrics = metrics;
}

void Engine::SetLayout( Type layout )
{
  mImpl->mLayout = layout;
}

Engine::Type Engine::GetLayout() const
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetLayout[%d]\n", mImpl->mLayout);
  return mImpl->mLayout;
}

void Engine::SetCursorWidth( int width )
{
  mImpl->mCursorWidth = static_cast<float>( width );
}

int Engine::GetCursorWidth() const
{
  return static_cast<int>( mImpl->mCursorWidth );
}

bool Engine::LayoutText( const Parameters& layoutParameters,
                         Vector<Vector2>& glyphPositions,
                         Vector<LineRun>& lines,
                         Size& layoutSize,
                         bool elideTextEnabled,
                         bool& isAutoScrollEnabled )
{
  return mImpl->LayoutText( layoutParameters,
                            glyphPositions,
                            lines,
                            layoutSize,
                            elideTextEnabled,
                            isAutoScrollEnabled );
}

void Engine::ReLayoutRightToLeftLines( const Parameters& layoutParameters,
                                       CharacterIndex startIndex,
                                       Length numberOfCharacters,
                                       Vector<Vector2>& glyphPositions )
{
  mImpl->ReLayoutRightToLeftLines( layoutParameters,
                                   startIndex,
                                   numberOfCharacters,
                                   glyphPositions );
}

void Engine::Align( const Size& size,
                    CharacterIndex startIndex,
                    Length numberOfCharacters,
                    Text::HorizontalAlignment::Type horizontalAlignment,
                    Vector<LineRun>& lines,
                    float& alignmentOffset,
                    Dali::LayoutDirection::Type layoutDirection,
                    bool matchSystemLanguageDirection )
{
  mImpl->Align( size,
                startIndex,
                numberOfCharacters,
                horizontalAlignment,
                lines,
                alignmentOffset,
                layoutDirection,
                matchSystemLanguageDirection );
}

void Engine::SetDefaultLineSpacing( float lineSpacing )
{
  mImpl->mDefaultLineSpacing = lineSpacing;
}

float Engine::GetDefaultLineSpacing() const
{
  return mImpl->mDefaultLineSpacing;
}

} // namespace Layout

} // namespace Text

} // namespace Toolkit

} // namespace Dali
