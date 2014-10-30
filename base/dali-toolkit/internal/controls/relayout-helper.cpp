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
#include "relayout-helper.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/text-actor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace RelayoutHelper
{

Vector3 GetNaturalSize( Actor actor )
{
  Vector3 size( 0.0f, 0.0f, 0.0f );

  Toolkit::Control control = Toolkit::Control::DownCast( actor );
  if( control )
  {
    size = control.GetNaturalSize();
  }
  else
  {
    size = actor.GetCurrentSize();
    const float depth = size.depth;

    // Get natural size for ImageActor.
    // TODO: currently it doesn't work as expected.
    ImageActor imageActor = ImageActor::DownCast( actor );
    if( ( imageActor ) && ( imageActor.GetImage() ) )
    {
      Image image = imageActor.GetImage();
      size = Vector3( static_cast<float>( image.GetWidth() ), static_cast<float>( image.GetHeight() ), depth );
    }
    else
    {
      // Get natural size for TextActor.
      TextActor textActor = TextActor::DownCast( actor );
      if( textActor )
      {
        Font font = textActor.GetFont();
        if( !font )
        {
          font = Font::New();
        }
        size = font.MeasureText( textActor.GetText() );
        size.depth = depth;
      }
    }
  }

  return size;
}

float GetHeightForWidth( Actor actor, float width )
{
  float height = 0.0f;

  Toolkit::Control control = Toolkit::Control::DownCast( actor );
  if( control )
  {
    height = control.GetHeightForWidth( width );
  }
  else
  {
    bool constrainSize = false;
    Vector3 size( 0.0f, 0.0f, 0.0f );

    ImageActor imageActor = ImageActor::DownCast( actor );
    if( ( imageActor ) && ( imageActor.GetImage() ) )
    {
      Image image = imageActor.GetImage();
      size = Vector3( static_cast<float>( image.GetWidth() ), static_cast<float>( image.GetHeight() ), 0.0f );

      constrainSize = true;
    }
    else
    {
      TextActor textActor = TextActor::DownCast( actor );
      if( textActor )
      {
        Font font = textActor.GetFont();
        if( !font )
        {
          font = Font::New();
        }
        size = font.MeasureText( textActor.GetText() );

        constrainSize = true;
      }
      else
      {
        size = actor.GetCurrentSize();
      }
    }

    // Scale the actor
    float scaleRatio = width / size.width;
    if( constrainSize )
    {
      // Allow the scale to decrease if greater than input width but not increase if less than input width
      if( scaleRatio > 1.0f )
      {
        scaleRatio = 1.0f;
      }
    }

    height = size.height * scaleRatio;
  }

  return height;
}

} // namespace RelayoutHelper

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
