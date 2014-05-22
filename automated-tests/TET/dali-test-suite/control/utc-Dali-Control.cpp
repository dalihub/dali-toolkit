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
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

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
TEST_FUNCTION( UtcDaliControlConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlRegister, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlCopyAndAssignment, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlDownCastTemplate, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlKeyInputFocus, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlGetImplementation, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlSignalConnectDisconnect, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlSignalAutomaticDisconnect, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliControlTestParameters, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliControlConstructor()
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  DummyControl dummy;

  DALI_TEST_CHECK( !Control::DownCast(dummy) );

  dummy = DummyControl::New();

  DALI_TEST_CHECK( Control::DownCast(dummy) );
}

static void UtcDaliControlNew()
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK( !Control::DownCast(control) );

  control = Control::New();

  DALI_TEST_CHECK( Control::DownCast(control) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static void UtcDaliControlRegister()
{
  ToolkitTestApplication application;

  // Ensure the object is registered after creation
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    Alignment alignment = Alignment::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static void UtcDaliControlCopyAndAssignment()
{
  ToolkitTestApplication application;

  DummyControl control = DummyControl::New();
  Control emptyControl;

  Control controlCopy( control );
  DALI_TEST_CHECK( control == controlCopy );

  Control emptyControlCopy( emptyControl );
  DALI_TEST_CHECK( emptyControl == emptyControlCopy );

  Control controlEquals;
  controlEquals = control;
  DALI_TEST_CHECK( control == controlEquals );

  Control emptyControlEquals;
  emptyControlEquals = emptyControl;
  DALI_TEST_CHECK( emptyControl == emptyControlEquals );

  // Self assignment
  control = control;
  DALI_TEST_CHECK( control == controlCopy );
}

static void UtcDaliControlDownCast()
{
  ToolkitTestApplication application;

  DummyControl control;

  DALI_TEST_CHECK( !Control::DownCast( control ) );

  control = DummyControl::New();

  DALI_TEST_CHECK( Control::DownCast( control ) );

  Actor actor;

  DALI_TEST_CHECK( !Control::DownCast( actor ) );

  actor = Actor::New();

  DALI_TEST_CHECK( !Control::DownCast( actor ) );
}

static void UtcDaliControlDownCastTemplate()
{
  ToolkitTestApplication application;

  DummyControl control;

  DALI_TEST_CHECK( !DummyControl::DownCast( control ));

  control = DummyControl::New();

  DALI_TEST_CHECK( DummyControl::DownCast( control ) );

  Actor actor;

  DALI_TEST_CHECK( !DummyControl::DownCast( actor ) );

  actor = Actor::New();

  DALI_TEST_CHECK( !DummyControl::DownCast( actor ) );
}

static void UtcDaliControlKeyInputFocus()
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  DummyControl control;

  PushButton pushButton1 = PushButton::New();
  stage.Add( pushButton1 );

  pushButton1.SetKeyInputFocus();
  DALI_TEST_CHECK( pushButton1.HasKeyInputFocus() );

  pushButton1.ClearKeyInputFocus();
  DALI_TEST_CHECK( !pushButton1.HasKeyInputFocus() );
}

static void UtcDaliControlGetImplementation()
{
  ToolkitTestApplication application;

  DummyControl control;

  // Get Empty
  {
    try
    {
      ControlImpl& controlImpl = control.GetImplementation();
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_PASS);
    }
  }

  // Get Const Empty
  {
    try
    {
      const DummyControl constControl(control);
      const ControlImpl& controlImpl = constControl.GetImplementation();
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_PASS);
    }
  }

  control = DummyControl::New();

  // Get
  {
    try
    {
      ControlImpl& controlImpl = control.GetImplementation();
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_FAIL);
    }
  }

  // Get Const
  {
    try
    {
      const DummyControl constControl(control);
      const ControlImpl& controlImpl = constControl.GetImplementation();
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_FAIL);
    }
  }
}

static void UtcDaliControlSignalConnectDisconnect()
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControlImpl::New();

    Actor actor = Actor::New();
    DALI_TEST_EQUALS( actor.SetSizeSignal().GetConnectionCount(), 0u, TEST_LOCATION );
    actor.SetSizeSignal().Connect( &dummy, &DummyControl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.SetSizeSignal().GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Called, false, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Value,  Vector3::ZERO, TEST_LOCATION );

    const Vector3 newSize( 10, 10, 0 );
    actor.SetSize( newSize );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Called, true, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Value,  newSize, TEST_LOCATION );

    dummy.mCustomSlot1Called = false;
    actor.SetSizeSignal().Disconnect( &dummy, &DummyControl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.SetSizeSignal().GetConnectionCount(), 0u, TEST_LOCATION );
    const Vector3 ignoredSize( 20, 20, 0 );
    actor.SetSize( ignoredSize );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Called, false, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Value,  newSize/*not ignoredSize*/, TEST_LOCATION );
  }
}

static void UtcDaliControlSignalAutomaticDisconnect()
{
  ToolkitTestApplication application;

  Actor actor = Actor::New();

  {
    DummyControl dummy = DummyControlImpl::New();

    actor.SetSizeSignal().Connect( &dummy, &DummyControl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.SetSizeSignal().GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Called, false, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Value,  Vector3::ZERO, TEST_LOCATION );

    const Vector3 newSize( 10, 10, 0 );
    actor.SetSize( newSize );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Called, true, TEST_LOCATION );
    DALI_TEST_EQUALS( dummy.mCustomSlot1Value,  newSize, TEST_LOCATION );
  }
  // dummyControl automatically disconnects

  DALI_TEST_EQUALS( actor.SetSizeSignal().GetConnectionCount(), 0u, TEST_LOCATION );

  const Vector3 ignoredSize( 20, 20, 0 );
  actor.SetSize( ignoredSize );
}

static void UtcDaliControlTestParameters()
{
  ToolkitTestApplication application;
  DummyControl test = DummyControl::New();

  Vector3 maxSize = test.GetNaturalSize();
  Vector3 minSize = maxSize / 2.0f;

  Toolkit::Control::SizePolicy widthPolicy( Control::Fixed );
  Toolkit::Control::SizePolicy heightPolicy( Control::Fixed );
  test.SetSizePolicy( widthPolicy, heightPolicy );
  test.GetSizePolicy( widthPolicy, heightPolicy );

  DALI_TEST_CHECK( widthPolicy == Control::Fixed && heightPolicy == Control::Fixed );

  test.SetSize( 0.7f, 0.7f, 0.7f );
  float width = 640.0f;
  float height = test.GetHeightForWidth( width );
  DALI_TEST_CHECK( test.GetWidthForHeight( height ) == width );

  test.SetMinimumSize( minSize );
  DALI_TEST_CHECK( test.GetMinimumSize() == minSize );

  test.SetMaximumSize( maxSize );
  DALI_TEST_CHECK( test.GetMaximumSize() == maxSize );

  test.KeyEventSignal();
  DummyControl test2 = DummyControl::New();
  dynamic_cast< ConnectionTrackerInterface& >( test2 ).GetConnectionCount();

  // Provide coverage for pointer destructor
  Control* testControlPtr = new Control;
  DALI_TEST_CHECK( testControlPtr );
  delete testControlPtr;
}
