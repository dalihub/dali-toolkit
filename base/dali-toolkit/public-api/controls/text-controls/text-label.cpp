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
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

namespace Dali
{

namespace Toolkit
{

TextLabel TextLabel::New()
{
  return Internal::TextLabel::New();
}

TextLabel::TextLabel()
{
}

TextLabel::TextLabel( const TextLabel& handle )
: Control( handle )
{
}

TextLabel& TextLabel::operator=( const TextLabel& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TextLabel::~TextLabel()
{
}

TextLabel TextLabel::DownCast( BaseHandle handle )
{
  return Control::DownCast<TextLabel, Internal::TextLabel>(handle);
}

void TextLabel::SetRenderer( Text::RendererPtr renderer )
{
  GetImpl(*this).SetRenderer( renderer );
}

TextLabel::TextLabel( Internal::TextLabel& implementation )
: Control(implementation)
{
}

TextLabel::TextLabel( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TextLabel>( internal );
}

} // namespace Toolkit

} // namespace Dali
