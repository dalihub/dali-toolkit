/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include "toolkit-text-model.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Frees previously allocated bidirectional resources.
 *
 * @param[in] bidirectionalLineInfo Bidirectional info per line.
 * @param[in] index Index to the first line with bidirectional info to be freed.
 */
void FreeBidirectionalLineInfoResources( Vector<BidirectionalLineInfoRun> bidirectionalLineInfo,
                                         uint32_t index )
{
  // Free the allocated memory used to store the conversion table in the bidirectional line info run.
  for( Vector<BidirectionalLineInfoRun>::Iterator it = bidirectionalLineInfo.Begin() + index,
         endIt = bidirectionalLineInfo.End();
       it != endIt;
       ++it )
  {
    BidirectionalLineInfoRun& bidiLineInfo = *it;

    free( bidiLineInfo.visualToLogicalMap );
  }
}

/**
 * @brief Clear all the model data except for LogicalModel::mText.
 *
 * @param[in] characterIndex Clear data starting from the index.
 */
void ClearModelData( CharacterIndex characterIndex,
                     LogicalModelPtr logicalModel,
                     VisualModelPtr visualModel )
{
  // n.b. This does not Clear the mText from mLogicalModel

  // Frees previously allocated resources.
  FreeBidirectionalLineInfoResources( logicalModel->mBidirectionalLineInfo, 0u );

  logicalModel->mScriptRuns.Clear();
  logicalModel->mFontRuns.Clear();
  logicalModel->mWordBreakInfo.Clear();
  logicalModel->mBidirectionalParagraphInfo.Clear();
  logicalModel->mCharacterDirections.Clear();
  logicalModel->mBidirectionalLineInfo.Clear();
  visualModel->mGlyphs.Clear();
  visualModel->mGlyphsToCharacters.Clear();
  visualModel->mCharactersToGlyph.Clear();
  visualModel->mCharactersPerGlyph.Clear();
  visualModel->mGlyphsPerCharacter.Clear();
  visualModel->mGlyphPositions.Clear();
  visualModel->mLines.Clear();

  visualModel->ClearCaches();
}

