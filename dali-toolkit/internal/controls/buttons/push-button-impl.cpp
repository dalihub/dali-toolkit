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
#include "push-button-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const float TEXT_PADDING = 12.0f;

const float LABEL_DEPTH( 0.75f );
const float FOREGROUND_DEPTH( 0.5f );
const float BACKGROUND_DEPTH( 0.25f );

const float ANIMATION_TIME( 0.2f );

BaseHandle Create()
{
  return Toolkit::PushButton::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::PushButton), typeid(Toolkit::Button), Create );

} // unnamed namespace

namespace
{

/**
 * Find the first image actor in the actor hierarchy
 */
ImageActor FindImageActor( Actor root )
{
  ImageActor imageActor = ImageActor::DownCast( root );
  if( !imageActor && root )
  {
    for( unsigned int i = 0, numChildren = root.GetChildCount(); i < numChildren; ++i )
    {
      ImageActor childImageActor = FindImageActor( root.GetChildAt( i ) );
      if( childImageActor )
      {
        return childImageActor;
      }
    }
  }

  return imageActor;
}

} // unnamed namespace

Dali::Toolkit::PushButton PushButton::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr< PushButton > internalPushButton = new PushButton();

  // Pass ownership to CustomActor
  Dali::Toolkit::PushButton pushButton( *internalPushButton );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  internalPushButton->Initialize();

  return pushButton;
}

PushButton::PushButton()
: Button(),
  mSize(),
  mPaintState( UnselectedState )
{
  SetAnimationTime( ANIMATION_TIME );
}

PushButton::~PushButton()
{
}

void PushButton::SetButtonImage( Actor image )
{
  Actor& buttonImage = GetButtonImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case UnselectedState:
    {
      if( buttonImage && buttonImage.GetParent() )
      {
        StopFadeOutAnimation();
        FadeOutImage( Foreground, buttonImage );

        buttonImage = image;

        FadeInImage( buttonImage, 0.0f, 0 );

        StartFadeOutAnimation();
        StartFadeInAnimation();
      }
      else
      {
        buttonImage = image;
        Self().Insert( 0, buttonImage );
      }
      break;
    }
    case UnselectedSelectedTransition: // FALLTHROUGH
    case UnselectedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();

      // Replaces the button image.
      buttonImage = image;

      Self().Insert( 0, buttonImage );
      FadeOutImage( Foreground, buttonImage, opacity );

      StartFadeOutAnimation();
      break;
    }
    case SelectedUnselectedTransition: // FALLTHROUGH
    case DisabledUnselectedTransition:
    {
      StopFadeInAnimation();
      Self().Remove( buttonImage );

      buttonImage = image;

      FadeInImage( buttonImage, 0.0f, 0 );
      StartFadeInAnimation();
      break;
    }
    default:
      buttonImage = image;
      break;
  }

  buttonImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buttonImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buttonImage.SetPosition( 0.f, 0.f, FOREGROUND_DEPTH );
  buttonImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
  buttonImage.SetName( "BUTTON_IMAGE" );
}

void PushButton::SetSelectedImage( Actor image )
{
  Actor& selectedImage = GetSelectedImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case SelectedState:
    {
      if( selectedImage && selectedImage.GetParent() )
      {
        StopFadeOutAnimation();
        FadeOutImage( Foreground, selectedImage  );

        selectedImage = image;

        FadeInImage( selectedImage, 0.0f, 0 );

        StartFadeOutAnimation();
        StartFadeInAnimation();
      }
      else
      {
        selectedImage = image;
        Self().Insert( 0, selectedImage );
      }
      break;
    }
    case SelectedUnselectedTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();

      // Replaces the button image.
      selectedImage = image;

      Self().Insert( 0, selectedImage );
      FadeOutImage( Foreground, selectedImage, opacity );

      StartFadeOutAnimation();
      break;
    }
    case UnselectedSelectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      StopFadeInAnimation();
      Self().Remove( selectedImage );

      selectedImage = image;

      FadeInImage( selectedImage, 0.0f, 0 );
      StartFadeInAnimation();
      break;
    }
    default:
      selectedImage = image;
      break;
  }

  selectedImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  selectedImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  selectedImage.SetPosition( 0.f, 0.f, FOREGROUND_DEPTH );
  selectedImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
}

