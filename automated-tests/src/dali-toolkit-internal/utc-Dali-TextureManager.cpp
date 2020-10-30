/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <toolkit-timer.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

using namespace Dali::Toolkit::Internal;

void utc_dali_toolkit_texture_manager_startup(void)
{
  setenv( "LOG_TEXTURE_MANAGER", "3", 1 );
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_toolkit_texture_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* TEST_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/gallery-small-1.jpg";

}

class TestObserver : public Dali::Toolkit::TextureUploadObserver
{
public:
  enum class CompleteType
  {
    NOT_COMPLETED = 0,
    UPLOAD_COMPLETE,
    LOAD_COMPLETE
  };

public:
  TestObserver()
  : mCompleteType( CompleteType::NOT_COMPLETED ),
    mLoaded(false),
    mObserverCalled(false)
  {
  }

  virtual void UploadComplete( bool loadSuccess, int32_t textureId, TextureSet textureSet,
                               bool useAtlasing, const Vector4& atlasRect, bool preMultiplied ) override
  {
    mCompleteType = CompleteType::UPLOAD_COMPLETE;
    mLoaded = loadSuccess;
    mObserverCalled = true;
  }

  virtual void LoadComplete( bool loadSuccess, Devel::PixelBuffer pixelBuffer, const VisualUrl& url, bool preMultiplied ) override
  {
    mCompleteType = CompleteType::LOAD_COMPLETE;
    mLoaded = loadSuccess;
    mObserverCalled = true;
  }

  CompleteType mCompleteType;
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

int UtcTextureManagerCachingForDifferentLoadingType(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcTextureManagerCachingForDifferentLoadingType" );

  TextureManager textureManager; // Create new texture manager

  TestObserver observer1;
  std::string filename( TEST_IMAGE_FILE_NAME );
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer1,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS( observer1.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer1.mObserverCalled, false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( observer1.mLoaded, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer1.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer1.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION );

  TestObserver observer2;
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false,
    &observer2,
    true,
    preMultiply);

  DALI_TEST_EQUALS( observer2.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer2.mObserverCalled, false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( observer2.mLoaded, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer2.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer2.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION );

  END_TEST;
}

int UtcTextureManagerUseInvalidMask(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcTextureManagerUseInvalidMask" );

  TextureManager textureManager; // Create new texture manager

  TestObserver observer;
  std::string filename( TEST_IMAGE_FILE_NAME );
  std::string maskname("");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl = maskname;
  maskInfo->mAlphaMaskId = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto textureId( TextureManager::INVALID_TEXTURE_ID );
  Vector4 atlasRect( 0.f, 0.f, 1.f, 1.f );
  Dali::ImageDimensions atlasRectSize( 0,0 );
  bool synchronousLoading(false);
  bool atlasingStatus(false);
  bool loadingStatus(false);
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr atlasManager = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    synchronousLoading,
    textureId,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    WrapMode::DEFAULT,
    WrapMode::DEFAULT,
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply
  );

  DALI_TEST_EQUALS( observer.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer.mObserverCalled, false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( observer.mLoaded, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION );

  END_TEST;
}
