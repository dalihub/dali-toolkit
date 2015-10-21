/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <unistd.h>
#include <dali/dali.h>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>
#include <dali/devel-api/images/atlas.h>
#include <dali-toolkit/devel-api/image-atlas/image-atlas.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

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

// this is image is not exist, for negative test
static const char* gImageNonExist = "non-exist.jpg";

const int RENDER_FRAME_INTERVAL = 16; ///< Duration of each frame in ms. (at approx 60FPS)

Rect<int> TextureCoordinateToPixelArea( const Vector4& textureCoordinate, float size )
{
  Vector4 temp = textureCoordinate * size;
  Rect<int> pixelArea;
  pixelArea.x = static_cast<int>( temp.x );
  pixelArea.y = static_cast<int>( temp.y );
  pixelArea.width = static_cast<int>( temp.z-temp.x+1.f );
  pixelArea.height = static_cast<int>( temp.w-temp.y+1.f );

  return pixelArea;
}

bool IsOverlap( Rect<int> rect1, Rect<int> rect2 )
{
 return rect1.x < rect2.x+rect2.width
     && rect2.x < rect1.x+rect1.width
     && rect1.y < rect2.y+rect2.height
     && rect2.y < rect1.y+rect1.height;
}

}

void dali_image_atlas_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_image_atlas_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageAtlasNew(void)
{
  ToolkitTestApplication application;

  // invoke default handle constructor
  ImageAtlas atlas;

  DALI_TEST_CHECK( !atlas );

  // initialise handle
  atlas = ImageAtlas::New( 32, 32 );

  DALI_TEST_CHECK( atlas );
  END_TEST;
}

