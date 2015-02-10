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

// EXTERNAL INCLUDES
#include <dali/public-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/character-set-conversion.h>
#include <dali-toolkit/public-api/text/layouts/layout-engine.h>
#include <dali-toolkit/public-api/text/logical-model.h>
#include <dali-toolkit/public-api/text/multi-language-support.h>
#include <dali-toolkit/public-api/text/script-run.h>
#include <dali-toolkit/public-api/text/shaper.h>
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
  : mNewTextArrived( false )
  {
    mLogicalModel = LogicalModel::New();
    mVisualModel  = VisualModel::New();

    mView.SetVisualModel( mVisualModel );

    mFontClient = TextAbstraction::FontClient::Get();
  }

  std::string mNewText;
  bool mNewTextArrived;

  LogicalModelPtr mLogicalModel;
  VisualModelPtr  mVisualModel;

  View mView;

  LayoutEngine mLayoutEngine;

  TextAbstraction::FontClient mFontClient;
};

ControllerPtr Controller::New()
{
  return ControllerPtr( new Controller() );
}

void Controller::SetText( const std::string& text )
{
  // Keep until size negotiation
  mImpl->mNewText = text;
  mImpl->mNewTextArrived = true;
}

bool Controller::Relayout( const Vector2& size )
{
  bool viewUpdated( false );

  if( mImpl->mNewTextArrived )
  {
    std::string& text = mImpl->mNewText;

    //  Convert text into UTF-32
    Vector<Character> utf32Characters;
    utf32Characters.Resize( text.size() );

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>( text.c_str() );

    Length characterCount = Utf8ToUtf32( utf8, text.size(), &utf32Characters[0] );
    utf32Characters.Resize( characterCount );

    Vector<ScriptRun> scripts;
    MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

    multilanguageSupport.SetScripts( utf32Characters,
                                     scripts );

    Vector<FontRun> fonts;
    multilanguageSupport.ValidateFonts( utf32Characters,
                                        scripts,
                                        fonts );

    Vector<LineBreakInfo> lineBreakInfo;
    lineBreakInfo.Resize( characterCount, TextAbstraction::LINE_NO_BREAK );

    Vector<GlyphInfo> glyphs;
    Vector<CharacterIndex> characterIndices;
    Vector<Length> charactersPerGlyph;

    ShapeText( utf32Characters,
               lineBreakInfo,
               scripts,
               fonts,
               glyphs,
               characterIndices,
               charactersPerGlyph );

    // Manipulate the logical model
    mImpl->mLogicalModel->SetText( &utf32Characters[0], characterCount );
    mImpl->mLogicalModel->SetLineBreakInfo( &lineBreakInfo[0], characterCount );
    mImpl->mLogicalModel->SetScripts( &scripts[0], scripts.Count() );
    mImpl->mLogicalModel->SetFonts( &fonts[0], fonts.Count() );

    if( TextAbstraction::FontClient::Get().GetGlyphMetrics( &glyphs[0], glyphs.Size() ) )
    {
      // Update the visual model
      mImpl->mLayoutEngine.UpdateVisualModel( size,
                                              glyphs,
                                              characterIndices,
                                              charactersPerGlyph,
                                              *mImpl->mVisualModel );
    }

    // Discard temporary text
    mImpl->mNewTextArrived = false;
    text.clear();

    viewUpdated = true;
  }

  return viewUpdated;
}

View& Controller::GetView()
{
  return mImpl->mView;
}

LayoutEngine& Controller::GetLayoutEngine()
{
  return mImpl->mLayoutEngine;
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

} // namespace Text

} // namespace Toolkit

} // namespace Dali
