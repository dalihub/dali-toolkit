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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
} // namespace

void dali_texture_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_texture_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextureManagerAddRemoveP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextureManager");

  std::string url;
  std::string url2;
  std::string url3;
  std::string url4;
  std::string url5;
  std::string url6;
  // scope to ensure texturesets are kept alive by texture manager
  {
    auto texture = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, 88, 99);
    url          = TextureManager::AddTexture(texture);
    DALI_TEST_CHECK(url.size() > 0u);

    auto textureSet = TextureSet::New();
    textureSet.SetTexture(0u, texture);
    url2 = TextureManager::AddTexture(textureSet);
    DALI_TEST_CHECK(url2.size() > 0u);
    DALI_TEST_CHECK(url2 != url);

    // add same texture again, should give new Url
    url3 = TextureManager::AddTexture(texture);
    DALI_TEST_CHECK(url3.size() > 0u);
    DALI_TEST_CHECK(url3 != url);
    DALI_TEST_CHECK(url3 != url2);

    textureSet = TextureSet::New();
    url4       = TextureManager::AddTexture(textureSet);
    DALI_TEST_CHECK(url4.size() > 0u);
    DALI_TEST_CHECK(url4 != url);
    DALI_TEST_CHECK(url4 != url2);
    DALI_TEST_CHECK(url4 != url3);

    // add same texture again with preMultiplied, should give new Url
    url5 = TextureManager::AddTexture(texture, true);
    DALI_TEST_CHECK(url5.size() > 0u);
    DALI_TEST_CHECK(url5 != url);
    DALI_TEST_CHECK(url5 != url2);
    DALI_TEST_CHECK(url5 != url3);
    DALI_TEST_CHECK(url5 != url4);

    textureSet = TextureSet::New();
    url6       = TextureManager::AddTexture(textureSet, true);
    DALI_TEST_CHECK(url6.size() > 0u);
    DALI_TEST_CHECK(url6 != url);
    DALI_TEST_CHECK(url6 != url2);
    DALI_TEST_CHECK(url6 != url3);
    DALI_TEST_CHECK(url6 != url4);
    DALI_TEST_CHECK(url6 != url5);
  }

  auto textureSet = TextureManager::RemoveTexture(url);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  auto texture = textureSet.GetTexture(0u);
  DALI_TEST_EQUAL(texture.GetWidth(), 88u);
  DALI_TEST_EQUAL(texture.GetHeight(), 99u);
  textureSet = TextureManager::RemoveTexture(url);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  textureSet = TextureManager::RemoveTexture(url2);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  texture = textureSet.GetTexture(0u);
  DALI_TEST_EQUAL(texture.GetWidth(), 88u);
  DALI_TEST_EQUAL(texture.GetHeight(), 99u);
  textureSet = TextureManager::RemoveTexture(url2);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  textureSet = TextureManager::RemoveTexture(url3);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  texture = textureSet.GetTexture(0u);
  DALI_TEST_EQUAL(texture.GetWidth(), 88u);
  DALI_TEST_EQUAL(texture.GetHeight(), 99u);
  textureSet = TextureManager::RemoveTexture(url3);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  textureSet = TextureManager::RemoveTexture(url4);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  textureSet = TextureManager::RemoveTexture(url4);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  textureSet = TextureManager::RemoveTexture(url5);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  textureSet = TextureManager::RemoveTexture(url5);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  textureSet = TextureManager::RemoveTexture(url6);
  DALI_TEST_CHECK(textureSet && "Texture needs to be non empty handle");
  textureSet = TextureManager::RemoveTexture(url6);
  DALI_TEST_CHECK(!textureSet && "Texture needs to be removed from texture manager");

  END_TEST;
}

int UtcDaliTextureManagerAddN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextureManager");

  // empty texture is ok, though pointless from app point of view
  TextureSet  empty;
  std::string url = TextureManager::AddTexture(empty);
  DALI_TEST_CHECK(url.size() > 0u);

  END_TEST;
}

int UtcDaliTextureManagerRemoveN(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextureManager");

  // removing empty texture returns empty handle
  auto texture = TextureManager::RemoveTexture("");
  DALI_TEST_CHECK(!texture && "Texture should not be found");

  // removing empty texture returns empty handle
  texture = TextureManager::RemoveTexture("dali://");
  DALI_TEST_CHECK(!texture && "Texture should not be found");

  // empty texture is ok, though pointless from app point of view
  TextureSet  empty;
  std::string url = TextureManager::AddTexture(empty);
  DALI_TEST_CHECK(url.size() > 0u);
  // removing texture with wrong URL returns empty handle
  texture = TextureManager::RemoveTexture("dali://");
  DALI_TEST_CHECK(!texture && "Texture should not be found");

  // removing ftp texture returns empty handle
  texture = TextureManager::RemoveTexture("ftp://foobar");
  DALI_TEST_CHECK(!texture && "Texture should not be found");

  // add a texture
  url     = TextureManager::AddTexture(texture);
  texture = TextureManager::RemoveTexture(url + "foo");
  DALI_TEST_CHECK(!texture && "Texture should not be found");

  END_TEST;
}