void PushButton::SetBackgroundImage( Actor image )
{
  Actor& backgroundImage = GetBackgroundImage();
  Actor& fadeOutBackgroundImage = GetFadeOutBackgroundImage();

  switch( mPaintState )
  {
    case UnselectedState:             // FALLTHROUGH
    case SelectedState:
    case UnselectedSelectedTransition:
    case SelectedUnselectedTransition:
    {
      if( backgroundImage && backgroundImage.GetParent() )
      {
        StopFadeOutAnimation();
        FadeOutImage( Background, backgroundImage  );

        backgroundImage = image;

        FadeInImage( backgroundImage, 0.0f, 0 );

        StartFadeOutAnimation();
        StartFadeInAnimation();
      }
      else
      {
        backgroundImage = image;
        Self().Insert( 0, backgroundImage );
      }
      break;
    }
    case UnselectedDisabledTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();

      // Replaces the button image.
      backgroundImage = image;

      Self().Insert( 0, backgroundImage );
      FadeOutImage( Background, backgroundImage, opacity );

      StartFadeOutAnimation();
      break;
    }
    case DisabledUnselectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      StopFadeInAnimation();
      Self().Remove( backgroundImage );

      backgroundImage = image;

      FadeInImage( backgroundImage, 0.0f, 0 );
      StartFadeInAnimation();
      break;
    }
    default:
      backgroundImage = image;
      break;
    }

  backgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  backgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  backgroundImage.SetPosition( 0.f, 0.f, BACKGROUND_DEPTH );
  backgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
}

void PushButton::SetDisabledImage( Actor image )
{
  Actor& disabledImage = GetDisabledImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case DisabledUnselectedState: // FALLTHROUGH
    case DisabledSelectedState:
    {
      if( disabledImage && disabledImage.GetParent() )
      {
        StopFadeOutAnimation();
        FadeOutImage( Foreground, disabledImage  );

        disabledImage = image;

        FadeInImage( disabledImage, 0.0f, 0 );

        StartFadeOutAnimation();
        StartFadeInAnimation();
      }
      else
      {
        disabledImage = image;
        Self().Add( disabledImage );
      }
      break;
    }
    case UnselectedDisabledTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      StopFadeInAnimation();
      Self().Remove( disabledImage );

      disabledImage = image;

      FadeInImage( disabledImage, 0.0f, 0 );
      StartFadeInAnimation();
      break;
    }
    case DisabledUnselectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();

      // Replaces the button image.
      disabledImage = image;

      Self().Add( disabledImage );
      FadeOutImage( Foreground, disabledImage, opacity );

      StartFadeOutAnimation();
      break;
    }
    default:
      disabledImage = image;
      break;
  }

  disabledImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  disabledImage.SetPosition( 0.f, 0.f, FOREGROUND_DEPTH );
  disabledImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
}

void PushButton::SetDisabledBackgroundImage( Actor image )
{
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();
  Actor& fadeOutBackgroundImage = GetFadeOutBackgroundImage();

  switch( mPaintState )
  {
    case DisabledUnselectedState: // FALLTHROUGH
    case DisabledSelectedState:
    {
      if( disabledBackgroundImage && disabledBackgroundImage.GetParent() )
      {
        StopFadeOutAnimation();
        FadeOutImage( Background, disabledBackgroundImage  );

        disabledBackgroundImage = image;

        FadeInImage( disabledBackgroundImage, 0.0f, 0 );

        StartFadeOutAnimation();
        StartFadeInAnimation();
      }
      else
      {
        disabledBackgroundImage = image;
        Self().Add( disabledBackgroundImage );
      }
      break;
    }
    case UnselectedDisabledTransition: // FALLTHROUGH
    case SelectedDisabledTransition:
    {
      StopFadeInAnimation();
      Self().Remove( disabledBackgroundImage );

      disabledBackgroundImage = image;

      FadeInImage( disabledBackgroundImage, 0.0f, 0 );
      StartFadeInAnimation();
      break;
    }
    case DisabledUnselectedTransition: // FALLTHROUGH
    case DisabledSelectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutBackgroundImage )
      {
        opacity = fadeOutBackgroundImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();

      // Replaces the button image.
      disabledBackgroundImage = image;

      Self().Add( disabledBackgroundImage );
      FadeOutImage( Background, disabledBackgroundImage, opacity );

      StartFadeOutAnimation();
      break;
    }
    default:
      disabledBackgroundImage = image;
      break;
  }

  disabledBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  disabledBackgroundImage.SetParentOrigin( ParentOrigin::TOP_LEFT );
  disabledBackgroundImage.SetPosition( 0.f, 0.f, BACKGROUND_DEPTH );
  disabledBackgroundImage.SetResizePolicy( FILL_TO_PARENT, ALL_DIMENSIONS );
}

