/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>

#include <dali-toolkit/internal/visuals/svg/svg-loader.h>

#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/svg/svg-loader-observer.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h> ///< For VisualFactory's member SvgLoader.
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>

#include <test-encoded-image-buffer.h>

#if defined(ELDBUS_ENABLED)
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#endif

using namespace Dali::Toolkit::Internal;

void utc_dali_toolkit_internal_svg_loader_startup(void)
{
  setenv("LOG_SVG_LOADER", "3", 1);
  test_return_value = TET_UNDEF;
#if defined(ELDBUS_ENABLED)
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
#endif
}

void utc_dali_toolkit_internal_svg_loader_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_SVG_FILE_NAME                   = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_SVG_INVALID_RASTERIZE_FILE_NAME = TEST_RESOURCE_DIR "/invalid1.svg"; ///< Load succes but rasterize fail.

constexpr Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
constexpr float         DEFAULT_DPI(218.5f);

class TestObserver : public Dali::Toolkit::Internal::SvgLoaderObserver
{
public:
  TestObserver()
  : mLoadCalled(false),
    mLoadSuccess(false),
    mRasterizeCalled(false),
    mRasterizeSuccess(false),
    mVectorImageRenderer(),
    mTextureSet(),
    mAtlasRect(FULL_TEXTURE_RECT)
  {
  }

public: ///< Implement of SvgLoaderObserver
  void LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer) override
  {
    mLoadCalled          = true;
    mVectorImageRenderer = vectorImageRenderer;

    mLoadSuccess = !!mVectorImageRenderer;
  }

  void RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet, Vector4 atlasRect) override
  {
    mRasterizeCalled = true;
    mTextureSet      = textureSet;
    mAtlasRect       = atlasRect;

    mRasterizeSuccess = !!mTextureSet;
  }

public: /// Check for test
  void CheckTest(bool loadCalled, bool loadSuccess, bool rasterizeCalled, bool rasterizeSuccess, const char* location)
  {
    DALI_TEST_EQUALS(mLoadCalled, loadCalled, location);
    DALI_TEST_EQUALS(mLoadSuccess, loadSuccess, location);
    DALI_TEST_EQUALS(mRasterizeCalled, rasterizeCalled, location);
    DALI_TEST_EQUALS(mRasterizeSuccess, rasterizeSuccess, location);
  }
  void CheckLoadTest(bool loadCalled, bool loadSuccess, const char* location)
  {
    DALI_TEST_EQUALS(mLoadCalled, loadCalled, location);
    DALI_TEST_EQUALS(mLoadSuccess, loadSuccess, location);
  }
  void CheckRasterizeTest(bool rasterizeCalled, bool rasterizeSuccess, const char* location)
  {
    DALI_TEST_EQUALS(mRasterizeCalled, rasterizeCalled, location);
    DALI_TEST_EQUALS(mRasterizeSuccess, rasterizeSuccess, location);
  }

public:
  bool mLoadCalled;
  bool mLoadSuccess;
  bool mRasterizeCalled;
  bool mRasterizeSuccess;

  Dali::VectorImageRenderer mVectorImageRenderer;

  TextureSet mTextureSet;
  Vector4    mAtlasRect;
};

class TestObserverWithCustomFunction : public TestObserver
{
public:
  TestObserverWithCustomFunction()
  : TestObserver(),
    mLoadSignals{},
    mRasterizeSignals{},
    mLoadData{nullptr},
    mRasterizeData{nullptr},
    mKeepLoadSignal{false},
    mKeepRasterizeSignal{false}
  {
  }

public: ///< Implement of SvgLoaderObserver
  void LoadComplete(int32_t loadId, Dali::VectorImageRenderer vectorImageRenderer) override
  {
    TestObserver::LoadComplete(loadId, vectorImageRenderer);

    // Execute signals.
    for(size_t i = 0; i < mLoadSignals.size(); i++)
    {
      mLoadSignals[i](mLoadData);
    }

    // Clear signals.
    if(!mKeepLoadSignal)
    {
      mLoadSignals.clear();
    }
  }

  void RasterizeComplete(int32_t rasterizeId, Dali::TextureSet textureSet, Vector4 atlasRect) override
  {
    TestObserver::RasterizeComplete(rasterizeId, textureSet, atlasRect);

    // Execute signals.
    for(size_t i = 0; i < mRasterizeSignals.size(); i++)
    {
      mRasterizeSignals[i](mRasterizeData);
    }

    // Clear signals.
    if(!mKeepRasterizeSignal)
    {
      mRasterizeSignals.clear();
    }
  }

public:
  void ConnectLoadFunction(std::function<void(void*)> signal)
  {
    mLoadSignals.push_back(signal);
  }

