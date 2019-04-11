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

#include <iostream>

#include <stdlib.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>

using namespace Dali::Toolkit::Internal;


void utc_dali_toolkit_texture_manager_startup(void)
{
  setenv( "LOG_TEXTURE_MANAGER", "3", 1 );
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_texture_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

class TestObserver : public Dali::Toolkit::TextureUploadObserver
{
public:
  TestObserver()
  : mLoaded(false),
    mObserverCalled(false)
  {
  }

  virtual void UploadComplete( bool loadSuccess, int32_t textureId, TextureSet textureSet,
                               bool useAtlasing, const Vector4& atlasRect, bool preMultiplied ) override
  {
    mLoaded = loadSuccess;
    mObserverCalled = true;
  }

  bool mLoaded;
  bool mObserverCalled;
};


int UtcTextureManagerRequestLoad(void)
{
  ToolkitTestApplication application;

  TextureManager textureManager; // Create new texture manager

  TestObserver observer;
  std::string filename("image.png");
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  TextureManager::TextureId textureId = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  VisualUrl url = textureManager.GetVisualUrl( textureId );

  DALI_TEST_EQUALS( url.GetUrl().compare( filename ), 0, TEST_LOCATION );

  END_TEST;
}

int UtcTextureManagerGenerateHash(void)
{
  ToolkitTestApplication application;

  TextureManager textureManager; // Create new texture manager

  TestObserver observer;
  std::string filename( "image.png" );
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  TextureManager::TextureId textureId = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::USE_ATLAS,
    &observer,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  VisualUrl url = textureManager.GetVisualUrl( textureId );

  DALI_TEST_EQUALS( url.GetUrl().compare( filename ), 0, TEST_LOCATION );

  END_TEST;
}
