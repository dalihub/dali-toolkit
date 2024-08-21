/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/text-view.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;


namespace
{

  const std::string DEFAULT_FONT_DIR( "/resources/fonts" );

  struct ElideData
  {
    std::string                                                description;
    std::string                                                text;
    bool                                                       isMultiLines;
    DevelText::LineWrap::Mode                                  lineWrapMode;
    DevelText::EllipsisPosition::Type                          ellipsisPosition;
    bool                                                       isMarkup;
    Vector2                                                    size;
    unsigned int                                               numberOfLines;
    unsigned int                                               numberOfGlyphs;
    float*                                                     positions;
  };


  bool ElideTestViewModel( const ElideData& data )
  {
    std::cout << "  testing : " << data.description << std::endl;

    // Load some fonts.
    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 93u, 93u );

    char* pathNamePtr = get_current_dir_name();
    const std::string pathName( pathNamePtr );
    free( pathNamePtr );

    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );

    // Creates a text controller.
    ControllerPtr controller = Controller::New();

    // Tests the rendering controller has been created.
    TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
    DALI_TEST_CHECK(typesetter);

    // Tests the view model has been created.
    ViewModel* model = typesetter->GetViewModel();
    DALI_TEST_CHECK(model);

    // Configures the text controller similarly to the text-label.
    ConfigureTextLabel( controller );

    // Sets a text and relais-out.
    controller->SetMarkupProcessorEnabled( data.isMarkup );

    controller->SetMultiLineEnabled( data.isMultiLines );
    controller->SetLineWrapMode( (Text::LineWrap::Mode)(data.lineWrapMode) );
    controller->SetEllipsisPosition( data.ellipsisPosition );
    controller->SetRemoveFrontInset(true);
    controller->SetRemoveBackInset(true);

    controller->SetText(data.text);
    controller->Relayout( data.size );

    // Elide the glyphs.
    model->ElideGlyphs(fontClient);

    if( data.numberOfLines != model->GetNumberOfLines() )
    {
      std::cout << "  different number of lines : " << model->GetNumberOfLines() << ", expected : " << data.numberOfLines << std::endl;
      return false;
    }

    Length numberOfGlyphs = model->GetNumberOfGlyphs();

    if( data.numberOfGlyphs != numberOfGlyphs )
    {
      std::cout << "  different number of glyphs : " << numberOfGlyphs << ", expected : " << data.numberOfGlyphs << std::endl;
      return false;
    }

    const Vector2* const layoutBuffer              = model->GetLayout();
    const Length         numberOfLines             = model->GetNumberOfLines();
    const GlyphIndex     startIndexOfGlyphs        = model->GetStartIndexOfElidedGlyphs();
    const GlyphIndex     endIndexOfGlyphs          = model->GetEndIndexOfElidedGlyphs();
    const GlyphIndex     firstMiddleIndexOfGlyphs  = model->GetFirstMiddleIndexOfElidedGlyphs();


    //Test total height of lines is fit inside Controller's size
    Length heightOfLines = 0;
    for(Length lineIndex=0u; lineIndex < numberOfLines; lineIndex++)
    {
        const LineRun& tempLine         = *( model->GetLines() + lineIndex);
        heightOfLines+= (tempLine.ascender - tempLine.descender);
    }

    if(heightOfLines > data.size.height)
    {
        std::cout << "The heightOfLines should be less than height of controller.";
        std::cout << " The heightOfLines is "<< heightOfLines << "and the height of controller is "<< data.size.height <<std::endl;
        return false;
    }

    if( numberOfLines != 0u )
    {
      Length   elidedLineIndex    = 0u;
      for(Length lineIndex=0u; lineIndex < numberOfLines; lineIndex++)
      {
          const LineRun& tempLine         = *( model->GetLines() + elidedLineIndex);
          if(tempLine.ellipsis)
          {
            elidedLineIndex = lineIndex;
            break;
          }
      }
      const LineRun& elidedLine                   = *( model->GetLines() + elidedLineIndex);
      Length         numberOfLineGlyphs           = 0u;
      Length         numberOfLineGlyphsSecondHalf = 0u;

      switch(data.ellipsisPosition)
      {
        case DevelText::EllipsisPosition::START:
        {
          numberOfLineGlyphs = elidedLine.glyphRun.numberOfGlyphs - ( startIndexOfGlyphs - elidedLine.glyphRun.glyphIndex);
          break;
        }
        case DevelText::EllipsisPosition::MIDDLE:
        {
          numberOfLineGlyphs = firstMiddleIndexOfGlyphs - elidedLine.glyphRun.glyphIndex +1u ;
          break;
        }
        case DevelText::EllipsisPosition::END:
        default:
        {
          numberOfLineGlyphs = endIndexOfGlyphs - elidedLine.glyphRun.glyphIndex + 1u;
          break;
        }
      }

      unsigned int index = 0u;
      for( ; index < numberOfLineGlyphs; ++index )
      {
        if( *( data.positions + index ) != floor(elidedLine.alignmentOffset + ( *( layoutBuffer + index ) ).x ) )
        {
          std::cout << "  different layout :";
          for( unsigned int i = 0; i < numberOfLineGlyphs; ++i )
          {
            std::cout << " " << floor( elidedLine.alignmentOffset + ( *( layoutBuffer + i ) ).x );
          }
          std::cout << std::endl;
          std::cout << "          expected :";
          for( unsigned int i = 0; i < numberOfLineGlyphs; ++i )
          {
            std::cout << " " << *( data.positions + i );
          }
          std::cout << std::endl;
          return false;
        }
      }


      for( ; index < numberOfLineGlyphsSecondHalf; ++index )
      {
        if( *( data.positions + index ) != floor(elidedLine.alignmentOffset + ( *( layoutBuffer + index ) ).x ) )
        {
          std::cout << "  different layout :";
          for( unsigned int i = 0; i < numberOfLineGlyphsSecondHalf; ++i )
          {
            std::cout << " " << floor( elidedLine.alignmentOffset + ( *( layoutBuffer + i ) ).x );
          }
          std::cout << std::endl;
          std::cout << "          expected :";
          for( unsigned int i = 0; i < numberOfLineGlyphsSecondHalf; ++i )
          {
            std::cout << " " << *( data.positions + i );
          }
          std::cout << std::endl;
          return false;
        }
      }

    }

    return true;
  }

  bool ElideTestTextView( const ElideData& data )
  {
    std::cout << "  testing : " << data.description << std::endl;

    // Load some fonts.
    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 93u, 93u );

    char* pathNamePtr = get_current_dir_name();
    const std::string pathName( pathNamePtr );
    free( pathNamePtr );

    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );
    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
    fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );

    // Creates a text controller.
    ControllerPtr controller = Controller::New();

    if(data.isMultiLines)
    {
       // Configures the text controller similarly to the text-editor.
       ConfigureTextEditor( controller );
       controller->SetVerticalScrollEnabled( false );
    }
    else
    {
      // Configures the text controller similarly to the text-field.
      ConfigureTextField( controller );
    }

    controller->SetDefaultFontFamily("TizenSansRegular");
    controller->SetDefaultFontSize(12.0f, Text::Controller::POINT_SIZE);

    controller->SetMultiLineEnabled( data.isMultiLines );
    controller->SetLineWrapMode( (Text::LineWrap::Mode)(data.lineWrapMode) );

    // Sets a text and relais-out.
    controller->SetMarkupProcessorEnabled( data.isMarkup );

    controller->SetTextElideEnabled( true );
    controller->SetEllipsisPosition( data.ellipsisPosition );

    controller->SetRemoveFrontInset(true);
    controller->SetRemoveBackInset(true);

    controller->SetText( data.text );
    controller->Relayout( data.size );

    // Get view to elide the glyphs.
    Text::ViewInterface& view = controller->GetView();

    Length numberOfGlyphs = view.GetNumberOfGlyphs();

    if(numberOfGlyphs == 0u)
    {
      return data.numberOfGlyphs == 0u;
    }

    Vector<GlyphInfo> glyphs;
    glyphs.Resize(numberOfGlyphs);

    Vector<Vector2> positions;
    positions.Resize(numberOfGlyphs);

    float alignmentOffset = 0u;
    numberOfGlyphs = view.GetGlyphs(glyphs.Begin(),
                                    positions.Begin(),
                                    alignmentOffset,
                                    0u,
                                    numberOfGlyphs);

    glyphs.Resize(numberOfGlyphs);
    positions.Resize(numberOfGlyphs);


    if( data.numberOfGlyphs != numberOfGlyphs )
    {
      std::cout << "  different number of glyphs : " << numberOfGlyphs << ", expected : " << data.numberOfGlyphs << std::endl;
      return false;
    }

    // Tests the text model has been created.
    const ModelInterface* textModel  = controller->GetTextModel();
    DALI_TEST_CHECK(textModel);

    if( data.numberOfLines != textModel->GetNumberOfLines() )
    {
      std::cout << "  different number of lines : " << textModel->GetNumberOfLines() << ", expected : " << data.numberOfLines << std::endl;
      return false;
    }

    const Length     numberOfLines             = textModel->GetNumberOfLines();
    const GlyphIndex startIndexOfGlyphs        = textModel->GetStartIndexOfElidedGlyphs();
    const GlyphIndex endIndexOfGlyphs          = textModel->GetEndIndexOfElidedGlyphs();
    const GlyphIndex firstMiddleIndexOfGlyphs  = textModel->GetFirstMiddleIndexOfElidedGlyphs();
    const GlyphIndex secondMiddleIndexOfGlyphs = textModel->GetSecondMiddleIndexOfElidedGlyphs();

    //Test total height of lines is fit inside Controller's size
    Length heightOfLines = 0;
    for(Length lineIndex=0u; lineIndex < numberOfLines; lineIndex++)
    {
        const LineRun& tempLine         = *( textModel->GetLines() + lineIndex);
        heightOfLines+= (tempLine.ascender - tempLine.descender);
    }

    if(heightOfLines > data.size.height)
    {
        std::cout << "The heightOfLines should be less than height of controller.";
        std::cout << " The heightOfLines is "<< heightOfLines << "and the height of controller is "<< data.size.height <<std::endl;
        return false;
    }

    if( numberOfLines != 0u )
    {
      Length   elidedLineIndex    = 0u;
      for(Length lineIndex=0u; lineIndex < numberOfLines; lineIndex++)
      {
          const LineRun& tempLine         = *( textModel->GetLines() + lineIndex);
          if(tempLine.ellipsis)
          {
            elidedLineIndex = lineIndex;
            break;
          }
      }
      const LineRun& elidedLine         = *( textModel->GetLines() + elidedLineIndex);

      Length         numberOfLineGlyphs           = 0u;
      Length         numberOfLineGlyphsSecondHalf = 0u;

      switch(data.ellipsisPosition)
      {
        case DevelText::EllipsisPosition::START:
        {
          numberOfLineGlyphs = elidedLine.glyphRun.numberOfGlyphs - ( startIndexOfGlyphs - elidedLine.glyphRun.glyphIndex);
          break;
        }
        case DevelText::EllipsisPosition::MIDDLE:
        {
          numberOfLineGlyphs = firstMiddleIndexOfGlyphs == elidedLine.glyphRun.glyphIndex ? 0u : (firstMiddleIndexOfGlyphs - elidedLine.glyphRun.glyphIndex +1u);

          if(elidedLine.isSplitToTwoHalves)
          {
            numberOfLineGlyphsSecondHalf = (elidedLine.glyphRunSecondHalf.glyphIndex + elidedLine.glyphRunSecondHalf.numberOfGlyphs) - secondMiddleIndexOfGlyphs ;

          }
          break;
        }
        case DevelText::EllipsisPosition::END:
        default:
        {
          numberOfLineGlyphs = endIndexOfGlyphs - elidedLine.glyphRun.glyphIndex + 1u;
          break;
        }
      }


      unsigned int index = 0u;
      for(  ; index < numberOfLineGlyphs; ++index )
      {

        if( *( data.positions + index ) != floor( elidedLine.alignmentOffset + positions[index].x ))
        {
          std::cout << "  different layout :";
          for( unsigned int i = 0; i < numberOfLineGlyphs; ++i )
          {
            std::cout << " " << floor( elidedLine.alignmentOffset + positions[i].x );
          }
          std::cout << std::endl;
          std::cout << "          expected :";
          for( unsigned int i = 0; i < numberOfLineGlyphs; ++i )
          {
            std::cout << " " << *( data.positions + i );
          }
          std::cout << std::endl;
          return false;
        }
      }

      for( ; index < numberOfLineGlyphsSecondHalf; ++index )
      {
        if( *( data.positions + index ) != floor( elidedLine.alignmentOffset + positions[index].x ))
        {
          std::cout << "  different layout :";
          for( unsigned int i = 0; i < numberOfLineGlyphsSecondHalf; ++i )
          {
            std::cout << " " << floor( elidedLine.alignmentOffset + positions[i].x );
          }
          std::cout << std::endl;
          std::cout << "          expected :";
          for( unsigned int i = 0; i < numberOfLineGlyphsSecondHalf; ++i )
          {
            std::cout << " " << *( data.positions + i );
          }
          std::cout << std::endl;
          return false;
        }
      }
    }

    return true;
  }

}