  void ConnectRasterizeFunction(std::function<void(void*)> signal)
  {
    mRasterizeSignals.push_back(signal);
  }

public:
  std::vector<std::function<void(void*)>> mLoadSignals;
  std::vector<std::function<void(void*)>> mRasterizeSignals;
  void*                                   mLoadData;
  void*                                   mRasterizeData;
  bool                                    mKeepLoadSignal;
  bool                                    mKeepRasterizeSignal;
};

} // namespace

int UtcSvgLoaderBasicLoadAndRasterize(void)
{
  tet_infoline("Test various cases basic behavior\n");

  ToolkitTestApplication application;

  EncodedImageBuffer svgBuffer = Dali::ConvertFileToEncodedImageBuffer(TEST_SVG_FILE_NAME, EncodedImageBuffer::ImageType::VECTOR_IMAGE);

  auto       visualFactory = Toolkit::VisualFactory::Get();
  SvgLoader& svgLoader     = GetImplementation(visualFactory).GetSvgLoader(); // Use VisualFactory's svg loader, for use atlas and EncodedImageBuffer.

  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(svgBuffer);
  svgBuffer.Reset();

  const std::string fileNames[] = {
    TEST_SVG_FILE_NAME,
    TEST_SVG_INVALID_RASTERIZE_FILE_NAME,
    imageUrl.GetUrl(),
    "invalid.svg",
  };
  const int fileNamesCount = sizeof(fileNames) / sizeof(fileNames[0]);

  const std::vector<std::pair<uint32_t, uint32_t>> rasterizeSizes = {
    {0u, 0u},
    {100u, 100u},
    {600u, 600u}, ///< To big so atlas attempt failed.
  };
  const int rasterizeSizesCount = rasterizeSizes.size();

  for(int fileType = 0; fileType < fileNamesCount; ++fileType)
  {
    const bool loadSuccess      = (fileType == 0 || fileType == 1 || fileType == 2);
    const bool rasterizeSuccess = loadSuccess && (fileType == 0 || fileType == 2);
    for(int synchronousLoading = 0; synchronousLoading < 2; ++synchronousLoading)
    {
      for(int attemptAtlasing = 0; attemptAtlasing < 2; ++attemptAtlasing)
      {
        for(int sizeType = 0; sizeType < rasterizeSizesCount; ++sizeType)
        {
          const bool atlasAttempted = (attemptAtlasing == 1) && (sizeType == 0 || sizeType == 1);

          tet_printf("\n\nTesting fileType %d, synchronousLoading %d, attemptAtlasing %d, sizeType %d\n\n", fileType, synchronousLoading, attemptAtlasing, sizeType);

          TestObserver observer;
          std::string  filename(fileNames[fileType]);

          SvgLoader::SvgLoadId      loadId      = svgLoader.Load(filename, DEFAULT_DPI, &observer, synchronousLoading == 1);
          SvgLoader::SvgRasterizeId rasterizeId = svgLoader.Rasterize(loadId, rasterizeSizes[sizeType].first, rasterizeSizes[sizeType].second, attemptAtlasing == 1, &observer, synchronousLoading == 1);
          DALI_TEST_CHECK(loadId != SvgLoader::INVALID_SVG_LOAD_ID);
          DALI_TEST_CHECK(rasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID);

          if(synchronousLoading == 1)
          {
            observer.CheckTest(true, loadSuccess, true, rasterizeSuccess, TEST_LOCATION);
          }
          else
          {
            observer.CheckTest(false, false, false, false, TEST_LOCATION);

            // Wait async load complete
            DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

            observer.CheckTest(true, loadSuccess, false, false, TEST_LOCATION);

            if(loadSuccess)
            {
              // Wait async rasterize complete
              DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

              // TODO : We don't notify rasterize failed even if load failed. Should we notify it?
              observer.CheckTest(true, loadSuccess, true, rasterizeSuccess, TEST_LOCATION);
            }
          }

          DALI_TEST_EQUALS(!!observer.mVectorImageRenderer, loadSuccess, TEST_LOCATION);
          DALI_TEST_EQUALS(!!observer.mTextureSet, rasterizeSuccess, TEST_LOCATION);
          if(rasterizeSuccess)
          {
            if(atlasAttempted)
            {
              DALI_TEST_NOT_EQUALS(observer.mAtlasRect, FULL_TEXTURE_RECT, 0.01f, TEST_LOCATION);
            }
            else
            {
              DALI_TEST_EQUALS(observer.mAtlasRect, FULL_TEXTURE_RECT, TEST_LOCATION);
            }
          }

          // Remove cache
          svgLoader.RequestLoadRemove(loadId, &observer);
          svgLoader.RequestRasterizeRemove(rasterizeId, &observer, false);

          // Ensure svg loader cache removed.
          application.SendNotification();
          application.Render();
          application.SendNotification();
          application.Render();
        }
      }
    }
  }

  END_TEST;
}

