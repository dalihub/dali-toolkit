/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>

#include <dali-toolkit/internal/texture-manager/texture-async-loading-helper.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h> ///< For VisualFactory's member TextureManager.
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>

#include <test-encoded-image-buffer.h>

#if defined(ELDBUS_ENABLED)
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#endif

using namespace Dali::Toolkit::Internal;

void utc_dali_toolkit_texture_manager_startup(void)
{
  setenv("LOG_TEXTURE_MANAGER", "3", 1);
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
const char* TEST_IMAGE_FILE_NAME   = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
const char* TEST_IMAGE_2_FILE_NAME = TEST_RESOURCE_DIR "/icon-delete.png";
const char* TEST_IMAGE_3_FILE_NAME = TEST_RESOURCE_DIR "/icon-edit.png";
const char* TEST_IMAGE_4_FILE_NAME = TEST_RESOURCE_DIR "/application-icon-20.png";
const char* TEST_MASK_FILE_NAME    = TEST_RESOURCE_DIR "/mask.png";

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
  : mCompleteType(CompleteType::NOT_COMPLETED),
    mLoaded(false),
    mObserverCalled(false),
    mTextureSet()
  {
  }

  virtual void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override
  {
    if(textureInformation.returnType == TextureUploadObserver::ReturnType::TEXTURE)
    {
      mCompleteType = CompleteType::UPLOAD_COMPLETE;
    }
    else
    {
      mCompleteType = CompleteType::LOAD_COMPLETE;
    }
    mLoaded         = loadSuccess;
    mObserverCalled = true;
    mTextureSet     = textureInformation.textureSet;
  }

  CompleteType mCompleteType;
  bool         mLoaded;
  bool         mObserverCalled;
  TextureSet   mTextureSet;
};

class TestObserverRemoveAndGenerateUrl : public TestObserver
{
public:
  TestObserverRemoveAndGenerateUrl(TextureManager* textureManagerPtr)
  : TestObserver(),
    mTextureManagerPtr(textureManagerPtr)
  {
  }

  virtual void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override
  {
    if(textureInformation.returnType == TextureUploadObserver::ReturnType::TEXTURE)
    {
      mCompleteType = CompleteType::UPLOAD_COMPLETE;
    }
    else
    {
      mCompleteType = CompleteType::LOAD_COMPLETE;
    }
    mLoaded         = loadSuccess;
    mObserverCalled = true;
    mTextureSet     = textureInformation.textureSet;

    // Remove during LoadComplete
    mTextureManagerPtr->Remove(textureInformation.textureId, nullptr);

    // ...And generate string which using texture id.
    mGeneratedExternalUrl = mTextureManagerPtr->AddExternalTexture(mTextureSet);
  }

public:
  std::string mGeneratedExternalUrl;

protected:
  TextureManager* mTextureManagerPtr; // Keep the pointer of texture manager.
};

class TestObserverWithCustomFunction : public TestObserver
{
public:
  TestObserverWithCustomFunction()
  : TestObserver(),
    mSignals{},
    mData{nullptr},
    mKeepSignal{false}
  {
  }

  virtual void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override
  {
    if(textureInformation.returnType == TextureUploadObserver::ReturnType::TEXTURE)
    {
      mCompleteType = CompleteType::UPLOAD_COMPLETE;
    }
    else
    {
      mCompleteType = CompleteType::LOAD_COMPLETE;
    }
    mLoaded         = loadSuccess;
    mObserverCalled = true;
    mTextureSet     = textureInformation.textureSet;

    // Execute signals.
    for(size_t i = 0; i < mSignals.size(); i++)
    {
      mSignals[i](mData);
    }

    // Clear signals.
    if(!mKeepSignal)
    {
      mSignals.clear();
    }
  }

  void ConnectFunction(std::function<void(void*)> signal)
  {
    mSignals.push_back(signal);
  }

public:
  std::vector<std::function<void(void*)>> mSignals;
  void*                                   mData;
  bool                                    mKeepSignal;
};

} // namespace

