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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gCheckBoxButtonState = false;
bool CheckBoxButtonClicked( Button button )
{
  gCheckBoxButtonState = button.IsSelected();
  return true;
}

Image CreateSolidColorImage( const Vector4& color, unsigned int width, unsigned int height )
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

  return imageData;
}

} // namespace

void checkbox_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void checkbox_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCheckBoxButtonConstructorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox;

  DALI_TEST_CHECK( !checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonAssignmentOperatorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( checkBox == copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonNewP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  DALI_TEST_CHECK( checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  BaseHandle object(checkBox);

  CheckBoxButton checkBox2 = CheckBoxButton::DownCast( object );
  DALI_TEST_CHECK(checkBox2);

  CheckBoxButton checkBox3 = DownCast< CheckBoxButton >(object);
  DALI_TEST_CHECK(checkBox3);
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  CheckBoxButton checkBox1 = CheckBoxButton::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !checkBox1 );

  CheckBoxButton checkBox2 = DownCast< CheckBoxButton >( unInitializedObject );
  DALI_TEST_CHECK( !checkBox2 );
  END_TEST;
}

int UtcDaliCheckBoxButtonSetBackgroundImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetBackgroundImageP");

  Image image = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor = CreateSolidColorActor( Color::RED );
  imageActor.SetSize( 20, 20 );

  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  checkBoxButton.SetBackgroundImage( image );

  DALI_TEST_CHECK( checkBoxButton.GetBackgroundImage() );

  checkBoxButton.SetBackgroundImage( imageActor );

  DALI_TEST_CHECK( checkBoxButton.GetBackgroundImage() );

  END_TEST;
}

int UtcDaliCheckBoxButtonSetSelectedImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetSelectedImageP");

  Image image = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor = CreateSolidColorActor( Color::RED );
  imageActor.SetSize( 20, 20 );

  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  checkBoxButton.SetSelectedImage( image );

  DALI_TEST_CHECK( checkBoxButton.GetSelectedImage() );

  checkBoxButton.SetSelectedImage( imageActor );

  DALI_TEST_CHECK( checkBoxButton.GetSelectedImage() );

  END_TEST;
}

int UtcDaliCheckBoxButtonSetDisabledBackgroundImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetDisabledBackgroundImageP");

  Image image = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor = CreateSolidColorActor( Color::RED );
  imageActor.SetSize( 20, 20 );

  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  checkBoxButton.SetDisabledBackgroundImage( image );

  DALI_TEST_CHECK( checkBoxButton.GetDisabledBackgroundImage() );

  checkBoxButton.SetDisabledBackgroundImage( imageActor );

  DALI_TEST_CHECK( checkBoxButton.GetDisabledBackgroundImage() );

  END_TEST;
}

int UtcDaliCheckBoxButtonSetDisabledSelectedImageP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetDisabledSelectedImageP");

  Image image = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor = CreateSolidColorActor( Color::RED );
  imageActor.SetSize( 20, 20 );

  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  checkBoxButton.SetDisabledSelectedImage( image );

  DALI_TEST_CHECK( checkBoxButton.GetDisabledSelectedImage() );

  checkBoxButton.SetDisabledSelectedImage( imageActor );

  DALI_TEST_CHECK( checkBoxButton.GetDisabledSelectedImage() );

  END_TEST;
}

int UtcDaliCheckBoxButtonSetGetSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetGetSelected");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.StateChangedSignal().Connect( &CheckBoxButtonClicked );

  // global var used to check if CheckBoxButtonClicked is called;
  gCheckBoxButtonState = false;

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );

  checkBoxButton.SetSelected( false );

  DALI_TEST_CHECK( !checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( !gCheckBoxButtonState );

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );
  END_TEST;
}

int UtcDaliCheckBoxButtonSetImages(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetImages");

  Actor imageActor;

  Image image01 = CreateSolidColorImage( Color::RED, 10, 10 );
  ImageActor imageActor01 = CreateSolidColorActor( Color::RED );
  imageActor01.SetSize( 20, 20 );

  Image image02 = CreateSolidColorImage( Color::RED, 30, 30 );
  ImageActor imageActor02 = CreateSolidColorActor( Color::RED );
  imageActor02.SetSize( 40, 40 );
  imageActor02.SetName( "imageActor02" );

  Image image03 = CreateSolidColorImage( Color::RED, 50, 50 );
  ImageActor imageActor03 = CreateSolidColorActor( Color::RED );
  imageActor03.SetSize( 60, 60 );

  Image image04 = CreateSolidColorImage( Color::RED, 70, 70 );
  ImageActor imageActor04 = CreateSolidColorActor( Color::RED );
  imageActor04.SetSize( 80, 80 );

  Vector3 size;
  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.SetName( "UtcDaliCheckBoxButtonSetImages" );
  Stage::GetCurrent().Add( checkBoxButton );

  application.SendNotification();
  application.Render();

  // Just check if check box button size changes when a bigger image is set.

  checkBoxButton.SetBackgroundImage( image01 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );

  checkBoxButton.SetBackgroundImage( imageActor01 );

  END_TEST;
}