int UtcDaliTextLabelElideTextLocation(void)
{
  tet_infoline(" UtcDaliTextLabelElideTextLocation ");

  Size textSize00( 100.f, 100.f );

  Size  textSize01( 120.0f, 50.0f );
  float positions01[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 93.0f };

  Size  textSize02( 120.0f, 50.0f );
  float positions02[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f, 102.0f, 111.0f, 0.0f, 11.0f, 21.0f, 31.0f, 41.0f, 50.0f, 61.0f, 71.0f, 81.0f, 92.0f };

  Size  textSize03( 120.0f, 60.0f );
  float positions03[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 0.0f, 9.0f, 19.0f, 28.0f, 39.0f, 45.0f, 49.0f, 63.0f, 74.0f, 83.0f, 0.0f, 14.0f, 25.0f, 28.0f, 37.0f, 47.0f, 61.0f, 71.0f, 77.0f, 89.0f };

  Size  textSize04( 120.0f, 60.0f );
  float positions04[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 93.0f, 104.0f, 110.0f, 0.0f, 14.0f, 25.0f, 34.0f, 39.0f, 53.0f, 64.0f, 67.0f, 76.0f, 86.0f, 100.0f, 110.0f, 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 94.0f };

  Size  textSize05( 110.0f, 60.0f );
  float positions05[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 0.0f, 9.0f, 20.0f, 26.0f, 30.0f, 44.0f, 55.0f, 64.0f, 69.0f, 83.0f, 94.0f, 0.0f, 9.0f, 19.0f, 33.0f, 43.0f, 49.0f, 60.0f, 71.0f, 75.0f, 79.0f };

  Size  textSize06( 110.0f, 60.0f );
  float positions06[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 0.0f, 9.0f, 19.0f, 28.0f, 39.0f, 45.0f, 49.0f, 63.0f, 74.0f, 83.0f, 0.0f, 14.0f, 25.0f, 28.0f, 37.0f, 47.0f, 61.0f, 71.0f, 78.0f };

  Size  textSize07( 120.0f, 50.0f );
  float positions07[] = { 6.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f, 102.0f, 111.0f };

  Size  textSize08( 120.0f, 50.0f );
  float positions08[] = { 6.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f, 102.0f, 111.0f };

  Size  textSize09( 120.0f, 60.0f );
  float positions09[] = { 8.0f, 25.0f, 28.0f, 37.0f, 47.0f, 61.0f, 71.0f, 77.0f, 88.0f, 99.0f, 103.0f, 106.0f, 115.0f };

  Size  textSize10( 120.0f, 60.0f );
  float positions10[] = { 8.0f, 25.0f, 34.0f, 39.0f, 53.0f, 64.0f, 67.0f, 76.0f, 86.0f, 100.0f, 110.0f };

  Size  textSize11( 100.0f, 60.0f );
  float positions11[] = { 4.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f };

  Size  textSize12( 100.0f, 60.0f );
  float positions12[] = { 4.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f };

  Size  textSize13( 120.0f, 60.0f );
  float positions13[] = { 118.0f, 111.0f, 97.0f, 89.0f, 80.0f, 74.0f, 64.0f, 59.0f, 54.0f, 45.0f, 37.0f, 31.0f, 22.0f, 7.0f };

  Size  textSize14( 120.0f, 60.0f );
  float positions14[] = { 116.0f, 109.0f, 95.0f, 87.0f, 78.0f, 72.0f, 62.0f, 57.0f, 52.0f, 43.0f, 35.0f, 30.0f, 113.0f, 104.0f, 94.0f, 89.0f, 86.0f, 80.0f, 77.0f, 67.0f, 59.0f, 54.0f, 45.0f, 38.0f, 35.0f, 30.0f, 145.0f, 133.0f, 128.0f, 123.0f, 120.0f, 111.0f, 107.0f, 93.0f, 84.0f, 75.0f, 73.0f, 67.0f, 61.0f, 56.0f, 47.0f, 33.0f };

  Size  textSize15( 110.0f, 60.0f );
  float positions15[] = { 108.0f, 101.0f, 87.0f, 79.0f, 70.0f, 64.0f, 54.0f, 49.0f, 44.0f, 35.0f, 27.0f, 21.0f, 12.0f, 3.0f, 96.0f, 91.0f, 88.0f, 82.0f, 79.0f, 69.0f, 61.0f, 56.0f, 47.0f, 40.0f, 37.0f, 31.0f, 28.0f, 16.0f, 11.0f, 6.0f, 3.0f, 102.0f, 98.0f, 84.0f, 75.0f, 66.0f, 64.0f, 58.0f, 52.0f, 47.0f, 38.0f, 24.0f, 9.0f };

  Size  textSize16( 110.0f, 60.0f );
  float positions16[] = { 106.0f, 99.0f, 85.0f, 77.0f, 68.0f, 62.0f, 52.0f, 47.0f, 42.0f, 33.0f, 25.0f, 20.0f, 103.0f, 94.0f, 84.0f, 79.0f, 76.0f, 70.0f, 67.0f, 57.0f, 49.0f, 44.0f, 35.0f, 28.0f, 25.0f, 20.0f, 118.0f, 106.0f, 101.0f, 96.0f, 93.0f, 84.0f, 80.0f, 66.0f, 57.0f, 48.0f, 46.0f, 40.0f, 24.0f };

  Size  textSize17( 110.0f, 60.0f );
  float positions17[] = { 106.0f, 99.0f, 85.0f, 77.0f, 68.0f, 62.0f, 52.0f, 47.0f, 42.0f, 33.0f, 25.0f, 20.0f, 103.0f, 94.0f, 84.0f, 79.0f, 76.0f, 70.0f, 67.0f, 57.0f, 49.0f, 44.0f, 35.0f, 28.0f, 25.0f, 20.0f, 118.0f, 106.0f, 101.0f, 96.0f, 93.0f, 84.0f, 80.0f, 66.0f, 57.0f, 48.0f, 46.0f, 40.0f, 24.0f };

  Size  textSize18( 120.0f, 60.0f );
  float positions18[] = { 90.0f, 79.0f, 70.0f, 68.0f, 62.0f, 56.0f, 51.0f, 42.0f, 28.0f, 25.0f, 14.0f, 7.0f, 0.0f };

  Size  textSize19( 120.0f, 60.0f );
  float positions19[] = { 103.0f, 91.0f, 86.0f, 83.0f, 77.0f, 74.0f, 64.0f, 56.0f, 51.0f, 42.0f, 35.0f, 32.0f, 27.0f };

  Size  textSize20( 110.0f, 60.0f );
  float positions20[] = { 89.0f, 82.0f, 79.0f, 69.0f, 61.0f, 56.0f, 47.0f, 40.0f, 37.0f, 31.0f, 28.0f, 16.0f, 11.0f, 6.0f, 3.0f };

  Size  textSize21( 110.0f, 60.0f );
  float positions21[] = { 93.0f, 81.0f, 76.0f, 73.0f, 67.0f, 64.0f, 54.0f, 46.0f, 41.0f, 32.0f, 25.0f, 22.0f, 17.0f };

  Size  textSize22( 110.0f, 60.0f );
  float positions22[] = { 93.0f, 81.0f, 76.0f, 73.0f, 67.0f, 64.0f, 54.0f, 46.0f, 41.0f, 32.0f, 25.0f, 22.0f, 17.0f };

  Size  textSize23( 120.0f, 50.0f );
  float positions23[] = { 0.0f, 12.0f, 21.0f, 33.0f, 45.0f };

  Size  textSize24( 120.0f, 50.0f );
  float positions24[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 93.0f };

  Size  textSize25( 120.0f, 60.0f );
  float positions25[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 0.0f, 9.0f, 19.0f, 28.0f, 39.0f, 45.0f, 49.0f, 64.0f };

  Size  textSize26( 120.0f, 60.0f );
  float positions26[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 93.0f, 104.0f, 110.0f, 0.0f, 14.0f, 25.0f, 34.0f, 39.0f, 53.0f, 64.0f, 67.0f, 76.0f, 87.0f };

  Size  textSize27( 110.0f, 60.0f );
  float positions27[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 0.0f, 9.0f, 20.0f, 26.0f, 30.0f, 44.0f, 55.0f, 64.0f, 70.0f };

  Size  textSize28( 110.0f, 60.0f );
  float positions28[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 0.0f, 9.0f, 19.0f, 28.0f, 39.0f, 45.0f, 49.0f, 64.0f };

  Size  textSize29( 120.0f, 60.0f );
  float positions29[] = { 116.0f, 109.0f, 95.0f, 87.0f, 78.0f, 72.0f, 62.0f };

  Size  textSize30( 120.0f, 60.0f );
  float positions30[] = { 116.0f, 109.0f, 95.0f, 87.0f, 78.0f, 72.0f, 62.0f, 57.0f, 52.0f, 43.0f, 35.0f, 30.0f, 113.0f, 104.0f, 94.0f, 89.0f, 86.0f, 80.0f, 77.0f, 67.0f, 59.0f, 54.0f, 34.0f };

  Size  textSize31( 110.0f, 60.0f );
  float positions31[] = { 108.0f, 101.0f, 87.0f, 79.0f, 70.0f, 64.0f, 54.0f, 49.0f, 44.0f, 35.0f, 27.0f, 21.0f, 12.0f, 3.0f, 96.0f, 91.0f, 88.0f, 82.0f, 79.0f, 69.0f, 61.0f, 56.0f, 47.0f, 40.0f, 37.0f, 31.0f, 28.0f, 11.0f };

  Size  textSize32( 110.0f, 60.0f );
  float positions32[] = { 106.0f, 99.0f, 85.0f, 77.0f, 68.0f, 62.0f, 52.0f, 47.0f, 42.0f, 33.0f, 25.0f, 20.0f, 103.0f, 94.0f, 84.0f, 79.0f, 76.0f, 70.0f, 67.0f, 57.0f, 49.0f, 44.0f, 24.0f };

  Size  textSize33( 110.0f, 60.0f );
  float positions33[] = { 106.0f, 99.0f, 85.0f, 77.0f, 68.0f, 62.0f, 52.0f, 47.0f, 42.0f, 33.0f, 25.0f, 20.0f, 103.0f, 94.0f, 84.0f, 79.0f, 76.0f, 70.0f, 67.0f, 57.0f, 49.0f, 44.0f, 24.0f };

  Size  textSize34( 120.0f, 30.0f );
  float positions34[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 94.0f };

  Size  textSize35( 120.0f, 30.0f );
  float positions35[] = { 8.0f, 24.0f, 35.0f, 44.0f, 49.0f, 63.0f, 74.0f, 77.0f, 86.0f, 96.0f, 110.0f };

  Size  textSize36( 120.0f, 30.0f );
  float positions36[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f };

  Size  textSize37(145.0f, 30.0f);
  float positions37[] = { 128.0f, 123.0f, 118.0f, 113.0f, 104.0f, 99.0f, 89.0f, 83.0f, 16.0f, 25.0f, 30.0f, 38.0f, 43.0f, 53.0f, 63.0f };

  Size  textSize38(145.0f, 30.0f);
  float positions38[] = { 77.0f, 92.0f, 97.0f, 107.0f, 116.0f, 122.0f, 132.0f, 64.0f, 50.0f, 45.0f, 40.0f, 35.0f, 26.0f, 21.0f, 11.0f };

  struct ElideData data[] =
    {
      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      //END LTR cases
      {
        "EllipsisPosition: TextLabel: Basic case SingleLine LTR END",
        "A0123456789 B0123456789 C0123456789 D0123456789 ",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::END,
        false,
        textSize01,
        1u,
        10u,
        positions01},

      {"EllipsisPosition: TextLabel: Basic case Mulitlines LineWrap-WORD LTR END",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize02,
       2u,
       22u,
       positions02},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize03,
       3u,
       29u,
       positions03},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::END,
       false,
       textSize04,
       3u,
       40u,
       positions04},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHAN LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::END,
       false,
       textSize05,
       3u,
       32u,
       positions05},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::END,
       false,
       textSize06,
       3u,
       28u,
       positions06},

      //START LTR cases
      {
        "EllipsisPosition: TextLabel: Basic case SingleLine LTR START",
        "A0123456789 B0123456789 C0123456789 D0123456789 ",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::START,
        false,
        textSize07,
        1u,
        11u,
        positions07,

      },

      {"EllipsisPosition: TextLabel: Basic case Mulitlines LineWrap-WORD LTR START",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize08,
       2u,
       23u,
       positions08},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize09,
       3u,
       33u,
       positions09},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::START,
       false,
       textSize10,
       3u,
       37u,
       positions10},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHAN LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::START,
       false,
       textSize11,
       3u,
       25u,
       positions11},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::START,
       false,
       textSize12,
       3u,
       25u,
       positions12},

      //END RTL cases
      {
        "EllipsisPosition: TextLabel: SingleLine RTL END",
        "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::END,
        false,
        textSize13,
        1u,
        14u,
        positions13},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD RTL END",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize14,
       3u,
       42u,
       positions14},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER RTL END",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::END,
       false,
       textSize15,
       3u,
       43u,
       positions15},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHENATION RTL END",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::END,
       false,
       textSize16,
       3u,
       39u,
       positions16},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED RTL END",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::END,
       false,
       textSize17,
       3u,
       39u,
       positions17},

      //START RTL cases
      {
        "EllipsisPosition: TextLabel: SingleLine RTL START",
        "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::START,
        false,
        textSize18,
        1u,
        13u,
        positions18},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD RTL START",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize19,
       3u,
       33u,
       positions19},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER RTL START",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::START,
       false,
       textSize20,
       3u,
       30u,
       positions20},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHENATION RTL START",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::START,
       false,
       textSize21,
       3u,
       33u,
       positions21},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED RTL START",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::START,
       false,
       textSize22,
       3u,
       33u,
       positions22},

      //MIDDLE LTR cases
      {
        "EllipsisPosition: TextLabel: Basic case SingleLine LTR MIDDLE",
        "ABCDEFGHIJKLMNPQRSTUVWXYZ abcdefghijklmnpqrstuvwxyz",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::MIDDLE,
        false,
        textSize23,
        1u,
        10u,
        positions23},

      {"EllipsisPosition: TextLabel: Basic case Mulitlines LineWrap-WORD LTR MIDDLE",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize24,
       2u,
       22u,
       positions24},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Goodbye",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize25,
       3u,
       24u,
       positions25},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Goodbye",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize26,
       3u,
       36u,
       positions26},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHAN LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Goodbye",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize27,
       3u,
       27u,
       positions27},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Goodbye",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize28,
       3u,
       24u,
       positions28},

      //MIDDLE RTL cases
      {
        "EllipsisPosition: TextLabel: SingleLine RTL MIDDLE",
        "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
        false,
        DevelText::LineWrap::WORD,
        DevelText::EllipsisPosition::MIDDLE,
        false,
        textSize29,
        1u,
        13u,
        positions29},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-WORD RTL MIDDLE",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize30,
       3u,
       31u,
       positions30},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-CHARACTER RTL MIDDLE",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::CHARACTER,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize31,
       3u,
       29u,
       positions31},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-HYPHENATION RTL MIDDLE",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::HYPHENATION,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize32,
       3u,
       31u,
       positions32},

      {"EllipsisPosition: TextLabel: Mulitlines LineWrap-MIXED RTL MIDDLE",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       true,
       DevelText::LineWrap::MIXED,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize33,
       3u,
       31u,
       positions33},

      {"EllipsisPosition: TextLabel: One-Line for Mulitlines LineWrap-WORD LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize34,
       1u,
       13u,
       positions34},

      {"EllipsisPosition: TextLabel: One-Line for Mulitlines LineWrap-WORD LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize35,
       1u,
       11u,
       positions35},

      {"EllipsisPosition: TextLabel: One-Line for Mulitlines LineWrap-WORD LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       true,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize36,
       1u,
       12u,
       positions36},

      {"EllipsisPosition: TextLabel: Mixed Directions Languages END",
       "سیٹنگیں Projector سیٹنگیں",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize37,
       1u,
       15u,
       positions37},

      {"EllipsisPosition: TextLabel: Mixed Directions Languages START",
       "سیٹنگیں Projector سیٹنگیں",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize38,
       1u,
       15u,
       positions38},

    };
  const unsigned int numberOfTests = 41u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !ElideTestViewModel( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}


