/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextEditorSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliTextEditorSelectText" );

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 60.f ) );
  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello World" );

  // Add the text editor to the stage
  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl( textEditor );

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textEditorImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  std::string selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK( selectedText == "Hello World" );

  // Select None
  textEditorImpl.SelectNone();

  application.SendNotification();
  application.Render();

  selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK( selectedText == "" );

  END_TEST;
}

int UtcDaliTextEditorMarkupUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupUnderline ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add( textEditor );

  textEditor.SetProperty( TextEditor::Property::TEXT, "<u>ABC</u>EF<u>GH</u>" );
  textEditor.SetProperty( TextEditor ::Property::ENABLE_MARKUP,  true );

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfUnderlinedGlyphs = 5u;

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl( textEditor );
  const Text::Length numberOfUnderlineRuns = textEditorImpl.getController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS( numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION );

  Vector<GlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.getController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS( underlineRuns[0u].glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[1u].glyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[2u].glyphIndex, 2u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS( underlineRuns[3u].glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[4u].glyphIndex, 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorFontPointSizeLargerThanAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorFontPointSizeLargerThanAtlas ");

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set size to avoid automatic eliding
  textEditor.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textEditor.SetProperty( TextEditor::Property::POINT_SIZE, 1000);
  //Specify font-family
  textEditor.SetProperty( TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textEditor.SetProperty(TextEditor::Property::TEXT, "A");

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS( countAtlas, 1, TEST_LOCATION );

  END_TEST;
}


int UtcDaliTextEditorFontPointSizeLargerThanAtlasPlaceholderCase(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorFontPointSizeLargerThanAtlasPlaceholderCase ");

  //Set Map of placeholder: text, font-family and point-size
  Property::Map placeholderMapSet;
  placeholderMapSet["text"] = "A";
  placeholderMapSet["fontFamily"] = "DejaVu Sans";
  placeholderMapSet["pixelSize"] = 1000.0f;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set size to avoid automatic eliding
  textEditor.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set placeholder
  textEditor.SetProperty( TextEditor::Property::PLACEHOLDER, placeholderMapSet) ;

  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS( countAtlas, 1, TEST_LOCATION );

  END_TEST;
}