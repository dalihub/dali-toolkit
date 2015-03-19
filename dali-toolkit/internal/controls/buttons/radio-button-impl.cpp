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
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/resource-image.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

BaseHandle Create()
{
  return Toolkit::RadioButton::New();
}

TypeRegistration typeRegistration( typeid( Toolkit::RadioButton ), typeid( Toolkit::Button ), Create);

const char* const UNSELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-unselected.png";
const char* const SELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "radio-button-selected.png";

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
{
  SetTogglableButton(true);
}

RadioButton::~RadioButton()
{
}

void RadioButton::SetButtonImage( Actor image )
{
  Actor& buttonImage = GetButtonImage();

  if( !IsSelected() )
  {
    if( buttonImage && buttonImage.GetParent() )
    {
      buttonImage.GetParent().Remove( buttonImage );
      buttonImage.Reset();
    }

    Self().Add( image );

    Actor& label = GetLabel();

    if( label )
    {
      buttonImage.Remove( label );
      image.Add( label );
    }
  }

  buttonImage = image;

  buttonImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buttonImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
}

void RadioButton::SetSelectedImage( Actor image )
{
  Actor& selectedImage = GetSelectedImage();

  if( IsSelected() )
  {
    if( selectedImage && selectedImage.GetParent() )
    {
      selectedImage.GetParent().Remove( selectedImage );
      selectedImage.Reset();
    }

    Self().Add( image );

    Actor& label = GetLabel();

    if( label )
    {
      selectedImage.Remove( label );
      image.Add( label );
    }
  }

  selectedImage = image;

  selectedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selectedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
}

void RadioButton::OnButtonInitialize()
{
  Image buttonImage = Dali::ResourceImage::New( UNSELECTED_BUTTON_IMAGE_DIR );
  Image selectedImage = Dali::ResourceImage::New( SELECTED_BUTTON_IMAGE_DIR );

  SetButtonImage( ImageActor::New( buttonImage ) );
  SetSelectedImage( ImageActor::New( selectedImage ) );

  RelayoutRequest();
}

void RadioButton::OnButtonUp()
{
  if( ButtonDown == GetState() )
  {
    // Don't allow selection on an already selected radio button
    if( !IsSelected() )
    {
      SetSelected(!IsSelected());
    }
  }
}

void RadioButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    label.TranslateBy( DISTANCE_BETWEEN_IMAGE_AND_LABEL );

    if( IsSelected() )
    {
      GetSelectedImage().Add( label );
    }
    else
    {
      GetButtonImage().Add( label );
    }
  }
}

void RadioButton::OnSelected( bool selected )
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& label = GetLabel();

  if( selected )
  {
    Actor parent = Self().GetParent();
    if( parent )
    {
      for( unsigned int i = 0; i < parent.GetChildCount(); ++i )
      {
        Dali::Toolkit::RadioButton rbChild = Dali::Toolkit::RadioButton::DownCast(parent.GetChildAt(i));

        if( rbChild )
        {
          rbChild.SetSelected(false);
        }
      }
    }

    buttonImage.GetParent().Remove( buttonImage );
    Self().Add( selectedImage );

    if( label )
    {
      label.GetParent().Remove( label );
      selectedImage.Add( label );
    }
  }
  else
  {
    selectedImage.GetParent().Remove( selectedImage );
    Self().Add( buttonImage );

    if( label )
    {
      label.GetParent().Remove( label );
      buttonImage.Add( label );
    }
  }
}

void RadioButton::OnRelayout( const Vector2& /*size*/, ActorSizeContainer& container )
{
  Vector3 newSize;

  if( IsSelected() )
  {
    newSize = GetSelectedImage().GetNaturalSize();
  }
  else
  {
    newSize = GetButtonImage().GetNaturalSize();
  }

  Actor& label = GetLabel();

  if( label )
  {
    // Offset the label from the radio button image
    newSize.width += DISTANCE_BETWEEN_IMAGE_AND_LABEL.width;

    // Find the size of the control using size negotiation
    Vector3 actorNaturalSize( label.GetNaturalSize() );
    Control::Relayout( label, Vector2( actorNaturalSize.width, actorNaturalSize.height ), container );

    Vector3 actorSize( label.GetSize() );
    newSize.width += actorSize.width;
    newSize.height = std::max( newSize.height, actorSize.height );
  }

  Self().SetSize( newSize );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