void PushButton::OnButtonInitialize()
{
  // Push button requires the Leave event.
  Actor root = Self();
  root.SetLeaveRequired( true );

  // Set resize policy to natural size so that buttons will resize to background images
  root.SetResizePolicy( USE_NATURAL_SIZE, ALL_DIMENSIONS );
}

void PushButton::OnLabelSet()
{
  Actor& label = GetLabel();

  if( label )
  {
    label.SetAnchorPoint( AnchorPoint::CENTER );
    label.SetParentOrigin( ParentOrigin::CENTER );
    label.SetPosition( 0.f, 0.f, LABEL_DEPTH );
    label.SetSize( mSize );

    Self().Add( label );
  }
}

void PushButton::OnSelected( bool selected )
{
  Actor& selectedImage = GetSelectedImage();
  Actor& buttonImage = GetButtonImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case UnselectedState:
    {
      StopFadeOutAnimation();
      FadeOutImage( Foreground, buttonImage );
      FadeInImage( selectedImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case SelectedState:
    {
      StopFadeOutAnimation();
      FadeOutImage( Foreground, selectedImage );
      FadeInImage( buttonImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, selectedImage, 1.f - opacity );
      FadeInImage( buttonImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case SelectedUnselectedTransition:
    {
      float opacity = 0.f;
      if( fadeOutButtonImage )
      {
        opacity = 1.f - fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, buttonImage, 1.f - opacity );
      FadeInImage( selectedImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

void PushButton::OnDisabled( bool disabled )
{
  Actor& buttonImage = GetButtonImage();
  Actor& selectedImage = GetSelectedImage();
  Actor& backgroundImage = GetBackgroundImage();
  Actor& disabledImage = GetDisabledImage();
  Actor& disabledBackgroundImage = GetDisabledBackgroundImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
  case UnselectedState:
  {
    if( disabled )
    {
      StopFadeOutAnimation();
      FadeOutImage( Background, backgroundImage );
      FadeOutImage( Foreground, buttonImage );
      FadeInImage( disabledBackgroundImage, 0.0f, 0 );
      FadeInImage( disabledImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = UnselectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledUnselectedState;
      }
    }
    break;
  }
  case SelectedState:
  {
    if( disabled )
    {
      StopFadeOutAnimation();
      FadeOutImage( Background, backgroundImage );
      FadeOutImage( Foreground, selectedImage );
      FadeInImage( disabledBackgroundImage, 0.0f, 0 );
      FadeInImage( disabledImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = SelectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledSelectedState;
      }
    }
    break;
  }
  case DisabledUnselectedState:
  {
    if( !disabled )
    {
      StopFadeOutAnimation();
      FadeOutImage( Background, disabledBackgroundImage );
      FadeOutImage( Foreground, disabledImage );
      FadeInImage( backgroundImage, 0.0f, 0 );
      FadeInImage( buttonImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
    }
    break;
  }
  case DisabledSelectedState:
  {
    if( !disabled )
    {
      StopFadeOutAnimation();
      FadeOutImage( Background, disabledBackgroundImage );
      FadeOutImage( Foreground, disabledImage );
      FadeInImage( backgroundImage, 0.0f, 0 );
      FadeInImage( selectedImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
    }
    break;
  }
  case UnselectedSelectedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();
      StopFadeInAnimation();

      FadeOutImage( Foreground, selectedImage, 1.f - opacity );
      FadeOutImage( Background, backgroundImage );

      FadeInImage( disabledImage, 0.0f, 0 );
      FadeInImage( disabledBackgroundImage, 0.0f, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = SelectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledSelectedState;
      }
    }
    break;
  }
  case SelectedUnselectedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation();
      StopFadeInAnimation();

      FadeOutImage( Foreground, buttonImage, 1.f - opacity );
      FadeOutImage( Background, backgroundImage );

      FadeInImage( disabledImage, 0.0f, 0);
      FadeInImage( disabledBackgroundImage, 0.0f, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = UnselectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledUnselectedState;
      }
    }
    break;
  }
  case UnselectedDisabledTransition:
  {
    if( !disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, disabledImage, 1.f - opacity );
      FadeOutImage( Background, disabledBackgroundImage, 1.f - opacity );
      FadeInImage( buttonImage, opacity, 0 );
      FadeInImage( backgroundImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
    }
    break;
  }
  case DisabledUnselectedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, buttonImage, 1.f - opacity );
      FadeOutImage( Background, backgroundImage, 1.f - opacity );
      FadeInImage( disabledImage, opacity, 0 );
      FadeInImage( disabledBackgroundImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = UnselectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledUnselectedState;
      }
    }
    break;
  }
  case SelectedDisabledTransition:
  {
    if( !disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, disabledImage, 1.f - opacity );
      FadeOutImage( Background, disabledBackgroundImage, 1.f - opacity );
      FadeInImage( selectedImage, opacity, 0 );
      FadeInImage( backgroundImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = DisabledSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
    }
    break;
  }
  case DisabledSelectedTransition:
  {
    if( disabled )
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, selectedImage, 1.f - opacity );
      FadeOutImage( Background, backgroundImage, 1.f - opacity );
      FadeInImage( disabledImage, opacity, 0 );
      FadeInImage( disabledBackgroundImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( selectedImage || disabledImage || backgroundImage || disabledBackgroundImage )
      {
        mPaintState = SelectedDisabledTransition;
      }
      else
      {
        mPaintState = DisabledSelectedState;
      }
    }
    break;
  }
  default:
    break;
  }
}

