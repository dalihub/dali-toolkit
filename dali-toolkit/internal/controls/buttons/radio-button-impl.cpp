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

const float DISTANCE_BETWEEN_IMAGE_AND_LABEL = 5.0f;
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

void RadioButton::SetImage( Actor image )
{
  mLayoutContainer.RemoveChildAt( Toolkit::TableView::CellPosition( 0, 0 ) );
  mLayoutContainer.AddChild( image, Toolkit::TableView::CellPosition( 0, 0 ) );

  RelayoutRequest();
}

void RadioButton::SetButtonImage( Actor image )
{
  Actor& buttonImage = GetButtonImage();
  buttonImage = image;
}

void RadioButton::SetSelectedImage( Actor image )
{
  Actor& selectedImage = GetSelectedImage();
  selectedImage = image;
}

void RadioButton::OnButtonInitialize()
{
  Actor self = Self();

  // Wrap size of radio button around all its children
  self.SetResizePolicy( FIT_TO_CHILDREN, ALL_DIMENSIONS );

  // Create the layout container empty at first
  mLayoutContainer = Toolkit::TableView::New( 0, 0 );
  mLayoutContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  mLayoutContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
  mLayoutContainer.SetResizePolicy( FIT_TO_CHILDREN, ALL_DIMENSIONS );
  self.Add( mLayoutContainer );

  Image buttonImage = Dali::ResourceImage::New( UNSELECTED_BUTTON_IMAGE_DIR );
  Image selectedImage = Dali::ResourceImage::New( SELECTED_BUTTON_IMAGE_DIR );

  SetButtonImage( ImageActor::New( buttonImage ) );
  SetSelectedImage( ImageActor::New( selectedImage ) );

  SetImage( GetButtonImage() );

  RelayoutRequest();
}

void RadioButton::OnButtonUp()
{
  if( ButtonDown == GetState() )
  {
    // Don't allow selection on an already selected radio button
    if( !IsSelected() )
    {
      SetSelected( !IsSelected() );
    }
  }
}

void RadioButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    // Add padding to the left of the label to create distance from the image
    label.SetPadding( Padding( DISTANCE_BETWEEN_IMAGE_AND_LABEL, 0.0f, 0.0f, 0.0f ) );

    mLayoutContainer.RemoveChildAt( Toolkit::TableView::CellPosition( 0, 1 ) );
    mLayoutContainer.AddChild( label, Toolkit::TableView::CellPosition( 0, 1 ) );
  }
}

void RadioButton::OnSelected( bool selected )
{
  if( selected )
  {
    Actor parent = Self().GetParent();
    if( parent )
    {
      for( unsigned int i = 0; i < parent.GetChildCount(); ++i )
      {
        Dali::Toolkit::RadioButton radioButtonChild = Dali::Toolkit::RadioButton::DownCast( parent.GetChildAt( i ) );
        if( radioButtonChild )
        {
          radioButtonChild.SetSelected( false );
        }
      }
    }

    SetImage( GetSelectedImage() );
  }
  else
  {
    SetImage( GetButtonImage() );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
