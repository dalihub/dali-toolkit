//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/slider/slider.h>
#include <dali-toolkit/internal/controls/slider/slider-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Slider
///////////////////////////////////////////////////////////////////////////////////////////////////

// Signals
const char* const Slider::SIGNAL_VALUE_CHANGED = "value-changed";
const char* const Slider::SIGNAL_MARK = "mark";

// Properties
const std::string Slider::LOWER_BOUND_PROPERTY_NAME( "lower-bound" );
const std::string Slider::UPPER_BOUND_PROPERTY_NAME( "upper-bound" );
const std::string Slider::VALUE_PROPERTY_NAME( "value" );
const std::string Slider::HIT_REGION_PROPERTY_NAME( "hit-region" );
const std::string Slider::BACKING_REGION_PROPERTY_NAME( "backing-region" );
const std::string Slider::HANDLE_REGION_PROPERTY_NAME( "handle-region" );

const std::string Slider::BACKING_IMAGE_NAME_PROPERTY_NAME( "backing-image-name" );
const std::string Slider::HANDLE_IMAGE_NAME_PROPERTY_NAME( "handle-image-name" );
const std::string Slider::PROGRESS_IMAGE_NAME_PROPERTY_NAME( "progress-image-name" );
const std::string Slider::POPUP_IMAGE_NAME_PROPERTY_NAME( "popup-image-name" );
const std::string Slider::POPUP_ARROW_IMAGE_NAME_PROPERTY_NAME( "popup-arrow-image-name" );

const std::string Slider::BACKING_SCALE9_BORDER_PROPERTY_NAME( "backing-scale9-border" );
const std::string Slider::PROGRESS_SCALE9_BORDER_PROPERTY_NAME( "progress-scale9-border" );
const std::string Slider::POPUP_SCALE9_BORDER_PROPERTY_NAME( "popup-scale9-border" );

const std::string Slider::DISABLE_COLOR_PROPERTY_NAME( "disable-color" );
const std::string Slider::POPUP_TEXT_COLOR_PROPERTY_NAME( "popup-text-color" );

const std::string Slider::VALUE_PRECISION_PROPERTY_NAME( "value-precision" );

const std::string Slider::SHOW_POPUP_PROPERTY_NAME( "show-popup" );
const std::string Slider::SHOW_VALUE_PROPERTY_NAME( "show-value" );

const std::string Slider::ENABLED_PROPERTY_NAME( "enabled" );

const std::string Slider::MARKS_PROPERTY_NAME( "marks" );
const std::string Slider::SNAP_TO_MARKS_PROPERTY_NAME( "snap-to-marks" );
const std::string Slider::MARK_TOLERANCE_PROPERTY_NAME( "mark-tolerance" );

Slider::Slider()
{
}

Slider::Slider( const Slider& handle )
: Control( handle )
{
}

Slider& Slider::operator=( const Slider& handle )
{
  if( &handle != this )
  {
    Control::operator=( handle );
  }
  return *this;
}

Slider::Slider(Internal::Slider& implementation)
: Control(implementation)
{
}

Slider::Slider( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<Internal::Slider>(internal);
}

Slider Slider::New()
{
  return Internal::Slider::New();
}

Slider::~Slider()
{
}

Slider::ValueChangedSignalType& Slider::ValueChangedSignal()
{
  return GetImpl( *this ).ValueChangedSignal();
}

Slider::ValueChangedSignalType& Slider::SlidingFinishedSignal()
{
  return GetImpl( *this ).SlidingFinishedSignal();
}

Slider::MarkSignalType& Slider::MarkSignal()
{
  return GetImpl( *this ).MarkSignal();
}

Slider Slider::DownCast( BaseHandle handle )
{
  return Control::DownCast<Slider, Internal::Slider>(handle);
}



} // namespace Toolkit

} // namespace Dali
