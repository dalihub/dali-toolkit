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

#include <dali-toolkit/devel-api/toolkit-pre-initialize.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>

#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

namespace Dali::Toolkit
{
namespace
{
bool gPreloaded = false;
void Preload()
{
  // This function called at pre-initialize time, after ApplicationPreInitialize().
  // If adaptor exist now, we could create view at pre-initialize time.
  if(Dali::Adaptor::IsAvailable())
  {
    DALI_LOG_ERROR("IsSupportPreInitializedCreation() : true\n");

    [[maybe_unused]] auto control = Dali::Toolkit::Control::New(Dali::Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
    control.SetBackgroundColor(Color::WHITE);

    [[maybe_unused]] auto imageView = Dali::Toolkit::ImageView::New(Dali::Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS);
    [[maybe_unused]] auto textLabel = Dali::Toolkit::TextLabel::New(Dali::Toolkit::Control::ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS, "text");
  }
  else
  {
    DALI_LOG_ERROR("IsSupportPreInitializedCreation() : false\n");
  }
}

} // namespace
} // namespace Dali::Toolkit

extern "C" void DaliToolkitPreInitialize(int* argc, char** argv[], void* /* not in used */)
{
  if(!Dali::Toolkit::gPreloaded)
  {
    Dali::Toolkit::gPreloaded = true;
    ApplicationPreInitialize(argc, argv);
    Dali::Toolkit::Preload();
  }
  else
  {
    DALI_LOG_ERROR("DaliToolkitPreInitialize() called multiple. Ignore\n");
  }
}