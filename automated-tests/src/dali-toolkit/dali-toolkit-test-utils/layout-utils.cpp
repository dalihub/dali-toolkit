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

Control CreateLeafControl( int width, int height )
{
  auto control = Control::New();
  control.SetName( "Leaf" );

  auto pixelBuffer = Devel::PixelBuffer::New( 1, 1, Pixel::RGB888 );
  unsigned char* pixels = pixelBuffer.GetBuffer();
  pixels[0] = 0xff;
  pixels[1] = 0x00;
  pixels[2] = 0x00;
  auto texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGB888, 1, 1 );
  auto pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
  texture.Upload( pixelData );
  std::string url = TextureManager::AddTexture( texture );

  Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::IMAGE;
  map[ ImageVisual::Property::URL ] = url;
  map[ ImageVisual::Property::DESIRED_WIDTH ] = (float) width;
  map[ ImageVisual::Property::DESIRED_HEIGHT ] = (float) height;
  control.SetProperty( Control::Property::BACKGROUND, map );
  return control;
}

} // namespace Toolkit

} // namespace Dali
