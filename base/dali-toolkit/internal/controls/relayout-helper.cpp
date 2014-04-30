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

#include "relayout-helper.h"

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
  Vector3 size = actor.GetCurrentSize();
  const float depth = size.depth;

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

  // Get natural size for ImageActor.
  // TODO: currently it doesn't work as expected.
  ImageActor imageActor = ImageActor::DownCast( actor );
  if( ( imageActor ) && ( imageActor.GetImage() ) )
  {
    Image image = imageActor.GetImage();
    size = Vector3( static_cast<float>( image.GetWidth() ), static_cast<float>( image.GetHeight() ), depth );
  }

  return size;
}

float GetHeightForWidth( Actor actor, float width )
{
  Vector3 size = actor.GetCurrentSize();
  float height = 0.f;

  TextActor textActor = TextActor::DownCast( actor );
  if( textActor )
  {
    Font font = textActor.GetFont();
    if( !font )
    {
      font = Font::New();
    }
    size = font.MeasureText( textActor.GetText() );
  }

  ImageActor imageActor = ImageActor::DownCast( actor );
  if( ( imageActor ) && ( imageActor.GetImage() ) )
  {
    Image image = imageActor.GetImage();
    size = Vector3( static_cast<float>( image.GetWidth() ), static_cast<float>( image.GetHeight() ), 0.f );
  }

  height = size.height / ( size.width / width );

  return height;
}

} // namespace RelayoutHelper

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
