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

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

static void Startup();
static void Cleanup();

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}
} // namespace

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
TEST_FUNCTION( UtcDaliToolBarNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliToolBarSetBackground, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliToolBarAddControl01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliToolBarAddControl02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliToolBarRemoveControl01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliToolBarRemoveControl02, NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


static void UtcDaliToolBarNew()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarNew");

  ToolBar toolbar;

  DALI_TEST_CHECK( !toolbar );

  toolbar = ToolBar::New();

  DALI_TEST_CHECK( toolbar );

  ToolBar toolbar2(toolbar);

  DALI_TEST_CHECK( toolbar2 == toolbar );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    ToolBar toolbar = ToolBar::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  Actor actor = toolbar;
  toolbar == ToolBar::DownCast( actor );

  DALI_TEST_CHECK( toolbar );
}

static void UtcDaliToolBarSetBackground()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarSetBackground");

  try
  {
    ImageActor toolBarBackground = CreateSolidColorActor( Color::RED );

    ToolBar toolbar = ToolBar::New();
    toolbar.SetBackground( toolBarBackground );

    Stage::GetCurrent().Add( toolbar );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);

  application.SendNotification(); // VCC To be removed!!
  application.Render();     // VCC To be removed!!
}

static void UtcDaliToolBarAddControl01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarAddControl01");

  try
  {
    ImageActor control1 = CreateSolidColorActor( Color::RED );
    control1.SetSize( 100.f, 100.f );
    ImageActor control2 = CreateSolidColorActor( Color::RED );
    control2.SetSize( 100.f, 100.f );
    ImageActor control3 = CreateSolidColorActor( Color::RED );
    control3.SetSize( 100.f, 100.f );
    ImageActor control4 = CreateSolidColorActor( Color::RED );
    control4.SetSize( 100.f, 100.f );
    ImageActor control5 = CreateSolidColorActor( Color::RED );
    control5.SetSize( 100.f, 100.f );

    ToolBar toolbar = ToolBar::New();
    toolbar.SetSize( 600.f, 100.f );

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    toolbar.Add( control1 );
    toolbar.AddControl( control2, 0.1f, Alignment::HorizontalLeft, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
    toolbar.AddControl( control3, 0.1f, Alignment::HorizontalCenter, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
    toolbar.AddControl( control4, 0.1f, Alignment::HorizontalCenter, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
    toolbar.AddControl( control5, 0.1f, Alignment::HorizontalRight, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );

    ImageActor control6 = CreateSolidColorActor( Color::RED );
    control6.SetSize( 100.f, 100.f );
    ImageActor control7 = CreateSolidColorActor( Color::RED );
    control7.SetSize( 100.f, 100.f );
    ImageActor control8 = CreateSolidColorActor( Color::RED );
    control8.SetSize( 100.f, 100.f );

    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    toolbar.AddControl( control6, 0.4f, Alignment::HorizontalLeft, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
    toolbar.AddControl( control7, 0.2f, Alignment::HorizontalCenter, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
    toolbar.AddControl( control8, 0.2f, Alignment::HorizontalRight, Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
}

static void UtcDaliToolBarAddControl02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarAddControl02");

  bool daliAssert = false;

  try
  {
    ImageActor control = CreateSolidColorActor( Color::RED );

    ToolBar toolbar = ToolBar::New();

    toolbar.AddControl( control, 0.1f, static_cast<Alignment::Type>( 99 ), Alignment::Padding( 1.f, 1.f, 1.f, 1.f ) );
  }
  catch( DaliException e )
  {
    daliAssert = true;
    tet_result(TET_PASS);
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  if( !daliAssert )
  {
    tet_result(TET_FAIL);
  }
}

static void UtcDaliToolBarRemoveControl01()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarRemoveControl01");

  try
  {
    ImageActor control = CreateSolidColorActor( Color::RED );

    ToolBar toolbar = ToolBar::New();
    toolbar.AddControl( control, 0.1f, Alignment::HorizontalLeft );

    toolbar.RemoveControl( control );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
}

static void UtcDaliToolBarRemoveControl02()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarRemoveControl02");

  try
  {
    ImageActor control01 = CreateSolidColorActor( Color::RED );
    ImageActor control02 = CreateSolidColorActor( Color::RED );

    ToolBar toolbar01 = ToolBar::New();
    ToolBar toolbar02 = ToolBar::New();
    toolbar01.AddControl( control01, 0.1f, Alignment::HorizontalLeft );
    toolbar02.AddControl( control02, 0.1f, Alignment::HorizontalLeft );

    toolbar02.RemoveControl( control01 );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_EQUALS(e.mCondition, "false", TEST_LOCATION);
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  try
  {
    ImageActor control = CreateSolidColorActor( Color::RED );

    ToolBar toolbar = ToolBar::New();
    toolbar.AddControl( control, 0.1f, Alignment::HorizontalLeft );

    toolbar.RemoveControl( control );
    toolbar.RemoveControl( control );
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  tet_result(TET_PASS);
}
