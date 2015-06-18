/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/images/buffer-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>


namespace Dali
{

namespace Toolkit
{

namespace
{
const unsigned int MAX_BORDER_SIZE( 9 );
}

ImageActor CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
{
  ImageActor image;
  if( borderSize > MAX_BORDER_SIZE )
  {
    return image;
  }

  const unsigned int bitmapWidth = borderSize * 2 + 2;
  BufferImage imageData = BufferImage::New( bitmapWidth, bitmapWidth, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = imageData.GetBuffer();
  if( !pixbuf )
  {
    return image;
  }

  Vector4 outerColor = color;
  if ( border )
  {
    outerColor = borderColor;
  }

  // Using a (2 + border) x (2 + border) image gives a better blend with the GL implementation
  // than a (1 + border) x (1 + border) image
  const unsigned int bitmapSize = bitmapWidth * bitmapWidth;
  const unsigned int topLeft = bitmapWidth * borderSize + borderSize;
  const unsigned int topRight = topLeft + 1;
  const unsigned int bottomLeft = bitmapWidth * (borderSize + 1) + borderSize;
  const unsigned int bottomRight = bottomLeft + 1;

  for( size_t i = 0; i < bitmapSize; ++i )
  {
    if( i == topLeft ||
        i == topRight ||
        i == bottomLeft ||
        i == bottomRight )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }
    else
    {
      pixbuf[i*4+0] = 0xFF * outerColor.r;
      pixbuf[i*4+1] = 0xFF * outerColor.g;
      pixbuf[i*4+2] = 0xFF * outerColor.b;
      pixbuf[i*4+3] = 0xFF * outerColor.a;
    }
  }

  imageData.Update();
  image = ImageActor::New( imageData );
  image.SetParentOrigin( ParentOrigin::CENTER );

  if( border )
  {
    image.SetStyle( ImageActor::STYLE_NINE_PATCH );
    image.SetNinePatchBorder( Vector4::ONE * (float)borderSize * 2.0f );
  }

  return image;
}

} // namespace Toolkit

} // namespace Dali
