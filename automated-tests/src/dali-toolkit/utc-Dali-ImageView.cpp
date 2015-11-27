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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/scripting/scripting.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_image_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_image_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
const char* TEST_IMAGE_FILE_NAME2 =  "gallery_image_02.jpg";

void TestImage( ImageView imageView, BufferImage image )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  Property::Map map;
  DALI_TEST_CHECK( value.Get( map ) );

  DALI_TEST_CHECK( map.Find( "width" ) );
  DALI_TEST_CHECK( map.Find( "height" ) );
  DALI_TEST_CHECK( map.Find( "type" ) );

  int width = 0;
  DALI_TEST_CHECK( map[ "width" ].Get( width ) );
  DALI_TEST_EQUALS( (unsigned int)width, image.GetWidth(), TEST_LOCATION );

  int height = 0;
  DALI_TEST_CHECK( map[ "height" ].Get( height ) );
  DALI_TEST_EQUALS( (unsigned int)height, image.GetHeight(), TEST_LOCATION );

  std::string type;
  DALI_TEST_CHECK( map[ "type" ].Get( type ) );
  DALI_TEST_EQUALS( type, "BufferImage", TEST_LOCATION );
}

void TestImage( ImageView imageView, ResourceImage image )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  Property::Map map;
  DALI_TEST_CHECK( value.Get( map ) );

  if( map.Find( "width" ) )
  {
    int width = 0;
    DALI_TEST_CHECK( map[ "width" ].Get( width ) );
    DALI_TEST_EQUALS( (unsigned int)width, image.GetWidth(), TEST_LOCATION );
  }

  if( map.Find( "height" ) )
  {
    int height = 0;
    DALI_TEST_CHECK( map[ "height" ].Get( height ) );
    DALI_TEST_EQUALS( (unsigned int)height, image.GetHeight(), TEST_LOCATION );
  }

  DALI_TEST_CHECK( map.Find( "type" ) );

  std::string type;
  DALI_TEST_CHECK( map[ "type" ].Get( type ) );
  DALI_TEST_EQUALS( type, "ResourceImage", TEST_LOCATION );

  std::string filename;
  DALI_TEST_CHECK( map[ "filename" ].Get( filename ) );
  DALI_TEST_EQUALS( filename, image.GetUrl(), TEST_LOCATION );
}

void TestUrl( ImageView imageView, const std::string url )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  std::string urlActual;
  DALI_TEST_CHECK( value.Get( urlActual ) );
  DALI_TEST_EQUALS( urlActual, url, TEST_LOCATION );
}

} // namespace

int UtcDaliImageViewNewP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New();

  DALI_TEST_CHECK( imageView );

  END_TEST;
}

int UtcDaliImageViewNewImageP(void)
{
  TestApplication application;

  BufferImage image = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New( image );

  DALI_TEST_CHECK( imageView );
  TestImage( imageView, image );

  END_TEST;
}

int UtcDaliImageViewNewUrlP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( imageView );

  TestUrl( imageView, TEST_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliImageViewConstructorP(void)
{
  TestApplication application;

  ImageView imageView;

  DALI_TEST_CHECK( !imageView );

  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );

  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );
  DALI_TEST_EQUALS( imageView, copy, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewDownCastP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New();

  BaseHandle object(imageView);

  ImageView imageView2 = ImageView::DownCast( object );
  DALI_TEST_CHECK(imageView2);

  ImageView imageView3 = DownCast< ImageView >( object );
  DALI_TEST_CHECK(imageView3);

  END_TEST;
}

int UtcDaliImageViewDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  ImageView imageView1 = ImageView::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !imageView1 );

  ImageView imageView2 = DownCast< ImageView >( unInitializedObject );
  DALI_TEST_CHECK( !imageView2 );

  END_TEST;
}

int UtcDaliImageViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "ImageView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  ImageView imageView = ImageView::DownCast( handle );
  DALI_TEST_CHECK( imageView );

  END_TEST;
}

int UtcDaliImageViewSetGetProperty01(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Property::Index idx = imageView.GetPropertyIndex( "image" );
  DALI_TEST_EQUALS( idx, (Property::Index)ImageView::Property::IMAGE, TEST_LOCATION );

  imageView.SetProperty( idx, TEST_IMAGE_FILE_NAME );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliImageViewSizeWithBackground(void)
{
  ToolkitTestApplication application;

  int width = 100;
  int height = 200;
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );
  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSizeWithBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  int width = 300;
  int height = 400;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );
  imageView.SetImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackground(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  Control control = Control::DownCast( imageView );
  DALI_TEST_CHECK( control );
  DALI_TEST_EQUALS( imageView.GetHeightForWidth( 123.f ), control.GetHeightForWidth( 123.f ), TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( 321.f ), control.GetWidthForHeight( 321.f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  int width = 300;
  int height = 400;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );
  imageView.SetImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetHeightForWidth( width ), (float)height, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( height ), (float)width, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetBufferImage(void)
{
  ToolkitTestApplication application;

  int width1 = 300;
  int height1 = 400;
  BufferImage image1 = CreateBufferImage( width1, height1, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New();
  imageView.SetImage( image1 );

  TestImage( imageView, image1 );

  int width2 = 600;
  int height2 = 500;
  BufferImage image2 = CreateBufferImage( width2, height2, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );

  TestImage( imageView, image2 );

  END_TEST;
}

int UtcDaliImageViewSetImageUrl(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  imageView.SetImage( TEST_IMAGE_FILE_NAME );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME );


  imageView.SetImage( TEST_IMAGE_FILE_NAME2 );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME2 );

  END_TEST;
}

int UtcDaliImageViewSetImageOnstageP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  int width = 300;
  int height = 400;
  BufferImage image2 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );
  TestImage( imageView, image2 );

  END_TEST;
}

int UtcDaliImageViewSetImageOnstageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  Image image2;
  imageView.SetImage( image2 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  END_TEST;
}

int UtcDaliImageViewSetImageOffstageP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();
  Stage::GetCurrent().Remove( imageView );

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  int width = 300;
  int height = 400;
  BufferImage image2 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );
  TestImage( imageView, image2 );

  END_TEST;
}

int UtcDaliImageViewSetImageOffstageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();
  Stage::GetCurrent().Remove( imageView );

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  Image image2;
  imageView.SetImage( image2 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  END_TEST;
}

int UtcDaliImageViewSetImageN(void)
{
  ToolkitTestApplication application;

  Image image1;
  ImageView imageView = ImageView::New();
  imageView.SetImage( image1 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  std::string resource_url;
  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  DALI_TEST_CHECK( !val.Get( resource_url ) );

  END_TEST;
}

int UtcDaliImageViewSetImageTypeChangesP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();


  std::string url;
  Property::Map map;

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty

  // Set a URL
  imageView.SetImage( "TEST_URL" );
  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  DALI_TEST_CHECK( value.Get( url ) );   // Value should NOT be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty

  // Set an empty Image
  imageView.SetImage( Image() );
  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty

  // Set an Image
  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( value.Get( map ) );   // Value should NOT be empty

  // Set an empty URL
  imageView.SetImage( "" );
  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty

  END_TEST;
}
