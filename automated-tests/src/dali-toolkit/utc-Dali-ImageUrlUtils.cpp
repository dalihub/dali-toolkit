/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/public-api/image-loader/image-url-utils.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali/devel-api/adaptor-framework/native-image-queue.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>

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
  tet_infoline("UtcDaliImageConvertFrameBufferToUrl1");

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer  frameBuffer = FrameBuffer::New(width, height, FrameBuffer::Attachment::NONE);

  DALI_TEST_CHECK(frameBuffer);
  ImageUrl url = Dali::Toolkit::ImageUrlUtils::GenerateUrl(frameBuffer, Pixel::Format::RGBA8888, width, height);

  DALI_TEST_CHECK(url.GetUrl().Size() > 0u);

  END_TEST;
}

int UtcDaliImageConvertFrameBufferToUrl2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertFrameBufferToUrl2");

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer  frameBuffer = FrameBuffer::New(width, height, FrameBuffer::Attachment::NONE);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height);
  frameBuffer.AttachColorTexture(texture);

  DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(frameBuffer, 0).GetUrl().Size() > 0u);

  END_TEST;
}

int UtcDaliImageConvertDepthTextureFrameBufferToUrl(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertDepthTextureFrameBufferToUrl");

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer  frameBuffer = FrameBuffer::New(width, height, FrameBuffer::Attachment::NONE);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::DEPTH_UNSIGNED_INT, width, height);
  DevelFrameBuffer::AttachDepthTexture(frameBuffer, texture);

  DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateDepthUrl(frameBuffer).GetUrl().Size() > 0u);

  END_TEST;
}

int UtcDaliImageConvertPixelDataToUrl01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertPixelDataToUrl01");

  unsigned int width(64);
  unsigned int height(64);
  unsigned int bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::RGB888);

  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGB888, PixelData::FREE);

  DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(pixelData).GetUrl().Size() > 0u);

  END_TEST;
}

int UtcDaliImageConvertPixelDataToUrl02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertPixelDataToUrl02");

  unsigned int width(64);
  unsigned int height(64);
  unsigned int bufferSize = width * height * Pixel::GetBytesPerPixel(Pixel::RGBA8888);

  unsigned char* buffer    = reinterpret_cast<unsigned char*>(malloc(bufferSize));
  PixelData      pixelData = PixelData::New(buffer, bufferSize, width, height, Pixel::RGBA8888, PixelData::FREE);

  DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(pixelData, true).GetUrl().Size() > 0u);

  END_TEST;
}

int UtcDaliImageConvertNativeImageToUrl01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertNativeImageToUrl01");

  unsigned int width(64);
  unsigned int height(64);
  try
  {
    NativeImagePtr nativeImage = NativeImage::New(width, height, NativeImage::COLOR_DEPTH_DEFAULT);

    DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(nativeImage).GetUrl().Size() > 0u);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "Adaptor::IsAvailable()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliImageConvertNativeImageToUrl02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertNativeImageToUrl02");

  unsigned int width(64);
  unsigned int height(64);
  try
  {
    NativeImagePtr nativeImage = NativeImage::New(width, height, NativeImage::COLOR_DEPTH_DEFAULT);

    DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(nativeImage, true).GetUrl().Size() > 0u);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "Adaptor::IsAvailable()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliImageConvertNativeImageInterfaceToUrl01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertNativeImageInterfaceToUrl01");

  unsigned int width(64);
  unsigned int height(64);
  try
  {
    NativeImageQueuePtr nativeImageQueue = NativeImageQueue::New(width, height, (Dali::NativeImageQueue::ColorFormat::BGR888));

    DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(nativeImageQueue).GetUrl().Size() > 0u);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "Adaptor::IsAvailable()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliImageConvertNativeImageInterfaceToUrl02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertNativeImageInterfaceToUrl02");

  unsigned int width(64);
  unsigned int height(64);
  try
  {
    NativeImageQueuePtr nativeImageQueue = NativeImageQueue::New(width, height, (Dali::NativeImageQueue::ColorFormat::BGR888));

    DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(nativeImageQueue, true).GetUrl().Size() > 0u);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "Adaptor::IsAvailable()", TEST_LOCATION);
  }
  catch(...)
  {
    tet_printf("Assertion test failed - wrong Exception\n");
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliImageConvertEncodedImageBufferToUrl(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliImageConvertEncodedImageBufferToUrl");

  Dali::Vector<uint8_t> buffer;
  buffer.PushBack(0x11);
  buffer.PushBack(0x22);
  buffer.PushBack(0x33);

  DALI_TEST_CHECK(Dali::Toolkit::ImageUrlUtils::GenerateUrl(EncodedImageBuffer::New(buffer)).GetUrl().Size() > 0u);

  END_TEST;
}
