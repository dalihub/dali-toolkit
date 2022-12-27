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
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-selection-popup-impl.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
TextSelectionPopup TextSelectionPopup::New(TextSelectionPopupCallbackInterface* callbackInterface)
{
  return Internal::TextSelectionPopup::New(callbackInterface);
}

TextSelectionPopup::TextSelectionPopup()
{
}

TextSelectionPopup::TextSelectionPopup(const TextSelectionPopup& handle) = default;

TextSelectionPopup& TextSelectionPopup::operator=(const TextSelectionPopup& handle) = default;

TextSelectionPopup::TextSelectionPopup(TextSelectionPopup&& handle) = default;

TextSelectionPopup& TextSelectionPopup::operator=(TextSelectionPopup&& handle) = default;

TextSelectionPopup::~TextSelectionPopup()
{
}

TextSelectionPopup TextSelectionPopup::DownCast(BaseHandle handle)
{
  return Control::DownCast<TextSelectionPopup, Internal::TextSelectionPopup>(handle);
}

void TextSelectionPopup::EnableButtons(Toolkit::TextSelectionPopup::Buttons buttonsToEnable)
{
  GetImpl(*this).EnableButtons(buttonsToEnable);
}

void TextSelectionPopup::RaiseAbove(Actor target)
{
  GetImpl(*this).RaiseAbove(target);
}

void TextSelectionPopup::ShowPopup()
{
  GetImpl(*this).ShowPopup();
}

void TextSelectionPopup::HidePopup()
{
  GetImpl(*this).HidePopup();
}

void TextSelectionPopup::SetProperties(const Dali::Property::Map& properties)
{
  GetImpl(*this).SetProperties(properties);
}

void TextSelectionPopup::GetProperties(Dali::Property::Map& properties)
{
  GetImpl(*this).GetProperties(properties);
}

TextSelectionPopup::TextSelectionPopup(Internal::TextSelectionPopup& implementation)
: Control(implementation)
{
}

TextSelectionPopup::TextSelectionPopup(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::TextSelectionPopup>(internal);
}

} // namespace Toolkit

} // namespace Dali
