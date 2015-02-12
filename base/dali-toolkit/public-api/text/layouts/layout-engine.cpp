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
#include <dali-toolkit/public-api/text/layouts/layout-engine.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/logical-model.h>
#include <dali-toolkit/public-api/text/visual-model.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct LayoutEngine::Impl
{
  Impl()
  : mLayout( LayoutEngine::SINGLE_LINE_BOX )
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }

  void UpdateVisualModel( const Vector2& boundingBox, const LogicalModel& logicalModel, VisualModel& visualModel )
  {
    // TODO Switch between different layouts

    TextAbstraction::FontId fontId = mFontClient.GetFontId( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", 13*64 );

    const Length characterCount = logicalModel.GetNumberOfCharacters();

    Vector<GlyphInfo> glyphs;
    glyphs.Reserve( characterCount );

    Vector<CharacterIndex> characterIndices;
    characterIndices.Reserve( characterCount );

    std::vector<Length> charactersPerGlyph;
    charactersPerGlyph.assign( characterCount, 1 );

    for( unsigned int i=0; i<characterCount; ++i )
    {
      Character charcode;
      logicalModel.GetText( i, &charcode, 1 );

      // TODO - Perform shaping to get correct glyph indices
      GlyphIndex glyphIndex = mFontClient.GetGlyphIndex( fontId, charcode );

      glyphs.PushBack( GlyphInfo(fontId, glyphIndex) );
      characterIndices.PushBack( 1 );
    }

    if( mFontClient.GetGlyphMetrics( &glyphs[0], glyphs.Size() ) )
    {
      visualModel.SetGlyphs( &glyphs[0],
                             &characterIndices[0],
                             &charactersPerGlyph[0],
                             characterCount );

      UpdateGlyphPositions( boundingBox, visualModel );
    }
  }

  void UpdateGlyphPositions( const Vector2& boundingBox, VisualModel& visualModel )
  {
    if( LayoutEngine::SINGLE_LINE_BOX == mLayout )
    {
      SingleLineLayout( boundingBox, visualModel );
    }
    else
    {
      MultiLineLayout( boundingBox, visualModel );
    }
  }

  // TODO - Rewrite this to handle bidi
  void SingleLineLayout( const Vector2& boundingBox, VisualModel& visualModel )
  {
    Length glyphCount = visualModel.GetNumberOfGlyphs();

    std::vector<Vector2> glyphPositions;
    glyphPositions.reserve( glyphCount );

    if( glyphCount > 0 )
    {
      // FIXME Single font assumption
      Text::FontMetrics fontMetrics;
      GlyphInfo firstGlyph;
      visualModel.GetGlyphs( 0, &firstGlyph, 1 );
      mFontClient.GetFontMetrics( firstGlyph.fontId, fontMetrics );

      float penX( 0 );
      float penY( fontMetrics.ascender ); // Move to baseline

      for( unsigned int i=0; i<glyphCount; ++i )
      {
        GlyphInfo glyph;
        visualModel.GetGlyphs( i, &glyph, 1 );

        glyphPositions.push_back( Vector2( penX + glyph.xBearing,
                                           penY - glyph.yBearing ) );

        penX += glyph.advance;
      }

      visualModel.SetGlyphPositions( &glyphPositions[0], glyphCount );
    }
  }

  // TODO - Rewrite this to handle bidi
  void MultiLineLayout( const Vector2& boundingBox, VisualModel& visualModel )
  {
    Length glyphCount = visualModel.GetNumberOfGlyphs();

    std::vector<Vector2> glyphPositions;
    glyphPositions.reserve( glyphCount );

    if( glyphCount > 0 )
    {
      // FIXME Single font assumption
      Text::FontMetrics fontMetrics;
      GlyphInfo firstGlyph;
      visualModel.GetGlyphs( 0, &firstGlyph, 1 );
      mFontClient.GetFontMetrics( firstGlyph.fontId, fontMetrics );

      float penX( 0 );
      float penY( fontMetrics.ascender ); // Move to baseline

      unsigned int i=0;
      while( i < glyphCount )
      {
        // Skip initial whitespace
        for( ; i<glyphCount; ++i )
        {
          GlyphInfo glyph;
          visualModel.GetGlyphs( i, &glyph, 1 );

          if( glyph.width  > 0 &&
              glyph.height > 0 )
          {
            break;
          }
          else
          {
            glyphPositions.push_back( Vector2( penX + glyph.xBearing,
                                               penY - glyph.yBearing ) );
          }
        }

        // Find last glyph for the next line
        unsigned int endIndex = i;
        float endPenX = penX;
        unsigned int j=i;
        for( ; j<glyphCount; ++j )
        {
          GlyphInfo glyph;
          visualModel.GetGlyphs( j, &glyph, 1 );

          endPenX += glyph.advance;

          if( glyph.width  <= 0 ||
              glyph.height <= 0 )
          {
            // Potential line end found
            endIndex = j;
          }
          else if( endPenX > boundingBox.width )
          {
            break;
          }
        }

        // If end of text or no whitespace found
        if( glyphCount == j ||
            endIndex == i )
        {
          endIndex = j;
        }

        for( ; i<endIndex; ++i )
        {
          GlyphInfo glyph;
          visualModel.GetGlyphs( i, &glyph, 1 );

          glyphPositions.push_back( Vector2( penX + glyph.xBearing,
                                             penY - glyph.yBearing ) );

          penX += glyph.advance;
        }

        // Go to next line
        penX = 0;
        penY += fontMetrics.height;
      }

      visualModel.SetGlyphPositions( &glyphPositions[0], glyphCount );
    }
  }

  unsigned int mLayout;

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

void LayoutEngine::UpdateVisualModel( const Vector2& boundingBox, const LogicalModel& logicalModel, VisualModel& visualModel )
{
  mImpl->UpdateVisualModel( boundingBox, logicalModel, visualModel );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
