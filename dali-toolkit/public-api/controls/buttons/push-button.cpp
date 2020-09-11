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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/buttons/push-button.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/buttons/push-button-impl.h>

namespace Dali
{
namespace Toolkit
{
PushButton::PushButton()
: Button()
{
}

PushButton::PushButton(Internal::PushButton& implementation)
: Button(implementation)
{
}

PushButton::PushButton(const PushButton& pushButton) = default;

PushButton::PushButton(PushButton&& rhs) = default;

PushButton& PushButton::operator=(const PushButton& pushButton) = default;

PushButton& PushButton::operator=(PushButton&& rhs) = default;

PushButton::PushButton(Dali::Internal::CustomActor* internal)
: Button(internal)
{
  VerifyCustomActorPointer<Internal::PushButton>(internal);
}

PushButton::~PushButton()
{
}

PushButton PushButton::New()
{
  return Internal::PushButton::New();
}

PushButton PushButton::DownCast(BaseHandle handle)
{
  return Control::DownCast<PushButton, Internal::PushButton>(handle);
}

} // namespace Toolkit

} // namespace Dali
