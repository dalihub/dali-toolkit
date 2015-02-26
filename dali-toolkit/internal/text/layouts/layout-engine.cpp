/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Stores temporary layout info of the line.
 */
struct LineLayout
{
  GlyphIndex     glyphIndex;         ///< Index of the first glyph to be laid-out.
  CharacterIndex characterIndex;     ///< Index of the first character to be laid-out.
  Length         numberOfCharacters; ///< The number of characters which fit in one line.
  Length         numberOfGlyphs;     ///< The number of glyph which fit in one line.
  float          length;             ///< The length of the glyphs which fit in one line.
  float          wsLengthEndOfLine;  ///< The length of the white spaces at the end of the line.
  float          height;             ///< The maximum height of all fonts in the line.
  float          ascender;           ///< The maximum ascender of all fonts in the line.
};

struct LayoutEngine::Impl
{
  Impl()
  : mLayout( LayoutEngine::SINGLE_LINE_BOX )
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }

  /**
   * Retrieves the line layout for a given box width.
   */
  void GetLineLayoutForBox( const LayoutParameters& parameters,
                            LineLayout& lineLayout )
  {
    // Initializes the line layout.
    lineLayout.numberOfCharacters = 0u;
    lineLayout.numberOfGlyphs = 0u;
    lineLayout.length = 0.f;
    lineLayout.wsLengthEndOfLine = 0.f;
    lineLayout.height = 0.f;
    lineLayout.ascender = 0.f;

    // Get the last glyph index.
    const GlyphIndex lastGlyphIndex = parameters.totalNumberOfGlyphs - 1u;

    FontId lastFontId = 0u;
    for( GlyphIndex glyphIndex = lineLayout.glyphIndex;
         glyphIndex < parameters.totalNumberOfGlyphs;
         ++glyphIndex )
    {
      // Get the glyph info.
      const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + glyphIndex );

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length charactersPerGlyph = *( parameters.charactersPerGlyphBuffer + glyphIndex );

      // Increase the number of characters.
      lineLayout.numberOfCharacters += charactersPerGlyph;

      // Increase the number of glyphs.
      lineLayout.numberOfGlyphs++;

      // Increase the accumulated length.
      lineLayout.length += ( glyphIndex == lastGlyphIndex ) ? glyphInfo.width : glyphInfo.advance;

      if( lastFontId != glyphInfo.fontId )
      {
        Text::FontMetrics fontMetrics;
        mFontClient.GetFontMetrics( glyphInfo.fontId, fontMetrics );

        // Sets the maximum height.
        if( fontMetrics.height > lineLayout.height )
        {
          lineLayout.height = fontMetrics.height;
        }

        // Sets the maximum ascender.
        if( fontMetrics.ascender > lineLayout.ascender )
        {
          lineLayout.ascender = fontMetrics.ascender;
        }

        lastFontId = glyphInfo.fontId;
      }
    }
  }

  /**
   * Retrieves the line layout for a given box width.
   */
  void GetMultiLineLayoutForBox( const LayoutParameters& parameters,
                                 LineLayout& lineLayout )
  {
    // Initializes the line layout.
    lineLayout.numberOfCharacters = 0u;
    lineLayout.numberOfGlyphs = 0u;
    lineLayout.length = 0.f;
    lineLayout.wsLengthEndOfLine = 0.f;
    lineLayout.height = 0.f;
    lineLayout.ascender = 0.f;

    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;
    tmpLineLayout.numberOfCharacters = 0u;
    tmpLineLayout.numberOfGlyphs = 0u;
    tmpLineLayout.length = 0.f;
    tmpLineLayout.wsLengthEndOfLine = 0.f;
    tmpLineLayout.height = 0.f;
    tmpLineLayout.ascender = 0.f;

    // Get the last glyph index.
    const GlyphIndex lastGlyphIndex = parameters.totalNumberOfGlyphs - 1u;

    FontId lastFontId = 0u;
    for( GlyphIndex glyphIndex = lineLayout.glyphIndex;
         glyphIndex < parameters.totalNumberOfGlyphs;
         ++glyphIndex )
    {
      // Get the glyph info.
      const GlyphInfo& glyphInfo = *( parameters.glyphsBuffer + glyphIndex );

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length charactersPerGlyph = *( parameters.charactersPerGlyphBuffer + glyphIndex );
      const CharacterIndex characterFirstIndex = *( parameters.glyphsToCharactersBuffer + glyphIndex );
      const CharacterIndex characterLastIndex = characterFirstIndex + ( ( 1u > charactersPerGlyph ) ? 0u : charactersPerGlyph - 1u );

      // Get the line break info for the current character.
      const LineBreakInfo lineBreakInfo = *( parameters.lineBreakInfoBuffer + characterLastIndex );

      // Get the word break info for the current character.
      const WordBreakInfo wordBreakInfo = *( parameters.wordBreakInfoBuffer + characterLastIndex );

      // Increase the number of characters.
      tmpLineLayout.numberOfCharacters += charactersPerGlyph;

      // Increase the number of glyphs.
      tmpLineLayout.numberOfGlyphs++;

      // Increase the accumulated length.
      tmpLineLayout.length += ( glyphIndex == lastGlyphIndex ) ? glyphInfo.width : glyphInfo.advance;

      // Check if the accumulated length fits in the width of the box.
      if( lineLayout.length + tmpLineLayout.length > parameters.boundingBox.width )
      {
        // Current word does not fit in the box's width.
        return;
      }

      if( TextAbstraction::LINE_MUST_BREAK == lineBreakInfo )
      {
        if( glyphIndex == lastGlyphIndex )
        {
          // Must break the line. Update the line layout and return.
          lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
          lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;
          lineLayout.length += tmpLineLayout.length;
          lineLayout.wsLengthEndOfLine = tmpLineLayout.wsLengthEndOfLine;

          if( tmpLineLayout.height > lineLayout.height )
          {
            lineLayout.height = tmpLineLayout.height;
          }

          if( tmpLineLayout.ascender > lineLayout.ascender )
          {
            lineLayout.ascender = tmpLineLayout.ascender;
          }
        }

        tmpLineLayout.numberOfCharacters = 0u;
        tmpLineLayout.numberOfGlyphs = 0u;
        tmpLineLayout.length = 0u;
        tmpLineLayout.wsLengthEndOfLine = 0u;
        tmpLineLayout.height = 0.f;
        tmpLineLayout.ascender = 0.f;
        return;
      }

      if( TextAbstraction::WORD_BREAK == wordBreakInfo )
      {
        // Current glyph is the last one of the current word.
        // Add the temporal layout to the current one.
        lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
        lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;
        lineLayout.length += tmpLineLayout.length;
        lineLayout.wsLengthEndOfLine = tmpLineLayout.wsLengthEndOfLine;

        if( tmpLineLayout.height > lineLayout.height )
        {
          lineLayout.height = tmpLineLayout.height;
        }

        if( tmpLineLayout.ascender > lineLayout.ascender )
        {
          lineLayout.ascender = tmpLineLayout.ascender;
        }

        tmpLineLayout.numberOfCharacters = 0u;
        tmpLineLayout.numberOfGlyphs = 0u;
        tmpLineLayout.length = 0u;
        tmpLineLayout.wsLengthEndOfLine = 0u;
        tmpLineLayout.height = 0.f;
        tmpLineLayout.ascender = 0.f;
      }

      if( lastFontId != glyphInfo.fontId )
      {
        Text::FontMetrics fontMetrics;
        mFontClient.GetFontMetrics( glyphInfo.fontId, fontMetrics );

        // Sets the maximum height.
        if( fontMetrics.height > tmpLineLayout.height )
        {
          tmpLineLayout.height = fontMetrics.height;
        }

        // Sets the maximum ascender.
        if( fontMetrics.ascender > tmpLineLayout.ascender )
        {
          tmpLineLayout.ascender = fontMetrics.ascender;
        }

        lastFontId = glyphInfo.fontId;
      }
    }
  }

  bool LayoutText( const LayoutParameters& layoutParameters,
                   Vector<Vector2>& glyphPositions,
                   Vector<LineRun>& lines,
                   Size& actualSize )
  {
    // TODO Switch between different layouts
    bool update = false;

    switch( mLayout )
    {
      case LayoutEngine::SINGLE_LINE_BOX:
      {
        update = SingleLineLayout( layoutParameters,
                                   glyphPositions,
                                   lines,
                                   actualSize );
        break;
      }
      case LayoutEngine::MULTI_LINE_BOX:
      {
        update = MultiLineLayout( layoutParameters,
                                  glyphPositions,
                                  lines,
                                  actualSize );
        break;
      }
      default:
        break;
    }

    return update;
  }

  // TODO - Rewrite this to handle bidi
  bool SingleLineLayout( const LayoutParameters& layoutParameters,
                         Vector<Vector2>& glyphPositions,
                         Vector<LineRun>& lines,
                         Size& actualSize )
  {
    LineLayout layout;
    layout.glyphIndex = 0u;
    GetLineLayoutForBox( layoutParameters,
                         layout );

    // Create a line run and add it to the lines.
    const GlyphIndex lastGlyphIndex = layoutParameters.totalNumberOfGlyphs - 1u;

    LineRun lineRun;
    lineRun.glyphIndex = 0u;
    lineRun.numberOfGlyphs = layoutParameters.totalNumberOfGlyphs;
    lineRun.characterRun.characterIndex = 0u;
    lineRun.characterRun.numberOfCharacters = *( layoutParameters.glyphsToCharactersBuffer + lastGlyphIndex ) + *( layoutParameters.charactersPerGlyphBuffer + lastGlyphIndex );
    lineRun.lineSize.width = layout.length;
    lineRun.lineSize.height = layout.height;

    lines.PushBack( lineRun );

    // Update the actual size.
    actualSize.width = layout.length;
    actualSize.height = layout.height;

    float penX = 0.f;
    float penY = layout.height;

    Vector2* glyphPositionsBuffer = glyphPositions.Begin();
    for( GlyphIndex glyphIndex = 0u; glyphIndex < layout.numberOfGlyphs; ++glyphIndex )
    {
      const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + glyphIndex );
      Vector2& position = *( glyphPositionsBuffer + glyphIndex );

      position.x = penX + glyph.xBearing;
      position.y = penY - glyph.yBearing;

      penX += glyph.advance;
    }

    return true;
  }

  // TODO - Rewrite this to handle bidi
  bool MultiLineLayout( const LayoutParameters& layoutParameters,
                        Vector<Vector2>& glyphPositions,
                        Vector<LineRun>& lines,
                        Size& actualSize )
  {
    float penY = 0.f;
    for( GlyphIndex index = 0u; index < layoutParameters.totalNumberOfGlyphs; )
    {
      float penX = 0.f;

      // Get the layout for the line.
      LineLayout layout;
      layout.glyphIndex = index;
      GetMultiLineLayoutForBox( layoutParameters,
                                layout );

      if( 0u == layout.numberOfGlyphs )
      {
        // The width is too small and no characters are laid-out.
        return false;
      }

      // Create a line run and add it to the lines.
      const GlyphIndex lastGlyphIndex = index + layout.numberOfGlyphs - 1u;

      LineRun lineRun;
      lineRun.glyphIndex = index;
      lineRun.numberOfGlyphs = layout.numberOfGlyphs;
      lineRun.characterRun.characterIndex = *( layoutParameters.glyphsToCharactersBuffer + index );
      lineRun.characterRun.numberOfCharacters = ( *( layoutParameters.glyphsToCharactersBuffer + lastGlyphIndex ) + *( layoutParameters.charactersPerGlyphBuffer + lastGlyphIndex ) ) - lineRun.characterRun.characterIndex;
      lineRun.lineSize.width = layout.length;
      lineRun.lineSize.height = layout.height;

      lines.PushBack( lineRun );

      // Update the actual size.
      if( layout.length > actualSize.width )
      {
        actualSize.width = layout.length;
      }

      actualSize.height += layout.height;

      // Traverse the glyphs and set the positions.

      penY += layout.height;

      Vector2* glyphPositionsBuffer = glyphPositions.Begin();
      for( GlyphIndex i = index; i < index + layout.numberOfGlyphs; ++i )
      {
        const GlyphInfo& glyph = *( layoutParameters.glyphsBuffer + i );
        Vector2& position = *( glyphPositionsBuffer + i );

        position.x = penX + glyph.xBearing;
        position.y = penY - glyph.yBearing;

        penX += glyph.advance;
      }

      // Increase the glyph index.
      index += layout.numberOfGlyphs;
    }

    return true;
  }

  LayoutEngine::Layout mLayout;

  TextAbstraction::FontClient mFontClient;
};

LayoutEngine::LayoutEngine()
: mImpl( NULL )
{
  mImpl = new LayoutEngine::Impl();
}

LayoutEngine::~LayoutEngine()
{
  delete mImpl;
}

void LayoutEngine::SetLayout( Layout layout )
{
  mImpl->mLayout = layout;
}

unsigned int LayoutEngine::GetLayout() const
{
  return mImpl->mLayout;
}

bool LayoutEngine::LayoutText( const LayoutParameters& layoutParameters,
                               Vector<Vector2>& glyphPositions,
                               Vector<LineRun>& lines,
                               Size& actualSize )
{
  return mImpl->LayoutText( layoutParameters,
                            glyphPositions,
                            lines,
                            actualSize );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
