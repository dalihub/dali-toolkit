/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include "radio-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/text-actor.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/scripting/scripting.h>

using namespace Dali;
using namespace Dali::Toolkit::Internal;

namespace Dali
{
namespace Toolkit
{
const Property::Index RadioButton::PROPERTY_ACTIVE = Internal::Button::BUTTON_PROPERTY_END_INDEX + 11;
const Property::Index RadioButton::PROPERTY_LABEL_ACTOR = Internal::Button::BUTTON_PROPERTY_END_INDEX + 12;
}
}

namespace
{

BaseHandle Create()
{
  return Toolkit::RadioButton::New();
}

TypeRegistration typeRegistration( typeid( Toolkit::RadioButton ), typeid( Toolkit::Button ), Create);

PropertyRegistration property1(typeRegistration, "active", Toolkit::RadioButton::PROPERTY_ACTIVE, Property::BOOLEAN, &RadioButton::SetProperty, &RadioButton::GetProperty);
PropertyRegistration property2(typeRegistration, "label-actor", Toolkit::RadioButton::PROPERTY_LABEL_ACTOR, Property::MAP, &RadioButton::SetProperty, &RadioButton::GetProperty);

const char* const INACTIVE_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-inactive.png";
const char* const ACTIVE_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-active.png";

const Vector3 DISTANCE_BETWEEN_IMAGE_AND_LABEL(5.0f, 0.0f, 0.0f);
}

Dali::Toolkit::RadioButton RadioButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< RadioButton > internalRadioButton = new RadioButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::RadioButton radioButton(*internalRadioButton);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalRadioButton->Initialize();

  return radioButton;
}

RadioButton::RadioButton()
  : mActive(false)
{
  mInactiveImage = Dali::Image::New( INACTIVE_BUTTON_IMAGE_DIR );
  mActiveImage = Dali::Image::New( ACTIVE_BUTTON_IMAGE_DIR );

  mRadioIcon = Dali::ImageActor::New( mInactiveImage );
}

RadioButton::~RadioButton()
{
}

void RadioButton::SetLabel(const std::string& label)
{
  TextActor textActor = TextActor::DownCast( mLabel );
  if( textActor )
  {
    textActor.SetText( label );
  }
  else
  {
    Toolkit::TextView newTextView = Toolkit::TextView::New( label );
    SetLabel( newTextView );
  }

  RelayoutRequest();
}

void RadioButton::SetLabel(Actor label)
{
  if( mLabel != label )
  {
    if( mLabel )
    {
      mRadioIcon.Remove( mLabel );
    }

    if( label )
    {
      label.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
      label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      label.MoveBy( DISTANCE_BETWEEN_IMAGE_AND_LABEL );
      mRadioIcon.Add( label );
    }

    mLabel = label;

    RelayoutRequest();
  }
}

Actor RadioButton::GetLabel() const
{
  return mLabel;
}

void RadioButton::SetActive(bool active)
{
  if( mActive != active )
  {
    if( active )
    {
      Actor parent = Self().GetParent();
      if( parent )
      {
        for( unsigned int i = 0; i < parent.GetChildCount(); ++i )
        {
          Dali::Toolkit::RadioButton rbChild = Dali::Toolkit::RadioButton::DownCast(parent.GetChildAt(i));

          if( rbChild )
          {
            rbChild.SetActive(false);
          }
        }
      }

      mActive = true;
      mRadioIcon.SetImage(mActiveImage);
    }
    else
    {
      mActive = false;
      mRadioIcon.SetImage(mInactiveImage);
    }

    // Raise toggled signal
    Toolkit::RadioButton handle( GetOwner() );
    mToggledSignal.Emit( handle, mActive );

    RelayoutRequest();
  }
}

bool RadioButton::IsActive()const
{
  return mActive;
}

void RadioButton::ToggleState()
{
  SetActive(!mActive);
}

void RadioButton::OnRelaidOut( Vector2 /*size*/, ActorSizeContainer& container )
{
  Vector3 newSize( mRadioIcon.GetNaturalSize() );

  if( mLabel )
  {
    // Offset the label from the radio button image
    newSize.width += DISTANCE_BETWEEN_IMAGE_AND_LABEL.width;

    // Find the size of the control using size negotiation
    Vector3 actorNaturalSize( mLabel.GetNaturalSize() );
    Control::Relayout( mLabel, Vector2( actorNaturalSize.width, actorNaturalSize.height ), container );

    Vector3 actorSize( mLabel.GetSize() );
    newSize.width += actorSize.width;
    newSize.height = std::max( newSize.height, actorSize.height );
  }

  Self().SetSize( newSize );
}

void RadioButton::OnInitialize()
{
  mRadioIcon.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
  mRadioIcon.SetParentOrigin( ParentOrigin::CENTER_LEFT );
  Self().Add( mRadioIcon );

  RelayoutRequest();
}

void RadioButton::OnButtonUp()
{
  // Don't allow selection on an already active radio button
  if( !mActive )
  {
    ToggleState();
  }
}

void RadioButton::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Toolkit::RadioButton radioButton = Toolkit::RadioButton::DownCast( Dali::BaseHandle( object ) );

  if( radioButton )
  {
    RadioButton& radioButtonImpl( GetImplementation( radioButton ) );

    switch ( propertyIndex )
    {
      case Toolkit::RadioButton::PROPERTY_ACTIVE:
      {
        radioButtonImpl.SetActive( value.Get< bool >( ) );
        break;
      }
      case Toolkit::RadioButton::PROPERTY_LABEL_ACTOR:
      {
        radioButtonImpl.SetLabel( Scripting::NewActor( value.Get< Property::Map >( ) ) );
        break;
      }
    }
  }
}

Property::Value RadioButton::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::RadioButton radioButton = Toolkit::RadioButton::DownCast( Dali::BaseHandle(object) );

  if( radioButton )
  {
    RadioButton& radioButtonImpl( GetImplementation( radioButton ) );

    switch ( propertyIndex )
    {
      case Toolkit::RadioButton::PROPERTY_ACTIVE:
      {
        value = radioButtonImpl.mActive;
        break;
      }
      case Toolkit::RadioButton::PROPERTY_LABEL_ACTOR:
      {
        Property::Map map;
        Scripting::CreatePropertyMap( radioButtonImpl.mLabel, map );
        value = map;
        break;
      }
    }
  }

  return value;
}
