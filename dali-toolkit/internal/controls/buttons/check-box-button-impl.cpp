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
#include "check-box-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/resource-image.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float DISTANCE_BETWEEN_IMAGE_AND_LABEL( 5.0f );
const float ANIMATION_TIME( 0.26f );  // EFL checkbox tick time

BaseHandle Create()
{
  return Toolkit::CheckBoxButton::New();
}

TypeRegistration mType( typeid(Toolkit::CheckBoxButton), typeid(Toolkit::Button), Create );

const char* const UNSELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "checkbox-unselected.png";
const char* const SELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "checkbox-selected.png";
const char* const DISABLED_UNSELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "checkbox-unselected-disabled.png";
const char* const DISABLED_SELECTED_BUTTON_IMAGE_DIR = DALI_IMAGE_DIR "checkbox-selected-diabled.png";
}

Dali::Toolkit::CheckBoxButton CheckBoxButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< CheckBoxButton > internalCheckBoxButton = new CheckBoxButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::CheckBoxButton checkBoxButton( *internalCheckBoxButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalCheckBoxButton->Initialize();

  return checkBoxButton;
}

CheckBoxButton::CheckBoxButton()
: Button()
{
  SetTogglableButton( true );

  SetAnimationTime( ANIMATION_TIME );
}

CheckBoxButton::~CheckBoxButton()
{
}

void CheckBoxButton::SetTickUVEffect()
{
  Toolkit::ImageView imageView = Toolkit::ImageView::DownCast( mSelectedImage );
  if( imageView )
  {
    imageView.RegisterProperty( "uTextureRect", Vector4(0.f, 0.f, 1.f, 1.f ) );
    imageView.RegisterProperty( "uTopLeft", Vector2::ZERO );

    Property::Map shaderMap = CreateImageRegionEffect();
    imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, shaderMap );

    GetImpl( imageView ).SetDepthIndex( DepthIndex::DECORATION );
  }
}

void CheckBoxButton::OnInitialize()
{
  Button::OnInitialize();

  // Wrap around all children
  Self().SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  SetUnselectedImage( UNSELECTED_BUTTON_IMAGE_DIR );
  SetSelectedImage( SELECTED_BUTTON_IMAGE_DIR );
  SetDisabledImage( DISABLED_UNSELECTED_BUTTON_IMAGE_DIR );
  SetDisabledSelectedImage( DISABLED_SELECTED_BUTTON_IMAGE_DIR );

  mSelectedImage = GetSelectedImage();
  SetTickUVEffect();
}

void CheckBoxButton::OnLabelSet( bool noPadding )
{
  Actor& label = GetLabelActor();

  if( label )
  {
    label.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    label.SetAnchorPoint( AnchorPoint::CENTER_LEFT );

    if( IsDisabled() && GetDisabledBackgroundImage() )
    {
      label.SetX( GetDisabledBackgroundImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if ( GetBackgroundImage() )
    {
      label.SetX( GetBackgroundImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( IsSelected() && GetSelectedImage())
    {
      label.SetX( GetSelectedImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( GetUnselectedImage() )
    {
      label.SetX( GetUnselectedImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else
    {
      label.SetX( DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
  }
}

void CheckBoxButton::OnDisabled()
{
  Actor& backgroundImage = GetBackgroundImage();
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();

  Actor& label = GetLabelActor();
  if( label )
  {
    if( IsDisabled() && disabledBackgroundImage )
    {
      label.SetX( disabledBackgroundImage.GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( backgroundImage )
    {
      label.SetX( backgroundImage.GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( IsSelected() && GetSelectedImage())
    {
      label.SetX( GetSelectedImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else if( GetUnselectedImage() )
    {
      label.SetX( GetUnselectedImage().GetNaturalSize().width + DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
    else
    {
      label.SetX( DISTANCE_BETWEEN_IMAGE_AND_LABEL );
    }
  }
}

void CheckBoxButton::PrepareForTranstionIn( Actor actor )
{
  Actor& selectedImage = GetSelectedImage();
  if( actor == selectedImage )
  {
    actor.SetScale( Vector3( 0.0f, 1.0f, 1.0f ) );
    actor.RegisterProperty( "uBottomRight", Vector2( 0.0f, 1.0f ) );

    if( mSelectedImage != selectedImage )
    {
      mSelectedImage = selectedImage;
      SetTickUVEffect();
    }
  }
}

void CheckBoxButton::PrepareForTranstionOut( Actor actor )
{
  Actor& selectedImage = GetSelectedImage();
  if( actor == selectedImage )
  {
    actor.SetScale( Vector3::ONE );
    actor.RegisterProperty( "uBottomRight", Vector2::ONE );

    if( mSelectedImage != selectedImage )
    {
      mSelectedImage = selectedImage;
      SetTickUVEffect();
    }
  }
}

void CheckBoxButton::OnTransitionIn( Actor actor )
{
  Actor& selectedImage = GetSelectedImage();
  if( actor && actor == selectedImage )
  {
    if( GetPaintState() == UnselectedState )
    {
      Dali::Animation transitionAnimation = GetTransitionAnimation();
      if( transitionAnimation )
      {
        // UV anim
        transitionAnimation.AnimateTo( Property( actor, "uBottomRight" ), Vector2::ONE );

        // Actor size anim
        transitionAnimation.AnimateTo( Property( actor, Actor::Property::SCALE_X ), 1.0f );
      }
    }
    else
    {
      //explicitly end the swipe animation
      actor.SetScale( Vector3::ONE );
      if( mSelectedImage == selectedImage  )
      {
        actor.RegisterProperty( "uBottomRight", Vector2::ONE );
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
