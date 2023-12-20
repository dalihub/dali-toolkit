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

#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>

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

  uint32_t expectedNumberOfUnderlineRuns = 2u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[0u].glyphRun.numberOfGlyphs, 3u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);

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

  const uint32_t expectedNumberOfUnderlineRuns = 9u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

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

int UtcDaliTextLabelMarkupSpanUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupSpanUnderline ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

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

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfUnderlineRuns = 8u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

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

int UtcDaliTextLabelMarkupNestedUnderlineTags(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupNestedUnderlineTags ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  std::string testText = "start<u height='5.0f' color='green' >AB<u color='blue' >XYZ</u>CDE</u>end";

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfUnderlineRuns = 2u;

  Toolkit::Internal::TextLabel& textLabelImpl         = GetImpl(textLabel);
  const Text::Length            numberOfUnderlineRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS(numberOfUnderlineRuns, expectedNumberOfUnderlineRuns, TEST_LOCATION);

  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

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

int UtcDaliTextLabelMarkupNestedStrikethroughTags(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupNestedStrikethroughTags ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  std::string testText = "start<s height='5.0f' color='green' >AB<s color='blue' >XYZ</s>CDE</s>end";

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 2u;

  Toolkit::Internal::TextLabel& textLabelImpl             = GetImpl(textLabel);
  const Text::Length            numberOfStrikethroughRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

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

int UtcDaliTextLabelMarkupStrikethroughAttributes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupStrikethroughAttributes ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  std::string testText =
    "start<s>ABC1</s>then"
    "<s color='green'>ABC2</s>then"
    "<s height='5.0f'>ABC3</s>then"
    "<s color='blue' height='4.0f' >ABC4</s>end";

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 4u;

  Toolkit::Internal::TextLabel& textLabelImpl             = GetImpl(textLabel);
  const Text::Length            numberOfStrikethroughRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

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

int UtcDaliTextLabelMarkupSpanStrikethrough(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupSpanStrikethrough ");

  TextLabel textLabel = TextLabel::New();

  application.GetScene().Add(textLabel);

  std::string testText =
    "start<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red'>ABC1</span>then"
    "<span s-color='blue'>ABC2</span>then"
    "<span s-height='2.0f'>ABC3</span>then"
    "<span font-size='45' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' s-color='green' s-height='5.0f'>ABC4</span>end";

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);

  application.SendNotification();
  application.Render();

  const uint32_t expectedNumberOfStrikethroughRuns = 3u;

  Toolkit::Internal::TextLabel& textLabelImpl             = GetImpl(textLabel);
  const Text::Length            numberOfStrikethroughRuns = textLabelImpl.GetTextController()->GetTextModel()->GetNumberOfStrikethroughRuns();

  DALI_TEST_EQUALS(numberOfStrikethroughRuns, expectedNumberOfStrikethroughRuns, TEST_LOCATION);

  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  textLabelImpl.GetTextController()->GetTextModel()->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

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

int UtcDaliTextLabelSpanBackgroundTag(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextLabelSpanBackgroundTag\n");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  label.SetProperty(TextLabel::Property::TEXT, "H<span background-color='red'>e</span> Worl<span background-color='yellow'>d</span>");
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
  DALI_TEST_CHECK(strikethroughRuns[0u].properties.colorDefined);
  DALI_TEST_EQUALS(strikethroughRuns[0u].properties.color.r, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].properties.color.g, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[0u].properties.color.b, 0u, TEST_LOCATION);

  //GH have strikethrough
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].glyphRun.numberOfGlyphs, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(strikethroughRuns[1u].properties.colorDefined);
  DALI_TEST_EQUALS(strikethroughRuns[1u].properties.color.r, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].properties.color.g, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(strikethroughRuns[1u].properties.color.b, 0u, TEST_LOCATION);

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

int UtcDaliTextLabelMarkupParagraphTagAlignAttribute(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute ");

  // Apply alignment for each type on property level on three paragraphs and in-between text.
  // Apply align in markup on the three paragraphs (each one a type).
  // Using the same text to gain similar results from both the property level and the markup.
  // Compare line alignment between the property level and the markup.

  std::string textAlignOnPropertyLevel = "text outside<p>Paragraph end</p>text outside<p>Paragraph center</p>text outside<p>Paragraph begin</p><p>Paragraph property alignment</p>";
  std::string textAlignInMarkup        = "text outside<p align='end'>Paragraph end</p>text outside<p align='center'>Paragraph center</p>text outside<p align='begin' >Paragraph begin</p><p>Paragraph property alignment</p>";

  //Set size to avoid automatic eliding
  Vector2 controllerSize = Vector2(1025, 1025);

  TextLabel textLabelBeginAlign  = TextLabel::New();
  TextLabel textLabelCenterAlign = TextLabel::New();
  TextLabel textLabelEndAlign    = TextLabel::New();
  TextLabel textLabelMultiAlign  = TextLabel::New();

  application.GetScene().Add(textLabelBeginAlign);
  application.GetScene().Add(textLabelCenterAlign);
  application.GetScene().Add(textLabelEndAlign);
  application.GetScene().Add(textLabelMultiAlign);

  textLabelBeginAlign.SetProperty(TextLabel::Property::TEXT, textAlignOnPropertyLevel);
  textLabelBeginAlign.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabelBeginAlign.SetProperty(TextLabel::Property::MULTI_LINE, true);
  textLabelBeginAlign.SetProperty(TextLabel::Property::ELLIPSIS, false);
  textLabelBeginAlign.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::BEGIN);
  textLabelBeginAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textLabelCenterAlign.SetProperty(TextLabel::Property::TEXT, textAlignOnPropertyLevel);
  textLabelCenterAlign.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabelCenterAlign.SetProperty(TextLabel::Property::MULTI_LINE, true);
  textLabelCenterAlign.SetProperty(TextLabel::Property::ELLIPSIS, false);
  textLabelCenterAlign.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::CENTER);
  textLabelCenterAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textLabelEndAlign.SetProperty(TextLabel::Property::TEXT, textAlignOnPropertyLevel);
  textLabelEndAlign.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabelEndAlign.SetProperty(TextLabel::Property::MULTI_LINE, true);
  textLabelEndAlign.SetProperty(TextLabel::Property::ELLIPSIS, false);
  textLabelEndAlign.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::END);
  textLabelEndAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  textLabelMultiAlign.SetProperty(TextLabel::Property::TEXT, textAlignInMarkup);
  textLabelMultiAlign.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabelMultiAlign.SetProperty(TextLabel::Property::MULTI_LINE, true);
  textLabelMultiAlign.SetProperty(TextLabel::Property::ELLIPSIS, false);
  textLabelMultiAlign.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, Dali::Toolkit::Text::HorizontalAlignment::CENTER);
  textLabelMultiAlign.SetProperty(Actor::Property::SIZE, controllerSize);

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfBoundedParagraphRuns = 4u;
  uint32_t expectedNumberOfLines                = 7u;

  Toolkit::Internal::TextLabel& textLabelMultiAlignImpl  = GetImpl(textLabelMultiAlign);
  Toolkit::Internal::TextLabel& textLabelBeginAlignImpl  = GetImpl(textLabelBeginAlign);
  Toolkit::Internal::TextLabel& textLabelCenterAlignImpl = GetImpl(textLabelCenterAlign);
  Toolkit::Internal::TextLabel& textLabelEndAlignImpl    = GetImpl(textLabelEndAlign);

  const Text::Length numberOfBoundedParagraphRuns = textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetNumberOfBoundedParagraphRuns();
  DALI_TEST_EQUALS(numberOfBoundedParagraphRuns, expectedNumberOfBoundedParagraphRuns, TEST_LOCATION);

  DALI_TEST_EQUALS(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textLabelBeginAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textLabelBeginAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines());

  DALI_TEST_EQUALS(textLabelEndAlignImpl.GetTextController()->GetTextModel()->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(textLabelEndAlignImpl.GetTextController()->GetTextModel()->GetLines());

  const uint32_t LINE_INDEX_ALIGN_END    = 1u;
  const uint32_t LINE_INDEX_ALIGN_CENTER = 3u;
  const uint32_t LINE_INDEX_ALIGN_BEGIN  = 5u;
  const uint32_t LINE_INDEX_OUTSIDE_1    = 0u;
  const uint32_t LINE_INDEX_OUTSIDE_2    = 2u;
  const uint32_t LINE_INDEX_OUTSIDE_3    = 4u;
  const uint32_t LINE_INDEX_PARAGRAPH    = 6u;

  //<p align='end'>Paragraph end</p>
  const LineRun& lineEndFromMultiAlign = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_END);
  const LineRun& lineEndFromEndAlign   = *(textLabelEndAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_END);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - <p align='end'>Paragraph end</p>");
  DALI_TEST_EQUALS(lineEndFromMultiAlign.alignmentOffset, lineEndFromEndAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineEndFromMultiAlign.width, lineEndFromEndAlign.width, TEST_LOCATION);

  //<p align='center'>Paragraph center</p>
  const LineRun& lineCenterFromMultiAlign = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_CENTER);
  const LineRun& lineEndFromCenterAlign   = *(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_CENTER);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - <p align='center'>Paragraph center</p>");
  DALI_TEST_EQUALS(lineCenterFromMultiAlign.alignmentOffset, lineEndFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineCenterFromMultiAlign.width, lineEndFromCenterAlign.width, TEST_LOCATION);

  //<p align='begin' >Paragraph begin</p>
  const LineRun& lineBeginFromMultiAlign = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_BEGIN);
  const LineRun& lineEndFromBeginAlign   = *(textLabelBeginAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_ALIGN_BEGIN);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - <p align='begin' >Paragraph begin</p>");
  DALI_TEST_EQUALS(lineBeginFromMultiAlign.alignmentOffset, lineEndFromBeginAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineBeginFromMultiAlign.width, lineEndFromBeginAlign.width, TEST_LOCATION);

  //text outside
  const LineRun& lineOutsideOneFromMultiAlign  = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_1);
  const LineRun& lineOutsideOneFromCenterAlign = *(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_1);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - text outside one");
  DALI_TEST_EQUALS(lineOutsideOneFromMultiAlign.alignmentOffset, lineOutsideOneFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideOneFromMultiAlign.width, lineOutsideOneFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineOutsideTwoFromMultiAlign  = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_2);
  const LineRun& lineOutsideTwoFromCenterAlign = *(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_2);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - text outside two");
  DALI_TEST_EQUALS(lineOutsideTwoFromMultiAlign.alignmentOffset, lineOutsideTwoFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideTwoFromMultiAlign.width, lineOutsideTwoFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineOutsideThreeFromMultiAlign  = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_3);
  const LineRun& lineOutsideThreeFromCenterAlign = *(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_OUTSIDE_3);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - text outside three");
  DALI_TEST_EQUALS(lineOutsideThreeFromMultiAlign.alignmentOffset, lineOutsideThreeFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineOutsideThreeFromMultiAlign.width, lineOutsideThreeFromCenterAlign.width, TEST_LOCATION);

  const LineRun& lineParagraphFromMultiAlign  = *(textLabelMultiAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_PARAGRAPH);
  const LineRun& lineParagraphFromCenterAlign = *(textLabelCenterAlignImpl.GetTextController()->GetTextModel()->GetLines() + LINE_INDEX_PARAGRAPH);
  tet_infoline(" UtcDaliTextLabelMarkupParagraphTagAlignAttribute - <p>Paragraph property alignment</p>");
  DALI_TEST_EQUALS(lineParagraphFromMultiAlign.alignmentOffset, lineParagraphFromCenterAlign.alignmentOffset, TEST_LOCATION);
  DALI_TEST_EQUALS(lineParagraphFromMultiAlign.width, lineParagraphFromCenterAlign.width, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextLabelMarkupCharacterSpacingTag(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupCharacterSpacingTag ");

  const Length EXPECTED_NUMBER_OF_GLYPHS = 21u;

  const float expandedCharSpacing  = 10.0f;
  const float condensedCharSpacing = -5.0f;

  TextLabel textLabel = TextLabel::New();

  textLabel.SetProperty(TextLabel::Property::TEXT, "ABC EF\n<char-spacing value='-5.0f'>ABC EF\n</char-spacing><char-spacing value='10.0f'>ABC EF\n</char-spacing>");
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabel.SetProperty(TextLabel::Property::MULTI_LINE, true);

  application.GetScene().Add(textLabel);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextLabel& textLabelImpl = GetImpl(textLabel);
  Text::ViewInterface&          view          = textLabelImpl.GetTextController()->GetView();

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

int UtcDaliTextLabelMarkupSpanCharacterSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelMarkupSpanCharacterSpacing ");

  const Length EXPECTED_NUMBER_OF_GLYPHS = 21u;

  const float expandedCharSpacing  = 10.0f;
  const float condensedCharSpacing = -5.0f;

  std::string testText =
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='blue' >ABC EF\n</span>"
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='red' char-space-value='-5.0f'>ABC EF\n</span>"
    "<span font-size='20' font-family='DejaVu Sans' font-width='condensed' font-slant='italic' text-color='green' char-space-value='10.0f'>ABC EF\n</span>";

  TextLabel textLabel = TextLabel::New();

  textLabel.SetProperty(TextLabel::Property::TEXT, testText);
  textLabel.SetProperty(TextLabel ::Property::ENABLE_MARKUP, true);
  textLabel.SetProperty(TextLabel::Property::MULTI_LINE, true);

  application.GetScene().Add(textLabel);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextLabel& textLabelImpl = GetImpl(textLabel);
  Text::ViewInterface&          view          = textLabelImpl.GetTextController()->GetView();

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

int UtcDaliTextLabelLocaleChange(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLabelLocaleChange ");

  Adaptor &adaptor = application.GetAdaptor();
  TextLabel textLabel = TextLabel::New();
  application.GetScene().Add(textLabel);

  application.SendNotification();
  application.Render();

  std::string newLocale = "label_TEST";
  adaptor.LocaleChangedSignal().Emit(newLocale);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(newLocale.data(), GetImpl(textLabel).GetLocale(), TEST_LOCATION);

  END_TEST;
}