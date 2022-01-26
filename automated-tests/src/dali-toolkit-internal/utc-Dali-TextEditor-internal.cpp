/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextEditorSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorSelectText");

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  textEditor.SetProperty(TextEditor::Property::TEXT, "Hello World");

  // Add the text editor to the stage
  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl(textEditor);

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textEditorImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  std::string selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK(selectedText == "Hello World");

  // Select None
  textEditorImpl.SelectNone();

  application.SendNotification();
  application.Render();

  selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK(selectedText == "");

  END_TEST;
}

int UtcDaliTextEditorMarkupUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupUnderline ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  textEditor.SetProperty(TextEditor::Property::TEXT, "<u>ABC</u>EF<u>GH</u>");
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfUnderlinedGlyphs = 5u;

  Toolkit::Internal::TextEditor& textEditorImpl        = GetImpl(textEditor);
  const Text::Length             numberOfUnderlineRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<GlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS(underlineRuns[0u].glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[1u].glyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[2u].glyphIndex, 2u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS(underlineRuns[3u].glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[4u].glyphIndex, 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorFontPointSizeLargerThanAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorFontPointSizeLargerThanAtlas ");

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set size to avoid automatic eliding
  textEditor.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textEditor.SetProperty(TextEditor::Property::POINT_SIZE, 1000);
  //Specify font-family
  textEditor.SetProperty(TextEditor::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textEditor.SetProperty(TextEditor::Property::TEXT, "A");

  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS(countAtlas, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorFontPointSizeLargerThanAtlasPlaceholderCase(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorFontPointSizeLargerThanAtlasPlaceholderCase ");

  //Set Map of placeholder: text, font-family and point-size
  Property::Map placeholderMapSet;
  placeholderMapSet["text"]       = "A";
  placeholderMapSet["fontFamily"] = "DejaVu Sans";
  placeholderMapSet["pixelSize"]  = 1000.0f;

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  //Set size to avoid automatic eliding
  textEditor.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  //Set placeholder
  textEditor.SetProperty(TextEditor::Property::PLACEHOLDER, placeholderMapSet);

  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS(countAtlas, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorBackgroundTag(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorBackgroundTag\n");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  editor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  editor.SetProperty(TextEditor::Property::TEXT, "H<background color='red'>e</background> Worl<background color='yellow'>d</background>");
  application.GetScene().Add(editor);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& editorImpl                   = GetImpl(editor);
  const ColorIndex* const        backgroundColorIndicesBuffer = editorImpl.GetTextController()->GetTextModel()->GetBackgroundColorIndices();

  DALI_TEST_CHECK(backgroundColorIndicesBuffer);

  //default color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[0], 0u, TEST_LOCATION);

  //red color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[1], 1u, TEST_LOCATION);

  //yellow color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[7], 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorTextWithSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorTextWithSpan\n");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  editor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  editor.SetProperty(TextEditor::Property::TEXT, "Hello Span");
  application.GetScene().Add(editor);

  application.SendNotification();
  application.Render();

  Vector3 originalSize = editor.GetNaturalSize();
  editor.SetProperty(TextEditor::Property::TEXT, "H<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ello</span> Span");

  application.SendNotification();
  application.Render();

  Vector3 spanSize = editor.GetNaturalSize();

  DALI_TEST_GREATER(spanSize.width, originalSize.width, TEST_LOCATION);

  Toolkit::Internal::TextEditor& editorImpl          = GetImpl(editor);
  const ColorIndex* const        colorIndicesBuffer1 = editorImpl.GetTextController()->GetTextModel()->GetColorIndices();

  DALI_TEST_CHECK(colorIndicesBuffer1);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[0], 0u, TEST_LOCATION);

  //span color
  DALI_TEST_EQUALS(colorIndicesBuffer1[1], 1u, TEST_LOCATION);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[6], 0u, TEST_LOCATION);

  editor.SetProperty(TextEditor::Property::TEXT, "<span font-size='45'>H</span>ello <span text-color='red'>S</span>pan");

  application.SendNotification();
  application.Render();

  const ColorIndex* const colorIndicesBuffer2 = editorImpl.GetTextController()->GetTextModel()->GetColorIndices();

  DALI_TEST_CHECK(colorIndicesBuffer2);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer2[0], 0u, TEST_LOCATION);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer2[1], 0u, TEST_LOCATION);

  //span color
  DALI_TEST_EQUALS(colorIndicesBuffer2[6], 1u, TEST_LOCATION);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer2[7], 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorControlBackgroundColor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorControlBackgroundColor\n");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  Vector4 backgroundColor;

  editor.SetProperty(TextEditor::Property::TEXT, "Background Color");
  application.GetScene().Add(editor);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& editorImpl     = GetImpl(editor);
  ControllerPtr                  controller     = editorImpl.GetTextController();
  Controller::Impl&              controllerImpl = Controller::Impl::GetImplementation(*controller.Get());

  // Default color is transparent
  controllerImpl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
  DALI_TEST_EQUALS(backgroundColor, Color::TRANSPARENT, TEST_LOCATION);

  // Set background color to red
  editor.SetBackgroundColor(Color::RED);
  application.SendNotification();
  application.Render();

  // Should be red
  controllerImpl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
  DALI_TEST_EQUALS(backgroundColor, Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorTextPositionWithMinLineAndBigFont(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorTextPositionWithMinLine ");

  TextEditor textEditor = TextEditor::New();

  textEditor.SetProperty(TextEditor::Property::TEXT, "<span font-size='45'>H</span>\ni");
  textEditor.SetProperty(DevelTextEditor::Property::MIN_LINE_SIZE, 50);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl(textEditor);
  Text::ViewInterface&           view           = textEditorImpl.GetTextController()->GetView();

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  DALI_TEST_EQUALS(numberOfGlyphs, 3u, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Vector<GlyphInfo> glyphs;
  glyphs.Resize(numberOfGlyphs);

  Vector<Vector2> positions;
  positions.Resize(numberOfGlyphs);

  float alignmentOffset = 0u;
  numberOfGlyphs        = view.GetGlyphs(glyphs.Begin(),
                                  positions.Begin(),
                                  alignmentOffset,
                                  0u,
                                  numberOfGlyphs);

  DALI_TEST_EQUALS(positions[2].y, 165.f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMarkupStrikethrough(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupStrikethrough ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  textEditor.SetProperty(TextEditor::Property::TEXT, "<s>ABC</s>EF<s color='red'>GH</s>");
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 2u;

  Toolkit::Internal::TextEditor& textEditorImpl            = GetImpl(textEditor);
  const Text::Length             numberOfStrikethroughRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  //ABC have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].glyphRun.numberOfGlyphs, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(!strikethroughRuns[0u].isColorSet);

  //GH have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(strikethroughRuns[1u].isColorSet);

  END_TEST;
}