int UtcDaliImageAtlasCopyConstructor(void)
{
  ToolkitTestApplication application;

  ImageAtlas atlas = ImageAtlas::New( 32, 32);
  ImageAtlas atlasCopy(atlas);

  DALI_TEST_EQUALS( (bool)atlasCopy, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageAtlasAssignmentOperator(void)
{
  ToolkitTestApplication application;

  ImageAtlas atlas = ImageAtlas::New( 32, 32 );

  ImageAtlas atlas2;
  DALI_TEST_EQUALS( (bool)atlas2, false, TEST_LOCATION );

  atlas2 = atlas;
  DALI_TEST_EQUALS( (bool)atlas2, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageAtlasGetAtlas(void)
{
  ToolkitTestApplication application;

  ImageAtlas atlas = ImageAtlas::New( 32, 32 );
  Image image = atlas.GetAtlas();

  // test the atlas created
  DALI_TEST_EQUALS( (bool)image, true, TEST_LOCATION );
  DALI_TEST_CHECK( image.GetHeight() == 32u );
  DALI_TEST_CHECK( image.GetWidth() == 32u );

  Atlas coreAtlas = Atlas::DownCast( image );
  DALI_TEST_EQUALS( (bool)coreAtlas, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageAtlasSetBrokenImage(void)
{
  ToolkitTestApplication application;
  unsigned int size = 200;
  ImageAtlas atlas = ImageAtlas::New( size, size );

  Vector4 textureRect;
  atlas.Upload( textureRect, gImageNonExist );
  DALI_TEST_EQUALS( textureRect, Vector4::ZERO, TEST_LOCATION );

  // Set broken image
  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize(Vector2( 34, 34));
  atlas.SetBrokenImage( gImage_34_RGBA );

  // the non-exit image will be replaced with the broken image
  platform.SetClosestImageSize(Vector2( 0, 0));
  atlas.Upload( textureRect, gImageNonExist );

  Rect<int> pixelArea = TextureCoordinateToPixelArea(textureRect, size);
  DALI_TEST_EQUALS( pixelArea.width, 34.f, TEST_LOCATION );
  DALI_TEST_EQUALS( pixelArea.height, 34.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageAtlasUploadP(void)
{
  ToolkitTestApplication application;
  unsigned int size = 200;
  ImageAtlas atlas = ImageAtlas::New( size, size );

  EventThreadCallback* eventTrigger = EventThreadCallback::Get();
  CallbackBase* callback = eventTrigger->GetCallback();

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Vector4 textureRect1;
  atlas.Upload( textureRect1, gImage_34_RGBA, ImageDimensions(34, 34) );
  Vector4 textureRect2;
  atlas.Upload( textureRect2, gImage_50_RGBA, ImageDimensions(50, 50) );
  Vector4 textureRect3;
  atlas.Upload( textureRect3, gImage_128_RGB, ImageDimensions(128, 128) );

  eventTrigger->WaitingForTrigger( 3 );// waiting until all three images are loaded

  CallbackBase::Execute( *callback );

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  callStack.Enable(false);

  Rect<int> pixelArea1 = TextureCoordinateToPixelArea(textureRect1, size);
  DALI_TEST_EQUALS( pixelArea1.width, 34, TEST_LOCATION );
  DALI_TEST_EQUALS( pixelArea1.height, 34, TEST_LOCATION );
  std::stringstream out;
  out<<pixelArea1.x<<", "<<pixelArea1.y<<", "<<pixelArea1.width<<", "<<pixelArea1.height;
  DALI_TEST_CHECK(  callStack.FindMethodAndParams("TexSubImage2D", out.str()) );

  Rect<int> pixelArea2 = TextureCoordinateToPixelArea(textureRect2, size);
  DALI_TEST_EQUALS( pixelArea2.width, 50, TEST_LOCATION );
  DALI_TEST_EQUALS( pixelArea2.height, 50, TEST_LOCATION );
  out.str("");
  out<<pixelArea2.x<<", "<<pixelArea2.y<<", "<<pixelArea2.width<<", "<<pixelArea2.height;
  DALI_TEST_CHECK(  callStack.FindMethodAndParams("TexSubImage2D", out.str()) );

  Rect<int> pixelArea3 = TextureCoordinateToPixelArea(textureRect3, size);
  DALI_TEST_EQUALS( pixelArea3.width, 128, TEST_LOCATION );
  DALI_TEST_EQUALS( pixelArea3.height, 128, TEST_LOCATION );
  out.str("");
  out<<pixelArea3.x<<", "<<pixelArea3.y<<", "<<pixelArea3.width<<", "<<pixelArea3.height;
  DALI_TEST_CHECK( callStack.FindMethodAndParams("TexSubImage2D", out.str()) );

  DALI_TEST_CHECK( ! IsOverlap(pixelArea1, pixelArea2) );
  DALI_TEST_CHECK( ! IsOverlap(pixelArea1, pixelArea3) );
  DALI_TEST_CHECK( ! IsOverlap(pixelArea2, pixelArea3) );

  END_TEST;
}

int UtcDaliImageAtlasRemove(void)
{
  TestApplication application;
  unsigned int size = 100;
  ImageAtlas atlas = ImageAtlas::New( size, size );
  Vector4 textureRect1;
  atlas.Upload( textureRect1, gImage_34_RGBA, ImageDimensions(34, 34) );

  atlas.Remove( textureRect1 );

  Vector4 textureRect2;
  atlas.Upload( textureRect2, gImage_50_RGBA, ImageDimensions(50, 50) );

  // one pixel gap
  Rect<int> pixelArea = TextureCoordinateToPixelArea(textureRect2, size);
  DALI_TEST_EQUALS( pixelArea.x, 0.f, TEST_LOCATION );
  DALI_TEST_EQUALS( pixelArea.y, 0.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageAtlasImageView(void)
{
  ToolkitTestApplication application;

  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  ImageView imageView1 = ImageView::New( gImage_34_RGBA, ImageDimensions(34, 34) );
  ImageView imageView2 = ImageView::New( gImage_50_RGBA, ImageDimensions(50, 50) );
  Stage::GetCurrent().Add( imageView1 );
  Stage::GetCurrent().Add( imageView2 );

  EventThreadCallback* eventTrigger = EventThreadCallback::Get();
  while( eventTrigger == NULL) // waiting uintil the ImageAtlas is created by ImageAtlasManager
  {
    usleep(10);
    eventTrigger = EventThreadCallback::Get();
  }
  CallbackBase* callback = eventTrigger->GetCallback();

  eventTrigger->WaitingForTrigger( 2 );// waiting until both images are loaded

  CallbackBase::Execute( *callback );

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  callStack.Enable(false);

  DALI_TEST_CHECK(  callStack.FindMethodAndParams("TexSubImage2D", "0, 0, 34, 34" ) );
  DALI_TEST_CHECK(  callStack.FindMethodAndParams("TexSubImage2D", "0, 34, 50, 50" ) );

  callStack.Reset();
  callStack.Enable(true);

  // remove the imageView2 from stage, the second image will also be removed from atlas
  // then the space on the atlas will be used by the third image added.
  Stage::GetCurrent().Remove( imageView2 );
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  ImageView imageView3 = ImageView::New( gImage_128_RGB, ImageDimensions(100, 100) );
  Stage::GetCurrent().Add( imageView3 );

  eventTrigger->WaitingForTrigger( 3 ); // waiting for the third image loaded
  CallbackBase::Execute( *callback );

  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  callStack.Enable(false);
  DALI_TEST_CHECK(  callStack.FindMethodAndParams("TexSubImage2D", "0, 34, 100, 100" ) );

  END_TEST;
}
