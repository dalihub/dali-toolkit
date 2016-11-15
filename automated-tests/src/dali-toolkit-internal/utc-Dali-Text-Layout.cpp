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

#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-model.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the LayoutEngine methods.

//////////////////////////////////////////////////////////

namespace
{

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

struct LayoutTextData
{
  std::string          description;
  std::string          text;
  Size                 textArea;
  unsigned int         numberOfFonts;
  FontDescriptionRun*  fontDescriptions;
  Size                 layoutSize;
  unsigned int         totalNumberOfGlyphs;
  float*               positions;
  unsigned int         numberOfLines;
  LineRun*             lines;
  LayoutEngine::Layout layout;
  unsigned int         startIndex;
  unsigned int         numberOfGlyphs;
  bool                 ellipsis:1;
  bool                 updated:1;
};

void Print( const LineRun& line )
{
  std::cout << "        glyph run, index : " << line.glyphRun.glyphIndex << ", num glyphs : " << line.glyphRun.numberOfGlyphs << std::endl;
  std::cout << "    character run, index : " << line.characterRun.characterIndex << ", num chars : " << line.characterRun.numberOfCharacters << std::endl;
  std::cout << "                   width : " << line.width << std::endl;
  std::cout << "                ascender : " << line.ascender << std::endl;
  std::cout << "               descender : " << line.descender << std::endl;
  std::cout << "             extraLength : " << line.extraLength << std::endl;
  std::cout << "         alignmentOffset : " << line.alignmentOffset << std::endl;
  std::cout << "               direction : " << line.direction << std::endl;
  std::cout << "                ellipsis : " << line.ellipsis << std::endl;
}

bool LayoutTextTest( const LayoutTextData& data )
{
  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );

  // 1) Create the model.
  LogicalModelPtr logicalModel;
  VisualModelPtr visualModel;
  MetricsPtr metrics;
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  if( 0u != data.numberOfFonts )
  {
    fontDescriptionRuns.Insert( fontDescriptionRuns.End(),
                                data.fontDescriptions,
                                data.fontDescriptions + data.numberOfFonts );
  }

  LayoutOptions options;
  options.reorder = false;
  options.align = false;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel,
                   metrics );

  // 2) Clear the layout.
  Vector<LineRun>& lines = visualModel->mLines;

  const Length numberOfCharacters = logicalModel->mText.Count();
  const bool isLastNewParagraph = ( 0u == numberOfCharacters ) ? false : TextAbstraction::IsNewParagraph( *( logicalModel->mText.Begin() + ( numberOfCharacters - 1u ) ) );
  const GlyphIndex lastGlyphIndex = data.startIndex + data.numberOfGlyphs - 1u;
  const bool removeLastLine = isLastNewParagraph && ( lastGlyphIndex + 1u == visualModel->mGlyphs.Count() );

  LineIndex startRemoveIndex = 0u;

  if( 0u != lines.Count() )
  {
    startRemoveIndex = lines.Count();
    LineIndex endRemoveIndex = startRemoveIndex;
    ClearGlyphRuns( data.startIndex,
                    lastGlyphIndex + ( removeLastLine ? 1u : 0u ),
                    lines,
                    startRemoveIndex,
                    endRemoveIndex );

    // Update the character runs of the lines.
    const CharacterIndex* const glyphsToCharactersBuffer = visualModel->mGlyphsToCharacters.Begin();
    const Length* const charactersPerGlyph = visualModel->mCharactersPerGlyph.Begin();
    const CharacterIndex startCharacterIndex = *( glyphsToCharactersBuffer + data.startIndex );
    const CharacterIndex lastCharacterIndex = *( glyphsToCharactersBuffer + lastGlyphIndex ) + *( charactersPerGlyph + lastGlyphIndex ) - 1u;
    ClearCharacterRuns( startCharacterIndex,
                        lastCharacterIndex + ( removeLastLine ? 1u : 0u ),
                        lines,
                        startRemoveIndex,
                        endRemoveIndex );

    lines.Erase( lines.Begin() + startRemoveIndex,
                 lines.Begin() + endRemoveIndex );
  }

  Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;

  glyphPositions.Erase( glyphPositions.Begin() + data.startIndex,
                        glyphPositions.Begin() + data.startIndex + data.numberOfGlyphs );

  // 3) Layout
  LayoutEngine engine;
  engine.SetMetrics( metrics );
  engine.SetTextEllipsisEnabled( data.ellipsis );
  engine.SetLayout( data.layout );

  const Length totalNumberOfGlyphs = visualModel->mGlyphs.Count();

  LayoutParameters layoutParameters( data.textArea,
                                     logicalModel->mText.Begin(),
                                     logicalModel->mLineBreakInfo.Begin(),
                                     logicalModel->mWordBreakInfo.Begin(),
                                     ( 0u != logicalModel->mCharacterDirections.Count() ) ? logicalModel->mCharacterDirections.Begin() : NULL,
                                     visualModel->mGlyphs.Begin(),
                                     visualModel->mGlyphsToCharacters.Begin(),
                                     visualModel->mCharactersPerGlyph.Begin(),
                                     visualModel->mCharactersToGlyph.Begin(),
                                     visualModel->mGlyphsPerCharacter.Begin(),
                                     totalNumberOfGlyphs );

  layoutParameters.isLastNewParagraph = isLastNewParagraph;

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex = data.startIndex;
  layoutParameters.numberOfGlyphs = data.numberOfGlyphs;
  layoutParameters.startLineIndex = startRemoveIndex;
  layoutParameters.estimatedNumberOfLines = logicalModel->mParagraphInfo.Count();

  layoutSize = Vector2::ZERO;

  const bool updated = engine.LayoutText( layoutParameters,
                                          glyphPositions,
                                          lines,
                                          layoutSize );

  // 4) Compare the results.

  if( updated != data.updated )
  {
    std::cout << "  Different updated bool : " << updated << ", expected : " << data.updated << std::endl;
    return false;
  }

  if( layoutSize != data.layoutSize )
  {
    std::cout << "  Different layout size : " << layoutSize << ", expected : " << data.layoutSize << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.totalNumberOfGlyphs; ++index )
  {
    const Vector2& position = *( glyphPositions.Begin() + index );

    if( fabsf( position.x - *( data.positions + 2u * index ) ) > Math::MACHINE_EPSILON_1000 )
    {
      std::cout << "  Different position for glyph " << index << " x : " << position.x << ", expected : " << *( data.positions + 2u * index ) << std::endl;
      return false;
    }
    if( fabsf( position.y - *( data.positions + 2u * index + 1u ) ) > Math::MACHINE_EPSILON_1000 )
    {
      std::cout << "  Different position for glyph " << index << " y : " << position.y << ", expected : " << *( data.positions + 2u * index + 1u ) << std::endl;
      return false;
    }
  }

  if( lines.Count() != data.numberOfLines )
  {
    std::cout << "  Different number of lines : " << lines.Count() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.numberOfLines; ++index )
  {
    const LineRun& line = *( lines.Begin() + index );
    const LineRun& expectedLine = *( data.lines + index );

    if( line.glyphRun.glyphIndex != expectedLine.glyphRun.glyphIndex )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }
    if( line.glyphRun.numberOfGlyphs != expectedLine.glyphRun.numberOfGlyphs )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( line.characterRun.characterIndex != expectedLine.characterRun.characterIndex )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }
    if( line.characterRun.numberOfCharacters != expectedLine.characterRun.numberOfCharacters )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( fabsf( line.width - expectedLine.width ) > Math::MACHINE_EPSILON_1 )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( fabsf( line.ascender - expectedLine.ascender ) > Math::MACHINE_EPSILON_1 )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( fabsf( line.descender - expectedLine.descender ) > Math::MACHINE_EPSILON_1 )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( fabsf( line.extraLength - expectedLine.extraLength ) > Math::MACHINE_EPSILON_1 )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    if( line.ellipsis != expectedLine.ellipsis )
    {
      std::cout << "  Different line info for line : " << index << std::endl;
      Print( line );
      std::cout << "  expected" << std::endl;
      Print( expectedLine );
      return false;
    }

    // Do not compare the alignment offset as it's not calculated in the layout.
    // Do not compare the line direction as it's not set in the layout.
  }

  return true;
}

//////////////////////////////////////////////////////////

struct ReLayoutRightToLeftLinesData
{
  std::string         description;
  std::string         text;
  Size                textArea;
  unsigned int        numberOfFonts;
  FontDescriptionRun* fontDescriptions;
  unsigned int        totalNumberOfGlyphs;
  float*              positions;
  unsigned int        startIndex;
  unsigned int        numberOfCharacters;
};

