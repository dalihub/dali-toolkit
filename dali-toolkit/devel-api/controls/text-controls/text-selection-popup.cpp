/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

TextSelectionPopup TextSelectionPopup::New()
{
  return Internal::TextSelectionPopup::New();
}

TextSelectionPopup TextSelectionPopup::New( Buttons enabledButtons )
{
  return Internal::TextSelectionPopup::New( enabledButtons );
}

TextSelectionPopup::TextSelectionPopup()
{
}

TextSelectionPopup::TextSelectionPopup( const TextSelectionPopup& handle )
: Control( handle )
{
}

TextSelectionPopup& TextSelectionPopup::operator=( const TextSelectionPopup& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TextSelectionPopup::~TextSelectionPopup()
{
}

TextSelectionPopup TextSelectionPopup::DownCast( BaseHandle handle )
{
  return Control::DownCast<TextSelectionPopup, Internal::TextSelectionPopup>(handle);
}

TextSelectionPopup::TextSelectionPopup( Internal::TextSelectionPopup& implementation )
: Control(implementation)
{
}

TextSelectionPopup::TextSelectionPopup( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TextSelectionPopup>( internal );
}


} // namespace Toolkit

} // namespace Dali
