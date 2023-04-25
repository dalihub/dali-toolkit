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
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{
namespace Toolkit
{
TextField TextField::New()
{
  return Internal::TextField::New();
}

TextField TextField::New(ControlBehaviour additionalBehaviour)
{
  return Internal::TextField::New(static_cast<Toolkit::Internal::Control::ControlBehaviour>(additionalBehaviour));
}

TextField::TextField()
{
}

TextField::TextField(const TextField& handle) = default;

TextField::TextField(TextField&& rhs) = default;

TextField& TextField::operator=(const TextField& handle) = default;

TextField& TextField::operator=(TextField&& rhs) = default;

TextField::~TextField()
{
}

TextField TextField::DownCast(BaseHandle handle)
{
  return Control::DownCast<TextField, Internal::TextField>(handle);
}

TextField::TextChangedSignalType& TextField::TextChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).TextChangedSignal();
}

TextField::MaxLengthReachedSignalType& TextField::MaxLengthReachedSignal()
{
  return Dali::Toolkit::GetImpl(*this).MaxLengthReachedSignal();
}

TextField::InputStyleChangedSignalType& TextField::InputStyleChangedSignal()
{
  return Dali::Toolkit::GetImpl(*this).InputStyleChangedSignal();
}

TextField::TextField(Internal::TextField& implementation)
: Control(implementation)
{
}

TextField::TextField(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::TextField>(internal);
}

} // namespace Toolkit

} // namespace Dali
