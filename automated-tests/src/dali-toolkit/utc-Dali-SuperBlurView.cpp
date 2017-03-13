/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
const char* TEST_IMAGE_FILE_NAME("image.png");
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

void LoadBitmapResource(TestPlatformAbstraction& platform, int width, int height)
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, width, height, width, height);
}

class SignalHandler : public Dali::ConnectionTracker
{
public:
  SignalHandler() :
    mCalls( 0 )
  {
  }

  void Callback( SuperBlurView handle )
  {
    mCalls++;
    tet_infoline( "Signal called" );
  }

  unsigned int GetCalls() const
  {
    return mCalls;
  }

private:
  unsigned int mCalls;  ///< Keeps track of how many times the signal has been called.
};

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

int UtcDaliSuperBlurViewCreate(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewNew ");

  // Test default constructor.
  SuperBlurView blurView;
  DALI_TEST_CHECK( !blurView );

  // Test object creation
  TypeInfo type = TypeRegistry::Get().GetTypeInfo("SuperBlurView");
  if( type )
  {
    Dali::BaseHandle handle = type.CreateInstance();
    if( handle )
    {
      blurView = Dali::Toolkit::SuperBlurView::DownCast( handle );
    }
  }

  DALI_TEST_CHECK( blurView );

  END_TEST;
}


int UtcDaliSuperBlurViewSetImage(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetImage ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  blurView.SetSize( 100.f, 100.f );

  Image inputImage = CreateSolidColorImage( application, Color::GREEN, 50, 50 );
  blurView.SetImage( inputImage );
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() == 1+BLUR_LEVELS*2);

  {
    // create image renderers for the original image and each blurred image
    Stage::GetCurrent().Add( blurView );
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS+1, TEST_LOCATION );

    Wait(application);
    Stage::GetCurrent().Remove( blurView );
  }

  END_TEST;
}

int UtcDaliSuperBlurViewSetImage2(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliSuperBlurViewSetImage2 - test setting a second image ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  blurView.SetSize( 100.f, 100.f );

  tet_infoline("Call SetImage and add blurview to stage");
  Image inputImage = CreateSolidColorImage( application, Color::GREEN, 50, 50 );
  blurView.SetImage( inputImage );

  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() == 1+BLUR_LEVELS*2);
  {
    // create image renderers for the original image and each blurred image
    stage.Add( blurView );
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS+1, TEST_LOCATION );

    tet_infoline("Wait for a second to allow blur to finish");
    Wait(application, 1000);

    tet_infoline("Remove from stage");
    Stage::GetCurrent().Remove( blurView );
  }

  tet_infoline("Test that there are no render tasks remaining");
  DALI_TEST_EQUALS(blurView.GetRendererCount(), 0, TEST_LOCATION );

  tet_infoline("Call SetImage a second time and add blurview back to stage");
  Image inputImage2 = CreateSolidColorImage( application, Color::CYAN, 50, 50 );
  blurView.SetImage( inputImage2 );
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() == 1+BLUR_LEVELS*2);

  {
    // create image renderers for the original image and each blurred image
    Stage::GetCurrent().Add( blurView );
    Wait(application);
    DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS+1, TEST_LOCATION );

    tet_infoline("Wait for a second to allow blur to finish");
    Wait(application, 1000);

    tet_infoline("Remove from stage");
    Stage::GetCurrent().Remove( blurView );
  }

  tet_infoline("Test that there are no render tasks remaining");
  DALI_TEST_EQUALS(blurView.GetRendererCount(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliSuperBlurViewSetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetProperty ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  // create image renderers for the original image and each blurred image
  Stage::GetCurrent().Add( blurView );
  blurView.SetSize( 100.f, 100.f );

  tet_infoline(" Set property map. Set height and width large enough to avoid atlassing");
  int width(512);
  int height(513);
  LoadBitmapResource( application.GetPlatform(), width, height );

  Property::Map propertyMap;
  propertyMap["filename"] = TEST_IMAGE_FILE_NAME ;
  propertyMap["width"] = width;
  propertyMap["height"] = height;

  // Will create ResourceImage
  blurView.SetProperty(SuperBlurView::Property::IMAGE, propertyMap);
  Wait(application);

  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() == 1+BLUR_LEVELS*2);

  Wait(application);

  END_TEST;
}


int UtcDaliSuperBlurViewGetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSetProperty ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  blurView.SetSize( 100.f, 100.f );

  tet_infoline(" Set property map.");
  int width(512);
  int height(513); // Value large enough to avoid future atlassing
  LoadBitmapResource( application.GetPlatform(), width, height );

  Property::Map propertyMap;
  propertyMap["filename"] = TEST_IMAGE_FILE_NAME ;
  propertyMap["width"] = width;
  propertyMap["height"] = height;

  // Will create ResourceImage
  blurView.SetProperty(SuperBlurView::Property::IMAGE, propertyMap);
  Wait(application);

  // create image renderers for the original image and each blurred image
  Stage::GetCurrent().Add( blurView );

  Property::Value imageProperty = blurView.GetProperty(SuperBlurView::Property::IMAGE);
  Property::Map* map = imageProperty.GetMap();
  DALI_TEST_CHECK( map != NULL );
  if( map )
  {
    Property::Map& mapRef = *map;
    DALI_TEST_EQUALS( mapRef["filename"], TEST_IMAGE_FILE_NAME, TEST_LOCATION );
  }

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
  DALI_TEST_CHECK( FrameBufferImage::DownCast( image3 ) );

  END_TEST;
}

int UtcDaliSuperBlurViewBlurSignal(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliSuperBlurViewSignal ");

  SuperBlurView blurView = SuperBlurView::New( BLUR_LEVELS );
  blurView.SetSize( 100.f, 100.f );

  Image inputImage = CreateSolidColorImage( application, Color::GREEN, 50, 50 );
  blurView.SetImage( inputImage );
  // start multiple guassian blur call, each guassian blur creates two render tasks
  DALI_TEST_CHECK( Stage::GetCurrent().GetRenderTaskList().GetTaskCount() == 1+BLUR_LEVELS*2);

  SignalHandler signalHandler;
  blurView.BlurFinishedSignal().Connect(&signalHandler, &SignalHandler::Callback);

  // create image renderers for the original image and each blurred image
  Stage::GetCurrent().Add( blurView );
  Wait(application, 1000);

  DALI_TEST_EQUALS(blurView.GetRendererCount(), BLUR_LEVELS+1, TEST_LOCATION );
  //DALI_TEST_EQUALS(signalHandler.GetCalls(), 1, TEST_LOCATION);

  END_TEST;
}
