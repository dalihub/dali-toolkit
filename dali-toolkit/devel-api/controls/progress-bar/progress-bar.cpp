/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/progress-bar/progress-bar-impl.h>

namespace Dali
{

namespace Toolkit
{

ProgressBar::ProgressBar()
{
}

ProgressBar::ProgressBar( const ProgressBar& handle )
: Control( handle )
{
}

ProgressBar& ProgressBar::operator=( const ProgressBar& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

ProgressBar::ProgressBar(Internal::ProgressBar& implementation)
: Control(implementation)
{
}

ProgressBar::ProgressBar( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::ProgressBar>(internal);
}

ProgressBar ProgressBar::New()
{
  return Internal::ProgressBar::New();
}

ProgressBar::~ProgressBar()
{
}

ProgressBar::ValueChangedSignalType& ProgressBar::ValueChangedSignal()
{
  return GetImpl( *this ).ValueChangedSignal();
}

ProgressBar ProgressBar::DownCast( BaseHandle handle )
{
  return Control::DownCast<ProgressBar, Internal::ProgressBar>(handle);
}

} // namespace Toolkit

} // namespace Dali