void PushButton::OnPressed()
{
  Actor& selectedImage = GetSelectedImage();
  Actor& buttonImage = GetButtonImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case UnselectedState:
    {
      StopFadeOutAnimation();
      FadeOutImage( Foreground, buttonImage );
      FadeInImage( selectedImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      if( !IsAutoRepeating() )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      break;
    }
    case SelectedUnselectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, buttonImage, 1.f - opacity );
      FadeInImage( selectedImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = UnselectedSelectedTransition;
      }
      else
      {
        mPaintState = SelectedState;
      }
      break;
    }
    default:
      break;
  }
}

void PushButton::OnReleased()
{
  Actor& selectedImage = GetSelectedImage();
  Actor& buttonImage = GetButtonImage();
  Actor& fadeOutButtonImage = GetFadeOutButtonImage();

  switch( mPaintState )
  {
    case SelectedState:
    {
      StopFadeOutAnimation();
      FadeOutImage( Foreground, selectedImage );
      FadeInImage( buttonImage, 0.0f, 0 );
      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    case UnselectedSelectedTransition:
    {
      float opacity = 1.f;
      if( fadeOutButtonImage )
      {
        opacity = fadeOutButtonImage.GetCurrentOpacity();
      }
      StopFadeOutAnimation( false );
      StopFadeInAnimation();

      FadeOutImage( Foreground, selectedImage, 1.f - opacity );
      FadeInImage( buttonImage, opacity, 0 );

      StartFadeOutAnimation();
      StartFadeInAnimation();

      if( buttonImage || selectedImage )
      {
        mPaintState = SelectedUnselectedTransition;
      }
      else
      {
        mPaintState = UnselectedState;
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

void PushButton::OnClicked()
{
  OnReleased();
}

void PushButton::OnControlSizeSet( const Vector3& targetSize )
{
  if( targetSize != mSize )
  {
    mSize = targetSize;

    Actor& label = GetLabel();

    if( label )
    {
      label.SetSize( mSize );
    }
  }
}

Vector3 PushButton::GetNaturalSize()
{
  Vector3 size = Control::GetNaturalSize();

  const bool widthIsZero = EqualsZero( size.width );
  const bool heightIsZero = EqualsZero( size.height );

  if( widthIsZero || heightIsZero )
  {
    // If background and background not scale9 try get size from that
    ImageActor imageActor = FindImageActor( GetButtonImage() );
    if( imageActor && imageActor.GetStyle() != ImageActor::STYLE_NINE_PATCH )
    {
      Vector3 imageSize = imageActor.GetNaturalSize();

      if( widthIsZero )
      {
        size.width = imageSize.width;
      }

      if( heightIsZero )
      {
        size.height = imageSize.height;
      }
    }

    ImageActor backgroundImageActor = FindImageActor( GetBackgroundImage() );
    if( backgroundImageActor && backgroundImageActor.GetStyle() != ImageActor::STYLE_NINE_PATCH )
    {
      Vector3 imageSize = backgroundImageActor.GetNaturalSize();

      if( widthIsZero )
      {
        size.width = std::max( size.width, imageSize.width );
      }

      if( heightIsZero )
      {
        size.height = std::max( size.height, imageSize.height );
      }
    }

    // If label, test against it's size
    Toolkit::TextView textView = Toolkit::TextView::DownCast( GetLabel() );
    if( textView )
    {
      Vector3 textViewSize = textView.GetNaturalSize();

      if( widthIsZero )
      {
        size.width = std::max( size.width, textViewSize.width + TEXT_PADDING * 2.0f );
      }

      if( heightIsZero )
      {
        size.height = std::max( size.height, textViewSize.height + TEXT_PADDING * 2.0f );
      }
    }
  }

  return size;
}

Actor& PushButton::GetFadeOutButtonImage()
{
  return mFadeOutButtonContent;
}

Actor& PushButton::GetFadeOutBackgroundImage()
{
  return mFadeOutBackgroundContent;
}

void PushButton::AddToFadeInAnimation( Actor& actor )
{
  if( !mFadeInAnimation )
  {
    mFadeInAnimation = Dali::Animation::New( GetAnimationTime() );
  }

  mFadeInAnimation.AnimateTo( Property( actor, Actor::Property::COLOR_ALPHA ), 1.f );
}

void PushButton::StartFadeInAnimation()
{
  if( mFadeInAnimation )
  {
    mFadeInAnimation.FinishedSignal().Connect( this, &PushButton::FadeInAnimationFinished );
    mFadeInAnimation.Play();
  }
}

void PushButton::StopFadeInAnimation()
{
  if( mFadeInAnimation )
  {
    mFadeInAnimation.Clear();
    mFadeInAnimation.Reset();
  }
}

void PushButton::AddToFadeOutAnimation( Actor& actor )
{
  if( !mFadeOutAnimation )
  {
    mFadeOutAnimation = Dali::Animation::New( GetAnimationTime() );
  }

  mFadeOutAnimation.AnimateTo( Property( actor, Actor::Property::COLOR_ALPHA ), 0.f );
}

void PushButton::StartFadeOutAnimation()
{
  if( mFadeOutAnimation )
  {
    mFadeOutAnimation.FinishedSignal().Connect( this, &PushButton::FadeOutAnimationFinished );
    mFadeOutAnimation.Play();
  }
}

void PushButton::StopFadeOutAnimation( bool remove )
{
  if( mFadeOutAnimation )
  {
    mFadeOutAnimation.Clear();
    mFadeOutAnimation.Reset();
  }

  if( remove )
  {
    Actor& fadeOutButtonImage = GetFadeOutButtonImage();
    Actor& fadeOutBackgroundImage = GetFadeOutBackgroundImage();

    if( fadeOutButtonImage && fadeOutButtonImage.GetParent() )
    {
      fadeOutButtonImage.GetParent().Remove( fadeOutButtonImage );
    }

    if( fadeOutBackgroundImage && fadeOutBackgroundImage.GetParent() )
    {
      fadeOutBackgroundImage.GetParent().Remove( fadeOutBackgroundImage );
    }

    fadeOutButtonImage.Reset();
    fadeOutBackgroundImage.Reset();
  }
}

void PushButton::FadeInImage( Actor& image, float opacity, int priority )
{
  if( image )
  {
    image.SetOpacity( opacity );
    if( !image.GetParent() )
    {
      if( priority > -1 )
      {
        Self().Insert( priority, image );
      }
      else
      {
        Self().Add( image );
      }
    }

    AddToFadeInAnimation( image );
  }
}

void PushButton::FadeOutImage( ImageLayer layer, Actor& image, float opacity )
{
  if( image )
  {
    Actor& fadeOutButtonImage = GetFadeOutButtonImage();
    Actor& fadeOutBackgroundImage = GetFadeOutBackgroundImage();

    Actor& actorLayer = ( ( Background == layer ) ? fadeOutBackgroundImage : fadeOutButtonImage );

    actorLayer = image;
    actorLayer.SetOpacity( opacity );

    AddToFadeOutAnimation( actorLayer );
  }
}

void PushButton::FadeOutAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case UnselectedSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case UnselectedDisabledTransition:
    {
      mPaintState = DisabledUnselectedState;
      break;
    }
    case DisabledUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case SelectedDisabledTransition:
    {
      mPaintState = DisabledSelectedState;
      break;
    }
    case DisabledSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    default:
    {
      break;
    }
  }

  StopFadeOutAnimation();
}

void PushButton::FadeInAnimationFinished( Dali::Animation& source )
{
  switch( mPaintState )
  {
    case UnselectedSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    case SelectedUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case UnselectedDisabledTransition:
    {
      mPaintState = DisabledUnselectedState;
      break;
    }
    case DisabledUnselectedTransition:
    {
      mPaintState = UnselectedState;
      break;
    }
    case SelectedDisabledTransition:
    {
      mPaintState = DisabledSelectedState;
      break;
    }
    case DisabledSelectedTransition:
    {
      mPaintState = SelectedState;
      break;
    }
    default:
    {
      break;
    }
  }

  StopFadeInAnimation();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
