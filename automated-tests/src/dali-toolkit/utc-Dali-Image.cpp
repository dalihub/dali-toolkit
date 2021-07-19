/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/adaptor-framework/native-image-source.h>
#include <dali-toolkit/public-api/image-loader/image.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
} // namespace


void dali_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageConvertFrameBufferToUrl1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageConvertFrameBufferToUrl1" );

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  DALI_TEST_CHECK( frameBuffer );
  std::string url = Dali::Toolkit::Image::GenerateUrl( frameBuffer, Pixel::Format::RGBA8888, width, height );

  DALI_TEST_CHECK( url.size() > 0u );

  END_TEST;
}


int UtcDaliImageConvertFrameBufferToUrl2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageConvertFrameBufferToUrl2" );

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  DALI_TEST_CHECK( Dali::Toolkit::Image::GenerateUrl( frameBuffer, 0 ).size() > 0u );

  END_TEST;
}


int UtcDaliImageConvertPixelDataToUrl(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageConvertPixelDataToUrl" );

  unsigned int width(64);
  unsigned int height(64);
  unsigned int bufferSize = width*height*Pixel::GetBytesPerPixel( Pixel::RGB888 );

  unsigned char* buffer= reinterpret_cast<unsigned char*>( malloc( bufferSize ) );
  PixelData pixelData = PixelData::New( buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE );

  DALI_TEST_CHECK( Dali::Toolkit::Image::GenerateUrl( pixelData ).size() > 0u );

  END_TEST;
}


int UtcDaliImageConvertNativeImageSourceToUrl(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageConvertNativeImageSourceToUrl" );

  unsigned int width(64);
  unsigned int height(64);
  try
  {
    NativeImageSourcePtr nativeImageSource = NativeImageSource::New(width, height, NativeImageSource::COLOR_DEPTH_DEFAULT );

    DALI_TEST_CHECK( Dali::Toolkit::Image::GenerateUrl( nativeImageSource ).size() > 0u );
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "Adaptor::IsAvailable()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n" );
    tet_result(TET_FAIL);
  }

  END_TEST;
}