int UtcTextureManagerRequestLoad(void)
{
  ToolkitTestApplication application;

  TextureManager textureManager; // Create new texture manager

  TestObserver              observer;
  std::string               filename("image.png");
  auto                      preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  TextureManager::TextureId textureId   = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  VisualUrl url = textureManager.GetVisualUrl(textureId);

  DALI_TEST_EQUALS(url.GetUrl().compare(filename), 0, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerGenerateHash(void)
{
  ToolkitTestApplication application;

  TextureManager textureManager; // Create new texture manager

  TestObserver              observer;
  std::string               filename("image.png");
  auto                      preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  TextureManager::TextureId textureId   = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  VisualUrl url = textureManager.GetVisualUrl(textureId);

  DALI_TEST_EQUALS(url.GetUrl().compare(filename), 0, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerEncodedImageBuffer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerEncodedImageBuffer");

  auto  visualFactory  = Toolkit::VisualFactory::Get();
  auto& textureManager = GetImplementation(visualFactory).GetTextureManager(); // Use VisualFactory's texture manager

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer1 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);
  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);

  std::string url1 = textureManager.AddEncodedImageBuffer(buffer1);
  std::string url2 = textureManager.AddEncodedImageBuffer(buffer1);
  std::string url3 = VisualUrl::CreateBufferUrl(""); ///< Impossible Buffer URL. for coverage

  // Check if same EncodedImageBuffer get same url
  DALI_TEST_CHECK(url1 == url2);
  // Reduce reference count
  textureManager.RemoveEncodedImageBuffer(url1);
  // Check whethere url1 still valid
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));

  url2 = textureManager.AddEncodedImageBuffer(buffer2);
  // Check if difference EncodedImageBuffer get difference url
  DALI_TEST_CHECK(url1 != url2);

  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  TestObserver observer1;
  textureManager.RequestLoad(
    url1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer1,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

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

  DALI_TEST_CHECK(pixelBuffer);
  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);

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

  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION);

  textureManager.RemoveEncodedImageBuffer(url1);
  textureManager.RemoveEncodedImageBuffer(url2);

  // Now url1 and url2 is invalid type. mLoaded will return false

  TestObserver observer3;
  textureManager.RequestLoad(
    url1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer3,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  // Load will be success because url1 is cached
  DALI_TEST_EQUALS(observer3.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer3.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer3.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  TestObserver observer4;
  textureManager.RequestLoad(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer4,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::FORCED,
    preMultiply);

  DALI_TEST_EQUALS(observer4.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer4.mObserverCalled, false, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Load will be failed becuase reloadpolicy is forced
  DALI_TEST_EQUALS(observer4.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer4.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer4.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

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
  DALI_TEST_CHECK(!pixelBuffer);
  DALI_TEST_EQUALS(observer5.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer5.mObserverCalled, false, TEST_LOCATION);

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

  DALI_TEST_EQUALS(observer6.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer6.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Load will be failed because url3 is invalid URL
  DALI_TEST_EQUALS(observer6.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer6.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer6.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerEncodedImageBufferReferenceCount(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerEncodedImageBuffer check reference count works well");

  auto  visualFactory  = Toolkit::VisualFactory::Get();
  auto& textureManager = GetImplementation(visualFactory).GetTextureManager(); // Use VisualFactory's texture manager

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer1 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);
  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(TEST_IMAGE_FILE_NAME);

  std::string url1 = textureManager.AddEncodedImageBuffer(buffer1);
  std::string url2 = textureManager.AddEncodedImageBuffer(buffer1);

  // Check if same EncodedImageBuffer get same url
  DALI_TEST_CHECK(url1 == url2);

  // Reduce reference count
  textureManager.RemoveEncodedImageBuffer(url1);
  // Check whethere url1 still valid
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));

  // Reduce reference count
  textureManager.RemoveEncodedImageBuffer(url1);
  // Check whethere url1 is not valid anymore
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

  // UseExternalTexture doesn't create new buffer.
  // So, reference count is still zero.
  textureManager.UseExternalResource(url1);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

  url1 = textureManager.AddEncodedImageBuffer(buffer1);

  url2 = textureManager.AddEncodedImageBuffer(buffer2);
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
    TextureManager::UseAtlas::NO_ATLAS,
    &observer1,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  // LoadPixelBuffer doen't use cache. url2 will not be cached
  TestObserver       observer2;
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
    url2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false, ///< synchronousLoading
    &observer2,
    true, ///< orientationCorrection
    preMultiply);

  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION);

  // Decrease each url's reference count.
  textureManager.RemoveEncodedImageBuffer(url1);
  textureManager.RemoveEncodedImageBuffer(url2);

  // url1 buffer is still have 1 reference count because it is cached.
  // But url2 not valid because it is not cached.
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(url1));
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url2));

  // Check url1 buffer have 1 reference count because it is cached.
  textureManager.RemoveEncodedImageBuffer(url1);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(url1));

  END_TEST;
}

