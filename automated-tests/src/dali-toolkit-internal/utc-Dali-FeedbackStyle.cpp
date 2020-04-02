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

#include <dali-toolkit/internal/feedback/feedback-style.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/feedback-player.h>

using namespace Dali;
using namespace Toolkit;

namespace Dali
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
void SetLoadFileReturnValue( Dali::FeedbackPlayer feedbackPlayer, bool returnValue );

}

}

}

int UtcDaliFeedbackStyle(void)
{
  // Not too much to test. Just to improve coverage.

  ToolkitTestApplication application;
  tet_infoline(" UtcDaliFeedbackStyle");

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
