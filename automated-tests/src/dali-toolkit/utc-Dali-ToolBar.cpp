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
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}
} // namespace

void dali_toolbar_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_toolbar_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliToolBarNew(void)
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
  END_TEST;
}

int UtcDaliToolBarSetBackground(void)
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
  END_TEST;
}

int UtcDaliToolBarAddControl01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolBarAddControl01");

  try
  {
    ImageActor control1 = CreateSolidColorActor( Color::RED );
    control1.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control1.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control2 = CreateSolidColorActor( Color::RED );
    control2.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control2.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control3 = CreateSolidColorActor( Color::RED );
    control3.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control3.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control4 = CreateSolidColorActor( Color::RED );
    control4.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control4.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control5 = CreateSolidColorActor( Color::RED );
    control5.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control5.SetResizePolicy( FIXED, ALL_DIMENSIONS );

    ToolBar toolbar = ToolBar::New();
    toolbar.SetPreferredSize( Vector2( 600.f, 100.f ) );

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
    control6.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control6.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control7 = CreateSolidColorActor( Color::RED );
    control7.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control7.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    ImageActor control8 = CreateSolidColorActor( Color::RED );
    control8.SetPreferredSize( Vector2( 100.f, 100.f ) );
    control8.SetResizePolicy( FIXED, ALL_DIMENSIONS );

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
  END_TEST;
}

int UtcDaliToolBarAddControl02(void)
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
  END_TEST;
}

int UtcDaliToolBarRemoveControl01(void)
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
  END_TEST;
}

int UtcDaliToolBarRemoveControl02(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_EQUALS(e.condition, "false", TEST_LOCATION);
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
  END_TEST;
}
