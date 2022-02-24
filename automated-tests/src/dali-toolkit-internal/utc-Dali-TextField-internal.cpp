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

#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextFieldMultipleBackgroundText(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldMultipleBackgroundText");

  // Create a text field
  TextField textField = TextField::New();
  textField.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  textField.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  textField.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Add the text field to the stage
  application.GetScene().Add(textField);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& textFieldImpl  = GetImpl(textField);
  ControllerPtr                 controller     = textFieldImpl.GetTextController();
  Controller::Impl&             controllerImpl = Controller::Impl::GetImplementation(*controller.Get());

  // Add multiple background colors for the text.
  ColorRun backgroundColorRun1;
  backgroundColorRun1.characterRun.characterIndex     = 0u;
  backgroundColorRun1.characterRun.numberOfCharacters = 1u;
  backgroundColorRun1.color                           = Color::RED;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun1);

  ColorRun backgroundColorRun2;
  backgroundColorRun2.characterRun.characterIndex     = 5u;
  backgroundColorRun2.characterRun.numberOfCharacters = 8u;
  backgroundColorRun2.color                           = Color::CYAN;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun2);

  ColorRun backgroundColorRun3;
  backgroundColorRun3.characterRun.characterIndex     = 23u;
  backgroundColorRun3.characterRun.numberOfCharacters = 6u;
  backgroundColorRun3.color                           = Color::GREEN;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun3);

  // Check the case where there is only one character in the text
  controller->SetText("S");

  application.SendNotification();
  application.Render();

  // The offscreen root actor should have one child: the renderable.
  Actor stencil = textField.GetChildAt(0u);
  DALI_TEST_CHECK(stencil.GetChildCount() == 1u);

  // The renderable actor should have two children: the text and the background.
  Actor renderableActor = stencil.GetChildAt(0u);
  DALI_TEST_CHECK(renderableActor.GetChildCount() == 2u);

  // Check that the background is created
  Actor backgroundActor = renderableActor.GetChildAt(0u);
  DALI_TEST_CHECK(backgroundActor);
  DALI_TEST_CHECK(backgroundActor.GetProperty<std::string>(Dali::Actor::Property::NAME) == "TextBackgroundColorActor");

  // Change the text to contain more characters
  controller->SetText("Text Multiple Background Test");

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textFieldImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  // Now the offscreen root actor should have four children: the renderable, the clipped cursor, the highlight, and the background.
  DALI_TEST_CHECK(stencil.GetChildCount() == 4u);
  // The renderable actor should have one child only: the text
  DALI_TEST_CHECK(renderableActor.GetChildCount() == 1u);

  // The background should now be lowered below the highlight
  backgroundActor = stencil.GetChildAt(0u);
  DALI_TEST_CHECK(backgroundActor);
  DALI_TEST_CHECK(backgroundActor.GetProperty<std::string>(Dali::Actor::Property::NAME) == "TextBackgroundColorActor");

  END_TEST;
}

int UtcDaliTextFieldSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldSelectText");

  // Create a text field
  TextField textField = TextField::New();
  textField.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  textField.SetProperty(TextField::Property::TEXT, "Hello World");

  // Add the text field to the stage
  application.GetScene().Add(textField);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& textFieldImpl = GetImpl(textField);

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textFieldImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(textFieldImpl.GetSelectedText() == "Hello World");

  // Select None
  textFieldImpl.SelectNone();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(textFieldImpl.GetSelectedText() == "");

  END_TEST;
}

int UtcDaliTextFieldMarkupUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupUnderline ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(TextField::Property::TEXT, "<u>ABC</u>EF<u>GH</u>");
  textField.SetProperty(TextField ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfUnderlinedGlyphs = 5u;

  Toolkit::Internal::TextField& textFieldImpl         = GetImpl(textField);
  const Text::Length            numberOfUnderlineRuns = textFieldImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textFieldImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.glyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[2u].glyphRun.glyphIndex, 2u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS(underlineRuns[3u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[4u].glyphRun.glyphIndex, 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldMarkupUnderlineAttributes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupUnderlineAttributes ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  std::string testText =
    "start<u>ABC1</u>then"
    "<u type='solid'>ABC2</u>then"
    "<u type='dashed'>ABC3</u>then"
    "<u type='double'>ABC4</u>then"
    "<u color='green'>ABC5</u>then"
    "<u height='5.0f'>ABC6</u>then"
    "<u type='dashed' dash-gap='3.0f'>ABC7</u>then"
    "<u type='dashed' dash-width='4.0f'>ABC8</u>then"
    "<u color='blue' type='dashed' height='4.0f' dash-gap='2.0f' dash-width='3.0f'>ABC9</u>end"

    ;

  textField.SetProperty(TextField::Property::TEXT, testText);
  textField.SetProperty(TextField ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t NUMBER_OF_CASES                  = 9u;
  uint32_t       expectedNumberOfUnderlinedGlyphs = 36u;

  Toolkit::Internal::TextField& textFieldImpl         = GetImpl(textField);
  const Text::Length            numberOfUnderlineRuns = textFieldImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textFieldImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  struct DataOfCase
  {
    std::string              title;
    uint32_t                 startIndex;
    uint32_t                 endIndex;
    GlyphIndex               startGlyphIndex;
    GlyphIndex               endGlyphIndex;
    UnderlineStyleProperties properties;
  };
  DataOfCase data[] =
    {
      //<u>ABC1</u>
      {"<u>ABC1</u>",
       0u,
       3u,
       5u,
       8u,
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
       4u,
       7u,
       13u,
       16u,
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
       8u,
       11u,
       21u,
       24u,
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
       12u,
       15u,
       29u,
       32u,
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
       16u,
       19u,
       37u,
       40u,
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
       20u,
       23u,
       45u,
       48u,
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
       24u,
       27u,
       53u,
       56u,
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
       28u,
       31u,
       61u,
       64u,
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
       32u,
       35u,
       69u,
       72u,
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

  for(uint32_t i = 0; i < NUMBER_OF_CASES; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(underlineRuns[data[i].startIndex].glyphRun.glyphIndex, data[i].startGlyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(underlineRuns[data[i].endIndex].glyphRun.glyphIndex, data[i].endGlyphIndex, TEST_LOCATION);

    DALI_TEST_CHECK(data[i].properties == underlineRuns[data[i].startIndex].properties);
    DALI_TEST_CHECK(data[i].properties == underlineRuns[data[i].endIndex].properties);
  }

  END_TEST;
}

int UtcDaliTextFieldMarkupSpanUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupSpanUnderline ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  std::string testText =
    "start<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ABC1</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='solid'>ABC2</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed'>ABC3</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='double'>ABC4</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='green'>ABC5</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-height='5.0f'>ABC6</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-gap='3.0f'>ABC7</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-width='4.0f'>ABC8</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='blue' u-type='dashed' u-height='4.0f' u-dash-gap='2.0f' u-dash-width='3.0f'>ABC9</span>end"

    ;

  textField.SetProperty(TextField::Property::TEXT, testText);
  textField.SetProperty(TextField ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t NUMBER_OF_CASES                  = 8u;
  uint32_t       expectedNumberOfUnderlinedGlyphs = 32u;

  Toolkit::Internal::TextField& textFieldImpl         = GetImpl(textField);
  const Text::Length            numberOfUnderlineRuns = textFieldImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textFieldImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  struct DataOfCase
  {
    std::string              title;
    uint32_t                 startIndex;
    uint32_t                 endIndex;
    GlyphIndex               startGlyphIndex;
    GlyphIndex               endGlyphIndex;
    UnderlineStyleProperties properties;
  };
  DataOfCase data[] =
    {

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='solid'>ABC2</span>",
       0u,
       3u,
       13u,
       16u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed'>ABC3</span>",
       4u,
       7u,
       21u,
       24u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='double'>ABC4</span>",
       8u,
       11u,
       29u,
       32u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='green'>ABC5</span>",
       12u,
       15u,
       37u,
       40u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-height='5.0f'>ABC6</span>",
       16u,
       19u,
       45u,
       48u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-gap='3.0f'>ABC7</span>",
       20u,
       23u,
       53u,
       56u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-type='dashed' u-dash-width='4.0f'>ABC8</span>",
       24u,
       27u,
       61u,
       64u,
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

      {"<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' u-color='blue' u-type='dashed' u-height='4.0f' u-dash-gap='2.0f' u-dash-width='3.0f'>ABC9</span>",
       28u,
       31u,
       69u,
       72u,
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

  for(uint32_t i = 0; i < NUMBER_OF_CASES; i++)
  {
    tet_infoline(data[i].title.c_str());
    DALI_TEST_EQUALS(underlineRuns[data[i].startIndex].glyphRun.glyphIndex, data[i].startGlyphIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(underlineRuns[data[i].endIndex].glyphRun.glyphIndex, data[i].endGlyphIndex, TEST_LOCATION);

    DALI_TEST_CHECK(data[i].properties == underlineRuns[data[i].startIndex].properties);
    DALI_TEST_CHECK(data[i].properties == underlineRuns[data[i].endIndex].properties);
  }

  END_TEST;
}

int UtcDaliTextFieldFontPointSizeLargerThanAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldFontPointSizeLargerThanAtlas ");

  // Create a Text field
  TextField textField = TextField::New();
  //Set size to avoid automatic eliding
  textField.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textField.SetProperty(TextField::Property::POINT_SIZE, 1000);
  //Specify font-family
  textField.SetProperty(TextField::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textField.SetProperty(TextField::Property::TEXT, "A");

  application.GetScene().Add(textField);

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS(countAtlas, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldFontPointSizeLargerThanAtlasPlaceholderCase(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldFontPointSizeLargerThanAtlasPlaceholderCase ");

  //Set Map of placeholder: text, font-family and point-size
  Property::Map placeholderMapSet;
  placeholderMapSet["text"]       = "A";
  placeholderMapSet["fontFamily"] = "DejaVu Sans";
  placeholderMapSet["pixelSize"]  = 1000.0f;

  // Create a text editor
  TextField textField = TextField::New();
  //Set size to avoid automatic eliding
  textField.SetProperty(Actor::Property::SIZE, Vector2(1025, 1025));
  //Set placeholder
  textField.SetProperty(TextField::Property::PLACEHOLDER, placeholderMapSet);

  application.GetScene().Add(textField);

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS(countAtlas, 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldBackgroundTag(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldBackgroundTag\n");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  field.SetProperty(TextField ::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "H<background color='red'>e</background> Worl<background color='yellow'>d</background>");
  application.GetScene().Add(field);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& fieldImpl                    = GetImpl(field);
  const ColorIndex* const       backgroundColorIndicesBuffer = fieldImpl.GetTextController()->GetTextModel()->GetBackgroundColorIndices();

  DALI_TEST_CHECK(backgroundColorIndicesBuffer);

  //default color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[0], 0u, TEST_LOCATION);

  //red color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[1], 1u, TEST_LOCATION);

  //yellow color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[7], 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextFieldEllipsisInternalAPIs(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs ");
  TextField textField = TextField::New();

  Toolkit::Internal::TextField& textFieldImpl = GetImpl(textField);
  Text::ViewInterface&          view          = textFieldImpl.GetTextController()->GetView();

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - ELLIPSIS Disabled");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS, false);
  DALI_TEST_EQUALS(textField.GetProperty<bool>(DevelTextField::Property::ELLIPSIS), false, TEST_LOCATION);
  DALI_TEST_CHECK(!(view.IsTextElideEnabled()));

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - ELLIPSIS Enabled");
  textField.SetProperty(DevelTextField::Property::ELLIPSIS, true);
  DALI_TEST_EQUALS(textField.GetProperty<bool>(DevelTextField::Property::ELLIPSIS), true, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsTextElideEnabled());

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - GetStartIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(view.GetStartIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - GetEndIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(view.GetEndIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - GetFirstMiddleIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(view.GetFirstMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextFieldEllipsisInternalAPIs - GetSecondMiddleIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(view.GetSecondMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  END_TEST;
}
int UtcDaliTextFieldTextWithSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextFieldTextWithSpan\n");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  field.SetProperty(TextField ::Property::ENABLE_MARKUP, true);
  field.SetProperty(TextField::Property::TEXT, "Hello Span");
  application.GetScene().Add(field);

  application.SendNotification();
  application.Render();

  Vector3 originalSize = field.GetNaturalSize();
  field.SetProperty(TextField::Property::TEXT, "H<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ello</span> Span");

  application.SendNotification();
  application.Render();

  Vector3 spanSize = field.GetNaturalSize();

  DALI_TEST_GREATER(spanSize.width, originalSize.width, TEST_LOCATION);

  Toolkit::Internal::TextField& fieldImpl           = GetImpl(field);
  const ColorIndex* const       colorIndicesBuffer1 = fieldImpl.GetTextController()->GetTextModel()->GetColorIndices();

  DALI_TEST_CHECK(colorIndicesBuffer1);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[0], 0u, TEST_LOCATION);

  //span color
  DALI_TEST_EQUALS(colorIndicesBuffer1[1], 1u, TEST_LOCATION);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[6], 0u, TEST_LOCATION);

  field.SetProperty(TextField::Property::TEXT, "<span font-size='45'>H</span>ello <span text-color='red'>S</span>pan");

  application.SendNotification();
  application.Render();

  const ColorIndex* const colorIndicesBuffer2 = fieldImpl.GetTextController()->GetTextModel()->GetColorIndices();

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

int UtcDaliTextFieldControlBackgroundColor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldControlBackgroundColor\n");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  Vector4 backgroundColor;

  field.SetProperty(TextField::Property::TEXT, "Background Color");
  application.GetScene().Add(field);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& fieldImpl      = GetImpl(field);
  ControllerPtr                 controller     = fieldImpl.GetTextController();
  Controller::Impl&             controllerImpl = Controller::Impl::GetImplementation(*controller.Get());

  // Default color is transparent
  controllerImpl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
  DALI_TEST_EQUALS(backgroundColor, Color::TRANSPARENT, TEST_LOCATION);

  // Set background color to red
  field.SetBackgroundColor(Color::RED);
  application.SendNotification();
  application.Render();

  // Should be red
  controllerImpl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
  DALI_TEST_EQUALS(backgroundColor, Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextFieldMarkupStrikethrough(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupStrikethrough ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(TextField::Property::TEXT, "<s>ABC</s>EF<s color='red'>GH</s>");
  textField.SetProperty(TextField ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 2u;

  Toolkit::Internal::TextField& textFieldImpl             = GetImpl(textField);
  const Text::Length            numberOfStrikethroughRuns = textFieldImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textFieldImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

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

int UtcDaliTextFieldMarkupStrikethroughNoEndTag(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupStrikethroughNoEndTag ");

  TextField textField = TextField::New();

  application.GetScene().Add(textField);

  textField.SetProperty(TextField::Property::TEXT, "<s>ABC");
  textField.SetProperty(TextField ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 0u;

  Toolkit::Internal::TextField& textFieldImpl             = GetImpl(textField);
  Text::Length                  numberOfStrikethroughRuns = textFieldImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  END_TEST;
}