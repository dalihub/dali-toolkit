//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/public-api/controls/super-blur-view/super-blur-view.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const int BLUR_LEVELS = 3;
const int RENDER_FRAME_INTERVAL = 16;

static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
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

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

Image CreateSolidColorImage( ToolkitTestApplication& application, const Vector4& color, unsigned int width, unsigned int height )
{
  BitmapImage imageData = BitmapImage::New( width, height, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = imageData.GetBuffer();
  unsigned int size = width * height;

  for( size_t i = 0; i < size; i++ )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }
  imageData.Update();

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  return imageData;
}
}//namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliSuperBlurViewNew();
static void UtcDaliSuperBlurViewSetImage();
static void UtcDaliSuperBlurViewSetGetBlurStrength();
static void UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex();
static void UtcDaliSuperBlurViewGetBlurredImage();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliSuperBlurViewNew, POSITIVE_TC_IDX },
    { UtcDaliSuperBlurViewSetImage, POSITIVE_TC_IDX },
    { UtcDaliSuperBlurViewSetGetBlurStrength, POSITIVE_TC_IDX },
    { UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex, POSITIVE_TC_IDX },
    { UtcDaliSuperBlurViewGetBlurredImage, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliSuperBlurViewNew()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewNew ");

  // Test default constructor.
  SuperBlurView blurView;
  DALI_TEST_CHECK( !blurView );

  // Test object creation
  blurView = SuperBlurView::New( BLUR_LEVELS );
  DALI_TEST_CHECK( blurView );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  // Test copy constructor
  SuperBlurView blurViewCopy2( blurView );
  DALI_TEST_CHECK( blurViewCopy2 );

  // Test down cast
  Actor actorView;
  actorView = blurView;
  SuperBlurView downCastView = SuperBlurView::DownCast( actorView );
  DALI_TEST_CHECK( downCastView );
}

static void UtcDaliSuperBlurViewSetImage()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetImage ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  // create image actors for the original image and each blurred image
  DALI_TEST_CHECK( blurView.GetChildCount() == BLUR_LEVELS+1 );

  Image inputImage = CreateSolidColorImage( application, Color::GREEN, 50, 50 );
  blurView.SetImage( inputImage );
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() ==  BLUR_LEVELS*2 + 1);
}

static void UtcDaliSuperBlurViewSetGetBlurStrength()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetGetBlurStrength ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.f, TEST_LOCATION );

  blurView.SetBlurStrength( 0.65f );
  Wait(application);
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.65f, TEST_LOCATION );
}

static void UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  Property::Index blurPropertyIdx = blurView.GetBlurStrengthPropertyIndex();

  float blurStrength;
  (blurView.GetProperty( blurPropertyIdx )).Get(blurStrength);
  DALI_TEST_EQUALS(blurStrength, 0.f, TEST_LOCATION );

  blurView.SetBlurStrength( 0.65f );
  Wait(application);
  (blurView.GetProperty( blurPropertyIdx )).Get(blurStrength);
  DALI_TEST_EQUALS(blurStrength, 0.65f, TEST_LOCATION );
}

static void UtcDaliSuperBlurViewGetBlurredImage()
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliSuperBlurViewGetBlurredImage" );

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  blurView.SetSize( 100.f,100.f );
  Image inputImage = CreateSolidColorImage( application, Color::GREEN, 100, 100 );
  blurView.SetImage( inputImage );

  Wait(application, 200); // Make sure all the gaussian blur finished

  Image image1 = blurView.GetBlurredImage( 1 );
  DALI_TEST_CHECK( image1 );

  Image image2 = blurView.GetBlurredImage( 2 );
  DALI_TEST_CHECK( image2.GetWidth() == 25 );
  DALI_TEST_CHECK( image2.GetHeight() == 25 );

  Image image3 = blurView.GetBlurredImage( 3 );
  DALI_TEST_CHECK( FrameBufferImage::DownCast( image2 ) );

}