int UtcTextureManagerCachingForDifferentLoadingType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerCachingForDifferentLoadingType");

  TextureManager textureManager; // Create new texture manager

  TestObserver observer1;
  std::string  filename(TEST_IMAGE_FILE_NAME);
  auto         preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer1,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  TestObserver       observer2;
  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    false,
    &observer2,
    true,
    preMultiply);

  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mCompleteType, TestObserver::CompleteType::LOAD_COMPLETE, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerUseInvalidMask(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerUseInvalidMask");

  TextureManager textureManager; // Create new texture manager

  TestObserver                       observer;
  std::string                        filename(TEST_IMAGE_FILE_NAME);
  std::string                        maskname("");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto                          textureId(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
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
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerUseInvalidMaskAndMaskLoadedFirst(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerUseInvalidMask when normal image loaded first, and mask image loaded first");
  tet_infoline("Try to check PostLoad works well");

  TextureManager textureManager; // Create new texture manager

  TestObserver                       observer;
  std::string                        filename(TEST_IMAGE_FILE_NAME);
  std::string                        maskname("invalid.png");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto                          textureId(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
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
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerUseInvalidMaskAndMaskLoadedLater(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerUseInvalidMask when normal image loaded first, and mask image loaded later");
  tet_infoline("Try to check CheckForWaitingTexture called");

  TextureManager textureManager; // Create new texture manager

  TestObserver                       observer;
  std::string                        filename(TEST_IMAGE_FILE_NAME);
  std::string                        maskname("invalid.png");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto                          textureId(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
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
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // CAPTION : HARD-CODING for coverage. If you are a good boy, Do not follow this code.
  {
    Dali::Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(
      filename,
      ImageDimensions(),
      FittingMode::SCALE_TO_FILL,
      SamplingMode::BOX_THEN_LINEAR,
      true, ///< synchronousLoading
      nullptr,
      true, ///< orientationCorrection
      preMultiply);

    std::vector<Devel::PixelBuffer> pixelBuffers;
    pixelBuffers.push_back(pixelBuffer);
    textureManager.AsyncLoadComplete(textureId, pixelBuffers);
    std::vector<Devel::PixelBuffer> maskBuffers;
    textureManager.AsyncLoadComplete(maskInfo->mAlphaMaskId, maskBuffers);
    textureManager.Remove(maskInfo->mAlphaMaskId, nullptr);
    textureManager.Remove(textureId, &observer);
  }

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerSynchronousLoadingFail(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerSynchronousLoadingFail");

  TextureManager textureManager; // Create new texture manager

  std::string                        maskname("");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  std::string                   filename("dummy");
  auto                          textureId(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 0.f, 0.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  // load image synchronously.
  TestObserver observer;
  TextureSet   textureSet = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    true, // synchronous loading.
    textureId,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(loadingStatus, false, TEST_LOCATION);
  DALI_TEST_CHECK(!textureSet);                                     // texture loading fail.
  DALI_TEST_CHECK(textureId == TextureManager::INVALID_TEXTURE_ID); // invalid texture id is returned.

  END_TEST;
}

int UtcTextureManagerCachingSynchronousLoading(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerCachingSynchronousLoading");

  TextureManager textureManager; // Create new texture manager

  std::string filename(TEST_IMAGE_FILE_NAME);

  std::string                        maskname("");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  Vector4                       atlasRect(0.f, 0.f, 0.f, 0.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  // load image synchronously.
  TestObserver observer;
  auto         textureId(TextureManager::INVALID_TEXTURE_ID);
  TextureSet   textureSet = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    true, // synchronous loading.
    textureId,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(loadingStatus, false, TEST_LOCATION);
  DALI_TEST_CHECK(textureSet); // texture is loaded.

  // observer isn't called in synchronous loading.
  DALI_TEST_EQUALS(observer.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, false, TEST_LOCATION);

  // load same image asynchronously.
  TestObserver asyncObserver;
  auto         asyncTextureId(TextureManager::INVALID_TEXTURE_ID);
  loadingStatus              = false;
  TextureSet asyncTextureSet = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    false, // asynchronous loading.
    asyncTextureId,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &asyncObserver,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(asyncTextureId, textureId, TEST_LOCATION); // texture is loaded.
  DALI_TEST_EQUALS(loadingStatus, false, TEST_LOCATION);
  DALI_TEST_CHECK(asyncTextureSet); // Cached texture.

  // observer is directly called because textureSet is retrieved by cache.
  DALI_TEST_EQUALS(asyncObserver.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver.mObserverCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerAsyncSyncAsync(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerAsyncSyncAsync");

  TextureManager textureManager; // Create new texture manager

  std::string filename(TEST_IMAGE_FILE_NAME);

  std::string                        maskname("");
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  Vector4                       atlasRect(0.f, 0.f, 0.f, 0.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          atlasingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  // load image asynchronously.
  TestObserver asyncObserver1;
  auto         asyncTextureId1(TextureManager::INVALID_TEXTURE_ID);
  bool         asyncLoadingStatus1 = false;
  TextureSet   asyncTextureSet1    = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    false, // asynchronous loading.
    asyncTextureId1,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    asyncLoadingStatus1,
    &asyncObserver1,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(asyncLoadingStatus1, true, TEST_LOCATION); // texture is loading now.
  DALI_TEST_CHECK(!asyncTextureSet1);                         // texture is not loaded yet.

  // observer is still not called.
  DALI_TEST_EQUALS(asyncObserver1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver1.mObserverCalled, false, TEST_LOCATION);

  // load same image synchronously just after asynchronous loading.
  TestObserver syncObserver;
  auto         textureId(TextureManager::INVALID_TEXTURE_ID);
  bool         syncLoadingStatus = false;
  TextureSet   syncTextureSet    = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    true, // synchronous loading.
    textureId,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    syncLoadingStatus,
    &syncObserver,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(asyncTextureId1, textureId, TEST_LOCATION); // texture is loaded.
  DALI_TEST_EQUALS(syncLoadingStatus, false, TEST_LOCATION);   // texture is loaded.
  DALI_TEST_CHECK(syncTextureSet);                             // texture is loaded.

  // syncObserver isn't called in synchronous loading.
  DALI_TEST_EQUALS(syncObserver.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(syncObserver.mObserverCalled, false, TEST_LOCATION);

  // asyncObserver1 is still not called too.
  DALI_TEST_EQUALS(asyncObserver1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver1.mObserverCalled, false, TEST_LOCATION);

  // load image asynchronously.
  TestObserver asyncObserver2;
  auto         asyncTextureId2(TextureManager::INVALID_TEXTURE_ID);
  bool         asyncLoadingStatus2 = false;
  TextureSet   asyncTextureSet2    = textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    false, // asynchronous loading.
    asyncTextureId2,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    asyncLoadingStatus2,
    &asyncObserver2,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(asyncLoadingStatus2, false, TEST_LOCATION); // texture is loaded by previous sync request
  DALI_TEST_CHECK(asyncTextureSet2);                           // texture is loaded
  Texture syncTexture   = syncTextureSet.GetTexture(0u);
  Texture asyncTexture2 = asyncTextureSet2.GetTexture(0u);
  DALI_TEST_CHECK(syncTexture);
  DALI_TEST_CHECK(asyncTexture2);
  DALI_TEST_CHECK(asyncTexture2 == syncTexture); // check loaded two texture is same.

  // observer is called synchronously because the texture is cached.
  DALI_TEST_EQUALS(asyncObserver2.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver2.mObserverCalled, true, TEST_LOCATION);

  asyncObserver2.mLoaded         = false;
  asyncObserver2.mObserverCalled = false;

  application.SendNotification();
  application.Render();

  // Requested asynchronous loading at first is finished now and async observer is called now.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver1.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver1.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_CHECK(asyncObserver1.mTextureSet);

  Texture observerTexture = asyncObserver1.mTextureSet.GetTexture(0u);
  DALI_TEST_CHECK(observerTexture == asyncTexture2); // check loaded two texture is same.

  // asyncObserver2 was already called so it isn't called here.
  DALI_TEST_EQUALS(asyncObserver2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncObserver2.mObserverCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerQueueRemoveDuringObserve(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerQueueRemoveDuringObserve");

  TextureManager textureManager; // Create new texture manager

  TestObserverRemoveAndGenerateUrl observer(&textureManager); // special observer for this UTC.

  std::string filename(TEST_IMAGE_FILE_NAME);
  auto        preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  TextureManager::TextureId textureId = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  tet_printf("loaded textureId is %d, generated url is %s\n", static_cast<int>(textureId), observer.mGeneratedExternalUrl.c_str());

  DALI_TEST_CHECK(static_cast<int>(textureId) != std::stoi(VisualUrl::GetLocation(observer.mGeneratedExternalUrl))); // Check we don't reuse textureId during observe

  // Decrease external texture reference count who observer created
  textureManager.RemoveExternalTexture(observer.mGeneratedExternalUrl);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcTextureManagerRemoveDuringApplyMasking(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerRemoveDuringApplyMasking");

  TextureManager textureManager; // Create new texture manager

  TestObserver observer1;
  TestObserver observer2;

  std::string                        filename(TEST_IMAGE_FILE_NAME);
  std::string                        maskname(TEST_MASK_FILE_NAME);
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto                          textureId1(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    synchronousLoading,
    textureId1,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer1,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Load image and mask image.
  // Now, LoadState become MASK_APPLYING
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  tet_printf("Current textureId1:%d's state become MASK_APPLYING\n", static_cast<int>(textureId1));

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  // Remove current textureId1. and request new texture again.
  textureManager.Remove(textureId1, &observer1);
  auto textureId2 = textureManager.RequestLoad(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    TextureManager::UseAtlas::NO_ATLAS,
    &observer2,
    true, ///< orientationCorrection
    TextureManager::ReloadPolicy::CACHED,
    preMultiply,
    synchronousLoading);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);

  tet_printf("textureId1:%d removed and textureId2:%d requested\n", static_cast<int>(textureId1), static_cast<int>(textureId2));

  // CAPTION : HARD-CODING.
  {
    std::vector<Devel::PixelBuffer> pixelBuffers;
    textureManager.AsyncLoadComplete(textureId2, pixelBuffers);
    textureManager.Remove(textureId2, &observer2);
  }

  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION); ///< Note that we call AsyncLoadComplete hardly with empty pixelbuffer.
  DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mCompleteType, TestObserver::CompleteType::UPLOAD_COMPLETE, TEST_LOCATION);

  END_TEST;
}

int UtcTextureManagerMaskCacheTest(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerMaskCacheTest");

  TextureManager textureManager; // Create new texture manager

  TestObserver observer1;
  TestObserver observer2;

  std::string                        filename(TEST_IMAGE_FILE_NAME);
  std::string                        filename2(TEST_IMAGE_2_FILE_NAME);
  std::string                        maskname(TEST_MASK_FILE_NAME);
  TextureManager::MaskingDataPointer maskInfo = nullptr;
  maskInfo.reset(new TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl       = maskname;
  maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo->mCropToMask         = true;
  maskInfo->mContentScaleFactor = 1.0f;

  TextureManager::MaskingDataPointer maskInfo2 = nullptr;
  maskInfo2.reset(new TextureManager::MaskingData());
  maskInfo2->mAlphaMaskUrl       = maskname;
  maskInfo2->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
  maskInfo2->mCropToMask         = true;
  maskInfo2->mContentScaleFactor = 1.0f;

  auto                          textureId1(TextureManager::INVALID_TEXTURE_ID);
  auto                          textureId2(TextureManager::INVALID_TEXTURE_ID);
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  textureManager.LoadTexture(
    filename,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo,
    synchronousLoading,
    textureId1,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer1,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Load image and mask image.
  // Now, LoadState become MASK_APPLYING
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  tet_printf("Current textureId1:%d's state become MASK_APPLYING\n", static_cast<int>(textureId1));

  textureManager.LoadTexture(
    filename2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo2,
    synchronousLoading,
    textureId2,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer2,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  application.SendNotification();
  application.Render();

  // Load image2 + image1 apply mask + image2 apply mask = total 3 event trigger required.
  // Note that we use cached mask image.
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);

  try
  {
    // Remove textureId1 first, and then remove textureId2. Check whether segfault occured.
    textureManager.Remove(textureId1, &observer1);
    textureManager.Remove(textureId2, &observer2);

    TestObserver observer3;
    maskInfo.reset(new TextureManager::MaskingData());
    maskInfo->mAlphaMaskUrl       = maskname;
    maskInfo->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
    maskInfo->mCropToMask         = true;
    maskInfo->mContentScaleFactor = 1.0f;

    textureManager.LoadTexture(
      filename,
      ImageDimensions(),
      FittingMode::SCALE_TO_FILL,
      SamplingMode::BOX_THEN_LINEAR,
      maskInfo,
      synchronousLoading,
      textureId1,
      atlasRect,
      atlasRectSize,
      atlasingStatus,
      loadingStatus,
      &observer3,
      atlasUploadObserver,
      atlasManager,
      true,
      TextureManager::ReloadPolicy::CACHED,
      preMultiply);

    DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);

    application.SendNotification();
    application.Render();

    // Load image and mask image.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);

    // Apply mask.
    DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

    DALI_TEST_EQUALS(observer3.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, true, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcTextureManagerRemoveDuringGPUMasking(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcTextureManagerRemoveDuringGPUMasking");
  tet_infoline("Request 3 different GPU masking image.");
  tet_infoline("Control to mask image load last. and then, check execute result.");

  TextureManager textureManager; // Create new texture manager

  TestObserverWithCustomFunction observer1;
  TestObserverWithCustomFunction observer2;
  TestObserver                   observer3;
  TestObserver                   observer4;

  std::string filename1(TEST_IMAGE_FILE_NAME);
  std::string filename2(TEST_IMAGE_2_FILE_NAME);
  std::string filename3(TEST_IMAGE_3_FILE_NAME);
  std::string filename4(TEST_IMAGE_4_FILE_NAME);

  auto textureId1(TextureManager::INVALID_TEXTURE_ID);
  auto textureId2(TextureManager::INVALID_TEXTURE_ID);
  auto textureId3(TextureManager::INVALID_TEXTURE_ID);
  auto textureId4(TextureManager::INVALID_TEXTURE_ID);

  std::string                        maskname(TEST_MASK_FILE_NAME);
  TextureManager::MaskingDataPointer maskInfo[3] = {nullptr, nullptr, nullptr};
  for(int i = 0; i < 3; i++)
  {
    maskInfo[i].reset(new TextureManager::MaskingData());
    maskInfo[i]->mAlphaMaskUrl       = maskname;
    maskInfo[i]->mAlphaMaskId        = TextureManager::INVALID_TEXTURE_ID;
    maskInfo[i]->mCropToMask         = true;
    maskInfo[i]->mPreappliedMasking  = false; // To make GPU masking
    maskInfo[i]->mContentScaleFactor = 1.0f;
  }
  Vector4                       atlasRect(0.f, 0.f, 1.f, 1.f);
  Dali::ImageDimensions         atlasRectSize(0, 0);
  bool                          synchronousLoading(false);
  bool                          atlasingStatus(false);
  bool                          loadingStatus(false);
  auto                          preMultiply         = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  ImageAtlasManagerPtr          atlasManager        = nullptr;
  Toolkit::AtlasUploadObserver* atlasUploadObserver = nullptr;

  // Request image 1, 2, 3 with GPU masking
  textureManager.LoadTexture(
    filename1,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo[0],
    synchronousLoading,
    textureId1,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer1,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  textureManager.LoadTexture(
    filename2,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo[1],
    synchronousLoading,
    textureId2,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer2,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  textureManager.LoadTexture(
    filename3,
    ImageDimensions(),
    FittingMode::SCALE_TO_FILL,
    SamplingMode::BOX_THEN_LINEAR,
    maskInfo[2],
    synchronousLoading,
    textureId3,
    atlasRect,
    atlasRectSize,
    atlasingStatus,
    loadingStatus,
    &observer3,
    atlasUploadObserver,
    atlasManager,
    true,
    TextureManager::ReloadPolicy::CACHED,
    preMultiply);

  DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer4.mLoaded, false, TEST_LOCATION);
  DALI_TEST_EQUALS(observer4.mObserverCalled, false, TEST_LOCATION);

  // Check we use cached mask image
  DALI_TEST_CHECK(maskInfo[0]->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_EQUALS(maskInfo[0]->mAlphaMaskId, maskInfo[1]->mAlphaMaskId, TEST_LOCATION);
  DALI_TEST_EQUALS(maskInfo[0]->mAlphaMaskId, maskInfo[2]->mAlphaMaskId, TEST_LOCATION);

  // Connect observer1 custom function
  struct CustomData1
  {
    TextureManager*           textureManagerPtr{nullptr};
    TextureManager::TextureId removeTextureId{TextureManager::INVALID_TEXTURE_ID};
    TestObserver*             removeTextureObserver{nullptr};
  };
  CustomData1 data1;
  data1.textureManagerPtr     = &textureManager;
  data1.removeTextureId       = textureId3;
  data1.removeTextureObserver = &observer3;

  observer1.mData = &data1;
  observer1.ConnectFunction(
    [](void* data) {
      DALI_TEST_CHECK(data);
      CustomData1 data1 = *(CustomData1*)data;

      DALI_TEST_CHECK(data1.textureManagerPtr);
      DALI_TEST_CHECK(data1.removeTextureId != TextureManager::INVALID_TEXTURE_ID);
      DALI_TEST_CHECK(data1.removeTextureObserver);

      // Remove textureId3
      data1.textureManagerPtr->Remove(data1.removeTextureId, data1.removeTextureObserver);
    });

  // Connect observer2 custom function
  struct CustomData2
  {
    TextureManager*            textureManagerPtr{nullptr};
    std::string                addTextureUrl{};
    TextureManager::TextureId* addTextureIdPtr{nullptr};
    TestObserver*              addTextureObserver{nullptr};
  };
  CustomData2 data2;
  data2.textureManagerPtr  = &textureManager;
  data2.addTextureUrl      = filename4;
  data2.addTextureIdPtr    = &textureId4;
  data2.addTextureObserver = &observer4;

  observer2.mData = &data2;
  observer2.ConnectFunction(
    [](void* data) {
      DALI_TEST_CHECK(data);
      CustomData2 data2 = *(CustomData2*)data;

      DALI_TEST_CHECK(data2.textureManagerPtr);
      DALI_TEST_CHECK(!data2.addTextureUrl.empty());
      DALI_TEST_CHECK(data2.addTextureIdPtr);
      DALI_TEST_CHECK(data2.addTextureObserver);

      auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

      // Load textureId4
      (*data2.addTextureIdPtr) = data2.textureManagerPtr->RequestLoad(
        data2.addTextureUrl,
        ImageDimensions(),
        FittingMode::SCALE_TO_FILL,
        SamplingMode::BOX_THEN_LINEAR,
        TextureManager::UseAtlas::NO_ATLAS,
        data2.addTextureObserver,
        true,
        TextureManager::ReloadPolicy::CACHED,
        preMultiply);
    });

  application.SendNotification();
  application.Render();

  tet_printf("Id info - mask : {%d}, 1 : {%d}, 2 : {%d}, 3 : {%d}, 4 : {%d}\n", static_cast<int>(maskInfo[0]->mAlphaMaskId), static_cast<int>(textureId1), static_cast<int>(textureId2), static_cast<int>(textureId3), static_cast<int>(textureId4));

  // CAPTION : HARD-CODING.
  {
    // Complete async load 1, 2, 3.
    std::vector<Devel::PixelBuffer> pixelBuffers;

    pixelBuffers.clear();
    pixelBuffers.push_back(Devel::PixelBuffer::New(1, 1, Pixel::Format::RGB888));
    textureManager.AsyncLoadComplete(textureId1, pixelBuffers);
    pixelBuffers.clear();
    pixelBuffers.push_back(Devel::PixelBuffer::New(1, 1, Pixel::Format::RGB888));
    textureManager.AsyncLoadComplete(textureId2, pixelBuffers);
    pixelBuffers.clear();
    pixelBuffers.push_back(Devel::PixelBuffer::New(1, 1, Pixel::Format::RGB888));
    textureManager.AsyncLoadComplete(textureId3, pixelBuffers);

    DALI_TEST_EQUALS(observer1.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer1.mObserverCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mObserverCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mObserverCalled, false, TEST_LOCATION);

    // Complete mask load.
    pixelBuffers.clear();
    pixelBuffers.push_back(Devel::PixelBuffer::New(1, 1, Pixel::Format::L8));
    textureManager.AsyncLoadComplete(maskInfo[0]->mAlphaMaskId, pixelBuffers);

    tet_printf("Id info after observer notify - mask : {%d}, 1 : {%d}, 2 : {%d}, 3 : {%d}, 4 : {%d}\n", static_cast<int>(maskInfo[0]->mAlphaMaskId), static_cast<int>(textureId1), static_cast<int>(textureId2), static_cast<int>(textureId3), static_cast<int>(textureId4));

    // Check observer 1 and 2 called, but 3 and 4 not called.
    DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mObserverCalled, false, TEST_LOCATION);

    // Check textureId4
    DALI_TEST_CHECK(textureId4 != TextureManager::INVALID_TEXTURE_ID);

    // Complete 4.
    pixelBuffers.clear();
    pixelBuffers.push_back(Devel::PixelBuffer::New(1, 1, Pixel::Format::RGB888));
    textureManager.AsyncLoadComplete(textureId4, pixelBuffers);

    DALI_TEST_EQUALS(observer1.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer1.mObserverCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer2.mObserverCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mLoaded, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer3.mObserverCalled, false, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mLoaded, true, TEST_LOCATION);
    DALI_TEST_EQUALS(observer4.mObserverCalled, true, TEST_LOCATION);
  }

  END_TEST;
}