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
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

namespace Dali
{

namespace Toolkit
{

const std::string TextField::RENDERING_BACKEND_PROPERTY_NAME("rendering-backend");
const std::string TextField::PLACEHOLDER_TEXT_PROPERTY_NAME("placeholder-text");
const std::string TextField::TEXT_PROPERTY_NAME("text");
const std::string TextField::CURSOR_IMAGE_PROPERTY_NAME("cursor-image");
const std::string TextField::PRIMARY_CURSOR_COLOR_PROPERTY_NAME("primary-cursor-color");
const std::string TextField::SECONDARY_CURSOR_COLOR_PROPERTY_NAME("secondary-cursor-color");
const std::string TextField::ENABLE_CURSOR_BLINK_PROPERTY_NAME("enable-cursor-blink");
const std::string TextField::CURSOR_BLINK_INTERVAL_PROPERTY_NAME("cursor-blink-interval");
const std::string TextField::CURSOR_BLINK_DURATION_PROPERTY_NAME("cursor-blink-duration");
const std::string TextField::GRAB_HANDLE_IMAGE_PROPERTY_NAME("grab-handle-image");
const std::string TextField::DECORATION_BOUNDING_BOX("decoration-bounding-box");

TextField TextField::New()
{
  return Internal::TextField::New();
}

TextField::TextField()
{
}

TextField::TextField( const TextField& handle )
: Control( handle )
{
}

TextField& TextField::operator=( const TextField& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

TextField::~TextField()
{
}

TextField TextField::DownCast( BaseHandle handle )
{
  return Control::DownCast<TextField, Internal::TextField>(handle);
}

TextField::TextField( Internal::TextField& implementation )
: Control(implementation)
{
}

TextField::TextField( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::TextField>( internal );
}

} // namespace Toolkit

} // namespace Dali
