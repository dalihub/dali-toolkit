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

#include <bits/stdint-uintn.h>
#include <stdlib.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextLabelMarkupUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupUnderline ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  textLabel.SetProperty(TextLabel::Property::TEXT, "<u>ABC</u>EF<u>GH</u>");
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfUnderlinedGlyphs = 5u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.glyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[2u].glyphRun.glyphIndex, 2u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS(underlineRuns[3u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[4u].glyphRun.glyphIndex, 6u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLabelMarkupUnderlineAttributes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupUnderlineAttributes ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

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

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t NUMBER_OF_CASES                  = 9u;
  uint32_t       expectedNumberOfUnderlinedGlyphs = 36u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

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

int UtcDaliTextLabelBackgroundTag(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextLabelBackgroundTag\n");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  label.SetProperty(TextLabel::Property::TEXT, "H<background color='red'>e</background> Worl<background color='yellow'>d</background>");
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextLabel& labelImpl                    = GetImpl(label);
  const ColorIndex* const       backgroundColorIndicesBuffer = labelImpl.GetTextController()->GetTextModel()->GetBackgroundColorIndices();

  DALI_TEST_CHECK(backgroundColorIndicesBuffer);

  //default color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[0], 0u, TEST_LOCATION);

  //red color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[1], 1u, TEST_LOCATION);

  //yellow color
  DALI_TEST_EQUALS(backgroundColorIndicesBuffer[7], 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextlabelEllipsisInternalAPIs(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs ");
  TextLabel textLabel = TextLabel::New();

  Toolkit::Internal::TextLabel& textLabelImpl = GetImpl(textLabel);
  const ModelInterface* const   textModel     = textLabelImpl.GetTextController()->GetTextModel();

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - ELLIPSIS Disabled");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS, false);
  DALI_TEST_EQUALS(textLabel.GetProperty<bool>(DevelTextLabel::Property::ELLIPSIS), false, TEST_LOCATION);
  DALI_TEST_CHECK(!(textModel->IsTextElideEnabled()));

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - ELLIPSIS Enabled");
  textLabel.SetProperty(DevelTextLabel::Property::ELLIPSIS, true);
  DALI_TEST_EQUALS(textLabel.GetProperty<bool>(DevelTextLabel::Property::ELLIPSIS), true, TEST_LOCATION);
  DALI_TEST_CHECK(textModel->IsTextElideEnabled());

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetStartIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(textModel->GetStartIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetEndIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(textModel->GetEndIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetFirstMiddleIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(textModel->GetFirstMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetSecondMiddleIndexOfElidedGlyphs Default");
  DALI_TEST_EQUALS(textModel->GetSecondMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(textModel);
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - IsTextElideEnabled ViewModel");
  DALI_TEST_CHECK(model->IsTextElideEnabled());

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetStartIndexOfElidedGlyphs ViewModel");
  DALI_TEST_EQUALS(model->GetStartIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetEndIndexOfElidedGlyphs ViewModel");
  DALI_TEST_EQUALS(model->GetEndIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetFirstMiddleIndexOfElidedGlyphs ViewModel");
  DALI_TEST_EQUALS(model->GetFirstMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  tet_infoline(" UtcDaliToolkitTextlabelEllipsisInternalAPIs - GetSecondMiddleIndexOfElidedGlyphs ViewModel");
  DALI_TEST_EQUALS(model->GetSecondMiddleIndexOfElidedGlyphs(), 0u, TEST_LOCATION);

  END_TEST;
}
int UtcDaliTextLabelTextWithSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextLabelTextWithSpan\n");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  label.SetProperty(TextLabel::Property::TEXT, "Hello Span");
  application.GetScene().Add(label);

  application.SendNotification();
  application.Render();

  Vector3 originalSize = label.GetNaturalSize();
  label.SetProperty(TextLabel::Property::TEXT, "H<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ello</span> Span");

  application.SendNotification();
  application.Render();

  Vector3 spanSize = label.GetNaturalSize();

  DALI_TEST_GREATER(spanSize.width, originalSize.width, TEST_LOCATION);

  Toolkit::Internal::TextLabel& labelImpl           = GetImpl(label);
  const ColorIndex* const       colorIndicesBuffer1 = labelImpl.GetTextController()->GetTextModel()->GetColorIndices();

  DALI_TEST_CHECK(colorIndicesBuffer1);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[0], 0u, TEST_LOCATION);

  //span color
  DALI_TEST_EQUALS(colorIndicesBuffer1[1], 1u, TEST_LOCATION);

  //default color
  DALI_TEST_EQUALS(colorIndicesBuffer1[6], 0u, TEST_LOCATION);

  label.SetProperty(TextLabel::Property::TEXT, "<span font-size='45'>H</span>ello <span text-color='red'>S</span>pan");

  application.SendNotification();
  application.Render();

  const ColorIndex* const colorIndicesBuffer2 = labelImpl.GetTextController()->GetTextModel()->GetColorIndices();

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

int UtcDaliTextLabelMarkupStrikethrough(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupStrikethrough ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  textLabel.SetProperty(TextLabel::Property::TEXT, "<s color='red'>ABC</s>EF<s color='green'>GH</s>");
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 2u;

  Toolkit::Internal::TextLabel& textLabelImpl             = GetImpl(textLabel);
  const Text::Length            numberOfStrikethroughRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  //ABC have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].glyphRun.numberOfGlyphs, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(strikethroughRuns[0u].isColorSet);
  DALI_TEST_EQUALS(strikethroughRuns[0u].color.r, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].color.g, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].color.b, 0u, TEST_LOCATION);

  //GH have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(strikethroughRuns[1u].isColorSet);
  DALI_TEST_EQUALS(strikethroughRuns[1u].color.r, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].color.g, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].color.b, 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLabelMarkupStrikethroughNoEndTag(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupStrikethroughNoEndTag ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  textLabel.SetProperty(TextLabel::Property::TEXT, "<s>ABC");
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfStrikethroughGlyphs = 0u;

  Toolkit::Internal::TextLabel& textLabelImpl             = GetImpl(textLabel);
  Text::Length                  numberOfStrikethroughRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughGlyphs, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLabelMarkupParagraphTag(void)

{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliTextLabelMarkupParagraphTag ");
  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  textLabel.SetProperty(TextLabel::Property::TEXT, "text one <p>Paragraph two</p> text three <p>Paragraph four</p> text five");
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabel.SetProperty(TextLabel::Property::MULTI_LINE, true);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfBoundedParagraphRuns = 2u;

  Toolkit::Internal::TextLabel& textLabelImpl                = GetImpl(textLabel);
  const Text::Length            numberOfBoundedParagraphRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfBoundedParagraphRuns();
  DALI_TEST_EQUALS(numberOfBoundedParagraphRuns, expectedNumberOfBoundedParagraphRuns, TEST_LOCATION);

  const Vector<BoundedParagraphRun>& boundedParagraphRuns = textLabelImpl.GetTextController()->GetTextModel()->GetBoundedParagraphRuns();

  //<p>Paragraph two</p>
  DALI_TEST_EQUALS(boundedParagraphRuns[0u].characterRun.characterIndex, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(boundedParagraphRuns[0u].characterRun.numberOfCharacters, 14u, TEST_LOCATION);

  //<p>Paragraph four</p>
  DALI_TEST_EQUALS(boundedParagraphRuns[1u].characterRun.characterIndex, 37u, TEST_LOCATION);
  DALI_TEST_EQUALS(boundedParagraphRuns[1u].characterRun.numberOfCharacters, 15u, TEST_LOCATION);

  END_TEST;
}