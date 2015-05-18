/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/super-blur-view/super-blur-view.h>

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
  BufferImage imageData = BufferImage::New( width, height, Pixel::RGBA8888 );

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


int UtcDaliSuperBlurViewNew(void)
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
  END_TEST;
}

int UtcDaliSuperBlurViewSetImage(void)
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
  END_TEST;
}

int UtcDaliSuperBlurViewSetGetBlurStrength(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetGetBlurStrength ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.f, TEST_LOCATION );

  blurView.SetBlurStrength( 0.65f );
  Wait(application);
  DALI_TEST_EQUALS(blurView.GetCurrentBlurStrength(), 0.65f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSuperBlurViewGetBlurStrengthPropertyIndex(void)
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
  END_TEST;
}

int UtcDaliSuperBlurViewGetBlurredImage(void)
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
  DALI_TEST_EQUALS( image2.GetWidth(), 25u, TEST_LOCATION );
  DALI_TEST_EQUALS( image2.GetHeight(), 25u, TEST_LOCATION );

  Image image3 = blurView.GetBlurredImage( 3 );
  DALI_TEST_CHECK( FrameBufferImage::DownCast( image2 ) );

  END_TEST;
}