int UtcSvgLoaderCacheLoadAndRasterize01(void)
{
  tet_infoline("Test Load and Rsterize cached well\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserver observer1;
  TestObserver observer2;
  TestObserver observer3;
  TestObserver observer4;
  TestObserver observer5;

  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);
  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer2, false);

  tet_printf("Test Load cached well\n");
  DALI_TEST_EQUALS(loadId1, loadId2, TEST_LOCATION);

  observer1.CheckLoadTest(false, false, TEST_LOCATION);
  observer2.CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Check both observer1 and observer2 loaded.
  observer1.CheckLoadTest(true, true, TEST_LOCATION);
  observer2.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Test difference url and dpi return not equal id\n");
  auto loadId3 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, &observer3, false);
  auto loadId4 = svgLoader.Load(std::string(TEST_SVG_INVALID_RASTERIZE_FILE_NAME), DEFAULT_DPI, &observer4, false);
  DALI_TEST_CHECK(loadId1 != loadId3);
  DALI_TEST_CHECK(loadId1 != loadId4);
  DALI_TEST_CHECK(loadId3 != loadId4);

  observer3.CheckLoadTest(false, false, TEST_LOCATION);
  observer4.CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 2 times : loadId3 loadId4
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  observer3.CheckLoadTest(true, true, TEST_LOCATION);
  observer4.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Test Load cached well even after load completed\n");
  auto loadId5 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer5, false);

  DALI_TEST_EQUALS(loadId1, loadId5, TEST_LOCATION);
  // Check observer5 loaded.
  observer5.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Request Rasterize\n");
  auto rasterizeId1 = svgLoader.Rasterize(loadId1, 100u, 100u, false, &observer1, false);
  auto rasterizeId2 = svgLoader.Rasterize(loadId1, 100u, 100u, false, &observer2, false);

  tet_printf("Test Rasterize cached well\n");
  DALI_TEST_EQUALS(rasterizeId1, rasterizeId2, TEST_LOCATION);

  observer1.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer2.CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Check both observer1 and observer2 rasterized.
  observer1.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer2.CheckRasterizeTest(true, true, TEST_LOCATION);

  tet_printf("Test difference loadId and size return not equal id\n");
  auto rasterizeId3 = svgLoader.Rasterize(loadId1, 200u, 200u, false, &observer3, false);
  auto rasterizeId4 = svgLoader.Rasterize(loadId3, 100u, 100u, false, &observer4, false);
  DALI_TEST_CHECK(rasterizeId1 != rasterizeId3);
  DALI_TEST_CHECK(rasterizeId1 != rasterizeId4);
  DALI_TEST_CHECK(rasterizeId3 != rasterizeId4);

  observer3.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer4.CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 2 times : rasterizeId3 and rasterizeId4
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  observer3.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer4.CheckRasterizeTest(true, true, TEST_LOCATION);

  tet_printf("Test Rasterize cached well even after rasterize completed\n");
  auto rasterizeId5 = svgLoader.Rasterize(loadId1, 100u, 100u, false, &observer5, false);

  DALI_TEST_EQUALS(rasterizeId1, rasterizeId5, TEST_LOCATION);
  // Check observer5 loaded.
  observer5.CheckRasterizeTest(true, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderCacheLoadAndRasterize02(void)
{
  tet_infoline("Test Load removed during rasterize\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserver observer1;
  TestObserver observer2;
  TestObserver observer3;

  tet_printf("load request for loadId1\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  observer1.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Request Rasterize\n");
  auto rasterizeId1 = svgLoader.Rasterize(loadId1, 100u, 100u, false, &observer1, false);

  tet_printf("Remove loadId1 during rasterize execute\n");
  svgLoader.RequestLoadRemove(loadId1, &observer1);

  application.SendNotification();
  application.Render();

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  observer1.CheckRasterizeTest(true, true, TEST_LOCATION);

  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer2, false);

  tet_printf("Test rasterize request increase the reference count of loadId1\n");

  DALI_TEST_EQUALS(loadId1, loadId2, TEST_LOCATION);
  // Check observer2 loaded.
  observer2.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Remove loadId2 and rasterizeId1 synchronously\n");
  svgLoader.RequestLoadRemove(loadId2, &observer2);

  application.SendNotification();
  application.Render();

  svgLoader.RequestRasterizeRemove(rasterizeId1, &observer1, true);

  tet_printf("Test loadId3 is not cached.\n");
  [[maybe_unused]] auto loadId3 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer3, false);
  observer3.CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId3
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);
  observer3.CheckLoadTest(true, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderCacheLoadAndRasterize03(void)
{
  tet_infoline("Test Load and Rsterize call synchronously during async cached call\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserver observer1;
  TestObserver observer2;
  TestObserver observer3;

  tet_printf("Load request async / sync / and async again\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);
  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer2, true);
  auto loadId3 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer3, false);

  tet_printf("Test Load cached well\n");
  DALI_TEST_EQUALS(loadId1, loadId2, TEST_LOCATION);
  DALI_TEST_EQUALS(loadId1, loadId3, TEST_LOCATION);
  DALI_TEST_EQUALS(loadId2, loadId3, TEST_LOCATION);

  tet_printf("Test async observer didn't notify. (Sync load didn't notify other observers)\n");
  observer1.CheckLoadTest(false, false, TEST_LOCATION);
  observer2.CheckLoadTest(true, true, TEST_LOCATION);
  observer3.CheckLoadTest(true, true, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Check all observers loaded well.
  observer1.CheckLoadTest(true, true, TEST_LOCATION);
  observer2.CheckLoadTest(true, true, TEST_LOCATION);
  observer3.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Request Rasterize async / sync / and async again\n");
  auto rasterizeId1 = svgLoader.Rasterize(loadId1, 100u, 100u, false, &observer1, false);
  auto rasterizeId2 = svgLoader.Rasterize(loadId2, 100u, 100u, false, &observer2, true);
  auto rasterizeId3 = svgLoader.Rasterize(loadId3, 100u, 100u, false, &observer3, false);

  tet_printf("Test Rasterize cached well\n");
  DALI_TEST_EQUALS(rasterizeId1, rasterizeId2, TEST_LOCATION);
  DALI_TEST_EQUALS(rasterizeId1, rasterizeId3, TEST_LOCATION);
  DALI_TEST_EQUALS(rasterizeId2, rasterizeId3, TEST_LOCATION);

  tet_printf("Test async observer didn't notify. (Sync load didn't notify other observers)\n");
  observer1.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer2.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer3.CheckRasterizeTest(true, true, TEST_LOCATION);

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  // Check all observers rasterized well.
  observer1.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer2.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer3.CheckRasterizeTest(true, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderLoadCancel(void)
{
  tet_infoline("Test Load cancel well\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserver observer1;
  TestObserver observer2;
  TestObserver observer3;

  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);

  observer1.CheckLoadTest(false, false, TEST_LOCATION);

  svgLoader.RequestLoadRemove(loadId1, &observer1);

  application.SendNotification();
  application.Render();

  // load task is not finished yet.

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  tet_printf("load request for loadId1 not notify\n");
  observer1.CheckLoadTest(false, false, TEST_LOCATION);

  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer2, false);

  observer2.CheckLoadTest(false, false, TEST_LOCATION);

  svgLoader.RequestLoadRemove(loadId2, &observer2);

  application.SendNotification();
  application.Render();

  // load task is not finished yet.
  // But during loading task running, request same item again
  auto loadId3 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer3, false);
  DALI_TEST_EQUALS(loadId2, loadId3, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  tet_printf("load request for loadId2 not notify, but loadId3 notify\n");
  observer2.CheckLoadTest(false, false, TEST_LOCATION);
  observer3.CheckLoadTest(true, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderDestructDuringObserver01(void)
{
  tet_infoline("Test destroy observer during load observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction* observer1 = new TestObserverWithCustomFunction();
  TestObserverWithCustomFunction* observer2 = new TestObserverWithCustomFunction();

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* other{nullptr};

    bool loadCalled{false};
    bool loadSuccess{false};
  } mData;

  mData.self  = observer1;
  mData.other = observer2;

  observer1->mLoadData = &mData;
  observer1->ConnectLoadFunction([](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->other;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    tet_printf("Destroy observer1 and observer2 (self)n");

    customData->loadCalled  = observer1->mLoadCalled;
    customData->loadSuccess = observer1->mLoadSuccess;

    delete observer1;
    delete observer2;
  });

  observer2->ConnectLoadFunction([](void* data) {
    tet_printf("observer2 Should be destroyed by observer1. Test failed\n");

    tet_result(TET_FAIL);
  });
  tet_printf("load request for loadId1 and loadId2. observer1 should be called first.\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer1, false);
  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer2, false);
  DALI_TEST_EQUALS(loadId1, loadId2, TEST_LOCATION);

  observer1->CheckLoadTest(false, false, TEST_LOCATION);
  observer2->CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(mData.loadCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(mData.loadSuccess, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderDestructDuringObserver02(void)
{
  tet_infoline("Test destroy observer during rasterize observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction* observer1 = new TestObserverWithCustomFunction();
  TestObserverWithCustomFunction* observer2 = new TestObserverWithCustomFunction();

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* other{nullptr};

    bool rasterizeCalled{false};
    bool rasterizeSuccess{false};
  } mData;

  mData.self  = observer1;
  mData.other = observer2;

  observer1->mRasterizeData = &mData;
  observer1->ConnectRasterizeFunction([](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->other;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    tet_printf("Destroy observer1(self) and observer2\n");

    customData->rasterizeCalled  = observer1->mRasterizeCalled;
    customData->rasterizeSuccess = observer1->mRasterizeSuccess;

    delete observer1;
    delete observer2;
  });

  observer2->ConnectRasterizeFunction([](void* data) {
    tet_printf("observer2 Should be destroyed by observer1. Test failed\n");

    tet_result(TET_FAIL);
  });
  tet_printf("load request for loadId1 and loadId2. observer1 should be called first.\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer1, false);
  auto loadId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer2, false);
  DALI_TEST_EQUALS(loadId1, loadId2, TEST_LOCATION);

  observer1->CheckLoadTest(false, false, TEST_LOCATION);
  observer2->CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  observer1->CheckLoadTest(true, true, TEST_LOCATION);
  observer2->CheckLoadTest(true, true, TEST_LOCATION);

  auto rasterizeId1 = svgLoader.Rasterize(loadId1, 100u, 100u, false, observer1, false);
  auto rasterizeId2 = svgLoader.Rasterize(loadId2, 100u, 100u, false, observer2, false);
  DALI_TEST_EQUALS(rasterizeId1, rasterizeId2, TEST_LOCATION);

  observer1->CheckRasterizeTest(false, false, TEST_LOCATION);
  observer2->CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  DALI_TEST_EQUALS(mData.rasterizeCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(mData.rasterizeSuccess, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderReqestDuringObserver01(void)
{
  tet_infoline("Test request load observer during load observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction observer1;
  TestObserver                   observer2;
  TestObserver                   observer3;
  TestObserver                   observer4;
  TestObserver                   observer5;
  TestObserver*                  observer6 = new TestObserver();

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* loadCached{nullptr};
    TestObserver* loadNonCached1{nullptr};
    TestObserver* loadNonCached2{nullptr};

    TestObserver* loadAndRemove{nullptr};
    TestObserver* loadAndDestruct{nullptr};

    SvgLoader::SvgLoadId cachedId{SvgLoader::INVALID_SVG_LOAD_ID};
    SvgLoader::SvgLoadId nonCachedId1{SvgLoader::INVALID_SVG_LOAD_ID};
    SvgLoader::SvgLoadId nonCachedId2{SvgLoader::INVALID_SVG_LOAD_ID};
  } mData;

  mData.self            = &observer1;
  mData.loadCached      = &observer2;
  mData.loadNonCached1  = &observer3;
  mData.loadNonCached2  = &observer4;
  mData.loadAndRemove   = &observer5;
  mData.loadAndDestruct = observer6;

  observer1.mLoadData = &mData;
  observer1.ConnectLoadFunction([&svgLoader](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->loadCached;
    TestObserver* observer3  = customData->loadNonCached1;
    TestObserver* observer4  = customData->loadNonCached2;
    TestObserver* observer5  = customData->loadAndRemove;
    TestObserver* observer6  = customData->loadAndDestruct;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    DALI_TEST_CHECK(observer3);
    DALI_TEST_CHECK(observer4);
    DALI_TEST_CHECK(observer5);
    DALI_TEST_CHECK(observer6);

    tet_printf("Request for observer2(cached) and observer3, observer4(non-cached)\n");
    customData->cachedId     = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer2, false);
    customData->nonCachedId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer3, false);
    customData->nonCachedId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer4, false);

    tet_printf("Test observer2 still not notify yet even if it is cached\n");
    observer2->CheckLoadTest(false, false, TEST_LOCATION);
    observer3->CheckLoadTest(false, false, TEST_LOCATION);
    observer4->CheckLoadTest(false, false, TEST_LOCATION);

    tet_printf("Test observer5 load request and cancel\n");
    auto loadId = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer5, false);
    svgLoader.RequestLoadRemove(loadId, observer5);

    tet_printf("Test observer6 load request and destruct\n");
    loadId = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer6, false);
    delete observer6;
  });

  tet_printf("load request for loadId1.\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);

  observer1.CheckLoadTest(false, false, TEST_LOCATION);
  observer2.CheckLoadTest(false, false, TEST_LOCATION);
  observer3.CheckLoadTest(false, false, TEST_LOCATION);
  observer4.CheckLoadTest(false, false, TEST_LOCATION);
  observer5.CheckLoadTest(false, false, TEST_LOCATION);
  observer6->CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  observer1.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Test observer2 notify after observer1 notify finished\n");
  DALI_TEST_EQUALS(loadId1, mData.cachedId, TEST_LOCATION);
  DALI_TEST_CHECK(loadId1 != mData.nonCachedId1);
  DALI_TEST_EQUALS(mData.nonCachedId1, mData.nonCachedId2, TEST_LOCATION);
  observer2.CheckLoadTest(true, true, TEST_LOCATION);
  observer3.CheckLoadTest(false, false, TEST_LOCATION);
  observer4.CheckLoadTest(false, false, TEST_LOCATION);
  observer5.CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : mData.nonCachedId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  tet_printf("Test observer5 not notify\n");
  observer3.CheckLoadTest(true, true, TEST_LOCATION);
  observer4.CheckLoadTest(true, true, TEST_LOCATION);
  observer5.CheckLoadTest(false, false, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderReqestDuringObserver02(void)
{
  tet_infoline("Test request load observer during load observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction observer1;
  TestObserver                   observer2;
  TestObserver                   observer3;
  TestObserver                   observer4;

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* loadCached{nullptr};
    TestObserver* loadNonCached1{nullptr};
    TestObserver* loadNonCached2{nullptr};

    SvgLoader::SvgLoadId cachedId{SvgLoader::INVALID_SVG_LOAD_ID};
    SvgLoader::SvgLoadId nonCachedId1{SvgLoader::INVALID_SVG_LOAD_ID};
    SvgLoader::SvgLoadId nonCachedId2{SvgLoader::INVALID_SVG_LOAD_ID};
  } mData;

  mData.self           = &observer1;
  mData.loadCached     = &observer2;
  mData.loadNonCached1 = &observer3;
  mData.loadNonCached2 = &observer4;

  observer1.mLoadData = &mData;
  observer1.ConnectLoadFunction([&svgLoader](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->loadCached;
    TestObserver* observer3  = customData->loadNonCached1;
    TestObserver* observer4  = customData->loadNonCached2;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    DALI_TEST_CHECK(observer3);
    DALI_TEST_CHECK(observer4);

    tet_printf("Request for observer2(cached) and observer3, observer4(non-cached)\n");
    tet_printf("For here, let we request observer4 as sync!\n");
    customData->cachedId     = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, observer2, false);
    customData->nonCachedId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer3, false);
    customData->nonCachedId2 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI + 2.0f, observer4, true);

    tet_printf("Test observer2 still not notify yet even if it is cached\n");
    observer2->CheckLoadTest(false, false, TEST_LOCATION);

    tet_printf("Test observer4 notify, but observer3 yet\n");
    observer3->CheckLoadTest(false, false, TEST_LOCATION);
    observer4->CheckLoadTest(true, true, TEST_LOCATION);
  });

  tet_printf("load request for loadId1.\n");
  auto loadId1 = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, &observer1, false);

  observer1.CheckLoadTest(false, false, TEST_LOCATION);
  observer2.CheckLoadTest(false, false, TEST_LOCATION);
  observer3.CheckLoadTest(false, false, TEST_LOCATION);
  observer4.CheckLoadTest(false, false, TEST_LOCATION);

  // Wait async load complete 1 time : loadId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  observer1.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Test observer2 notify after observer1 notify finished\n");
  DALI_TEST_EQUALS(loadId1, mData.cachedId, TEST_LOCATION);
  DALI_TEST_CHECK(loadId1 != mData.nonCachedId1);
  DALI_TEST_EQUALS(mData.nonCachedId1, mData.nonCachedId2, TEST_LOCATION);
  observer2.CheckLoadTest(true, true, TEST_LOCATION);

  tet_printf("Test observer3 notify due to we load it synchronously already\n");
  observer3.CheckLoadTest(true, true, TEST_LOCATION);
  observer4.CheckLoadTest(true, true, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderReqestDuringObserver03(void)
{
  tet_infoline("Test request rasterize observer during rasterize observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction observer1;
  TestObserver                   observer2;
  TestObserver                   observer3;
  TestObserver                   observer4;
  TestObserver                   observer5;
  TestObserver*                  observer6 = new TestObserver();

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* rasterizeCached{nullptr};
    TestObserver* rasterizeNonCached1{nullptr};
    TestObserver* rasterizeNonCached2{nullptr};

    TestObserver* rasterizeAndRemove{nullptr};
    TestObserver* rasterizeAndDestruct{nullptr};

    SvgLoader::SvgRasterizeId cachedId{SvgLoader::INVALID_SVG_RASTERIZE_ID};
    SvgLoader::SvgRasterizeId nonCachedId1{SvgLoader::INVALID_SVG_RASTERIZE_ID};
    SvgLoader::SvgRasterizeId nonCachedId2{SvgLoader::INVALID_SVG_RASTERIZE_ID};
  } mData;

  mData.self                 = &observer1;
  mData.rasterizeCached      = &observer2;
  mData.rasterizeNonCached1  = &observer3;
  mData.rasterizeNonCached2  = &observer4;
  mData.rasterizeAndRemove   = &observer5;
  mData.rasterizeAndDestruct = observer6;

  // Sync load and cache it.
  auto loadId = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, nullptr, true);

  observer1.mRasterizeData = &mData;
  observer1.ConnectRasterizeFunction([&svgLoader, &loadId](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->rasterizeCached;
    TestObserver* observer3  = customData->rasterizeNonCached1;
    TestObserver* observer4  = customData->rasterizeNonCached2;
    TestObserver* observer5  = customData->rasterizeAndRemove;
    TestObserver* observer6  = customData->rasterizeAndDestruct;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    DALI_TEST_CHECK(observer3);
    DALI_TEST_CHECK(observer4);
    DALI_TEST_CHECK(observer5);
    DALI_TEST_CHECK(observer6);

    tet_printf("Request for observer2(cached) and observer3, observer4(non-cached)\n");
    customData->cachedId     = svgLoader.Rasterize(loadId, 100u, 100u, false, observer2, false);
    customData->nonCachedId1 = svgLoader.Rasterize(loadId, 200u, 200u, false, observer3, false);
    customData->nonCachedId2 = svgLoader.Rasterize(loadId, 200u, 200u, false, observer4, false);

    tet_printf("Test observer2 still not notify yet even if it is cached\n");
    observer2->CheckRasterizeTest(false, false, TEST_LOCATION);
    observer3->CheckRasterizeTest(false, false, TEST_LOCATION);
    observer4->CheckRasterizeTest(false, false, TEST_LOCATION);

    tet_printf("Test observer5 rasterize request and cancel\n");
    auto rasterizeId = svgLoader.Rasterize(loadId, 200u, 200u, false, observer5, false);
    svgLoader.RequestRasterizeRemove(rasterizeId, observer5, true);

    tet_printf("Test observer6 rasterize request and destruct\n");
    rasterizeId = svgLoader.Rasterize(loadId, 200u, 200u, false, observer6, false);
    delete observer6;
  });

  tet_printf("rasterize request for rasterizeId1.\n");
  auto rasterizeId1 = svgLoader.Rasterize(loadId, 100u, 100u, false, &observer1, false);

  observer1.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer2.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer3.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer4.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer5.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer6->CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  observer1.CheckRasterizeTest(true, true, TEST_LOCATION);

  tet_printf("Test observer2 notify after observer1 notify finished\n");
  DALI_TEST_EQUALS(rasterizeId1, mData.cachedId, TEST_LOCATION);
  DALI_TEST_CHECK(rasterizeId1 != mData.nonCachedId1);
  DALI_TEST_EQUALS(mData.nonCachedId1, mData.nonCachedId2, TEST_LOCATION);
  observer2.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer3.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer4.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer5.CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 1 time : mData.nonCachedId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  tet_printf("Test observer5 not notify\n");
  observer3.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer4.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer5.CheckRasterizeTest(false, false, TEST_LOCATION);

  END_TEST;
}

int UtcSvgLoaderReqestDuringObserver04(void)
{
  tet_infoline("Test request rasterize observer during rasterize observer\n");

  ToolkitTestApplication application;

  SvgLoader svgLoader; ///Create svg loader without visual factory cache.

  TestObserverWithCustomFunction observer1;
  TestObserver                   observer2;
  TestObserver                   observer3;
  TestObserver                   observer4;

  struct CustomData
  {
    TestObserver* self{nullptr};
    TestObserver* rasterizeCached{nullptr};
    TestObserver* rasterizeNonCached1{nullptr};
    TestObserver* rasterizeNonCached2{nullptr};

    SvgLoader::SvgRasterizeId cachedId{SvgLoader::INVALID_SVG_RASTERIZE_ID};
    SvgLoader::SvgRasterizeId nonCachedId1{SvgLoader::INVALID_SVG_RASTERIZE_ID};
    SvgLoader::SvgRasterizeId nonCachedId2{SvgLoader::INVALID_SVG_RASTERIZE_ID};
  } mData;

  mData.self                = &observer1;
  mData.rasterizeCached     = &observer2;
  mData.rasterizeNonCached1 = &observer3;
  mData.rasterizeNonCached2 = &observer4;

  // Sync load and cache it.
  auto loadId = svgLoader.Load(std::string(TEST_SVG_FILE_NAME), DEFAULT_DPI, nullptr, true);

  observer1.mRasterizeData = &mData;
  observer1.ConnectRasterizeFunction([&svgLoader, &loadId](void* data) {
    DALI_TEST_CHECK(data);
    CustomData*   customData = static_cast<CustomData*>(data);
    TestObserver* observer1  = customData->self;
    TestObserver* observer2  = customData->rasterizeCached;
    TestObserver* observer3  = customData->rasterizeNonCached1;
    TestObserver* observer4  = customData->rasterizeNonCached2;
    DALI_TEST_CHECK(observer1);
    DALI_TEST_CHECK(observer2);
    DALI_TEST_CHECK(observer3);
    DALI_TEST_CHECK(observer4);

    tet_printf("Request for observer2(cached) and observer3, observer4(non-cached)\n");
    tet_printf("For here, let we request observer4 as sync!\n");
    customData->cachedId     = svgLoader.Rasterize(loadId, 100u, 100u, false, observer2, false);
    customData->nonCachedId1 = svgLoader.Rasterize(loadId, 200u, 200u, false, observer3, false);
    customData->nonCachedId2 = svgLoader.Rasterize(loadId, 200u, 200u, false, observer4, true);

    tet_printf("Test observer2 still not notify yet even if it is cached\n");
    observer2->CheckRasterizeTest(false, false, TEST_LOCATION);

    tet_printf("Test observer4 notify, but observer3 yet\n");
    observer3->CheckRasterizeTest(false, false, TEST_LOCATION);
    observer4->CheckRasterizeTest(true, true, TEST_LOCATION);
  });

  tet_printf("rasterize request for rasterizeId1.\n");
  auto rasterizeId1 = svgLoader.Rasterize(loadId, 100u, 100u, false, &observer1, false);

  observer1.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer2.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer3.CheckRasterizeTest(false, false, TEST_LOCATION);
  observer4.CheckRasterizeTest(false, false, TEST_LOCATION);

  // Wait async rasterize complete 1 time : rasterizeId1
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  observer1.CheckRasterizeTest(true, true, TEST_LOCATION);
  tet_printf("Test observer2 notify after observer1 notify finished\n");
  DALI_TEST_EQUALS(rasterizeId1, mData.cachedId, TEST_LOCATION);
  DALI_TEST_CHECK(rasterizeId1 != mData.nonCachedId1);
  DALI_TEST_EQUALS(mData.nonCachedId1, mData.nonCachedId2, TEST_LOCATION);
  observer2.CheckRasterizeTest(true, true, TEST_LOCATION);

  tet_printf("Test observer3 notify due to we load it synchronously already\n");
  observer3.CheckRasterizeTest(true, true, TEST_LOCATION);
  observer4.CheckRasterizeTest(true, true, TEST_LOCATION);

  END_TEST;
}