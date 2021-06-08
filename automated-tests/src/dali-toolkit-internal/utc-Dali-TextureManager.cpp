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
#include <dali-toolkit/internal/visuals/visual-factory-impl.h> ///< For VisualFactory's member TextureManager.

#include <test-encoded-image-buffer.h>

#if defined(ELDBUS_ENABLED)
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#endif

using namespace Dali::Toolkit::Internal;

void utc_dali_toolkit_texture_manager_startup(void)
{
  setenv( "LOG_TEXTURE_MANAGER", "3", 1 );
  test_return_value = TET_UNDEF;
#if defined(ELDBUS_ENABLED)
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
#endif
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

int UtcTextureManagerEncodedImageBuffer(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcTextureManagerEncodedImageBuffer" );

  auto  visualFactory  = Toolkit::VisualFactory::Get();
  auto& textureManager = GetImplementation(visualFactory).GetTextureManager(); // Use VisualFactory's texture manager

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer1 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);
  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);

  std::string  url1 = textureManager.AddExternalTexture(buffer1);
  std::string  url2 = textureManager.AddExternalTexture(buffer1);
  std::string  url3 = VisualUrl::CreateBufferUrl(""); ///< Impossible Buffer URL. for coverage

  // Check if same EncodedImageBuffer get same url
  DALI_TEST_CHECK(url1 == url2);
  // Reduce reference count
  textureManager.RemoveExternalTexture(url1);
  // Check whethere url1 still valid
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));

  url2 = textureManager.AddExternalTexture(buffer2);
  // Check if difference EncodedImageBuffer get difference url
  DALI_TEST_CHECK(url1 != url2);

  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  TestObserver observer1;
  textureManager.RequestLoad(
    url1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer1,
    true, ///< orientationCorrection
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
  // Syncload
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    true, ///< synchronousLoading
    &observer2,
    true, ///< orientationCorrection
    preMultiply);

  DALI_TEST_CHECK( pixelBuffer );
  DALI_TEST_EQUALS( observer2.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer2.mObserverCalled, false, TEST_LOCATION );

  // Asyncload
  pixelBuffer = textureManager.LoadPixelBuffer(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false, ///< synchronousLoading
    &observer2,
    true, ///< orientationCorrection
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

  textureManager.RemoveExternalTexture(url1);
  textureManager.RemoveExternalTexture(url2);

  // Now url1 and url2 is invalid type. mLoaded will return false

  TestObserver observer3;
  textureManager.RequestLoad(
    url1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer3,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  // Load will be success because url1 is cached
  DALI_TEST_EQUALS( observer3.mLoaded, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer3.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer3.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION );

  TestObserver observer4;
  textureManager.RequestLoad(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer4,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::FORCED,
    preMultiply);

  DALI_TEST_EQUALS( observer4.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer4.mObserverCalled, false, TEST_LOCATION );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Load will be failed becuase reloadpolicy is forced
  DALI_TEST_EQUALS( observer4.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer4.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer4.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION );

  TestObserver observer5;
  pixelBuffer = textureManager.LoadPixelBuffer(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    true, ///< synchronousLoading
    &observer5,
    true, ///< orientationCorrection
    preMultiply);

  // Load will be faild because synchronousLoading doesn't use cached texture
  DALI_TEST_CHECK( !pixelBuffer );
  DALI_TEST_EQUALS( observer5.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer5.mObserverCalled, false, TEST_LOCATION );

  TestObserver observer6;
  pixelBuffer = textureManager.LoadPixelBuffer(
    url3,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false, ///< synchronousLoading
    &observer6,
    true, ///< orientationCorrection
    preMultiply);

  DALI_TEST_EQUALS( observer6.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer6.mObserverCalled, false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Load will be failed because url3 is invalid URL
  DALI_TEST_EQUALS( observer6.mLoaded, false, TEST_LOCATION );
  DALI_TEST_EQUALS( observer6.mObserverCalled, true, TEST_LOCATION );
  DALI_TEST_EQUALS( observer6.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION );

  END_TEST;
}

int UtcTextureManagerEncodedImageBufferReferenceCount(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcTextureManagerEncodedImageBuffer check reference count works well" );

  auto  visualFactory  = Toolkit::VisualFactory::Get();
  auto& textureManager = GetImplementation(visualFactory).GetTextureManager(); // Use VisualFactory's texture manager

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer1 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);
  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);

  std::string  url1    = textureManager.AddExternalTexture(buffer1);
  std::string  url2    = textureManager.AddExternalTexture(buffer1);

  // Check if same EncodedImageBuffer get same url
  DALI_TEST_CHECK(url1 == url2);

  // Reduce reference count
  textureManager.RemoveExternalTexture(url1);
  // Check whethere url1 still valid
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));

  // Reduce reference count
  textureManager.RemoveExternalTexture(url1);
  // Check whethere url1 is not valid anymore
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

  // UseExternalTexture doesn't create new buffer.
  // So, reference count is still zero.
  textureManager.UseExternalTexture(url1);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

  url1 = textureManager.AddExternalTexture(buffer1);
  // Check if difference EncodedImageBuffer get difference url
  // Previous EncodedImageBuffer was deleted, so we get new url even same buffer.
  DALI_TEST_CHECK(url1 != url2);

  url2 = textureManager.AddExternalTexture(buffer2);
  // Check if difference EncodedImageBuffer get difference url
  DALI_TEST_CHECK(url1 != url2);

  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  // url1 load image by cache
  TestObserver observer1;
  textureManager.RequestLoad(
    url1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::NO_ATLAS,
    &observer1,
    true, ///< orientationCorrection
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

  // LoadPixelBuffer doen't use cache. url2 will not be cached
  TestObserver observer2;
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false, ///< synchronousLoading
    &observer2, 
    true, ///< orientationCorrection
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

  // Decrease each url's reference count.
  textureManager.RemoveExternalTexture(url1);
  textureManager.RemoveExternalTexture(url2);
  
  // url1 buffer is still have 1 reference count because it is cached.
  // But url2 not valid because it is not cached.
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url2));

  // Check url1 buffer have 1 reference count because it is cached.
  textureManager.RemoveExternalTexture(url1);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

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
