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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/public-api/controls/tool-bar/tool-bar.h>
#include <dali-toolkit/public-api/controls/view/view.h>
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit/public-api/controls/default-controls/push-button-factory.h>
#include <dali-toolkit/public-api/controls/default-controls/check-button-factory.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliDefaultControlsCreateSolidColorActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDefaultControlsCreatePushButton, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDefaultControlsCreateCheckBoxButton, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliDefaultControlsCreateSolidColorActor()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultControlsCreateSolidColorActor");

  ImageActor image1 = CreateSolidColorActor( Color::RED );
  ImageActor image2 = CreateSolidColorActor( Color::RED, true, Color::BLUE, 2 );
  ImageActor image3 = CreateSolidColorActor( Color::RED, true, Color::BLUE, 12 );

  DALI_TEST_CHECK(image1);
  DALI_TEST_CHECK(image2);
  DALI_TEST_CHECK(!image3);
}

static void UtcDaliDefaultControlsCreatePushButton()
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliDefaultControlsCreatePushButton");

  const std::string imagePath( "Facke image path" );
  const std::string voidImagePath( "" );

  ImageActor image = CreateSolidColorActor( Color::RED );
  ImageActor voidImage;

  PushButton button0, button1, button2, button3, button4, button5, button6;

  try
  {
    button0 = CreatePushButton( voidImagePath, voidImagePath, voidImagePath, voidImagePath, voidImagePath );
    button1 = CreatePushButton( imagePath, voidImagePath, voidImagePath, voidImagePath, voidImagePath );
    button2 = CreatePushButton( voidImagePath, imagePath, voidImagePath, voidImagePath, voidImagePath );
    button3 = CreatePushButton( voidImagePath, voidImagePath, imagePath, voidImagePath, voidImagePath );
    button4 = CreatePushButton( voidImagePath, voidImagePath, voidImagePath, imagePath, voidImagePath );
    button5 = CreatePushButton( voidImagePath, voidImagePath, voidImagePath, voidImagePath, imagePath );
    button6 = CreatePushButton( imagePath, imagePath, imagePath, imagePath, imagePath );
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
  DALI_TEST_CHECK( button6 );

  try
  {
    button0 = CreatePushButton( voidImage, voidImage, voidImage, voidImage, voidImage );
    button1 = CreatePushButton( image, voidImage, voidImage, voidImage, voidImage );
    button2 = CreatePushButton( voidImage, image, voidImage, voidImage, voidImage );
    button3 = CreatePushButton( voidImage, voidImage, image, voidImage, voidImage );
    button4 = CreatePushButton( voidImage, voidImage, voidImage, image, voidImage );
    button5 = CreatePushButton( voidImage, voidImage, voidImage, voidImage, image );
    button6 = CreatePushButton( image, image, image, image, image );
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
  DALI_TEST_CHECK( button6 );

  try
  {
    button0 = CreatePushButton( voidImagePath );
    button1 = CreatePushButton( imagePath );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );

  try
  {
    button0 = CreatePushButton( voidImage );
    button1 = CreatePushButton( image );
  }
  catch( ... )
  {
    tet_result( TET_FAIL );
  }

  DALI_TEST_CHECK( button0 );
  DALI_TEST_CHECK( button1 );
}

static void UtcDaliDefaultControlsCreateCheckBoxButton()
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
}
