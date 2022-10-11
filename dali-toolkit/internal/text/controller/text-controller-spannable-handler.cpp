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

// CLASS HEADER
#include <dali-toolkit/internal/text/controller/text-controller-spannable-handler.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>
#include <dali-toolkit/internal/text/controller/text-controller-text-updater.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void Controller::SpannableHandler::SetSpannedText(Controller& controller, const Text::Spanned& spannedText)
{
  // Copy content(text)
  const std::string& text = spannedText.ToString();
  Controller::TextUpdater::SetText(controller, text);

  Controller::Impl& impl         = *controller.mImpl;
  LogicalModelPtr&  logicalModel = impl.mModel->mLogicalModel;

  // Set spanned-text
  logicalModel->mSpannedTextPlaced = true;

  std::vector<Dali::Toolkit::Text::BaseSpan> spans;
  std::vector<Dali::Toolkit::Text::Range>    ranges;
  spannedText.RetrieveAllSpansAndRanges(spans, ranges);

  for(std::vector<Dali::Toolkit::Text::BaseSpan>::size_type i = 0; i < spans.size(); i++)
  {
    GetImplementation(spans[i]).CreateStyleCharacterRun(logicalModel, ranges[i]);
  }
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali
