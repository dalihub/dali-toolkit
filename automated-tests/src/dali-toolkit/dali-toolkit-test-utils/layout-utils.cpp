/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include "layout-utils.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/flex-layout.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Toolkit
{

std::string CreateImageURL( Vector4 color, ImageDimensions size )
{
  auto pixelBuffer = Devel::PixelBuffer::New( size.GetWidth(), size.GetHeight(), Pixel::RGB888 );
  unsigned char* pixels = pixelBuffer.GetBuffer();
  pixels[0] = static_cast<unsigned char>( color.r );
  pixels[1] = static_cast<unsigned char>( color.g );
  pixels[2] = static_cast<unsigned char>( color.b );
  auto texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGB888, size.GetWidth(), size.GetHeight() );
  auto pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
  texture.Upload( pixelData );
  return TextureManager::AddTexture( texture );
}

Control CreateLeafControl( int width, int height )
{
  auto control = Control::New();
  control.SetName( "Leaf" );
  std::string url = CreateImageURL( Vector4( 255, 0, 0, 255 ), ImageDimensions( 1, 1 ) );

  Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::IMAGE;
  map[ ImageVisual::Property::URL ] = url;
  map[ ImageVisual::Property::DESIRED_WIDTH ] = (float) width;
  map[ ImageVisual::Property::DESIRED_HEIGHT ] = (float) height;
  control.SetProperty( Control::Property::BACKGROUND, map );
  return control;
}

TextLabel CreateTextLabel( const char* text )
{
  TextLabel textLabel = TextLabel::New(text);
  textLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  textLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  textLabel.SetName( "TextLabel" );
  textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  return textLabel;
}

ImageView CreateImageView( std::string& url, ImageDimensions size )
{
  ImageView imageView = ImageView::New( url, size );
  imageView.SetName( "ImageView" );
  imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  return imageView;
}

} // namespace Toolkit

} // namespace Dali
