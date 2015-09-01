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

  Image image = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New( image );

  DALI_TEST_CHECK( imageView );
  DALI_TEST_EQUALS( image, imageView.GetImage(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewNewUrlP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( imageView );
  DALI_TEST_CHECK( imageView.GetImage() );

  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "resource-url" ) );
  std::string resource_url;
  DALI_TEST_CHECK( val.Get( resource_url ) );
  DALI_TEST_EQUALS( resource_url, TEST_IMAGE_FILE_NAME, TEST_LOCATION );

  Image image = imageView.GetImage();
  DALI_TEST_CHECK( image );

  ResourceImage resourceImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resourceImage );
  DALI_TEST_EQUALS( resourceImage.GetUrl(), TEST_IMAGE_FILE_NAME, TEST_LOCATION );

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

int UtcDaliImageViewSetGetProperty(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Property::Index idx = imageView.GetPropertyIndex( "resource-url" );
  DALI_TEST_EQUALS( idx, ImageView::Property::RESOURCE_URL, TEST_LOCATION );

  imageView.SetProperty( idx, TEST_IMAGE_FILE_NAME );
  Property::Value val = imageView.GetProperty( idx );
  std::string resource_url;
  DALI_TEST_CHECK( val.Get( resource_url ) );
  DALI_TEST_EQUALS( resource_url, TEST_IMAGE_FILE_NAME, TEST_LOCATION );

  Image image = imageView.GetImage();
  DALI_TEST_CHECK( image );

  ResourceImage resourceImage = ResourceImage::DownCast( image );
  DALI_TEST_CHECK( resourceImage );
  DALI_TEST_EQUALS( resourceImage.GetUrl(), TEST_IMAGE_FILE_NAME, TEST_LOCATION );

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

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, height, TEST_LOCATION );

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

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, height, TEST_LOCATION );

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

  DALI_TEST_EQUALS( imageView.GetHeightForWidth( width ), height, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( height ), width, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetBufferImage(void)
{
  ToolkitTestApplication application;

  int width = 300;
  int height = 400;
  Image image = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New();
  imageView.SetImage( image );

  std::string resource_url;
  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "resource-url" ) );
  DALI_TEST_CHECK( val.Get( resource_url ) );
  DALI_TEST_CHECK( resource_url.empty() );

  END_TEST;
}

int UtcDaliImageViewSetResourceImage(void)
{
  ToolkitTestApplication application;

  Image image = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  ImageView imageView = ImageView::New();
  imageView.SetImage( image );

  std::string resource_url;
  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "resource-url" ) );
  DALI_TEST_CHECK( val.Get( resource_url ) );
  DALI_TEST_EQUALS( resource_url, TEST_IMAGE_FILE_NAME, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageOnstageP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  Image image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );

  Image image2 = imageView.GetImage();
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  int width = 300;
  int height = 400;
  Image image3 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image3 );

  Image image4 = imageView.GetImage();
  DALI_TEST_EQUALS( image3, image4, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageOnstageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  Image image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );

  Image image2 = imageView.GetImage();
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  Image image3;
  imageView.SetImage( image3 );

  Image image4 = imageView.GetImage();
  DALI_TEST_CHECK( !image4 );

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

  Image image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );

  Image image2 = imageView.GetImage();
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  int width = 300;
  int height = 400;
  Image image3 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image3 );

  Image image4 = imageView.GetImage();
  DALI_TEST_EQUALS( image3, image4, TEST_LOCATION );

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

  Image image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );

  Image image2 = imageView.GetImage();
  DALI_TEST_EQUALS( image1, image2, TEST_LOCATION );

  Image image3;
  imageView.SetImage( image3 );

  Image image4 = imageView.GetImage();
  DALI_TEST_CHECK( !image4 );

  END_TEST;
}

int UtcDaliImageViewSetImageN(void)
{
  ToolkitTestApplication application;

  Image image1;
  ImageView imageView = ImageView::New();
  imageView.SetImage( image1 );

  Image image2 = imageView.GetImage();
  DALI_TEST_CHECK( !image2 );

  std::string resource_url;
  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "resource-url" ) );
  DALI_TEST_CHECK( val.Get( resource_url ) );
  DALI_TEST_CHECK( resource_url.empty() );

  END_TEST;
}