int UtcDaliTextFieldlElideTextLocation(void)
{
  tet_infoline(" UtcDaliTextFieldlElideTextLocation ");

  Size textSize00( 100.f, 100.f );

  Size  textSize01( 120.0f, 50.0f );
  float positions01[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 93.0f };

  Size  textSize02( 120.0f, 50.0f );
  float positions02[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 94.0f };

  Size  textSize03( 120.0f, 50.0f );
  float positions03[] = { 121.0f, 114.0f, 100.0f, 92.0f, 83.0f, 77.0f, 67.0f, 62.0f, 57.0f, 48.0f, 40.0f, 34.0f, 25.0f, 10.0f };

  Size  textSize04( 120.0f, 50.0f );
  float positions04[] = { 6.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f, 102.0f, 111.0f };

  Size  textSize05( 120.0f, 50.0f );
  float positions05[] = { 8.0f, 24.0f, 35.0f, 44.0f, 49.0f, 63.0f, 74.0f, 77.0f, 86.0f, 96.0f, 110.0f };

  Size  textSize06( 120.0f, 50.0f );
  float positions06[] = { 90.0f, 79.0f, 70.0f, 68.0f, 62.0f, 56.0f, 51.0f, 42.0f, 28.0f, 25.0f, 14.0f, 7.0f, 0.0f };

  Size  textSize07( 120.0f, 50.0f );
  float positions07[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 55.0f };

  Size  textSize08( 120.0f, 50.0f );
  float positions08[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f };

  Size  textSize09( 120.0f, 50.0f );
  float positions09[] = { 117.0f, 110.0f, 96.0f, 88.0f, 79.0f, 73.0f, 63.0f };

  Size  textSize10( 120.0f, 50.0f );
  float positions10[] = { 121.0f, 116.0f, 111.0f, 106.0f, 101.0f, 96.0f, 91.0f, 86.0f, 83.0f, 76.0f, 62.0f, 54.0f, 45.0f, 39.0f, 29.0f, 15.0f };

  Size  textSize11( 120.0f, 50.0f );
  float positions11[] = { 95.0f, 79.0f, 76.0f, 65.0f, 58.0f, 51.0f, 46.0f, 41.0f, 36.0f, 31.0f, 26.0f, 21.0f, 16.0f, 11.0f, 6.0f };

  Size  textSize12( 120.0f, 50.0f );
  float positions12[] = { 118.0f, 113.0f, 108.0f, 103.0f, 98.0f, 93.0f, 88.0f, 83.0f, 80.0f, 73.0f };

  Size  textSize13(145.0f, 30.0f);
  float positions13[] = { 139.0f, 134.0f, 129.0f, 124.0f, 115.0f, 110.0f, 100.0f, 94.0f, 27.0f, 36.0f, 41.0f, 49.0f, 54.0f, 64.0f, 74.0f };

  Size  textSize14(145.0f, 30.0f);
  float positions14[] = { 88.0f, 103.0f, 108.0f, 118.0f, 127.0f, 133.0f, 143.0f, 75.0f, 61.0f, 56.0f, 51.0f, 46.0f, 37.0f, 32.0f, 22.0f };

  struct ElideData data[] =
    {
      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      {"void text",
       "",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize00,
       0u,
       0u,
       nullptr},

      {"EllipsisPosition: TextField: Basic case SingleLine LTR END",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize01,
       1u,
       10u,
       positions01},

      {"EllipsisPosition: TextField: SingleLine LTR END",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize02,
       1u,
       13u,
       positions02},

      {"EllipsisPosition: TextField: SingleLine RTL END",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize03,
       1u,
       14u,
       positions03},

      {"EllipsisPosition: TextField: Basic case SingleLine LTR START",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize04,
       1u,
       11u,
       positions04},

      {"EllipsisPosition: TextField: SingleLine LTR START",
       "Hello Hi Experimen Welcome Hello Hi Experimen Welcome",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize05,
       1u,
       11u,
       positions05},

      {"EllipsisPosition: TextField: SingleLine RTL START",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize06,
       1u,
       13u,
       positions06},

      {"EllipsisPosition: TextField: Basic case SingleLine LTR MIDDLE",
       "A0123456789 B0123456789 C0123456789 D0123456789 ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize07,
       1u,
       11u,
       positions07},

      {"EllipsisPosition: TextField: SingleLine LTR MIDDLE",
       "Hello Hi Experimen Welcome Hello Hi Experimen Goodbye",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize08,
       1u,
       13u,
       positions08},

      {"EllipsisPosition: TextField: SingleLine RTL MIDDLE",
       "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize09,
       1u,
       13u,
       positions09},

      {"EllipsisPosition: TextField: Head and Tail whitespaces RTL END",
       "        السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة         ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize10,
       1u,
       16u,
       positions10},

      {"EllipsisPosition: TextField: Head and Tail whitespaces RTL START",
       "        السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة         ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize11,
       1u,
       15u,
       positions11},

      {"EllipsisPosition: TextField: Head and Tail whitespaces RTL MIDDLE",
       "        السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة         ",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::MIDDLE,
       false,
       textSize12,
       1u,
       11u,
       positions12},

      {"EllipsisPosition: TextField: Mixed Directions Languages END",
       "سیٹنگیں Projector سیٹنگیں",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::END,
       false,
       textSize13,
       1u,
       15,
       positions13},

      {"EllipsisPosition: TextField: Mixed Directions Languages START",
       "سیٹنگیں Projector سیٹنگیں",
       false,
       DevelText::LineWrap::WORD,
       DevelText::EllipsisPosition::START,
       false,
       textSize14,
       1u,
       15,
       positions14},

    };

  const unsigned int numberOfTests = 17u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !ElideTestTextView( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }
  tet_result(TET_PASS);
  END_TEST;

}


