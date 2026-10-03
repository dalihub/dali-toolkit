/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// Hijack private members of FeedbackStyle to improve coverage
#define private public
#include <dali-toolkit/internal/feedback/feedback-style.h>
#undef private

#include <dali/devel-api/adaptor-framework/feedback-player.h>

using namespace Dali;
using namespace Toolkit;

namespace DALI_NAMESPACE
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Sets the return value of the FeedbackPlayer::LoadFile() method of the stub implementation.
 *
 * Used to improve the line coverage.
 *
 * @param[in] feedbackPlayer The FeedbackPlayer singleton.
 * @param[in] returnValue The desired return value for the FeedbackPlayer::LoadFile() method. Either true or false.
 */
void SetLoadFileReturnValue(Dali::FeedbackPlayer feedbackPlayer, bool returnValue);

} //namespace Adaptor

} //namespace Internal

} //namespace DALI_NAMESPACE

int UtcDaliFeedbackStyleP(void)
{
  // Not too much to test. Just to improve coverage.

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliFeedbackStyleP");

  try
  {
    Toolkit::Internal::FeedbackStyle feedbackStyle;

    Dali::FeedbackPlayer feedbackPlayer = Dali::FeedbackPlayer::Get();
    Dali::Internal::Adaptor::SetLoadFileReturnValue(feedbackPlayer, false);

    Toolkit::Internal::FeedbackStyle feedbackStyle2;

    Dali::Internal::Adaptor::SetLoadFileReturnValue(feedbackPlayer, true);
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliFeedbackStyleThemeAndPatternP(void)
{
  ToolkitTestApplication           application;
  Toolkit::Internal::FeedbackStyle style;
  const std::string                theme = R"JSON({
    "style": {
      "Actor": {
        "signals": {
          "first": {
            "type": "onScene",
            "hapticFeedbackPattern": "FEEDBACK_PATTERN_TAP",
            "soundFeedbackPattern": "FEEDBACK_PATTERN_MESSAGE"
          },
          "replacement": {
            "type": "onScene",
            "hapticFeedbackFile": "/tmp/haptic-feedback"
          },
          "file": {
            "type": "offScene",
            "hapticFeedbackFile": "/tmp/combined-feedback"
          },
          "ignored": {
            "type": "touched",
            "hapticFeedbackPattern": 12
          }
        }
      }
    }
  })JSON";

  DALI_TEST_CHECK(style.LoadTheme(theme));
  DALI_TEST_CHECK(!style.mStyleInfoLut.empty());
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_TAP"), FEEDBACK_PATTERN_TAP, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_SLIDER_SWEEP"), FEEDBACK_PATTERN_SLIDER_SWEEP, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("unknown"), FEEDBACK_PATTERN_NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_KEY5"), FEEDBACK_PATTERN_KEY5, TEST_LOCATION);

  style.PlayFeedback("Actor", "onScene");
  style.PlayFeedback("Actor", "offScene");
  style.PlayFeedback("Actor", "missing");
  style.PlayFeedback("MissingType", "onScene");
  END_TEST;
}
