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
#include <dali-toolkit/public-api/text/text-controller.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/character-set-conversion.h>
#include <dali-toolkit/public-api/text/logical-model.h>
#include <dali-toolkit/public-api/text/text-view.h>
#include <dali-toolkit/public-api/text/visual-model.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Controller::Impl
{
  Impl()
  {
    mLogicalModel = LogicalModel::New();
    mVisualModel  = VisualModel::New();

    mView.SetVisualModel( mVisualModel );

    mFontClient = TextAbstraction::FontClient::Get();
  }

  LogicalModelPtr mLogicalModel;
  VisualModelPtr  mVisualModel;

  View mView;

  TextAbstraction::FontClient mFontClient;
};

ControllerPtr Controller::New()
{
  return ControllerPtr( new Controller() );
}

void Controller::SetText( const std::string& text )
{
  //  Convert text into UTF-32
  Vector<Character> utf32Characters;
  utf32Characters.Resize( text.size() );

  // This is a bit horrible but std::string returns a (signed) char*
  const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

  Length characterCount = Utf8ToUtf32( utf8, text.size(), &utf32Characters[0] );

  // Manipulate the logical model
  mImpl->mLogicalModel->SetText( &utf32Characters[0], characterCount );

  UpdateVisualModel();
}

View& Controller::GetView()
{
  return mImpl->mView;
}

Controller::~Controller()
{
  delete mImpl;
}

Controller::Controller()
: mImpl( NULL )
{
  mImpl = new Controller::Impl();
}

// TODO - Move this with LayoutEngine
void Controller::UpdateVisualModel()
{
  if( mImpl->mLogicalModel &&
      mImpl->mVisualModel )
  {
    const LogicalModel& logicalModel = *(mImpl->mLogicalModel);
    VisualModel& visualModel = *(mImpl->mVisualModel);

    TextAbstraction::FontId fontId = mImpl->mFontClient.GetFontId( "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", 13*64 );

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
      GlyphIndex glyphIndex = mImpl->mFontClient.GetGlyphIndex( fontId, charcode );

      glyphs.PushBack( GlyphInfo(fontId, glyphIndex) );
      characterIndices.PushBack( 1 );
    }

    if( mImpl->mFontClient.GetGlyphMetrics( &glyphs[0], glyphs.Size() ) )
    {
      visualModel.SetGlyphs( &glyphs[0],
                             &characterIndices[0],
                             &charactersPerGlyph[0],
                             characterCount );

      UpdateVisualPositions();
    }
  }
}

// TODO - Move this with LayoutEngine
void Controller::UpdateVisualPositions()
{
  if( mImpl->mVisualModel )
  {
    VisualModel& visualModel = *(mImpl->mVisualModel);

    Length glyphCount = visualModel.GetNumberOfGlyphs();

    std::vector<Vector2> glyphPositions;
    glyphPositions.reserve( glyphCount );

    if( glyphCount > 0 )
    {
      // FIXME Single font assumption
      Text::FontMetrics fontMetrics;
      GlyphInfo firstGlyph;
      visualModel.GetGlyphs( 0, &firstGlyph, 1 );
      mImpl->mFontClient.GetFontMetrics( firstGlyph.fontId, fontMetrics );

      int penX( 0 );
      int penY( fontMetrics.ascender ); // Move to baseline

      for( unsigned int i=0; i<glyphCount; ++i )
      {
        GlyphInfo glyph;
        visualModel.GetGlyphs( i, &glyph, 1 );

        if( glyph.width > 0 &&
            glyph.height > 0 ) // Skip whitespace
        {
          glyphPositions.push_back( Vector2( penX + glyph.xBearing,
                                             penY - glyph.yBearing ) );
        }

        penX += glyph.advance;
      }

      visualModel.SetGlyphPositions( &glyphPositions[0], glyphCount );
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
