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
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>

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

  uint32_t expectedNumberOfUnderlineRuns = 2u;

  Toolkit::Internal::TextEditor& textEditorImpl        = GetImpl(textEditor);
  const Text::Length             numberOfUnderlineRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.numberOfGlyphs, 3u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMarkupUnderlineAttributes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupUnderlineAttributes ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText =
    "start<u>ABC1</u>then"
    "<u type='solid'>ABC2</u>then"
    "<u type='dashed'>ABC3</u>then"
    "<u type='double'>ABC4</u>then"
    "<u color='green'>ABC5</u>then"
    "<u height='5.0f'>ABC6</u>then"
    "<u type='dashed' dash-gap='3.0f'>ABC7</u>then"
    "<u type='dashed' dash-width='4.0f'>ABC8</u>then"
    "<u color='blue' type='dashed' height='4.0f' dash-gap='2.0f' dash-width='3.0f'>ABC9</u>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfUnderlineRuns = 9u;

  Toolkit::Internal::TextEditor& textEditorImpl        = GetImpl(textEditor);
  const Text::Length             numberOfUnderlineRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  struct DataOfCase
  {
    std::string              title;
    GlyphIndex               glyphIndex;
    Length                   numberOfGlyphs;
    UnderlineStyleProperties properties;
  };
  DataOfCase data[] =
    {
      //<u>ABC1</u>
      {"<u>ABC1</u>",
       5u,
       4u,
       {
         Text::Underline::SOLID,
         Color::BLACK,
         0u,
         1u,
         2u,
         false,
         false,
         false,
         false,
         false,
       }},

      //<u type='solid'>ABC2</u>
      {"<u type='solid'>ABC2</u>",
       13u,
       4u,
       {
         Text::Underline::SOLID,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u type='dashed'>ABC3</u>
      {"<u type='dashed'>ABC3</u>",
       21u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u type='double'>ABC4</u>
      {"<u type='double'>ABC4</u>",
       29u,
       4u,
       {
         Text::Underline::DOUBLE,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u color='green'>ABC5</u>
      {"<u color='green'>ABC5</u>",
       37u,
       4u,
       {
         Text::Underline::SOLID,
         Color::GREEN,
         0u,
         1u,
         2u,
         false,
         true,
         false,
         false,
         false,
       }},

      //<u height='5.0f'>ABC6</u>
      {"<u height='5.0f'>ABC6</u>",
       45u,
       4u,
       {
         Text::Underline::SOLID,
         Color::BLACK,
         5u,
         1u,
         2u,
         false,
         false,
         true,
         false,
         false,
       }},

      //<u type='dashed' dash-gap='3.0f'>ABC7</u>
      {"<u type='dashed' dash-gap='3.0f'>ABC7</u>",
       53u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         3u,
         2u,
         true,
         false,
         false,
         true,
         false,
       }},

      //<u type='dashed' dash-width='4.0f'>ABC8</u>
      {"<u type='dashed' dash-width='4.0f'>ABC8</u>",
       61u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         1u,
         4u,
         true,
         false,
         false,
         false,
         true,
       }},

      //<u color='blue' type='dashed' height='4.0f' dash-gap='2.0f' dash-width='3.0f'>
      {"<u color='blue' type='dashed' height='4.0f' dash-gap='2.0f' dash-width='3.0f'>",
       69u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLUE,
         4u,
         2u,
         3u,
         true,
         true,
         true,
         true,
         true,
       }},

    };

  for(uint32_t i = 0; i < expectedNumberOfUnderlineRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == underlineRuns[i].properties);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupSpanUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupSpanUnderline ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText =
    "start<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ABC1</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='solid'>ABC2</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed'>ABC3</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='double'>ABC4</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='green'>ABC5</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-height='5.0f'>ABC6</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-gap='3.0f'>ABC7</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-width='4.0f'>ABC8</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='blue' u-type='dashed' u-height='4.0f' u-dash-gap='2.0f' u-dash-width='3.0f'>ABC9</span>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfUnderlineRuns = 8u;

  Toolkit::Internal::TextEditor& textEditorImpl        = GetImpl(textEditor);
  const Text::Length             numberOfUnderlineRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  struct DataOfCase
  {
    std::string              title;
    GlyphIndex               glyphIndex;
    Length                   numberOfGlyphs;
    UnderlineStyleProperties properties;
  };
  DataOfCase data[] =
    {
      //<u type='solid'>ABC2</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='solid'>ABC2</span>",
       13u,
       4u,
       {
         Text::Underline::SOLID,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u type='dashed'>ABC3</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed'>ABC3</span>",
       21u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u type='double'>ABC4</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='double'>ABC4</span>",
       29u,
       4u,
       {
         Text::Underline::DOUBLE,
         Color::BLACK,
         0u,
         1u,
         2u,
         true,
         false,
         false,
         false,
         false,
       }},

      //<u color='green'>ABC5</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='green'>ABC5</span>",
       37u,
       4u,
       {
         Text::Underline::SOLID,
         Color::GREEN,
         0u,
         1u,
         2u,
         false,
         true,
         false,
         false,
         false,
       }},

      //<u height='5.0f'>ABC6</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-height='5.0f'>ABC6</span>",
       45u,
       4u,
       {
         Text::Underline::SOLID,
         Color::BLACK,
         5u,
         1u,
         2u,
         false,
         false,
         true,
         false,
         false,
       }},

      //<u type='dashed' dash-gap='3.0f'>ABC7</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-gap='3.0f'>ABC7</span>",
       53u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         3u,
         2u,
         true,
         false,
         false,
         true,
         false,
       }},

      //<u type='dashed' dash-width='4.0f'>ABC8</u>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-width='4.0f'>ABC8</span>",
       61u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLACK,
         0u,
         1u,
         4u,
         true,
         false,
         false,
         false,
         true,
       }},

      //<u color='blue' type='dashed' height='4.0f' dash-gap='2.0f' dash-width='3.0f'>
      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='blue' u-type='dashed' u-height='4.0f' u-dash-gap='2.0f' u-dash-width='3.0f'>ABC9</span>",
       69u,
       4u,
       {
         Text::Underline::DASHED,
         Color::BLUE,
         4u,
         2u,
         3u,
         true,
         true,
         true,
         true,
         true,
       }},

    };

  for(uint32_t i = 0; i < expectedNumberOfUnderlineRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == underlineRuns[i].properties);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupNestedUnderlineTags(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupNestedUnderlineTags ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText = "start<u height='5.0f' color='green' >AB<u color='blue' >XYZ</u>CDE</u>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfUnderlineRuns = 2u;

  Toolkit::Internal::TextEditor& textEditorImpl        = GetImpl(textEditor);
  const Text::Length             numberOfUnderlineRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  struct DataOfCase
  {
    std::string              title;
    GlyphIndex               glyphIndex;
    Length                   numberOfGlyphs;
    UnderlineStyleProperties properties;
  };
  DataOfCase data[] =
    {
      //Outter
      {"<u height='5.0f' color='green' >AB<u color='blue' >XYZ</u>CDE</u>",
       5u,
       8u,
       {
         Text::Underline::SOLID,
         Color::GREEN,
         5u,
         1u,
         2u,
         false,
         true,
         true,
         false,
         false,
       }},

      //Inner
      {"<u color='blue' >XYZ</u>",
       7u,
       3u,
       {
         Text::Underline::SOLID,
         Color::BLUE,
         5u,
         1u,
         2u,
         false,
         true,
         true,
         false,
         false,
       }},

    };

  for(uint32_t i = 0; i < expectedNumberOfUnderlineRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(underlineRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == underlineRuns[i].properties);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupNestedStrikethroughTags(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupNestedStrikethroughTags ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText = "start<s height='5.0f' color='green' >AB<s color='blue' >XYZ</s>CDE</s>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 2u;

  Toolkit::Internal::TextEditor& textEditorImpl            = GetImpl(textEditor);
  const Text::Length             numberOfStrikethroughRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  struct DataOfCase
  {
    std::string                  title;
    GlyphIndex                   glyphIndex;
    Length                       numberOfGlyphs;
    StrikethroughStyleProperties properties;
  };
  DataOfCase data[] =
    {
      //Outter
      {"<s height='5.0f' color='green' >AB<s color='blue' >XYZ</s>CDE</s>",
       5u,
       8u,
       {
         Color::GREEN,
         5.0f,
         true,
         true,
       }},

      //Inner
      {"<s color='blue' >XYZ</s>",
       7u,
       3u,
       {
         Color::BLUE,
         5.0f,
         true,
         true,
       }},

    };

  for(uint32_t i = 0; i < expectedNumberOfStrikethroughRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == strikethroughRuns[i].properties);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupStrikethroughAttributes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupStrikethroughAttributes ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText =
    "start<s>ABC1</s>then"
    "<s color='green'>ABC2</s>then"
    "<s height='5.0f'>ABC3</s>then"
    "<s color='blue' height='4.0f' >ABC4</s>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 4u;

  Toolkit::Internal::TextEditor& textEditorImpl            = GetImpl(textEditor);
  const Text::Length             numberOfStrikethroughRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  struct DataOfCase
  {
    std::string                  title;
    GlyphIndex                   glyphIndex;
    Length                       numberOfGlyphs;
    StrikethroughStyleProperties properties;
  };
  DataOfCase data[] =
    {

      {"<s>ABC1</s>",
       5u,
       4u,
       {Color::BLACK,
        0.0f,
        false,
        false}},

      {"<s color='green'>ABC2</s>",
       13u,
       4u,
       {Color::GREEN,
        0.0f,
        true,
        false}},

      {"<s height='5.0f'>ABC3</s>",
       21u,
       4u,
       {Color::BLACK,
        5.0f,
        false,
        true}},

      {"<s color='blue' height='4.0f' >ABC4</s>",
       29u,
       4u,
       {Color::BLUE,
        4.0f,
        true,
        true}},

    };

  for(uint32_t i = 0; i < expectedNumberOfStrikethroughRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == strikethroughRuns[i].properties);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupSpanStrikethrough(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupSpanStrikethrough ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  std::string testText =
    "start<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ABC1</span>then"
    "<span s-color='blue'>ABC2</span>then"
    "<span s-height='2.0f'>ABC3</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' s-color='green' s-height='5.0f'>ABC4</span>end";

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 3u;

  Toolkit::Internal::TextEditor& textEditorImpl            = GetImpl(textEditor);
  const Text::Length             numberOfStrikethroughRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textEditorImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  struct DataOfCase
  {
    std::string                  title;
    GlyphIndex                   glyphIndex;
    Length                       numberOfGlyphs;
    StrikethroughStyleProperties properties;
  };
  DataOfCase data[] =
    {

      {"<span s-color='blue'>ABC2</span>then",
       13u,
       4u,
       {Color::BLUE,
        0.0f,
        true,
        false}},

      {"<span s-height='2.0f'>ABC3</span>then",
       21u,
       4u,
       {Color::BLACK,
        2.0f,
        false,
        true}},

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' s-color='green' s-height='5.0f'>ABC4</span>",
       29u,
       4u,
       {Color::GREEN,
        5.0f,
        true,
        true}},

    };

  for(uint32_t i = 0; i < expectedNumberOfStrikethroughRuns; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.glyphIndex, data[i].glyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(strikethroughRuns[i].glyphRun.numberOfGlyphs, data[i].numberOfGlyphs, TEST_LOCATION);
    DALI_TEST_CHECK(data[i].properties == strikethroughRuns[i].properties);
  }

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

int UtcDaliTextEditorSpanBackgroundTag(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextEditorSpanBackgroundTag\n");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  editor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  editor.SetProperty(TextEditor::Property::TEXT, "H<span background-color='red'>e</span> Worl<span background-color='yellow'>d</span>");
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
  DALI_TEST_CHECK(!strikethroughRuns[0u].properties.colorDefined);

  //GH have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(strikethroughRuns[1u].properties.colorDefined);

  END_TEST;
}

int UtcDaliTextEditorMarkupStrikethroughNoEndTag(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupStrikethroughNoEndTag ");

  TextEditor textEditor = TextEditor::New();

  application.GetScene().Add(textEditor);

  textEditor.SetProperty(TextEditor::Property::TEXT, "<s>ABC");
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 0u;

  Toolkit::Internal::TextEditor& textEditorImpl            = GetImpl(textEditor);
  Text::Length                   numberOfStrikethroughRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMarkupParagraphTag(void)

{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTag ");

  TextEditor textEditor = TextEditor::New();
  application.GetScene().Add(textEditor);

  textEditor.SetProperty(TextEditor::Property::TEXT, "text one <p>Paragraph two</p> text three <p>Paragraph four</p> text five");
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfBoundedParagraphRuns = 2u;

  Toolkit::Internal::TextEditor& textEditorImpl               = GetImpl(textEditor);
  const Text::Length             numberOfBoundedParagraphRuns = textEditorImpl.GetTextController()->GetTextModel()->GetNumberOfBoundedParagraphRuns();
  DALI_TEST_EQUALS(numberOfBoundedParagraphRuns, expectedNumberOfBoundedParagraphRuns, TEST_LOCATION);

  const Vector<BoundedParagraphRun>& boundedParagraphRuns = textEditorImpl.GetTextController()->GetTextModel()->GetBoundedParagraphRuns();

  //<p>Paragraph two</p>
  DALI_TEST_EQUALS(boundedParagraphRuns[0u].characterRun.characterIndex, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(boundedParagraphRuns[0u].characterRun.numberOfCharacters, 14u, TEST_LOCATION);

  //<p>Paragraph four</p>
  DALI_TEST_EQUALS(boundedParagraphRuns[1u].characterRun.characterIndex, 37u, TEST_LOCATION);
  DALI_TEST_EQUALS(boundedParagraphRuns[1u].characterRun.numberOfCharacters, 15u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMarkupParagraphTagAlignAttribute(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute ");

  // Apply alignment for each type on property level on three paragraphs and in-between text.
  // Apply align in markup on the three paragraphs (each one a type).
  // Using the same text to gain similar results from both the property level and the markup.
  // Compare line alignment between the property level and the markup.

  std::string textAlignOnPropertyLevel = "text outside<p>Paragraph end</p>text outside<p>Paragraph center</p>text outside<p>Paragraph begin</p><p>Paragraph property alignment</p>";
  std::string textAlignInMarkup        = "text outside<p align='end'>Paragraph end</p>text outside<p align='center'>Paragraph center</p>text outside<p align='begin' >Paragraph begin</p><p>Paragraph property alignment</p>";

  //Set size to avoid automatic eliding
  Vector2 controllerSize = Vector2(1025, 1025);

  TextEditor textEditorBeginAlign  = TextEditor::New();
  TextEditor textEditorCenterAlign = TextEditor::New();
  TextEditor textEditorEndAlign    = TextEditor::New();
  TextEditor textEditorMultiAlign  = TextEditor::New();

  application.GetScene().Add(textEditorBeginAlign);
  application.GetScene().Add(textEditorCenterAlign);
  application.GetScene().Add(textEditorEndAlign);
  application.GetScene().Add(textEditorMultiAlign);

  textEditorBeginAlign.SetProperty(TextEditor::Property::TEXT, textAlignOnPropertyLevel);
  textEditorBeginAlign.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  textEditorBeginAlign.SetProperty(DevelTextEditor::Property::ELLIPSIS, false);
  textEditorBeginAlign.SetProperty(TextEditor::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::BEGIN);
  textEditorBeginAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textEditorCenterAlign.SetProperty(TextEditor::Property::TEXT, textAlignOnPropertyLevel);
  textEditorCenterAlign.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  textEditorCenterAlign.SetProperty(DevelTextEditor::Property::ELLIPSIS, false);
  textEditorCenterAlign.SetProperty(TextEditor::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::CENTER);
  textEditorCenterAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textEditorEndAlign.SetProperty(TextEditor::Property::TEXT, textAlignOnPropertyLevel);
  textEditorEndAlign.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  textEditorEndAlign.SetProperty(DevelTextEditor::Property::ELLIPSIS, false);
  textEditorEndAlign.SetProperty(TextEditor::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::END);
  textEditorEndAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textEditorMultiAlign.SetProperty(TextEditor::Property::TEXT, textAlignInMarkup);
  textEditorMultiAlign.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);
  textEditorMultiAlign.SetProperty(DevelTextEditor::Property::ELLIPSIS, false);
  textEditorMultiAlign.SetProperty(TextEditor::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::CENTER);
  textEditorMultiAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfBoundedParagraphRuns = 4u;
  uint32_t expectedNumberOfLines                = 7u;

  Toolkit::Internal::TextEditor& textEditorMultiAlignImpl  = GetImpl(textEditorMultiAlign);
  Toolkit::Internal::TextEditor& textEditorBeginAlignImpl  = GetImpl(textEditorBeginAlign);
  Toolkit::Internal::TextEditor& textEditorCenterAlignImpl = GetImpl(textEditorCenterAlign);
  Toolkit::Internal::TextEditor& textEditorEndAlignImpl    = GetImpl(textEditorEndAlign);

  const Text::Length numberOfBoundedParagraphRuns = textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetNumberOfBoundedParagraphRuns();
  DALI_TEST_EQUALS(numberOfBoundedParagraphRuns, expectedNumberOfBoundedParagraphRuns, TEST_LOCATION);

  DALI_TEST_EQUALS(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textEditorBeginAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textEditorBeginAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textEditorEndAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textEditorEndAlignImpl.GetTextController()->GetTextModel()->GetLines());

  const uint32_t LINE_INDEX_ALIGN_END    = 1u;
  const uint32_t LINE_INDEX_ALIGN_CENTER = 3u;
  const uint32_t LINE_INDEX_ALIGN_BEGIN  = 5u;
  const uint32_t LINE_INDEX_OUTSIDE_1    = 0u;
  const uint32_t LINE_INDEX_OUTSIDE_2    = 2u;
  const uint32_t LINE_INDEX_OUTSIDE_3    = 4u;
  const uint32_t LINE_INDEX_PARAGRAPH    = 6u;

  //<p align='end'>Paragraph end</p>
  const LineRun& lineEndFromMultiAlign = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_END);
  const LineRun& lineEndFromEndAlign   = *(textEditorEndAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_END);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - <p align='end'>Paragraph end</p>");
  DALI_TEST_EQUALS(lineEndFromMultiAlign.alignmentOffset, lineEndFromEndAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineEndFromMultiAlign.width, lineEndFromEndAlign.width, TEST_LOCATION);

  //<p align='center'>Paragraph center</p>
  const LineRun& lineCenterFromMultiAlign = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_CENTER);
  const LineRun& lineEndFromCenterAlign   = *(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_CENTER);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - <p align='center'>Paragraph center</p>");
  DALI_TEST_EQUALS(lineCenterFromMultiAlign.alignmentOffset, lineEndFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineCenterFromMultiAlign.width, lineEndFromCenterAlign.width, TEST_LOCATION);

  //<p align='begin' >Paragraph begin</p>
  const LineRun& lineBeginFromMultiAlign = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_BEGIN);
  const LineRun& lineEndFromBeginAlign   = *(textEditorBeginAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_BEGIN);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - <p align='begin' >Paragraph begin</p>");
  DALI_TEST_EQUALS(lineBeginFromMultiAlign.alignmentOffset, lineEndFromBeginAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineBeginFromMultiAlign.width, lineEndFromBeginAlign.width, TEST_LOCATION);

  //text outside
  const LineRun& lineOutsideOneFromMultiAlign  = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_1);
  const LineRun& lineOutsideOneFromCenterAlign = *(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_1);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - text outside one");
  DALI_TEST_EQUALS(lineOutsideOneFromMultiAlign.alignmentOffset, lineOutsideOneFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideOneFromMultiAlign.width, lineOutsideOneFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineOutsideTwoFromMultiAlign  = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_2);
  const LineRun& lineOutsideTwoFromCenterAlign = *(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_2);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - text outside two");
  DALI_TEST_EQUALS(lineOutsideTwoFromMultiAlign.alignmentOffset, lineOutsideTwoFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideTwoFromMultiAlign.width, lineOutsideTwoFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineOutsideThreeFromMultiAlign  = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_3);
  const LineRun& lineOutsideThreeFromCenterAlign = *(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_3);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - text outside three");
  DALI_TEST_EQUALS(lineOutsideThreeFromMultiAlign.alignmentOffset, lineOutsideThreeFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideThreeFromMultiAlign.width, lineOutsideThreeFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineParagraphFromMultiAlign  = *(textEditorMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_PARAGRAPH);
  const LineRun& lineParagraphFromCenterAlign = *(textEditorCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_PARAGRAPH);
  tet_infoline(" UtcDaliTextEditorMarkupParagraphTagAlignAttribute - <p>Paragraph property alignment</p>");
  DALI_TEST_EQUALS(lineParagraphFromMultiAlign.alignmentOffset, lineParagraphFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineParagraphFromMultiAlign.width, lineParagraphFromCenterAlign.width, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextEditorMarkupCharacterSpacingTag(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupCharacterSpacingTag ");

  const Length EXPECTED_NUMBER_OF_GLYPHS = 21u;

  const float expandedCharSpacing  = 10.0f;
  const float condensedCharSpacing = -5.0f;

  TextEditor textEditor = TextEditor::New();

  textEditor.SetProperty(TextEditor::Property::TEXT, "ABC EF\n<char-spacing value='-5.0f'>ABC EF\n</char-spacing><char-spacing value='10.0f'>ABC EF\n</char-spacing>");
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl(textEditor);
  Text::ViewInterface&           view           = textEditorImpl.GetTextController()->GetView();

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  DALI_TEST_EQUALS(numberOfGlyphs, EXPECTED_NUMBER_OF_GLYPHS, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

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

  const Length numberOfGlyphsOneLine = 7u;
  for(Length i = 0; i < numberOfGlyphsOneLine - 1u; i++)
  {
    float diffLineNoCharSpacing = positions[i + 1].x - positions[i].x;

    float diffLineCondensedCharSpacing = positions[numberOfGlyphsOneLine + i + 1].x - positions[numberOfGlyphsOneLine + i].x;
    DALI_TEST_EQUALS(diffLineCondensedCharSpacing, diffLineNoCharSpacing + condensedCharSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

    float diffLineExpandedCharSpacing = positions[2u * numberOfGlyphsOneLine + i + 1].x - positions[2u * numberOfGlyphsOneLine + i].x;
    DALI_TEST_EQUALS(diffLineExpandedCharSpacing, diffLineNoCharSpacing + expandedCharSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextEditorMarkupSpanCharacterSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEditorMarkupSpanCharacterSpacing ");

  const Length EXPECTED_NUMBER_OF_GLYPHS = 21u;

  const float expandedCharSpacing  = 10.0f;
  const float condensedCharSpacing = -5.0f;

  std::string testText =
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='blue' >ABC EF\n</span>"
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' char-space-value='-5.0f'>ABC EF\n</span>"
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='green' char-space-value='10.0f'>ABC EF\n</span>";

  TextEditor textEditor = TextEditor::New();

  textEditor.SetProperty(TextEditor::Property::TEXT, testText);
  textEditor.SetProperty(TextEditor ::Property::ENABLE_MARKUP, true);

  application.GetScene().Add(textEditor);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl(textEditor);
  Text::ViewInterface&           view           = textEditorImpl.GetTextController()->GetView();

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  DALI_TEST_EQUALS(numberOfGlyphs, EXPECTED_NUMBER_OF_GLYPHS, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

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

  const Length numberOfGlyphsOneLine = 7u;
  for(Length i = 0; i < numberOfGlyphsOneLine - 1u; i++)
  {
    float diffLineNoCharSpacing = positions[i + 1].x - positions[i].x;

    float diffLineCondensedCharSpacing = positions[numberOfGlyphsOneLine + i + 1].x - positions[numberOfGlyphsOneLine + i].x;
    DALI_TEST_EQUALS(diffLineCondensedCharSpacing, diffLineNoCharSpacing + condensedCharSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

    float diffLineExpandedCharSpacing = positions[2u * numberOfGlyphsOneLine + i + 1].x - positions[2u * numberOfGlyphsOneLine + i].x;
    DALI_TEST_EQUALS(diffLineExpandedCharSpacing, diffLineNoCharSpacing + expandedCharSpacing, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  }

  END_TEST;
}
