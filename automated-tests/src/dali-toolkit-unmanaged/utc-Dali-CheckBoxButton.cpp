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
  gCheckBoxButtonState = static_cast<CheckBoxButton&>( button ).IsChecked();
  return true;
}



Image CreateSolidColorImage( const Vector4& color, unsigned int width, unsigned int height )
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

int UtcDaliCheckBoxButtonSetGetChecked(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetGetChecked");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.ClickedSignal().Connect( &CheckBoxButtonClicked );

  // global var used to check if CheckBoxButtonClicked is called;
  gCheckBoxButtonState = false;

  checkBoxButton.SetChecked( true );

  DALI_TEST_CHECK( checkBoxButton.IsChecked() );
  DALI_TEST_CHECK( gCheckBoxButtonState );

  checkBoxButton.SetChecked( false );

  DALI_TEST_CHECK( !checkBoxButton.IsChecked() );
  DALI_TEST_CHECK( !gCheckBoxButtonState );

  checkBoxButton.SetChecked( true );

  DALI_TEST_CHECK( checkBoxButton.IsChecked() );
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

  Image image03 = CreateSolidColorImage( Color::RED, 50, 50 );
  ImageActor imageActor03 = CreateSolidColorActor( Color::RED );
  imageActor03.SetSize( 60, 60 );

  Image image04 = CreateSolidColorImage( Color::RED, 70, 70 );
  ImageActor imageActor04 = CreateSolidColorActor( Color::RED );
  imageActor04.SetSize( 80, 80 );

  Vector3 size;
  CheckBoxButton checkBoxButton = CheckBoxButton::New();

  application.SendNotification();
  application.Render();

  // Just check if check box button size changes when a bigger image is set.

  checkBoxButton.SetBackgroundImage( image01 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 10.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 10.f, TEST_LOCATION );

  checkBoxButton.SetBackgroundImage( imageActor01 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 20.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 20.f, TEST_LOCATION );

  checkBoxButton.SetCheckedImage( image02 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetCheckedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 30.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 30.f, TEST_LOCATION );

  checkBoxButton.SetCheckedImage( imageActor02 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetCheckedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 40.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 40.f, TEST_LOCATION );

  checkBoxButton.SetDimmedBackgroundImage( image03 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetDimmedBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 50.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 50.f, TEST_LOCATION );

  checkBoxButton.SetDimmedBackgroundImage( imageActor03 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetDimmedBackgroundImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 60.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 60.f, TEST_LOCATION );

  checkBoxButton.SetDimmedCheckedImage( image04 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetDimmedCheckedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 70.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 70.f, TEST_LOCATION );

  checkBoxButton.SetDimmedCheckedImage( imageActor04 );

  application.SendNotification();
  application.Render();

  size = checkBoxButton.GetDimmedCheckedImage().GetCurrentSize();

  DALI_TEST_EQUALS( size.width, 80.f, TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, 80.f, TEST_LOCATION );
  END_TEST;
}
