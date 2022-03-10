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

#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_super_blur_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_super_blur_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const int          BLUR_LEVELS           = 3;
const int          RENDER_FRAME_INTERVAL = 16;
static const char* TEST_IMAGE_FILE_NAME  = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
static bool        gObjectCreatedCallBackCalled;
static void        TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= (duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

Texture CreateSolidColorTexture(ToolkitTestApplication& application, const Vector4& color, unsigned int width, unsigned int height)
{
  unsigned int size   = width * height;
  uint8_t*     pixbuf = new uint8_t[size * 4];

  for(size_t i = 0; i < size; i++)
  {
    pixbuf[i * 4 + 0] = 0xFF * color.r;
    pixbuf[i * 4 + 1] = 0xFF * color.g;
    pixbuf[i * 4 + 2] = 0xFF * color.b;
    pixbuf[i * 4 + 3] = 0xFF * color.a;
  }

  PixelData pixels = PixelData::New(pixbuf, size, width, height, Pixel::RGBA8888, PixelData::ReleaseFunction::DELETE_ARRAY);

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
  texture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());

  return texture;
}

class SignalHandler : public Dali::ConnectionTracker
{
public:
  SignalHandler()
  : mCalls(0)
  {
  }

  void Callback(SuperBlurView handle)
  {
    mCalls++;
    tet_infoline("Signal called");
  }

  unsigned int GetCalls() const
  {
    return mCalls;
  }

private:
  unsigned int mCalls; ///< Keeps track of how many times the signal has been called.
};

} //namespace

int UtcDaliSuperBlurViewNew(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewNew ");

  // Test default constructor.
  SuperBlurView blurView;
  DALI_TEST_CHECK(!blurView);

  // Test object creation
  blurView = SuperBlurView::New(BLUR_LEVELS);
  DALI_TEST_CHECK(blurView);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  }
  DALI_TEST_CHECK(gObjectCreatedCallBackCalled);

  // Test copy constructor
  SuperBlurView blurViewCopy2(blurView);
  DALI_TEST_CHECK(blurViewCopy2);

  // Test down cast
  Actor actorView;
  actorView                  = blurView;
  SuperBlurView downCastView = SuperBlurView::DownCast(actorView);
  DALI_TEST_CHECK(downCastView);
  END_TEST;
}

int UtcDaliSuperBlurViewCreate(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewNew ");

  // Test default constructor.
  SuperBlurView blurView;
  DALI_TEST_CHECK(!blurView);

  // Test object creation
  TypeInfo type = TypeRegistry::Get().GetTypeInfo("SuperBlurView");
  if(type)
  {
    Dali::BaseHandle handle = type.CreateInstance();
    if(handle)
    {
      blurView = Dali::Toolkit::SuperBlurView::DownCast(handle);
    }
  }

  DALI_TEST_CHECK(blurView);

  END_TEST;
}

int UtcDaliSuperBlurViewSetTexture(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetTexture ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  Texture inputTexture = CreateSolidColorTexture(application, Color::GREEN, 50, 50);
  blurView.SetTexture(inputTexture);
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK(application.GetScene().GetRenderTaskList().GetTaskCount() == 1 + BLUR_LEVELS * 2);

  {
    // create renderers for the original image and each blurred image
    application.GetScene().Add(blurView);
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS + 1, TEST_LOCATION);

    Wait(application);
    application.GetScene().Remove(blurView);
  }

  END_TEST;
}

