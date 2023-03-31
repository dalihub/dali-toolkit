/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/public-api/image-loader/image-url.h>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcImageUrlConstructor01(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcImageUrlValid ");

  // Test default constructor.
  ImageUrl imageUrl;
  DALI_TEST_CHECK(!imageUrl);

  // Test object creation
  Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u); // test texture
  imageUrl      = ImageUrl::New(image);
  DALI_TEST_CHECK(imageUrl);

  // Test copy constructor
  ImageUrl ImageUrlCopy(imageUrl);
  DALI_TEST_CHECK(ImageUrlCopy);

  // Test down cast
  BaseHandle baseUrl;
  baseUrl              = imageUrl;
  ImageUrl downcastUrl = ImageUrl::DownCast(baseUrl);
  DALI_TEST_CHECK(downcastUrl);
  END_TEST;
}

int UtcImageUrlConstructor02(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcImageUrlValid ");

  // Test default constructor.
  ImageUrl imageUrl;
  DALI_TEST_CHECK(!imageUrl);

  // Test object creation
  Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u); // test texture
  imageUrl      = ImageUrl::New(image, true);
  DALI_TEST_CHECK(imageUrl);

  // Test copy constructor
  ImageUrl ImageUrlCopy(imageUrl);
  DALI_TEST_CHECK(ImageUrlCopy);

  // Test down cast
  BaseHandle baseUrl;
  baseUrl              = imageUrl;
  ImageUrl downcastUrl = ImageUrl::DownCast(baseUrl);
  DALI_TEST_CHECK(downcastUrl);
  END_TEST;
}