void CreateTextModel( const std::string& text,
                      const Size& textArea,
                      const Vector<FontDescriptionRun>& fontDescriptions,
                      const LayoutOptions& options,
                      Size& layoutSize,
                      LogicalModelPtr& logicalModel,
                      VisualModelPtr& visualModel,
                      MetricsPtr& metrics )
{
  logicalModel = LogicalModel::New();
  visualModel = VisualModel::New();

  // 1) Convert to utf32
  Vector<Character>& utf32Characters = logicalModel->mText;
  utf32Characters.Resize( text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( text.c_str() ),
                                                   text.size(),
                                                   &utf32Characters[0u] );
  utf32Characters.Resize( numberOfCharacters );

  // 2) Set the break and paragraph info.
  Vector<LineBreakInfo>& lineBreakInfo = logicalModel->mLineBreakInfo;
  lineBreakInfo.Resize( numberOfCharacters );

  SetLineBreakInfo( utf32Characters,
                    0u,
                    numberOfCharacters,
                    lineBreakInfo );

  if( 0u == numberOfCharacters )
  {
    // Nothing else to do if the number of characters is zero.
    return;
  }

  // Retrieves the word break info. The word break info is used to layout the text (where to wrap the text in lines).
  Vector<WordBreakInfo>& wordBreakInfo = logicalModel->mWordBreakInfo;
  wordBreakInfo.Resize( numberOfCharacters );

  SetWordBreakInfo( utf32Characters,
                    0u,
                    numberOfCharacters,
                    wordBreakInfo );

  // 3) Set the script info.
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

  Vector<ScriptRun>& scripts = logicalModel->mScriptRuns;
  multilanguageSupport.SetScripts( utf32Characters,
                                   0u,
                                   numberOfCharacters,
                                   scripts );

  // 4) Set the font info
  Vector<FontDescriptionRun>& fontDescriptionRuns = logicalModel->mFontDescriptionRuns;
  fontDescriptionRuns = fontDescriptions;
  Vector<FontRun>& validFonts = logicalModel->mFontRuns;

  // The default font id.
  FontDefaults fontDefaults;
  fontDefaults.mFontDescription.family = "";
  fontDefaults.familyDefined = true;
  fontDefaults.mDefaultPointSize = 12.f;
  fontDefaults.sizeDefined = true;

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  const FontId defaultFontId = fontDefaults.GetFontId( fontClient );

  // Validates the fonts. If there is a character with no assigned font it sets a default one.
  // After this call, fonts are validated.
  multilanguageSupport.ValidateFonts( utf32Characters,
                                      scripts,
                                      fontDescriptionRuns,
                                      defaultFontId,
                                      0u,
                                      numberOfCharacters,
                                      validFonts );

  // 5) Set the bidirectional info per paragraph.
  Vector<Character> mirroredUtf32Characters;
  bool textMirrored = false;

  // Reserve some space for the vector of paragraph's bidirectional info.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = logicalModel->mBidirectionalParagraphInfo;

  // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
  SetBidirectionalInfo( utf32Characters,
                        scripts,
                        lineBreakInfo,
                        0u,
                        numberOfCharacters,
                        bidirectionalInfo );

  // Create the paragraph info.
  logicalModel->CreateParagraphInfo( 0u,
                                     numberOfCharacters );

  // 6) Set character directions.
  Vector<CharacterDirection>& characterDirections = logicalModel->mCharacterDirections;
  if( 0u != bidirectionalInfo.Count() )
  {
    // Only set the character directions if there is right to left characters.
    GetCharactersDirection( bidirectionalInfo,
                            numberOfCharacters,
                            0u,
                            numberOfCharacters,
                            characterDirections );


    // This paragraph has right to left text. Some characters may need to be mirrored.
    textMirrored = GetMirroredText( utf32Characters,
                                    characterDirections,
                                    bidirectionalInfo,
                                    0u,
                                    numberOfCharacters,
                                    mirroredUtf32Characters );
  }
  else
  {
    // There is no right to left characters. Clear the directions vector.
    characterDirections.Clear();
  }

  // 7) Shape the text.

  Vector<GlyphInfo>& glyphs = visualModel->mGlyphs;
  Vector<CharacterIndex>& glyphsToCharactersMap = visualModel->mGlyphsToCharacters;
  Vector<Length>& charactersPerGlyph = visualModel->mCharactersPerGlyph;
  Vector<GlyphIndex> newParagraphGlyphs;

  const Vector<Character>& textToShape = textMirrored ? mirroredUtf32Characters : utf32Characters;

  ShapeText( textToShape,
             lineBreakInfo,
             scripts,
             validFonts,
             0u,
             0u,
             numberOfCharacters,
             glyphs,
             glyphsToCharactersMap,
             charactersPerGlyph,
             newParagraphGlyphs );

  // Create the 'number of glyphs' per character and the glyph to character conversion tables.
  visualModel->CreateGlyphsPerCharacterTable( 0u, 0u, numberOfCharacters );
  visualModel->CreateCharacterToGlyphTable( 0u, 0u, numberOfCharacters );

  const Length numberOfGlyphs = glyphs.Count();

  // 8) Get the glyph metrics
  metrics = Metrics::New( fontClient );

  GlyphInfo* glyphsBuffer = glyphs.Begin();
  metrics->GetGlyphMetrics( glyphsBuffer, numberOfGlyphs );

  // Update the width and advance of all new paragraph characters.
  for( Vector<GlyphIndex>::ConstIterator it = newParagraphGlyphs.Begin(),
         endIt = newParagraphGlyphs.End();
       it != endIt;
       ++it )
  {
    const GlyphIndex index = *it;
    GlyphInfo& glyph = *( glyphsBuffer + index );

    glyph.xBearing = 0.f;
    glyph.width = 0.f;
    glyph.advance = 0.f;
  }

  // 9) Layout the text
  LayoutEngine layoutEngine;
  layoutEngine.SetMetrics( metrics );
  layoutEngine.SetLayout( LayoutEngine::MULTI_LINE_BOX );

  // Set the layout parameters.
  const Vector<GlyphIndex>& charactersToGlyph = visualModel->mCharactersToGlyph;
  const Vector<Length>& glyphsPerCharacter = visualModel->mGlyphsPerCharacter;

  LayoutParameters layoutParameters( textArea,
                                     utf32Characters.Begin(),
                                     lineBreakInfo.Begin(),
                                     wordBreakInfo.Begin(),
                                     ( 0u != characterDirections.Count() ) ? characterDirections.Begin() : NULL,
                                     glyphs.Begin(),
                                     glyphsToCharactersMap.Begin(),
                                     charactersPerGlyph.Begin(),
                                     charactersToGlyph.Begin(),
                                     glyphsPerCharacter.Begin(),
                                     numberOfGlyphs );

  Vector<LineRun>& lines = visualModel->mLines;

  Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
  glyphPositions.Resize( numberOfGlyphs );

  layoutParameters.isLastNewParagraph = TextAbstraction::IsNewParagraph( *( utf32Characters.Begin() + ( numberOfCharacters - 1u ) ) );

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex = 0u;
  layoutParameters.numberOfGlyphs = numberOfGlyphs;
  layoutParameters.startLineIndex = 0u;
  layoutParameters.estimatedNumberOfLines = logicalModel->mParagraphInfo.Count();

  layoutEngine.LayoutText( layoutParameters,
                           glyphPositions,
                           lines,
                           layoutSize );

  // 10) Reorder the lines
  if( 0u != bidirectionalInfo.Count() )
  {
    Vector<BidirectionalLineInfoRun>& bidirectionalLineInfo = logicalModel->mBidirectionalLineInfo;

    // Get the lines
    const Length numberOfLines = lines.Count();

    // Reorder the lines.
    bidirectionalLineInfo.Reserve( numberOfLines ); // Reserve because is not known yet how many lines have right to left characters.
    ReorderLines( bidirectionalInfo,
                  0u,
                  numberOfCharacters,
                  lines,
                  bidirectionalLineInfo );

    // Set the bidirectional info per line into the layout parameters.
    layoutParameters.lineBidirectionalInfoRunsBuffer = bidirectionalLineInfo.Begin();
    layoutParameters.numberOfBidirectionalInfoRuns = bidirectionalLineInfo.Count();

    if( options.reorder )
    {
      // Re-layout the text. Reorder those lines with right to left characters.
      layoutEngine.ReLayoutRightToLeftLines( layoutParameters,
                                             0u,
                                             numberOfCharacters,
                                             glyphPositions );
    }
  }

  if( options.align )
  {
    layoutEngine.Align( textArea,
                        0u,
                        numberOfCharacters,
                        lines );
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
