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
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-selection-toolbar-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

TextSelectionToolbar TextSelectionToolbar::New()
{
  return Internal::TextSelectionToolbar::New();
}

TextSelectionToolbar::TextSelectionToolbar()
{
}

TextSelectionToolbar::TextSelectionToolbar( const TextSelectionToolbar& handle )
: Control( handle )
{
}

TextSelectionToolbar& TextSelectionToolbar::operator=( const TextSelectionToolbar& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TextSelectionToolbar::~TextSelectionToolbar()
{
}

void TextSelectionToolbar::AddOption( Actor& option )
{
  GetImpl(*this).AddOption(option);
}


void TextSelectionToolbar::AddDivider( Actor& divider )
{
  GetImpl(*this).AddDivider( divider );
}

void TextSelectionToolbar::ResizeDividers( Size& size )
{
  GetImpl(*this).ResizeDividers( size );
}

void TextSelectionToolbar::RaiseAbove( Layer target )
{
  GetImpl(*this).RaiseAbove( target );
}

TextSelectionToolbar TextSelectionToolbar::DownCast( BaseHandle handle )
{
  return Control::DownCast<TextSelectionToolbar, Internal::TextSelectionToolbar>(handle);
}

TextSelectionToolbar::TextSelectionToolbar( Internal::TextSelectionToolbar& implementation )
: Control(implementation)
{
}

TextSelectionToolbar::TextSelectionToolbar( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TextSelectionToolbar>( internal );
}


} // namespace Toolkit

} // namespace Dali
