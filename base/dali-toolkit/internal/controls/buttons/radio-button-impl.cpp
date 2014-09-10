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

TypeRegistration typeRegistration(typeid (Toolkit::RadioButton ), typeid (Toolkit::Button ), Create);

PropertyRegistration property1(typeRegistration, "active", Toolkit::RadioButton::PROPERTY_ACTIVE, Property::BOOLEAN, &RadioButton::SetProperty, &RadioButton::GetProperty);
PropertyRegistration property2(typeRegistration, "label-actor", Toolkit::RadioButton::PROPERTY_LABEL_ACTOR, Property::MAP, &RadioButton::SetProperty, &RadioButton::GetProperty);
}

namespace
{
const char* const INACTIVE_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-inactive.png";
const char* const ACTIVE_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-active.png";
const Vector3 IMAGE_WIDTH(16.f, 0.f, 0.f);
const Vector3 DISTANCE_BETWEEN_IMAGE_AND_LABEL(5.f, 0.f, 0.f);
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
  : Button(),
  mActive(false)
{
  mInactiveImage = Dali::Image::New(INACTIVE_BUTTON_IMAGE_DIR);
  mActiveImage = Dali::Image::New(ACTIVE_BUTTON_IMAGE_DIR);

  mImageActor = Dali::ImageActor::New(mInactiveImage);
  mLabel = Actor::New();
}

RadioButton::~RadioButton()
{
}

void RadioButton::SetLabel(const std::string& label)
{
  mLabel.Reset();
  mLabel = Actor::New();

  Toolkit::TextView textView = Toolkit::TextView::New(label);
  textView.SetWidthExceedPolicy(Toolkit::TextView::ShrinkToFit); // Make sure our text always fits inside the button
  textView.SetAnchorPoint(AnchorPoint::TOP_LEFT);

  mLabel.Add(textView);
}

void RadioButton::SetLabel(Actor label)
{
  if( mLabel != label )
  {
    Self().Remove(mLabel);
    mLabel = label;
    Self().Add(mLabel);
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
      mImageActor.SetImage(mActiveImage);
    }
    else
    {
      mActive = false;
      mImageActor.SetImage(mInactiveImage);
    }
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

void RadioButton::OnInitialize()
{
  mImageActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Self().Add(mImageActor);

  mLabel.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mLabel.MoveBy(IMAGE_WIDTH + DISTANCE_BETWEEN_IMAGE_AND_LABEL);
  Self().Add(mLabel);
}

void RadioButton::OnButtonUp()
{
  ToggleState();
}

void RadioButton::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Toolkit::RadioButton radioButton = Toolkit::RadioButton::DownCast(Dali::BaseHandle(object));

  if( radioButton )
  {
    RadioButton & radioButtonImpl(GetImplementation(radioButton));

    switch ( propertyIndex )
    {
      case Toolkit::RadioButton::PROPERTY_ACTIVE:
      {
        radioButtonImpl.SetActive(value.Get< bool >( ));
        break;
      }
      case Toolkit::RadioButton::PROPERTY_LABEL_ACTOR:
      {
        radioButtonImpl.SetLabel(Scripting::NewActor(value.Get< Property::Map >( )));
        break;
      }
    }
  }
}

Property::Value RadioButton::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::RadioButton radioButton = Toolkit::RadioButton::DownCast(Dali::BaseHandle(object));

  if( radioButton )
  {
    RadioButton & radioButtonImpl(GetImplementation(radioButton));

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
        Scripting::CreatePropertyMap(radioButtonImpl.mLabel, map);
        value = map;
        break;
      }
    }
  }

  return value;
}