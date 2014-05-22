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

void dali_default_controls_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_default_controls_cleanup(void)
{
  test_return_value = TET_PASS;
}



int UtcDaliDefaultControlsCreateCheckBoxButton(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultControlsCreateCheckBoxButton");

  const std::string imagePath( "Facke image path" );
  const std::string voidImagePath( "" );

  ImageActor image = CreateSolidColorActor( Color::RED );
  ImageActor voidImage;

  CheckBoxButton button0, button1, button2, button3, button4, button5;

  try
  {
    button0 = CreateCheckBoxButton( voidImagePath, voidImagePath, voidImagePath, voidImagePath );
    button1 = CreateCheckBoxButton( imagePath, voidImagePath, voidImagePath, voidImagePath );
    button2 = CreateCheckBoxButton( voidImagePath, imagePath, voidImagePath, voidImagePath );
    button3 = CreateCheckBoxButton( voidImagePath, voidImagePath, imagePath, voidImagePath );
    button4 = CreateCheckBoxButton( voidImagePath, voidImagePath, voidImagePath, imagePath );
    button5 = CreateCheckBoxButton( imagePath, imagePath, imagePath, imagePath );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );
  DALI_TEST_CHECK( button2 );
  DALI_TEST_CHECK( button3 );
  DALI_TEST_CHECK( button4 );
  DALI_TEST_CHECK( button5 );

  try
  {
    button0 = CreateCheckBoxButton( voidImage, voidImage, voidImage, voidImage );
    button1 = CreateCheckBoxButton( image, voidImage, voidImage, voidImage );
    button2 = CreateCheckBoxButton( voidImage, image, voidImage, voidImage );
    button3 = CreateCheckBoxButton( voidImage, voidImage, image, voidImage );
    button4 = CreateCheckBoxButton( voidImage, voidImage, voidImage, image );
    button5 = CreateCheckBoxButton( image, image, image, image );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );
  DALI_TEST_CHECK( button2 );
  DALI_TEST_CHECK( button3 );
  DALI_TEST_CHECK( button4 );
  DALI_TEST_CHECK( button5 );

  try
  {
    button0 = CreateCheckBoxButton( voidImagePath, voidImagePath );
    button1 = CreateCheckBoxButton( voidImagePath, imagePath );
    button2 = CreateCheckBoxButton( imagePath, voidImagePath );
    button3 = CreateCheckBoxButton( imagePath, imagePath );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );
  DALI_TEST_CHECK( button2 );
  DALI_TEST_CHECK( button3 );

  try
  {
    button0 = CreateCheckBoxButton( voidImage, voidImage );
    button2 = CreateCheckBoxButton( voidImage, image );
    button3 = CreateCheckBoxButton( voidImage, image );
    button4 = CreateCheckBoxButton( image, image );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );
  DALI_TEST_CHECK( button2 );
  DALI_TEST_CHECK( button3 );
  END_TEST;
}