int UtcDaliTextEditorElideTextLocation(void)
{
  tet_infoline(" UtcDaliTextEditorElideTextLocation ");

  Size textSize00( 100.f, 100.f );

  Size  textSize01( 120.0f, 50.0f );
  float positions01[] =  { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f };

  Size  textSize02( 120.0f, 60.0f );
  float positions02[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 0.0f };

  Size  textSize03( 120.0f, 60.0f );
  float positions03[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 93.0f };

  Size  textSize04( 110.0f, 60.0f );
  float positions04[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f };

  Size  textSize05( 110.0f, 60.0f );
  float positions05[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f };

  Size  textSize06( 120.0f, 50.0f );
  float positions06[] = { 6.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 92.0f, 102.0f, 111.0f };

  Size  textSize07( 120.0f, 60.0f );
  float positions07[] = { 8.0f, 25.0f, 28.0f, 37.0f, 47.0f, 61.0f, 71.0f, 77.0f, 88.0f, 99.0f, 103.0f, 106.0f, 115.0f };

  Size  textSize08( 120.0f, 60.0f );
  float positions08[] = { 8.0f, 25.0f, 34.0f, 39.0f, 53.0f, 64.0f, 67.0f, 76.0f, 86.0f, 100.0f, 110.0f };

  Size  textSize09( 100.0f, 60.0f );
  float positions09[] = { 4.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f };

  Size  textSize10( 100.0f, 60.0f );
  float positions10[] = { 4.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f };

  Size  textSize11( 120.0f, 60.0f );
  float positions11[] = { 117.0f, 110.0f, 96.0f, 88.0f, 79.0f, 73.0f, 63.0f, 58.0f, 53.0f, 44.0f, 36.0f, 31.0f, 111.0f, 102.0f, 92.0f, 87.0f };

  Size  textSize12( 110.0f, 60.0f );
  float positions12[] = { 109.0f, 102.0f, 88.0f, 80.0f, 71.0f, 65.0f, 55.0f, 50.0f, 45.0f, 36.0f, 28.0f, 22.0f };

  Size  textSize13( 110.0f, 60.0f );
  float positions13[] = { 116.0f, 109.0f, 95.0f, 87.0f, 78.0f, 72.0f, 62.0f, 57.0f, 52.0f, 43.0f, 35.0f, 30.0f, 110.0f };

  Size  textSize14( 110.0f, 60.0f );
  float positions14[] = { 116.0f, 109.0f, 95.0f, 87.0f, 78.0f, 72.0f, 62.0f, 57.0f, 52.0f, 43.0f, 35.0f, 30.0f, 110.0f };

  Size  textSize15( 120.0f, 60.0f );
  float positions15[] = { 130.0f, 118.0f, 113.0f, 110.0f, 104.0f, 101.0f, 91.0f, 83.0f, 78.0f, 69.0f, 62.0f, 59.0f, 54.0f };

  Size  textSize16( 110.0f, 60.0f );
  float positions16[] = { 92.0f, 85.0f, 82.0f, 72.0f, 64.0f, 59.0f, 50.0f, 43.0f, 40.0f, 34.0f, 31.0f, 19.0f, 14.0f, 9.0f, 6.0f };

  Size  textSize17( 110.0f, 60.0f );
  float positions17[] = { 110.0f, 98.0f, 93.0f, 90.0f, 84.0f, 81.0f, 71.0f, 63.0f, 58.0f, 49.0f, 42.0f, 39.0f, 34.0f };

  Size  textSize18( 110.0f, 60.0f );
  float positions18[] = { 110.0f, 98.0f, 93.0f, 90.0f, 84.0f, 81.0f, 71.0f, 63.0f, 58.0f, 49.0f, 42.0f, 39.0f, 34.0f };

  Size  textSize19( 120.0f, 50.0f );
  float positions19[] = { 0.0f, 12.0f, 22.0f, 32.0f, 42.0f, 51.0f, 62.0f, 72.0f, 82.0f, 93.0f };

  Size  textSize20( 120.0f, 60.0f );
  float positions20[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f };

  Size  textSize21( 120.0f, 60.0f );
  float positions21[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f };

  Size  textSize22( 110.0f, 60.0f );
  float positions22[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f };

  Size  textSize23( 110.0f, 60.0f );
  float positions23[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f };

  Size  textSize24( 120.0f, 60.0f );
  float positions24[] = { 143.0f, 136.0f, 122.0f, 114.0f, 105.0f, 99.0f, 89.0f, 84.0f, 79.0f, 70.0f, 62.0f };

  Size  textSize25( 110.0f, 60.0f );
  float positions25[] = { 111.0f, 104.0f, 90.0f, 82.0f, 73.0f, 67.0f, 57.0f, 52.0f, 47.0f, 38.0f, 30.0f, 24.0f, 15.0f, 6.0f };

  Size  textSize26( 110.0f, 60.0f );
  float positions26[] = { 123.0f, 116.0f, 102.0f, 94.0f, 85.0f, 79.0f, 69.0f, 64.0f, 59.0f, 50.0f, 42.0f };

  Size  textSize27( 110.0f, 60.0f );
  float positions27[] = { 123.0f, 116.0f, 102.0f, 94.0f, 85.0f, 79.0f, 69.0f, 64.0f, 59.0f, 50.0f, 42.0f };

  Size  textSize28( 120.0f, 30.0f );
  float positions28[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f, 59.0f, 65.0f, 74.0f, 84.0f, 94.0f };

  Size  textSize29( 120.0f, 30.0f );
  float positions29[] = { 8.0f, 24.0f, 35.0f, 44.0f, 49.0f, 63.0f, 74.0f, 77.0f, 86.0f, 96.0f, 110.0f };

  Size  textSize30( 120.0f, 30.0f );
  float positions30[] = { 0.0f, 11.0f, 22.0f, 26.0f, 29.0f, 38.0f, 44.0f, 56.0f };


  struct ElideData data[] =
  {
    {
      "void text",
      "",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::END,
      false,
      textSize00,
      0u,
      0u,
      nullptr
    },

    {
      "void text",
      "",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::START,
      false,
      textSize00,
      0u,
      0u,
      nullptr
    },

    {
      "void text",
      "",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize00,
      0u,
      0u,
      nullptr
    },

   //END LTR cases

    {
      "EllipsisPosition: TextEditor: Basic case Mulitlines LineWrap-WORD LTR END",
      "A0123456789 B0123456789 C0123456789 D0123456789 ",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::END,
      false,
      textSize01,
      2u,
      22u,
      positions01
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD LTR END",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::END,
      false,
      textSize02,
      3u,
      29u,
      positions02
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER LTR END",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::END,
      false,
      textSize03,
      3u,
      40u,
      positions03
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHAN LTR END",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::END,
      false,
      textSize04,
      3u,
      32u,
      positions04
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED LTR END",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::END,
      false,
      textSize05,
      3u,
      28u,
      positions05
    },

   //START LTR cases

    {
      "EllipsisPosition: TextEditor: Basic case Mulitlines LineWrap-WORD LTR START",
      "A0123456789 B0123456789 C0123456789 D0123456789 ",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::START,
      false,
      textSize06,
      2u,
      23u,
      positions06
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD LTR START",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::START,
      false,
      textSize07,
      3u,
      33u,
      positions07
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER LTR START",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::START,
      false,
      textSize08,
      3u,
      37u,
      positions08
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHAN LTR START",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::START,
      false,
      textSize09,
      3u,
      25u,
      positions09
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED LTR START",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::START,
      false,
      textSize10,
      3u,
      25u,
      positions10
    },

  //END RTL cases

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD RTL END",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::END,
      false,
      textSize11,
      3u,
      42u,
      positions11
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER RTL END",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::END,
      false,
      textSize12,
      3u,
      43u,
      positions12
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHENATION RTL END",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::END,
      false,
      textSize13,
      3u,
      39u,
      positions13
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED RTL END",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::END,
      false,
      textSize14,
      3u,
      39u,
      positions14
    },

   //START RTL cases

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD RTL START",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::START,
      false,
      textSize15,
      3u,
      33u,
      positions15
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER RTL START",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::START,
      false,
      textSize16,
      3u,
      30u,
      positions16
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHENATION RTL START",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::START,
      false,
      textSize17,
      3u,
      33u,
      positions17
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED RTL START",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::START,
      false,
      textSize18,
      3u,
      33u,
      positions18
    },

