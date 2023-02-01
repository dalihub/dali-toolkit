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
 *
 */
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali/dali.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <stdlib.h>
#include <toolkit-event-thread-callback.h>
#include <unistd.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
// resolution: 34*34, pixel format: RGBA8888
static const char* gImage_34_RGBA = TEST_RESOURCE_DIR "/icon-edit.png";
// resolution: 50*50, pixel format: RGBA8888
static const char* gImage_50_RGBA = TEST_RESOURCE_DIR "/icon-delete.png";
// resolution: 128*128, pixel format: RGB888
static const char* gImage_128_RGB = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
// animated image
static const char* gImage_gif = TEST_RESOURCE_DIR "/canvas-none.gif";

// for testing the ImageLoadedSignal
class ImageLoadedSignalVerifier : public ConnectionTracker
{
public:
  ImageLoadedSignalVerifier()
  : mCount(0)
  {
  }

  virtual ~ImageLoadedSignalVerifier()
  {
  }

  void ImageLoaded(uint32_t id, PixelData pixelData)
  {
    if(pixelData)
    {
      mIDs.push_back(id);
      mPixelDataList.push_back(pixelData);
    }
    mCount++;
  }

  int LoadedImageCount()
  {
    return mCount;
  }

  bool Verify(uint32_t id, uint32_t width, uint32_t height)
  {
    int size = mIDs.size();
    for(int i = 0; i < size; i++)
    {
      if(mIDs[i] == id)
      {
        return mPixelDataList[i].GetWidth() == width && mPixelDataList[i].GetHeight() == height;
      }
    }

    return false;
  }

private:
  int mCount;

  std::vector<uint32_t>  mIDs;
  std::vector<PixelData> mPixelDataList;
};

} // anonymous namespace

void dali_async_image_loader_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_async_image_loader_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageAtlasNew01(void)
{
  ToolkitTestApplication application;

  //invoke default handle constructor
  AsyncImageLoader loader;

  DALI_TEST_CHECK(!loader);

  // initialise handle
  loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);

  END_TEST;
}

int UtcDaliAsyncImageLoaderCopyConstructor(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);

  AsyncImageLoader loaderCopy(loader);
  DALI_TEST_CHECK(loaderCopy);

  END_TEST;
}

int UtcDaliAsyncImageLoaderMoveConstructor(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);
  DALI_TEST_EQUALS(1, loader.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  AsyncImageLoader moved = std::move(loader);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!loader);

  END_TEST;
}

int UtcDaliAsyncImageLoaderAssignmentOperator(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);

  AsyncImageLoader loader2;
  DALI_TEST_CHECK(!loader2);

  loader2 = loader;
  DALI_TEST_CHECK(loader2);
  DALI_TEST_CHECK(loader == loader2); // the two handles are pointing to the same object.

  END_TEST;
}

int UtcDaliAsyncImageLoaderMoveAssignment(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();
  DALI_TEST_CHECK(loader);
  DALI_TEST_EQUALS(1, loader.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  AsyncImageLoader moved;
  moved = std::move(loader);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!loader);

  END_TEST;
}

int UtcDaliAsyncImageLoaderDownCastP(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader asyncImageLoader = AsyncImageLoader::New();
  BaseHandle       object(asyncImageLoader);

  AsyncImageLoader asyncImageLoader2 = AsyncImageLoader::DownCast(object);

  DALI_TEST_CHECK(asyncImageLoader2);

  END_TEST;
}

int UtcDaliAsyncImageLoaderDownCastN(void)
{
  ToolkitTestApplication application;

  BaseHandle       unInitializedObject;
  AsyncImageLoader asyncImageLoader = AsyncImageLoader::DownCast(unInitializedObject);

  DALI_TEST_CHECK(!asyncImageLoader);

  END_TEST;
}