int UtcDaliSuperBlurViewSetTexture2(void)
{
  ToolkitTestApplication application;
  Integration::Scene     stage = application.GetScene();

  tet_infoline(" UtcDaliSuperBlurViewSetTexture2 - test setting a second texture ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  tet_infoline("Call SetTexture and add blurview to stage");
  Texture inputTexture = CreateSolidColorTexture(application, Color::GREEN, 50, 50);
  blurView.SetTexture(inputTexture);

  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK(application.GetScene().GetRenderTaskList().GetTaskCount() == 1 + BLUR_LEVELS * 2);
  {
    // create renderers for the original image and each blurred image
    stage.Add(blurView);
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS + 1, TEST_LOCATION);

    tet_infoline("Wait for a second to allow blur to finish");
    Wait(application, 1000);

    tet_infoline("Remove from stage");
    application.GetScene().Remove(blurView);
  }

  tet_infoline("Test that there are no render tasks remaining");
  DALI_TEST_EQUALS(blurView.GetRendererCount(), 0, TEST_LOCATION);

  tet_infoline("Call SetTexture a second time and add blurview back to stage");
  Texture inputTexture2 = CreateSolidColorTexture(application, Color::CYAN, 50, 50);
  blurView.SetTexture(inputTexture2);
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK(application.GetScene().GetRenderTaskList().GetTaskCount() == 1 + BLUR_LEVELS * 2);

  {
    // create renderers for the original image and each blurred image
    application.GetScene().Add(blurView);
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS + 1, TEST_LOCATION);

    tet_infoline("Wait for a second to allow blur to finish");
    Wait(application, 1000);

    tet_infoline("Remove from stage");
    application.GetScene().Remove(blurView);
  }

  tet_infoline("Test that there are no render tasks remaining");
  DALI_TEST_EQUALS(blurView.GetRendererCount(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSuperBlurViewSetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetProperty ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  // create renderers for the original image and each blurred image
  application.GetScene().Add(blurView);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // Will create ResourceImage
  blurView.SetProperty(SuperBlurView::Property::IMAGE_URL, TEST_IMAGE_FILE_NAME);
  Wait(application);

  // start multiple guassian blur call, each guassian blur creates two render tasks

  unsigned int count = application.GetScene().GetRenderTaskList().GetTaskCount();
  DALI_TEST_CHECK(count == 1 + BLUR_LEVELS * 2);

  Wait(application);

  END_TEST;
}

int UtcDaliSuperBlurViewGetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetProperty ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  blurView.SetProperty(SuperBlurView::Property::IMAGE_URL, TEST_IMAGE_FILE_NAME);
  Wait(application);

  // create renderers for the original image and each blurred image
  application.GetScene().Add(blurView);

  std::string imageUrl = blurView.GetProperty<std::string>(SuperBlurView::Property::IMAGE_URL);
  DALI_TEST_EQUALS(imageUrl, TEST_IMAGE_FILE_NAME, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSuperBlurViewSetGetBlurStrength(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetGetBlurStrength ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.f, TEST_LOCATION);

  blurView.SetBlurStrength(0.65f);
  Wait(application);
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.65f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex ");

  SuperBlurView   blurView        = SuperBlurView::New(BLUR_LEVELS);
  Property::Index blurPropertyIdx = blurView.GetBlurStrengthPropertyIndex();

  float blurStrength;
  (blurView.GetProperty(blurPropertyIdx)).Get(blurStrength);
  DALI_TEST_EQUALS(blurStrength, 0.f, TEST_LOCATION);

  blurView.SetBlurStrength(0.65f);
  Wait(application);
  (blurView.GetProperty(blurPropertyIdx)).Get(blurStrength);
  DALI_TEST_EQUALS(blurStrength, 0.65f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSuperBlurViewGetBlurredTexture(void)
{
  ToolkitTestApplication application;

  tet_infoline("UtcDaliSuperBlurViewGetBlurredTexture");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  Texture inputTexture = CreateSolidColorTexture(application, Color::GREEN, 100, 100);
  blurView.SetTexture(inputTexture);

  Wait(application, 200); // Make sure all the gaussian blur finished

  Texture texture1 = blurView.GetBlurredTexture(1);
  DALI_TEST_CHECK(texture1);

  Texture texture2 = blurView.GetBlurredTexture(2);
  DALI_TEST_EQUALS(texture2.GetWidth(), 25u, TEST_LOCATION);
  DALI_TEST_EQUALS(texture2.GetHeight(), 25u, TEST_LOCATION);

  Texture texture3 = blurView.GetBlurredTexture(3);
  DALI_TEST_CHECK(texture3);

  END_TEST;
}

int UtcDaliSuperBlurViewBlurSignal(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSignal ");

  SuperBlurView blurView = SuperBlurView::New(BLUR_LEVELS);
  blurView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  Texture inputTexture = CreateSolidColorTexture(application, Color::GREEN, 50, 50);
  blurView.SetTexture(inputTexture);
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK(application.GetScene().GetRenderTaskList().GetTaskCount() == 1 + BLUR_LEVELS * 2);

  SignalHandler signalHandler;
  blurView.BlurFinishedSignal().Connect(&signalHandler, &SignalHandler::Callback);

  // create renderers for the original image and each blurred image
  application.GetScene().Add(blurView);
  Wait(application, 1000);

  DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS + 1, TEST_LOCATION);
  //DALI_TEST_EQUALS(signalHandler.GetCalls(), 1, TEST_LOCATION);

  END_TEST;
}