//MIDDLE LTR cases

    {
      "EllipsisPosition: TextEditor: Basic case Mulitlines LineWrap-WORD LTR MIDDLE",
      "A0123456789 B0123456789 C0123456789 D0123456789 ",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize19,
      2u,
      22u,
      positions19
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD LTR MIDDLE",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize20,
      3u,
      24u,
      positions20
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER LTR MIDDLE",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize21,
      3u,
      36u,
      positions21
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHAN LTR MIDDLE",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize22,
      3u,
      27u,
      positions22
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED LTR MIDDLE",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize23,
      3u,
      24u,
      positions23
    },

//MIDDLE RTL cases

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-WORD RTL MIDDLE",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize24,
      3u,
      31u,
      positions24
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-CHARACTER RTL MIDDLE",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::CHARACTER,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize25,
      3u,
      29u,
      positions25
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-HYPHENATION RTL MIDDLE",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::HYPHENATION,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize26,
      3u,
      31u,
      positions26
    },

    {
      "EllipsisPosition: TextEditor: Mulitlines LineWrap-MIXED RTL MIDDLE",
      "السلام عليكم مرحبا اهلا هذا اختبار شكرا للمساعدة",
      true,
      DevelText::LineWrap::MIXED,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize27,
      3u,
      31u,
      positions27
    },

    {
      "EllipsisPosition: TextEditor: One-Line for Mulitlines LineWrap-WORD LTR END",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::END,
      false,
      textSize28,
      1u,
      13u,
      positions28
    },

    {
      "EllipsisPosition: TextEditor: One-Line for Mulitlines LineWrap-WORD LTR START",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::START,
      false,
      textSize29,
      1u,
      11u,
      positions29
    },

    {
      "EllipsisPosition: TextEditor: One-Line for Mulitlines LineWrap-WORD LTR MIDDLE",
      "Hello Hi Experimen Welcome Hello Hi Experimen Welcome" ,
      true,
      DevelText::LineWrap::WORD,
      DevelText::EllipsisPosition::MIDDLE,
      false,
      textSize30,
      1u,
      12u,
      positions30
    },

  };
  const unsigned int numberOfTests = 33u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    ToolkitTestApplication application;
    if( !ElideTestTextView( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}