bool ReLayoutRightToLeftLinesTest( const ReLayoutRightToLeftLinesData& data )
{
  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );

  // 1) Create the model.
  LogicalModelPtr logicalModel;
  VisualModelPtr visualModel;
  MetricsPtr metrics;
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  if( 0u != data.numberOfFonts )
  {
    fontDescriptionRuns.Insert( fontDescriptionRuns.End(),
                                data.fontDescriptions,
                                data.fontDescriptions + data.numberOfFonts );
  }

  LayoutOptions options;
  options.reorder = false;
  options.align = false;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel,
                   metrics );

  // 2) Call the ReLayoutRightToLeftLines() method.
  LayoutEngine engine;
  engine.SetMetrics( metrics );

  LayoutParameters layoutParameters( data.textArea,
                                     logicalModel->mText.Begin(),
                                     logicalModel->mLineBreakInfo.Begin(),
                                     logicalModel->mWordBreakInfo.Begin(),
                                     ( 0u != logicalModel->mCharacterDirections.Count() ) ? logicalModel->mCharacterDirections.Begin() : NULL,
                                     visualModel->mGlyphs.Begin(),
                                     visualModel->mGlyphsToCharacters.Begin(),
                                     visualModel->mCharactersPerGlyph.Begin(),
                                     visualModel->mCharactersToGlyph.Begin(),
                                     visualModel->mGlyphsPerCharacter.Begin(),
                                     visualModel->mGlyphs.Count() );

  layoutParameters.numberOfBidirectionalInfoRuns = logicalModel->mBidirectionalLineInfo.Count();
  layoutParameters.lineBidirectionalInfoRunsBuffer = logicalModel->mBidirectionalLineInfo.Begin();

  engine.ReLayoutRightToLeftLines( layoutParameters,
                                   data.startIndex,
                                   data.numberOfCharacters,
                                   visualModel->mGlyphPositions );

  // 3) Compare the results.
  Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;

  if( data.totalNumberOfGlyphs != visualModel->mGlyphs.Count() )
  {
    std::cout << "  Different number of glyphs : " << visualModel->mGlyphs.Count() << ", expected : " << data.totalNumberOfGlyphs << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < data.totalNumberOfGlyphs; ++index )
  {
    const Vector2& position = *( glyphPositions.Begin() + index );

    if( fabsf( position.x - *( data.positions + 2u * index ) ) > Math::MACHINE_EPSILON_1000 )
    {
      std::cout << "  Different position for glyph " << index << " x : " << position.x << ", expected : " << *( data.positions + 2u * index ) << std::endl;
      return false;
    }
    if( fabsf( position.y - *( data.positions + 2u * index + 1u ) ) > Math::MACHINE_EPSILON_1000 )
    {
      std::cout << "  Different position for glyph " << index << " y : " << position.y << ", expected : " << *( data.positions + 2u * index + 1u ) << std::endl;
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////////////

struct AlignData
{
  std::string                       description;
  std::string                       text;
  Size                              textArea;
  unsigned int                      numberOfFonts;
  FontDescriptionRun*               fontDescriptions;
  LayoutEngine::HorizontalAlignment horizontalAlignment;
  LayoutEngine::VerticalAlignment   verticalAlignment;
  unsigned int                      startIndex;
  unsigned int                      numberOfCharacters;
  unsigned int                      numberOfLines;
  float*                            lineOffsets;
};

bool AlignTest( const AlignData& data )
{
  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi( 96u, 96u );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );

  // 1) Create the model.
  LogicalModelPtr logicalModel;
  VisualModelPtr visualModel;
  MetricsPtr metrics;
  Size layoutSize;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  if( 0u != data.numberOfFonts )
  {
    fontDescriptionRuns.Insert( fontDescriptionRuns.End(),
                                data.fontDescriptions,
                                data.fontDescriptions + data.numberOfFonts );
  }

  LayoutOptions options;
  options.align = false;
  CreateTextModel( data.text,
                   data.textArea,
                   fontDescriptionRuns,
                   options,
                   layoutSize,
                   logicalModel,
                   visualModel,
                   metrics );

  // Call the Align method.
  LayoutEngine engine;
  engine.SetMetrics( metrics );

  engine.SetHorizontalAlignment( data.horizontalAlignment );
  engine.SetVerticalAlignment( data.verticalAlignment );

  engine.Align( data.textArea,
                data.startIndex,
                data.numberOfCharacters,
                visualModel->mLines );

  // Compare results.
  if( data.numberOfLines != visualModel->mLines.Count() )
  {
    std::cout << "  Different number of lines : " << visualModel->mLines.Count() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  const LineRun* const linesBuffer = visualModel->mLines.Begin();
  for( unsigned int index = 0u; index < data.numberOfLines; ++index )
  {
    const LineRun& line = *( linesBuffer + index );

    if( line.alignmentOffset != *( data.lineOffsets + index ) )
    {
      std::cout << "  different line offset for index " << index << " : " << line.alignmentOffset << ", expected : " << *( data.lineOffsets + index ) << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////
//
// UtcDaliTextLayoutSetGetLayout
// UtcDaliTextLayoutSetGetTextEllipsisEnabled
// UtcDaliTextLayoutSetGetHorizontalAlignment
// UtcDaliTextLayoutSetGetVerticalAlignment
// UtcDaliTextLayoutSetGetCursorWidth
// UtcDaliTextLayoutNoText
// UtcDaliTextLayoutSmallTextArea01
// UtcDaliTextLayoutSmallTextArea02
// UtcDaliTextLayoutMultilineText01
// UtcDaliTextLayoutMultilineText02
// UtcDaliTextLayoutMultilineText03
// UtcDaliTextLayoutMultilineText04
// UtcDaliTextLayoutMultilineText05
// UtcDaliTextUpdateLayout01
// UtcDaliTextUpdateLayout02
// UtcDaliTextUpdateLayout03
// UtcDaliTextLayoutEllipsis01
// UtcDaliTextLayoutEllipsis02
// UtcDaliTextLayoutEllipsis03
// UtcDaliTextLayoutEllipsis04
// UtcDaliTextLayoutEllipsis04
// UtcDaliTextReorderLayout01
// UtcDaliTextReorderLayout02
// UtcDaliTextReorderLayout03
// UtcDaliTextReorderLayout04
// UtcDaliTextAlign01
// UtcDaliTextAlign02
// UtcDaliTextAlign03
// UtcDaliTextAlign04
// UtcDaliTextAlign05
// UtcDaliTextAlign06
// UtcDaliTextAlign07
// UtcDaliTextAlign08
// UtcDaliTextAlign09
//
//////////////////////////////////////////////////////////

int UtcDaliTextLayoutSetGetLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetLayout");

  LayoutEngine engine;

  DALI_TEST_CHECK( LayoutEngine::SINGLE_LINE_BOX == engine.GetLayout() );

  engine.SetLayout( LayoutEngine::MULTI_LINE_BOX );
  DALI_TEST_CHECK( LayoutEngine::MULTI_LINE_BOX == engine.GetLayout() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetTextEllipsisEnabled(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetTextEllipsisEnabled");

  LayoutEngine engine;

  DALI_TEST_CHECK( !engine.GetTextEllipsisEnabled() );

  engine.SetTextEllipsisEnabled( true );
  DALI_TEST_CHECK( engine.GetTextEllipsisEnabled() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetHorizontalAlignment(void)
{
  ToolkitTestApplication application;
  tet_infoline(" ");

  LayoutEngine engine;

  DALI_TEST_CHECK( LayoutEngine::HORIZONTAL_ALIGN_BEGIN == engine.GetHorizontalAlignment() );

  engine.SetHorizontalAlignment( LayoutEngine::HORIZONTAL_ALIGN_END );
  DALI_TEST_CHECK( LayoutEngine::HORIZONTAL_ALIGN_END == engine.GetHorizontalAlignment() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetVerticalAlignment(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetVerticalAlignment");

  LayoutEngine engine;

  DALI_TEST_CHECK( LayoutEngine::VERTICAL_ALIGN_TOP == engine.GetVerticalAlignment() );

  engine.SetVerticalAlignment( LayoutEngine::VERTICAL_ALIGN_TOP );
  DALI_TEST_CHECK( LayoutEngine::VERTICAL_ALIGN_TOP == engine.GetVerticalAlignment() );


  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetCursorWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" ");

  LayoutEngine engine;

  DALI_TEST_EQUALS( 1, engine.GetCursorWidth(), TEST_LOCATION );

  engine.SetCursorWidth( 2 );
  DALI_TEST_EQUALS( 2, engine.GetCursorWidth(), TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutNoText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutNoText");

  Size textArea(100.f, 60.f);
  Size layoutSize = Vector2::ZERO;
  LayoutTextData data =
  {
    "No text",
    "",
    textArea,
    0u,
    NULL,
    layoutSize,
    0u,
    NULL,
    0u,
    NULL,
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    0u,
    false,
    false
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSmallTextArea01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSmallTextArea01");

  // Won't layout the text in multi-line if the width is too small.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun.familyLength = fontFamily.size();
  fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
  memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined = false;
  fontDescriptionRun.slantDefined = false;
  fontDescriptionRun.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun );
  Size textArea(1.f, 1.f);
  Size layoutSize = Vector2::ZERO;
  LayoutTextData data =
  {
    "Layout text in a small area",
    "Hello world",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    0u,
    NULL,
    0u,
    NULL,
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    11u,
    false,
    false
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSmallTextArea02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSmallTextArea02");

  // Will layout the text in single line as it can be scrolled.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun.familyLength = fontFamily.size();
  fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
  memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined = false;
  fontDescriptionRun.slantDefined = false;
  fontDescriptionRun.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun );
  Size textArea(1.f, 1.f);
  Size layoutSize(80.f, 20.f);
  float positions[] = { 1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f };
  struct LineRun line =
  {
    { 0u, 11u },
    { 0u, 11u },
    80.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line );

  LayoutTextData data =
  {
    "Layout text in a small area",
    "Hello world",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    11u,
    positions,
    1u,
    lines.Begin(),
    LayoutEngine::SINGLE_LINE_BOX,
    0u,
    11u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText01");

  // Layout some lines of left to right text.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun1.familyLength = fontFamily.size();
  fontDescriptionRun1.familyName = new char[fontDescriptionRun1.familyLength];
  memcpy( fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength );
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined = false;
  fontDescriptionRun1.slantDefined = false;
  fontDescriptionRun1.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex = 18u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 31u;
  fontDescriptionRun2.familyLength = fontFamily.size();
  fontDescriptionRun2.familyName = new char[fontDescriptionRun2.familyLength];
  memcpy( fontDescriptionRun2.familyName, fontFamily.c_str(), fontDescriptionRun2.familyLength );
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined = false;
  fontDescriptionRun2.slantDefined = false;
  fontDescriptionRun2.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun1 );
  fontDescriptionRuns.PushBack( fontDescriptionRun2 );
  Size textArea(100.f, 300.f);
  Size layoutSize(95.f, 97.f);
  float positions[] =
  {
    1.f, -12.f, 12.f,  -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f,  -0.f, 40.f, -9.f, 51.f,  -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f,  -9.f, 18.f,  -9.f, 30.f,  -9.f, 39.f, -3.f, 42.f, -12.f,
    1.f, -12.f,  9.f,  -9.f, 17.f,  -9.f, 27.f,  -9.f, 36.f, -9.f, 45.f, -11.f, 51.f, -0.f,
    1.f, -12.f, 11.f, -12.f, 15.f, -12.f, 26.f,  -9.f, 35.f, -9.f, 41.f,  -9.f, 50.f, -9.f, 59.f, -11.f, 65.f, -0.f,
    1.f, -12.f,  5.f, -12.f,  9.f,  -9.f, 19.f,  -9.f, 28.f, -9.f, 35.f,  -0.f, 41.f, -9.f, 50.f, -12.f, 54.f, -0.f, 59.f, -11.f, 66.f,  -9.f, 75.f, -9.f, 83.f, -11.f, 91.f, -2.f
  };
  struct LineRun line0 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line1 =
  {
    { 12u, 6u },
    { 12u, 6u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line2 =
  {
    { 18u, 7u },
    { 18u, 7u },
    52.f,
    15.f,
    -4.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line3 =
  {
    { 25u, 9u },
    { 25u, 10u },
    66.f,
    15.f,
    -4.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line4 =
  {
    { 34u, 14u },
    { 35u, 14u },
    95.f,
    15.f,
    -4.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line0 );
  lines.PushBack( line1 );
  lines.PushBack( line2 );
  lines.PushBack( line3 );
  lines.PushBack( line4 );

  LayoutTextData data =
  {
    "Layout simple multi-line text",
    "Hello world demo.\n"
    "Layout different lines of text.",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    48u,
    positions,
    5u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    48u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText02");

  // Layout some lines of bidirectional text.

  const std::string fontFamily1( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun1.familyLength = fontFamily1.size();
  fontDescriptionRun1.familyName = new char[fontDescriptionRun1.familyLength];
  memcpy( fontDescriptionRun1.familyName, fontFamily1.c_str(), fontDescriptionRun1.familyLength );
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined = false;
  fontDescriptionRun1.slantDefined = false;
  fontDescriptionRun1.sizeDefined = false;

  const std::string fontFamily2( "TizenSansHebrew" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex = 17u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun2.familyLength = fontFamily2.size();
  fontDescriptionRun2.familyName = new char[fontDescriptionRun2.familyLength];
  memcpy( fontDescriptionRun2.familyName, fontFamily2.c_str(), fontDescriptionRun2.familyLength );
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined = false;
  fontDescriptionRun2.slantDefined = false;
  fontDescriptionRun2.sizeDefined = false;

  const std::string fontFamily3( "TizenSansHebrew" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun3;
  fontDescriptionRun3.characterRun.characterIndex = 28u;
  fontDescriptionRun3.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun3.familyLength = fontFamily3.size();
  fontDescriptionRun3.familyName = new char[fontDescriptionRun3.familyLength];
  memcpy( fontDescriptionRun3.familyName, fontFamily3.c_str(), fontDescriptionRun3.familyLength );
  fontDescriptionRun3.familyDefined = true;
  fontDescriptionRun3.weightDefined = false;
  fontDescriptionRun3.widthDefined = false;
  fontDescriptionRun3.slantDefined = false;
  fontDescriptionRun3.sizeDefined = false;

  const std::string fontFamily4( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun4;
  fontDescriptionRun4.characterRun.characterIndex = 38u;
  fontDescriptionRun4.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun4.familyLength = fontFamily4.size();
  fontDescriptionRun4.familyName = new char[fontDescriptionRun4.familyLength];
  memcpy( fontDescriptionRun4.familyName, fontFamily4.c_str(), fontDescriptionRun4.familyLength );
  fontDescriptionRun4.familyDefined = true;
  fontDescriptionRun4.weightDefined = false;
  fontDescriptionRun4.widthDefined = false;
  fontDescriptionRun4.slantDefined = false;
  fontDescriptionRun4.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun1 );
  fontDescriptionRuns.PushBack( fontDescriptionRun2 );
  fontDescriptionRuns.PushBack( fontDescriptionRun3 );
  fontDescriptionRuns.PushBack( fontDescriptionRun4 );
  Size textArea(100.f, 300.f);
  Size layoutSize(81.f, 120.f);
  float positions[] =
  {
    1.f, -12.f, 12.f,  -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f,  -0.f, 40.f,  -9.f, 51.f,  -9.f, 61.f,  -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f,  -9.f, 18.f,  -9.f, 30.f,  -9.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f,  -0.f,
    1.f, -10.f,  9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f,  -0.f,
    1.f, -13.f, 10.f,  -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f,  -0.f, 39.f,  -9.f, 50.f,  -9.f, 60.f,  -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -13.f, 10.f,  -9.f, 18.f,  -9.f, 30.f,  -9.f, 39.f, -3.f,
  };
  struct LineRun line0 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line1 =
  {
    { 12u, 10u },
    { 12u, 10u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line2 =
  {
    { 22u, 6u },
    { 22u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line3 =
  {
    { 28u, 10u },
    { 28u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line4 =
  {
    { 38u, 12u },
    { 38u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line5 =
  {
    { 50u, 5u },
    { 50u, 5u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line0 );
  lines.PushBack( line1 );
  lines.PushBack( line2 );
  lines.PushBack( line3 );
  lines.PushBack( line4 );
  lines.PushBack( line5 );

  LayoutTextData data =
  {
    "Layout bidirectional text.",
    "Hello world demo שלום עולם.\n"
    "שלום עולם hello world demo.",
    textArea,
    4u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    55u,
    positions,
    6u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    55u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText03");

  // Layout a long word which doesn't fit in the width of the text area.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 29u;
  fontDescriptionRun.familyLength = fontFamily.size();
  fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
  memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined = false;
  fontDescriptionRun.slantDefined = false;
  fontDescriptionRun.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun );
  Size textArea(100.f, 300.f);
  Size layoutSize(96.f, 60.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f,  -9.f, 36.f,  -9.f, 47.f, -9.f, 57.f, -9.f, 63.f, -13.f, 66.f, -13.f, 75.f, -13.f, 85.f,  -9.f,
    1.f,  -9.f, 13.f, -9.f, 23.f, -13.f, 32.f,  -9.f, 40.f, -13.f, 44.f, -13.f, 47.f, -9.f, 56.f, -9.f, 67.f,  -9.f, 77.f,  -9.f, 83.f, -13.f, 86.f, -13.f,
    0.f, -13.f, 10.f, -9.f, 18.f,  -9.f, 30.f,  -9.f, 39.f,  -3.f,
  };
  struct LineRun line0 =
  {
    { 0u, 12u },
    { 0u, 12u },
    94.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line1 =
  {
    { 12u, 12u },
    { 12u, 12u },
    96.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line2 =
  {
    { 24u, 5u },
    { 24u, 5u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line0 );
  lines.PushBack( line1 );
  lines.PushBack( line2 );

  LayoutTextData data =
  {
    "Layout a long word which doesn't fit in the width of the text area.",
    "Helloworlddemohelloworlddemo.",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    29u,
    positions,
    3u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    29u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText04");

  // Layout simple text ending with a \n. It has to add a void line at the end.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = 13u;
  fontDescriptionRun.familyLength = fontFamily.size();
  fontDescriptionRun.familyName = new char[fontDescriptionRun.familyLength];
  memcpy( fontDescriptionRun.familyName, fontFamily.c_str(), fontDescriptionRun.familyLength );
  fontDescriptionRun.familyDefined = true;
  fontDescriptionRun.weightDefined = false;
  fontDescriptionRun.widthDefined = false;
  fontDescriptionRun.slantDefined = false;
  fontDescriptionRun.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun );
  Size textArea(100.f, 300.f);
  Size layoutSize(83.f, 40.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -3.f, 82.f, -12.f
  };
  struct LineRun line0 =
  {
    { 0u, 13u },
    { 0u, 13u },
    83.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line1 =
  {
    { 13u, 0u },
    { 13u, 0u },
    0.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line0 );
  lines.PushBack( line1 );

  LayoutTextData data =
  {
    "Layout simple text ending with a \n.",
    "Hello world.\n",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    13u,
    positions,
    2u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    13u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutMultilineText05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutMultilineText05");

  // Layout simple text with one character with a different font size.

  const std::string fontFamily( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun1;
  fontDescriptionRun1.characterRun.characterIndex = 0u;
  fontDescriptionRun1.characterRun.numberOfCharacters = 6u;
  fontDescriptionRun1.familyLength = fontFamily.size();
  fontDescriptionRun1.familyName = new char[fontDescriptionRun1.familyLength];
  memcpy( fontDescriptionRun1.familyName, fontFamily.c_str(), fontDescriptionRun1.familyLength );
  fontDescriptionRun1.familyDefined = true;
  fontDescriptionRun1.weightDefined = false;
  fontDescriptionRun1.widthDefined = false;
  fontDescriptionRun1.slantDefined = false;
  fontDescriptionRun1.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun2;
  fontDescriptionRun2.characterRun.characterIndex = 6u;
  fontDescriptionRun2.characterRun.numberOfCharacters = 1u;
  fontDescriptionRun2.familyLength = fontFamily.size();
  fontDescriptionRun2.familyName = new char[fontDescriptionRun2.familyLength];
  memcpy( fontDescriptionRun2.familyName, fontFamily.c_str(), fontDescriptionRun2.familyLength );
  fontDescriptionRun2.size = 1280u;
  fontDescriptionRun2.familyDefined = true;
  fontDescriptionRun2.weightDefined = false;
  fontDescriptionRun2.widthDefined = false;
  fontDescriptionRun2.slantDefined = false;
  fontDescriptionRun2.sizeDefined = true;

  FontDescriptionRun fontDescriptionRun3;
  fontDescriptionRun3.characterRun.characterIndex = 7u;
  fontDescriptionRun3.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun3.familyLength = fontFamily.size();
  fontDescriptionRun3.familyName = new char[fontDescriptionRun3.familyLength];
  memcpy( fontDescriptionRun3.familyName, fontFamily.c_str(), fontDescriptionRun3.familyLength );
  fontDescriptionRun3.familyDefined = true;
  fontDescriptionRun3.weightDefined = false;
  fontDescriptionRun3.widthDefined = false;
  fontDescriptionRun3.slantDefined = false;
  fontDescriptionRun3.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun1 );
  fontDescriptionRuns.PushBack( fontDescriptionRun2 );
  fontDescriptionRuns.PushBack( fontDescriptionRun3 );
  Size textArea(100.f, 300.f);
  Size layoutSize(88.f, 53.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -14.f, 59.f, -9.f, 69.f, -9.f, 75.f, -13.f, 78.f, -13.f, 87.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f,  -9.f, 30.f,  -9.f, 39.f, -3.f
  };
  struct LineRun line0 =
  {
    { 0u, 12u },
    { 0u, 12u },
    88.f,
    25.f,
    -8.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line1 =
  {
    { 12u, 5u },
    { 12u, 5u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line0 );
  lines.PushBack( line1 );

  LayoutTextData data =
  {
    "Layout simple text with one character with a different font size.",
    "Hello world demo.",
    textArea,
    3u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    17u,
    positions,
    2u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    17u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout01");

  // Layout some lines of bidirectional text. Update the paragraphs at the beginning.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 28u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 42u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 54u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 64u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength = fontHebrew.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex = 74u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun07.familyLength = fontLatin.size();
  fontDescriptionRun07.familyName = new char[fontDescriptionRun07.familyLength];
  memcpy( fontDescriptionRun07.familyName, fontLatin.c_str(), fontDescriptionRun07.familyLength );
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined = false;
  fontDescriptionRun07.slantDefined = false;
  fontDescriptionRun07.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex = 92u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun08.familyLength = fontLatin.size();
  fontDescriptionRun08.familyName = new char[fontDescriptionRun08.familyLength];
  memcpy( fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength );
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined = false;
  fontDescriptionRun08.slantDefined = false;
  fontDescriptionRun08.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex = 104u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun09.familyLength = fontArabic.size();
  fontDescriptionRun09.familyName = new char[fontDescriptionRun09.familyLength];
  memcpy( fontDescriptionRun09.familyName, fontArabic.c_str(), fontDescriptionRun09.familyLength );
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined = false;
  fontDescriptionRun09.slantDefined = false;
  fontDescriptionRun09.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex = 118u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun10.familyLength = fontHebrew.size();
  fontDescriptionRun10.familyName = new char[fontDescriptionRun10.familyLength];
  memcpy( fontDescriptionRun10.familyName, fontHebrew.c_str(), fontDescriptionRun10.familyLength );
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined = false;
  fontDescriptionRun10.slantDefined = false;
  fontDescriptionRun10.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex = 128u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun11.familyLength = fontLatin.size();
  fontDescriptionRun11.familyName = new char[fontDescriptionRun11.familyLength];
  memcpy( fontDescriptionRun11.familyName, fontLatin.c_str(), fontDescriptionRun11.familyLength );
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined = false;
  fontDescriptionRun11.slantDefined = false;
  fontDescriptionRun11.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex = 145u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun12.familyLength = fontHebrew.size();
  fontDescriptionRun12.familyName = new char[fontDescriptionRun12.familyLength];
  memcpy( fontDescriptionRun12.familyName, fontHebrew.c_str(), fontDescriptionRun12.familyLength );
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined = false;
  fontDescriptionRun12.slantDefined = false;
  fontDescriptionRun12.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex = 156u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun13.familyLength = fontHebrew.size();
  fontDescriptionRun13.familyName = new char[fontDescriptionRun13.familyLength];
  memcpy( fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength );
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined = false;
  fontDescriptionRun13.slantDefined = false;
  fontDescriptionRun13.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex = 166u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun14.familyLength = fontLatin.size();
  fontDescriptionRun14.familyName = new char[fontDescriptionRun14.familyLength];
  memcpy( fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength );
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined = false;
  fontDescriptionRun14.slantDefined = false;
  fontDescriptionRun14.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex = 178u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun15.familyLength = fontArabic.size();
  fontDescriptionRun15.familyName = new char[fontDescriptionRun15.familyLength];
  memcpy( fontDescriptionRun15.familyName, fontArabic.c_str(), fontDescriptionRun15.familyLength );
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined = false;
  fontDescriptionRun15.slantDefined = false;
  fontDescriptionRun15.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );
  fontDescriptionRuns.PushBack( fontDescriptionRun07 );
  fontDescriptionRuns.PushBack( fontDescriptionRun08 );
  fontDescriptionRuns.PushBack( fontDescriptionRun09 );
  fontDescriptionRuns.PushBack( fontDescriptionRun10 );
  fontDescriptionRuns.PushBack( fontDescriptionRun11 );
  fontDescriptionRuns.PushBack( fontDescriptionRun12 );
  fontDescriptionRuns.PushBack( fontDescriptionRun13 );
  fontDescriptionRuns.PushBack( fontDescriptionRun14 );
  fontDescriptionRuns.PushBack( fontDescriptionRun15 );
  Size textArea(100.f, 300.f);
  Size layoutSize(92.f, 380.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -13.f, 68.f, -9.f, 76.f, -13.f, 80.f, -13.f, 83.f, -9.f, 92.f, -0.f,
    0.f, -9.f, 11.f, -9.f, 21.f, -9.f, 27.f, -13.f, 30.f, -13.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -3.f, 42.f, -11.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -10.f, 70.f, -13.f, 77.f, -10.f, 82.f, -10.f, 90.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 41.f, -9.f, 52.f, -9.f, 62.f, -9.f, 68.f, -13.f, 71.f, -13.f, 80.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 45.f, -10.f, 56.f, -13.f, 63.f, -10.f, 68.f, -10.f, 76.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f,
  };
  struct LineRun line01 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line02 =
  {
    { 12u, 10u },
    { 12u, 10u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line03 =
  {
    { 22u, 6u },
    { 22u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line04 =
  {
    { 28u, 20u },
    { 28u, 20u },
    92.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line05 =
  {
    { 48u, 11u },
    { 48u, 11u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line06 =
  {
    { 59u, 5u },
    { 59u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line07 =
  {
    { 64u, 10u },
    { 64u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line08 =
  {
    { 74u, 12u },
    { 74u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line09 =
  {
    { 86u, 6u },
    { 86u, 6u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line10 =
  {
    { 92u, 12u },
    { 92u, 12u },
    79.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line11 =
  {
    { 104u, 19u },
    { 104u, 19u },
    90.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line12 =
  {
    { 123u, 5u },
    { 123u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line13 =
  {
    { 128u, 12u },
    { 128u, 12u },
    82.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line14 =
  {
    { 140u, 10u },
    { 140u, 10u },
    77.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line15 =
  {
    { 150u, 6u },
    { 150u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line16 =
  {
    { 156u, 10u },
    { 156u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line17 =
  {
    { 166u, 12u },
    { 166u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line18 =
  {
    { 178u, 14u },
    { 178u, 14u },
    55.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line19 =
  {
    { 192u, 0u },
    { 192u, 0u },
    0.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );
  lines.PushBack( line02 );
  lines.PushBack( line03 );
  lines.PushBack( line04 );
  lines.PushBack( line05 );
  lines.PushBack( line06 );
  lines.PushBack( line07 );
  lines.PushBack( line08 );
  lines.PushBack( line09 );
  lines.PushBack( line10 );
  lines.PushBack( line11 );
  lines.PushBack( line12 );
  lines.PushBack( line13 );
  lines.PushBack( line14 );
  lines.PushBack( line15 );
  lines.PushBack( line16 );
  lines.PushBack( line17 );
  lines.PushBack( line18 );
  lines.PushBack( line19 );

  LayoutTextData data =
  {
    "Layout bidirectional text.",
    "Hello world demo שלום עולם.\n"
    "مرحبا بالعالم hello world שלום עולם\n"
    "שלום עולם hello world demo.\n"
    "hello world مرحبا بالعالم שלום עולם\n"
    "Hello world demo שלום עולם.\n"
    "שלום עולם hello world مرحبا بالعالم\n",
    textArea,
    15u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    192u,
    positions,
    19u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    64u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout02");

  // Layout some lines of bidirectional text. Update the paragraphs at the middle.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 28u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 42u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 54u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 64u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength = fontHebrew.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex = 74u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun07.familyLength = fontLatin.size();
  fontDescriptionRun07.familyName = new char[fontDescriptionRun07.familyLength];
  memcpy( fontDescriptionRun07.familyName, fontLatin.c_str(), fontDescriptionRun07.familyLength );
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined = false;
  fontDescriptionRun07.slantDefined = false;
  fontDescriptionRun07.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex = 92u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun08.familyLength = fontLatin.size();
  fontDescriptionRun08.familyName = new char[fontDescriptionRun08.familyLength];
  memcpy( fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength );
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined = false;
  fontDescriptionRun08.slantDefined = false;
  fontDescriptionRun08.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex = 104u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun09.familyLength = fontArabic.size();
  fontDescriptionRun09.familyName = new char[fontDescriptionRun09.familyLength];
  memcpy( fontDescriptionRun09.familyName, fontArabic.c_str(), fontDescriptionRun09.familyLength );
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined = false;
  fontDescriptionRun09.slantDefined = false;
  fontDescriptionRun09.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex = 118u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun10.familyLength = fontHebrew.size();
  fontDescriptionRun10.familyName = new char[fontDescriptionRun10.familyLength];
  memcpy( fontDescriptionRun10.familyName, fontHebrew.c_str(), fontDescriptionRun10.familyLength );
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined = false;
  fontDescriptionRun10.slantDefined = false;
  fontDescriptionRun10.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex = 128u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun11.familyLength = fontLatin.size();
  fontDescriptionRun11.familyName = new char[fontDescriptionRun11.familyLength];
  memcpy( fontDescriptionRun11.familyName, fontLatin.c_str(), fontDescriptionRun11.familyLength );
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined = false;
  fontDescriptionRun11.slantDefined = false;
  fontDescriptionRun11.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex = 145u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun12.familyLength = fontHebrew.size();
  fontDescriptionRun12.familyName = new char[fontDescriptionRun12.familyLength];
  memcpy( fontDescriptionRun12.familyName, fontHebrew.c_str(), fontDescriptionRun12.familyLength );
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined = false;
  fontDescriptionRun12.slantDefined = false;
  fontDescriptionRun12.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex = 156u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun13.familyLength = fontHebrew.size();
  fontDescriptionRun13.familyName = new char[fontDescriptionRun13.familyLength];
  memcpy( fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength );
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined = false;
  fontDescriptionRun13.slantDefined = false;
  fontDescriptionRun13.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex = 166u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun14.familyLength = fontLatin.size();
  fontDescriptionRun14.familyName = new char[fontDescriptionRun14.familyLength];
  memcpy( fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength );
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined = false;
  fontDescriptionRun14.slantDefined = false;
  fontDescriptionRun14.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex = 178u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun15.familyLength = fontArabic.size();
  fontDescriptionRun15.familyName = new char[fontDescriptionRun15.familyLength];
  memcpy( fontDescriptionRun15.familyName, fontArabic.c_str(), fontDescriptionRun15.familyLength );
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined = false;
  fontDescriptionRun15.slantDefined = false;
  fontDescriptionRun15.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );
  fontDescriptionRuns.PushBack( fontDescriptionRun07 );
  fontDescriptionRuns.PushBack( fontDescriptionRun08 );
  fontDescriptionRuns.PushBack( fontDescriptionRun09 );
  fontDescriptionRuns.PushBack( fontDescriptionRun10 );
  fontDescriptionRuns.PushBack( fontDescriptionRun11 );
  fontDescriptionRuns.PushBack( fontDescriptionRun12 );
  fontDescriptionRuns.PushBack( fontDescriptionRun13 );
  fontDescriptionRuns.PushBack( fontDescriptionRun14 );
  fontDescriptionRuns.PushBack( fontDescriptionRun15 );
  Size textArea(100.f, 300.f);
  Size layoutSize(92.f, 380.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -13.f, 68.f, -9.f, 76.f, -13.f, 80.f, -13.f, 83.f, -9.f, 92.f, -0.f,
    0.f, -9.f, 11.f, -9.f, 21.f, -9.f, 27.f, -13.f, 30.f, -13.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -3.f, 42.f, -11.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -10.f, 70.f, -13.f, 77.f, -10.f, 82.f, -10.f, 90.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 41.f, -9.f, 52.f, -9.f, 62.f, -9.f, 68.f, -13.f, 71.f, -13.f, 80.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 45.f, -10.f, 56.f, -13.f, 63.f, -10.f, 68.f, -10.f, 76.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f,
  };
  struct LineRun line01 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line02 =
  {
    { 12u, 10u },
    { 12u, 10u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line03 =
  {
    { 22u, 6u },
    { 22u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line04 =
  {
    { 28u, 20u },
    { 28u, 20u },
    92.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line05 =
  {
    { 48u, 11u },
    { 48u, 11u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line06 =
  {
    { 59u, 5u },
    { 59u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line07 =
  {
    { 64u, 10u },
    { 64u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line08 =
  {
    { 74u, 12u },
    { 74u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line09 =
  {
    { 86u, 6u },
    { 86u, 6u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line10 =
  {
    { 92u, 12u },
    { 92u, 12u },
    79.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line11 =
  {
    { 104u, 19u },
    { 104u, 19u },
    90.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line12 =
  {
    { 123u, 5u },
    { 123u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line13 =
  {
    { 128u, 12u },
    { 128u, 12u },
    82.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line14 =
  {
    { 140u, 10u },
    { 140u, 10u },
    77.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line15 =
  {
    { 150u, 6u },
    { 150u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line16 =
  {
    { 156u, 10u },
    { 156u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line17 =
  {
    { 166u, 12u },
    { 166u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line18 =
  {
    { 178u, 14u },
    { 178u, 14u },
    55.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line19 =
  {
    { 192u, 0u },
    { 192u, 0u },
    0.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );
  lines.PushBack( line02 );
  lines.PushBack( line03 );
  lines.PushBack( line04 );
  lines.PushBack( line05 );
  lines.PushBack( line06 );
  lines.PushBack( line07 );
  lines.PushBack( line08 );
  lines.PushBack( line09 );
  lines.PushBack( line10 );
  lines.PushBack( line11 );
  lines.PushBack( line12 );
  lines.PushBack( line13 );
  lines.PushBack( line14 );
  lines.PushBack( line15 );
  lines.PushBack( line16 );
  lines.PushBack( line17 );
  lines.PushBack( line18 );
  lines.PushBack( line19 );

  LayoutTextData data =
  {
    "Layout bidirectional text.",
    "Hello world demo שלום עולם.\n"
    "مرحبا بالعالم hello world שלום עולם\n"
    "שלום עולם hello world demo.\n"
    "hello world مرحبا بالعالم שלום עולם\n"
    "Hello world demo שלום עולם.\n"
    "שלום עולם hello world مرحبا بالعالم\n",
    textArea,
    15u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    192u,
    positions,
    19u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    64u,
    64u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextUpdateLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextUpdateLayout03");

  // Layout some lines of bidirectional text. Update the paragraphs at the middle.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 17u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 28u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 42u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 54u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 64u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun06.familyLength = fontHebrew.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontHebrew.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun07;
  fontDescriptionRun07.characterRun.characterIndex = 74u;
  fontDescriptionRun07.characterRun.numberOfCharacters = 18u;
  fontDescriptionRun07.familyLength = fontLatin.size();
  fontDescriptionRun07.familyName = new char[fontDescriptionRun07.familyLength];
  memcpy( fontDescriptionRun07.familyName, fontLatin.c_str(), fontDescriptionRun07.familyLength );
  fontDescriptionRun07.familyDefined = true;
  fontDescriptionRun07.weightDefined = false;
  fontDescriptionRun07.widthDefined = false;
  fontDescriptionRun07.slantDefined = false;
  fontDescriptionRun07.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun08;
  fontDescriptionRun08.characterRun.characterIndex = 92u;
  fontDescriptionRun08.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun08.familyLength = fontLatin.size();
  fontDescriptionRun08.familyName = new char[fontDescriptionRun08.familyLength];
  memcpy( fontDescriptionRun08.familyName, fontLatin.c_str(), fontDescriptionRun08.familyLength );
  fontDescriptionRun08.familyDefined = true;
  fontDescriptionRun08.weightDefined = false;
  fontDescriptionRun08.widthDefined = false;
  fontDescriptionRun08.slantDefined = false;
  fontDescriptionRun08.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun09;
  fontDescriptionRun09.characterRun.characterIndex = 104u;
  fontDescriptionRun09.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun09.familyLength = fontArabic.size();
  fontDescriptionRun09.familyName = new char[fontDescriptionRun09.familyLength];
  memcpy( fontDescriptionRun09.familyName, fontArabic.c_str(), fontDescriptionRun09.familyLength );
  fontDescriptionRun09.familyDefined = true;
  fontDescriptionRun09.weightDefined = false;
  fontDescriptionRun09.widthDefined = false;
  fontDescriptionRun09.slantDefined = false;
  fontDescriptionRun09.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun10;
  fontDescriptionRun10.characterRun.characterIndex = 118u;
  fontDescriptionRun10.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun10.familyLength = fontHebrew.size();
  fontDescriptionRun10.familyName = new char[fontDescriptionRun10.familyLength];
  memcpy( fontDescriptionRun10.familyName, fontHebrew.c_str(), fontDescriptionRun10.familyLength );
  fontDescriptionRun10.familyDefined = true;
  fontDescriptionRun10.weightDefined = false;
  fontDescriptionRun10.widthDefined = false;
  fontDescriptionRun10.slantDefined = false;
  fontDescriptionRun10.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun11;
  fontDescriptionRun11.characterRun.characterIndex = 128u;
  fontDescriptionRun11.characterRun.numberOfCharacters = 17u;
  fontDescriptionRun11.familyLength = fontLatin.size();
  fontDescriptionRun11.familyName = new char[fontDescriptionRun11.familyLength];
  memcpy( fontDescriptionRun11.familyName, fontLatin.c_str(), fontDescriptionRun11.familyLength );
  fontDescriptionRun11.familyDefined = true;
  fontDescriptionRun11.weightDefined = false;
  fontDescriptionRun11.widthDefined = false;
  fontDescriptionRun11.slantDefined = false;
  fontDescriptionRun11.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun12;
  fontDescriptionRun12.characterRun.characterIndex = 145u;
  fontDescriptionRun12.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun12.familyLength = fontHebrew.size();
  fontDescriptionRun12.familyName = new char[fontDescriptionRun12.familyLength];
  memcpy( fontDescriptionRun12.familyName, fontHebrew.c_str(), fontDescriptionRun12.familyLength );
  fontDescriptionRun12.familyDefined = true;
  fontDescriptionRun12.weightDefined = false;
  fontDescriptionRun12.widthDefined = false;
  fontDescriptionRun12.slantDefined = false;
  fontDescriptionRun12.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun13;
  fontDescriptionRun13.characterRun.characterIndex = 156u;
  fontDescriptionRun13.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun13.familyLength = fontHebrew.size();
  fontDescriptionRun13.familyName = new char[fontDescriptionRun13.familyLength];
  memcpy( fontDescriptionRun13.familyName, fontHebrew.c_str(), fontDescriptionRun13.familyLength );
  fontDescriptionRun13.familyDefined = true;
  fontDescriptionRun13.weightDefined = false;
  fontDescriptionRun13.widthDefined = false;
  fontDescriptionRun13.slantDefined = false;
  fontDescriptionRun13.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun14;
  fontDescriptionRun14.characterRun.characterIndex = 166u;
  fontDescriptionRun14.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun14.familyLength = fontLatin.size();
  fontDescriptionRun14.familyName = new char[fontDescriptionRun14.familyLength];
  memcpy( fontDescriptionRun14.familyName, fontLatin.c_str(), fontDescriptionRun14.familyLength );
  fontDescriptionRun14.familyDefined = true;
  fontDescriptionRun14.weightDefined = false;
  fontDescriptionRun14.widthDefined = false;
  fontDescriptionRun14.slantDefined = false;
  fontDescriptionRun14.sizeDefined = false;

  // Set a known font description
  FontDescriptionRun fontDescriptionRun15;
  fontDescriptionRun15.characterRun.characterIndex = 178u;
  fontDescriptionRun15.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun15.familyLength = fontArabic.size();
  fontDescriptionRun15.familyName = new char[fontDescriptionRun15.familyLength];
  memcpy( fontDescriptionRun15.familyName, fontArabic.c_str(), fontDescriptionRun15.familyLength );
  fontDescriptionRun15.familyDefined = true;
  fontDescriptionRun15.weightDefined = false;
  fontDescriptionRun15.widthDefined = false;
  fontDescriptionRun15.slantDefined = false;
  fontDescriptionRun15.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );
  fontDescriptionRuns.PushBack( fontDescriptionRun07 );
  fontDescriptionRuns.PushBack( fontDescriptionRun08 );
  fontDescriptionRuns.PushBack( fontDescriptionRun09 );
  fontDescriptionRuns.PushBack( fontDescriptionRun10 );
  fontDescriptionRuns.PushBack( fontDescriptionRun11 );
  fontDescriptionRuns.PushBack( fontDescriptionRun12 );
  fontDescriptionRuns.PushBack( fontDescriptionRun13 );
  fontDescriptionRuns.PushBack( fontDescriptionRun14 );
  fontDescriptionRuns.PushBack( fontDescriptionRun15 );
  Size textArea(100.f, 300.f);
  Size layoutSize(92.f, 380.f);
  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -13.f, 68.f, -9.f, 76.f, -13.f, 80.f, -13.f, 83.f, -9.f, 92.f, -0.f,
    0.f, -9.f, 11.f, -9.f, 21.f, -9.f, 27.f, -13.f, 30.f, -13.f, 39.f, -0.f, 44.f, -10.f, 55.f, -13.f, 62.f, -10.f, 67.f, -10.f, 75.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -3.f, 42.f, -11.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -10.f, 70.f, -13.f, 77.f, -10.f, 82.f, -10.f, 90.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 41.f, -9.f, 52.f, -9.f, 62.f, -9.f, 68.f, -13.f, 71.f, -13.f, 80.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 45.f, -10.f, 56.f, -13.f, 63.f, -10.f, 68.f, -10.f, 76.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 32.f, -2.f, 35.f, -11.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f,
    1.f, -13.f, 10.f, -9.f, 18.f, -13.f, 22.f, -13.f, 25.f, -9.f, 34.f, -0.f, 39.f, -9.f, 50.f, -9.f, 60.f, -9.f, 66.f, -13.f, 69.f, -13.f, 78.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f,
  };
  struct LineRun line01 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line02 =
  {
    { 12u, 10u },
    { 12u, 10u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line03 =
  {
    { 22u, 6u },
    { 22u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line04 =
  {
    { 28u, 20u },
    { 28u, 20u },
    92.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line05 =
  {
    { 48u, 11u },
    { 48u, 11u },
    76.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line06 =
  {
    { 59u, 5u },
    { 59u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line07 =
  {
    { 64u, 10u },
    { 64u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line08 =
  {
    { 74u, 12u },
    { 74u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line09 =
  {
    { 86u, 6u },
    { 86u, 6u },
    43.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line10 =
  {
    { 92u, 12u },
    { 92u, 12u },
    79.f,
    15.f,
    -5.f,
    5.f,
    0.f,
    false,
    false
  };
  struct LineRun line11 =
  {
    { 104u, 19u },
    { 104u, 19u },
    90.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line12 =
  {
    { 123u, 5u },
    { 123u, 5u },
    31.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line13 =
  {
    { 128u, 12u },
    { 128u, 12u },
    82.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line14 =
  {
    { 140u, 10u },
    { 140u, 10u },
    77.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line15 =
  {
    { 150u, 6u },
    { 150u, 6u },
    36.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line16 =
  {
    { 156u, 10u },
    { 156u, 10u },
    67.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line17 =
  {
    { 166u, 12u },
    { 166u, 12u },
    80.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    false
  };
  struct LineRun line18 =
  {
    { 178u, 14u },
    { 178u, 14u },
    55.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };
  struct LineRun line19 =
  {
    { 192u, 0u },
    { 192u, 0u },
    0.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    false
  };

  Vector<LineRun> lines;
  lines.PushBack( line01 );
  lines.PushBack( line02 );
  lines.PushBack( line03 );
  lines.PushBack( line04 );
  lines.PushBack( line05 );
  lines.PushBack( line06 );
  lines.PushBack( line07 );
  lines.PushBack( line08 );
  lines.PushBack( line09 );
  lines.PushBack( line10 );
  lines.PushBack( line11 );
  lines.PushBack( line12 );
  lines.PushBack( line13 );
  lines.PushBack( line14 );
  lines.PushBack( line15 );
  lines.PushBack( line16 );
  lines.PushBack( line17 );
  lines.PushBack( line18 );
  lines.PushBack( line19 );

  LayoutTextData data =
  {
    "Layout bidirectional text.",
    "Hello world demo שלום עולם.\n"
    "مرحبا بالعالم hello world שלום עולם\n"
    "שלום עולם hello world demo.\n"
    "hello world مرحبا بالعالم שלום עולם\n"
    "Hello world demo שלום עולם.\n"
    "שלום עולם hello world مرحبا بالعالم\n",
    textArea,
    15u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    192u,
    positions,
    19u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    128u,
    64u,
    false,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis01");

  // Layout single-line LTR text with ellipsis.

  const std::string fontLatin( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 51u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );

  struct LineRun line01 =
  {
    { 0u, 13u },
    { 0u, 13u },
    93.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    true
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );

  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f, 83.f, -13.f,
  };

  Size textArea( 100.f, 50.f );
  Size layoutSize( 100.f, 20.f );

  LayoutTextData data =
  {
    "Layout single-line LTR text with ellipsis.",
    "Hello world demo hello world demo hello world demo.",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    13u,
    positions,
    1u,
    lines.Begin(),
    LayoutEngine::SINGLE_LINE_BOX,
    0u,
    51u,
    true,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis02");

  // Layout multi-line LTR text with ellipsis.

  const std::string fontLatin( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 51u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );

  struct LineRun line01 =
  {
    { 0u, 12u },
    { 0u, 12u },
    81.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line02 =
  {
    { 12u, 12u },
    { 12u, 12u },
    93.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    true
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );
  lines.PushBack( line02 );

  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f, 79.f, -0.f,
    0.f, -13.f, 10.f, -9.f, 18.f, -9.f, 30.f, -9.f, 39.f, -0.f, 44.f, -13.f, 53.f, -9.f, 61.f, -13.f, 65.f, -13.f, 68.f, -9.f, 77.f, -0.f, 81.f, -9.f,
  };

  Size textArea( 100.f, 50.f );
  Size layoutSize( 100.f, 60.f );

  LayoutTextData data =
  {
    "Layout multi-line LTR text with ellipsis.",
    "Hello world demo hello world demo hello world demo.",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    24u,
    positions,
    2u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    51u,
    true,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis03");

  // Layout single-line RTL text with ellipsis.

  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength = fontHebrew.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength = fontArabic.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun03.familyLength = fontHebrew.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontHebrew.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 34u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength = fontArabic.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  struct LineRun line01 =
  {
    { 0u, 16u },
    { 0u, 16u },
    95.f,
    15.f,
    -5.f,
    0.f,
    0.f,
    false,
    true
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );

  float positions[] =
  {
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
  };

  Size textArea( 100.f, 50.f );
  Size layoutSize( 100.f, 20.f );

  LayoutTextData data =
  {
    "Layout single-line RTL text with ellipsis.",
    "שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    16u,
    positions,
    1u,
    lines.Begin(),
    LayoutEngine::SINGLE_LINE_BOX,
    0u,
    72u,
    true,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutEllipsis04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutEllipsis04");

  // Layout multi-line RTL text with ellipsis.

  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength = fontHebrew.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength = fontArabic.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun03.familyLength = fontHebrew.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontHebrew.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 34u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun04.familyLength = fontArabic.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontArabic.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  struct LineRun line01 =
  {
    { 0u, 16u },
    { 0u, 16u },
    96.f,
    15.f,
    -5.f,
    3.f,
    0.f,
    false,
    false
  };
  struct LineRun line02 =
  {
    { 16u, 18u },
    { 16u, 18u },
    97.f,
    15.f,
    -5.f,
    4.f,
    0.f,
    false,
    true
  };
  Vector<LineRun> lines;
  lines.PushBack( line01 );
  lines.PushBack( line02 );

  float positions[] =
  {
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
    0.f, -7.f, 5.f, -11.f, 6.f, -11.f, 10.f, -8.f, 17.f, -11.f, 18.f, -11.f, 22.f, -8.f, 28.f, -0.f, 32.f, -10.f, 43.f, -13.f, 50.f, -10.f, 55.f, -10.f, 63.f, -0.f, 68.f, -10.f, 76.f, -10.f, 81.f, -13.f, 89.f, -10.f, 97.f, -0.f,
  };

  Size textArea( 100.f, 50.f );
  Size layoutSize( 100.f, 60.f );

  LayoutTextData data =
  {
    "Layout single-line RTL text with ellipsis.",
    "שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم שלום עולם مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    layoutSize,
    34u,
    positions,
    2u,
    lines.Begin(),
    LayoutEngine::MULTI_LINE_BOX,
    0u,
    72u,
    true,
    true
  };

  if( !LayoutTextTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextReorderLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextReorderLayout01");

  // Reorder lines. No right to left characters.

  const std::string fontLatin( "TizenSans" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 11u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );

  float positions[] =
  {
    1.f, -12.f, 12.f, -9.f, 20.f, -13.f, 24.f, -13.f, 27.f, -9.f, 36.f, -0.f, 40.f, -9.f, 51.f, -9.f, 61.f, -9.f, 67.f, -13.f, 70.f, -13.f,
  };

  Size textArea( 100.f, 300.f );

  ReLayoutRightToLeftLinesData data =
  {
    "Text with no right to left text.",
    "Hello world",
    textArea,
    1u,
    fontDescriptionRuns.Begin(),
    11u,
    positions,
    0u,
    11u
  };

  if( !ReLayoutRightToLeftLinesTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextReorderLayout02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextReorderLayout02");

  // Reorder lines of the first paragraph.

  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength = fontHebrew.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength = fontArabic.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 38u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun04.familyLength = fontHebrew.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontHebrew.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] =
  {
    87.f, -10.f, 79.f, -13.f, 74.f, -10.f, 66.f, -10.f, 61.f, -0.f, 53.f, -10.f, 48.f, -10.f, 41.f, -13.f, 32.f, -10.f, 27.f, -0.f, 20.f, -8.f, 15.f, -6.f, 8.f, -7.f, 5.f, -7.f, 4.f, -11.f, 0.f, -0.f,
    23.f, -7.f, 22.f, -11.f, 17.f, -11.f, 12.f, -8.f, 10.f, -11.f, 5.f, -11.f, 0.f, -8.f, 0.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -10.f, 70.f, -13.f, 77.f, -10.f, 82.f, -10.f, 90.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
    0.f, -7.f, 5.f, -11.f, 6.f, -11.f, 10.f, -8.f, 17.f, -11.f, 18.f, -11.f, 22.f, -8.f, 30.f, -2.f,
  };

  Size textArea( 100.f, 300.f );

  ReLayoutRightToLeftLinesData data =
  {
    "Paragraphs with right to left text.",
    "שלום עולם مرحبا بالعالم\n"
    "مرحبا بالعالم שלום עולם\n"
    "שלום עולם مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    72u,
    positions,
    0u,
    24u
  };

  if( !ReLayoutRightToLeftLinesTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextReorderLayout03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextReorderLayout03");

  // Reorder lines of the mid paragraph.

  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength = fontHebrew.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength = fontArabic.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 38u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun04.familyLength = fontHebrew.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontHebrew.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] =
  {
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
    0.f, -7.f,  5.f, -11.f, 6.f, -11.f, 10.f, -8.f, 17.f, -11.f, 18.f, -11.f, 22.f, -8.f, 28.f, -0.f,
    86.f, -8.f, 81.f, -6.f, 74.f, -7.f, 71.f, -7.f, 70.f, -11.f, 66.f, -0.f, 62.f, -7.f, 61.f, -11.f, 56.f, -11.f, 51.f, -8.f, 49.f, -11.f, 44.f, -11.f, 39.f, -8.f, 36.f, -0.f, 26.f, -10.f, 18.f, -13.f, 13.f, -10.f, 5.f, -10.f, 0.f, -0.f,
    22.f, -10.f, 17.f, -10.f, 10.f, -13.f, 1.f, -10.f, 0.f, -0.f,
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
    0.f, -7.f, 5.f, -11.f, 6.f, -11.f, 10.f, -8.f, 17.f, -11.f, 18.f, -11.f, 22.f, -8.f, 30.f, -2.f,
  };

  Size textArea( 100.f, 300.f );

  ReLayoutRightToLeftLinesData data =
  {
    "Paragraphs with right to left text.",
    "שלום עולם مرحبا بالعالم\n"
    "مرحبا بالعالم שלום עולם\n"
    "שלום עולם مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    72u,
    positions,
    24u,
    24u
  };

  if( !ReLayoutRightToLeftLinesTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextReorderLayout04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextReorderLayout04");

  // Reorder lines of the last paragraph.

  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun01.familyLength = fontHebrew.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontHebrew.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 10u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun02.familyLength = fontArabic.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontArabic.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 24u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 38u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun04.familyLength = fontHebrew.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontHebrew.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun05.familyLength = fontHebrew.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontHebrew.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 58u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 15u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] =
  {
    1.f, -10.f, 12.f, -13.f, 19.f, -10.f, 24.f, -10.f, 32.f, -0.f, 37.f, -10.f, 45.f, -10.f, 50.f, -13.f, 58.f, -10.f, 66.f, -0.f, 69.f, -8.f, 76.f, -6.f, 81.f, -7.f, 87.f, -7.f, 92.f, -11.f, 94.f, -0.f,
    0.f, -7.f, 5.f, -11.f, 6.f, -11.f, 10.f, -8.f, 17.f, -11.f, 18.f, -11.f, 22.f, -8.f, 28.f, -0.f,
    0.f, -8.f, 7.f, -6.f, 12.f, -7.f, 18.f, -7.f, 23.f, -11.f, 25.f, -0.f, 27.f, -7.f, 32.f, -11.f, 33.f, -11.f, 37.f, -8.f, 44.f, -11.f, 45.f, -11.f, 49.f, -8.f, 55.f, -0.f, 59.f, -10.f, 70.f, -13.f, 77.f, -10.f, 82.f, -10.f, 90.f, -0.f,
    1.f, -10.f, 9.f, -10.f, 14.f, -13.f, 22.f, -10.f, 30.f, -0.f,
    87.f, -10.f, 79.f, -13.f, 74.f, -10.f, 66.f, -10.f, 61.f, -0.f, 53.f, -10.f, 48.f, -10.f, 41.f, -13.f, 32.f, -10.f, 27.f, -0.f, 20.f, -8.f, 15.f, -6.f, 8.f, -7.f, 5.f, -7.f, 4.f, -11.f, 0.f, -0.f,
    28.f, -7.f, 27.f, -11.f, 22.f, -11.f, 17.f, -8.f, 15.f, -11.f, 10.f, -11.f, 5.f, -8.f, 2.f, -2.f,
  };

  Size textArea( 100.f, 300.f );

  ReLayoutRightToLeftLinesData data =
  {
    "Paragraphs with right to left text.",
    "שלום עולם مرحبا بالعالم\n"
    "مرحبا بالعالم שלום עולם\n"
    "שלום עולם مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    72u,
    positions,
    48u,
    24u
  };

  if( !ReLayoutRightToLeftLinesTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign01");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Begin alignment for the first paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_BEGIN,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    0u,
    22u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign02");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, 2.f, 60.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Begin alignment for the mid paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_BEGIN,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    22u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign03");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Begin alignment for the last paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_BEGIN,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    48u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign04(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign04");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 10.f, 16.f, 0.f, 0.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Center alignment for the first paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_CENTER,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    0u,
    22u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign05");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, -1.f, 30.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Center alignment for the mid paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_CENTER,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    22u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign06(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign06");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, 0.f, 0.f, 10.f, 20.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "Center alignment for the last paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_CENTER,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    48u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign07(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign07");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 20.f, 33.f, 0.f, 0.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "End alignment for the first paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_END,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    0u,
    22u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign08(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign08");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, -4.f, 0.f, 0.f, 0.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "End alignment for the mid paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_END,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    22u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextAlign09(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextAlign09");

  // Calculate text alignment.

  const std::string fontLatin( "TizenSans" );
  const std::string fontHebrew( "TizenSansHebrew" );
  const std::string fontArabic( "TizenSansArabic" );

  // Set a known font description
  FontDescriptionRun fontDescriptionRun01;
  fontDescriptionRun01.characterRun.characterIndex = 0u;
  fontDescriptionRun01.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun01.familyLength = fontLatin.size();
  fontDescriptionRun01.familyName = new char[fontDescriptionRun01.familyLength];
  memcpy( fontDescriptionRun01.familyName, fontLatin.c_str(), fontDescriptionRun01.familyLength );
  fontDescriptionRun01.familyDefined = true;
  fontDescriptionRun01.weightDefined = false;
  fontDescriptionRun01.widthDefined = false;
  fontDescriptionRun01.slantDefined = false;
  fontDescriptionRun01.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun02;
  fontDescriptionRun02.characterRun.characterIndex = 12u;
  fontDescriptionRun02.characterRun.numberOfCharacters = 10u;
  fontDescriptionRun02.familyLength = fontHebrew.size();
  fontDescriptionRun02.familyName = new char[fontDescriptionRun02.familyLength];
  memcpy( fontDescriptionRun02.familyName, fontHebrew.c_str(), fontDescriptionRun02.familyLength );
  fontDescriptionRun02.familyDefined = true;
  fontDescriptionRun02.weightDefined = false;
  fontDescriptionRun02.widthDefined = false;
  fontDescriptionRun02.slantDefined = false;
  fontDescriptionRun02.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun03;
  fontDescriptionRun03.characterRun.characterIndex = 22u;
  fontDescriptionRun03.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun03.familyLength = fontArabic.size();
  fontDescriptionRun03.familyName = new char[fontDescriptionRun03.familyLength];
  memcpy( fontDescriptionRun03.familyName, fontArabic.c_str(), fontDescriptionRun03.familyLength );
  fontDescriptionRun03.familyDefined = true;
  fontDescriptionRun03.weightDefined = false;
  fontDescriptionRun03.widthDefined = false;
  fontDescriptionRun03.slantDefined = false;
  fontDescriptionRun03.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun04;
  fontDescriptionRun04.characterRun.characterIndex = 36u;
  fontDescriptionRun04.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun04.familyLength = fontLatin.size();
  fontDescriptionRun04.familyName = new char[fontDescriptionRun04.familyLength];
  memcpy( fontDescriptionRun04.familyName, fontLatin.c_str(), fontDescriptionRun04.familyLength );
  fontDescriptionRun04.familyDefined = true;
  fontDescriptionRun04.weightDefined = false;
  fontDescriptionRun04.widthDefined = false;
  fontDescriptionRun04.slantDefined = false;
  fontDescriptionRun04.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun05;
  fontDescriptionRun05.characterRun.characterIndex = 48u;
  fontDescriptionRun05.characterRun.numberOfCharacters = 12u;
  fontDescriptionRun05.familyLength = fontLatin.size();
  fontDescriptionRun05.familyName = new char[fontDescriptionRun05.familyLength];
  memcpy( fontDescriptionRun05.familyName, fontLatin.c_str(), fontDescriptionRun05.familyLength );
  fontDescriptionRun05.familyDefined = true;
  fontDescriptionRun05.weightDefined = false;
  fontDescriptionRun05.widthDefined = false;
  fontDescriptionRun05.slantDefined = false;
  fontDescriptionRun05.sizeDefined = false;

  FontDescriptionRun fontDescriptionRun06;
  fontDescriptionRun06.characterRun.characterIndex = 60u;
  fontDescriptionRun06.characterRun.numberOfCharacters = 14u;
  fontDescriptionRun06.familyLength = fontArabic.size();
  fontDescriptionRun06.familyName = new char[fontDescriptionRun06.familyLength];
  memcpy( fontDescriptionRun06.familyName, fontArabic.c_str(), fontDescriptionRun06.familyLength );
  fontDescriptionRun06.familyDefined = true;
  fontDescriptionRun06.weightDefined = false;
  fontDescriptionRun06.widthDefined = false;
  fontDescriptionRun06.slantDefined = false;
  fontDescriptionRun06.sizeDefined = false;

  Vector<FontDescriptionRun> fontDescriptionRuns;
  fontDescriptionRuns.PushBack( fontDescriptionRun01 );
  fontDescriptionRuns.PushBack( fontDescriptionRun02 );
  fontDescriptionRuns.PushBack( fontDescriptionRun03 );
  fontDescriptionRuns.PushBack( fontDescriptionRun04 );
  fontDescriptionRuns.PushBack( fontDescriptionRun05 );
  fontDescriptionRuns.PushBack( fontDescriptionRun06 );

  float positions[] = { 0.f, 0.f, 0.f, 0.f, 20.f, 40.f };

  Size textArea( 100.f, 300.f );
  AlignData data =
  {
    "End alignment for the last paragraph.",
    "Hello world שלום עולם\nمرحبا بالعالم Hello world\nHello world مرحبا بالعالم.",
    textArea,
    6u,
    fontDescriptionRuns.Begin(),
    LayoutEngine::HORIZONTAL_ALIGN_END,
    LayoutEngine::VERTICAL_ALIGN_TOP,
    48u,
    26u,
    6u,
    positions
  };

  if( !AlignTest( data ) )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}