int UtcDaliAsyncImageLoaderLoadAndLoadedSignal(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader          loader = AsyncImageLoader::New();
  ImageLoadedSignalVerifier loadedSignalVerifier;

  loader.ImageLoadedSignal().Connect(&loadedSignalVerifier, &ImageLoadedSignalVerifier::ImageLoaded);

  loader.Load(gImage_34_RGBA);                                                                                                             // Trigger 1
  uint32_t id02 = loader.Load(gImage_50_RGBA, ImageDimensions(25, 25));                                                                    // Trigger 2
  uint32_t id03 = loader.Load(gImage_128_RGB, ImageDimensions(100, 100), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, true); // Trigger 3

  // Try load animted image
  Dali::AnimatedImageLoading animatedImageLoading = Dali::AnimatedImageLoading::New(gImage_gif, true);
  DevelAsyncImageLoader::LoadAnimatedImage(loader, animatedImageLoading, 0, DevelAsyncImageLoader::PreMultiplyOnLoad::OFF); // Trigger 4

  // Try apply mask image
  Devel::PixelBuffer imageData = Devel::PixelBuffer::New(50, 50, Dali::Pixel::RGBA8888);
  Devel::PixelBuffer maskData  = Devel::PixelBuffer::New(50, 50, Dali::Pixel::RGBA8888);
  DevelAsyncImageLoader::ApplyMask(loader, imageData, maskData, 0.0f, false, DevelAsyncImageLoader::PreMultiplyOnLoad::OFF); // Trigger 5 - ApplyMask also uses AsyncTask

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(5), true, TEST_LOCATION); // Wait for 5 triggers

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(loadedSignalVerifier.LoadedImageCount() == 5);
  DALI_TEST_CHECK(loadedSignalVerifier.Verify(id02, 25, 25));
  DALI_TEST_CHECK(loadedSignalVerifier.Verify(id03, 100, 100));

  END_TEST;
}

// Note: This is not an ideal test, but we cannot guarantee we can call Cancel() before the image has finished loading.
int UtcDaliAsyncImageLoaderCancel01(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader          loader = AsyncImageLoader::New();
  ImageLoadedSignalVerifier loadedSignalVerifier;

  loader.ImageLoadedSignal().Connect(&loadedSignalVerifier, &ImageLoadedSignalVerifier::ImageLoaded);

  uint32_t id01 = loader.Load(gImage_34_RGBA, ImageDimensions(34, 34));
  uint32_t id02 = loader.Load(gImage_50_RGBA, ImageDimensions(25, 25));
  uint32_t id03 = loader.Load(gImage_128_RGB, ImageDimensions(100, 100), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, true);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(loadedSignalVerifier.LoadedImageCount() == 3);

  DALI_TEST_CHECK(!loader.Cancel(id03)); // Cannot cancel a task that is already implemeted

  DALI_TEST_CHECK(loadedSignalVerifier.Verify(id01, 34, 34));   // first image is loaded
  DALI_TEST_CHECK(loadedSignalVerifier.Verify(id02, 25, 25));   // second image is loaded
  DALI_TEST_CHECK(loadedSignalVerifier.Verify(id03, 100, 100)); // third image is loaded

  END_TEST;
}

int UtcDaliAsyncImageLoaderCancel02(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();
  uint32_t         id01   = loader.Load(gImage_34_RGBA, ImageDimensions(34, 34));
  DALI_TEST_CHECK(loader.Cancel(id01)); // Cancle a task

  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliAsyncImageLoaderCancelAll(void)
{
  ToolkitTestApplication application;

  AsyncImageLoader loader = AsyncImageLoader::New();

  // Test that it is safe to call CancelAll even there is no loading task requested.
  try
  {
    loader.CancelAll();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "AsyncImageLoader::LoadAll", TEST_LOCATION);
  }

  // Test that cancelling a non-existing loading task will return false
  uint32_t id = 1;
  DALI_TEST_CHECK(!(loader.Cancel(id)));

  uint32_t id01 = loader.Load(gImage_34_RGBA, ImageDimensions(34, 34));
  uint32_t id02 = loader.Load(gImage_50_RGBA, ImageDimensions(25, 25));
  uint32_t id03 = loader.Load(gImage_128_RGB, ImageDimensions(100, 100), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, true);
  loader.CancelAll();

  // Test that cancelling a non-existing loading task will return false
  DALI_TEST_CHECK(!(loader.Cancel(id01)));
  DALI_TEST_CHECK(!(loader.Cancel(id02)));
  DALI_TEST_CHECK(!(loader.Cancel(id03)));

  application.SendNotification();
  application.Render();

  END_TEST;
}
