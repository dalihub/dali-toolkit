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
#include <algorithm>
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include "push-button-default-painter-impl.h"

#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include <dali-toolkit/internal/controls/relayout-helper.h>

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
  return Toolkit::PushButton::New();
}

TypeRegistration typeRegistration( typeid(Toolkit::PushButton), typeid(Toolkit::Button), Create );

} // unnamed namespace

namespace
{

const float TEXT_PADDING = 12.0f;

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

void PushButton::OnButtonInitialize()
{
  // Push button requires the Leave event.
  Actor root = Self();
  root.SetLeaveRequired( true );
}

PushButton::PushButton()
: Button()
{
  // Creates specific painter.GetBu
  ButtonPainterPtr painter = PushButtonDefaultPainterPtr( new PushButtonDefaultPainter() );
  SetPainter( painter );
}

PushButton::~PushButton()
{
  SetPainter( NULL );
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
      Vector3 imageSize = RelayoutHelper::GetNaturalSize( imageActor );

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
      Vector3 imageSize = RelayoutHelper::GetNaturalSize( backgroundImageActor );

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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
