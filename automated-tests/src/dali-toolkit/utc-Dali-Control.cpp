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

#include <iostream>
#include <stdlib.h>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_control_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_control_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

bool gObjectCreatedCallBackCalled;

void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

void TestVoidCallback()
{
}

static bool gKeyInputFocusCallBackCalled;

static void TestKeyInputFocusCallback( Control control )
{
  tet_infoline(" TestKeyInputFocusCallback");

  gKeyInputFocusCallBackCalled = true;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////


int UtcDaliControlConstructor(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  DummyControl dummy;

  DALI_TEST_CHECK( !Control::DownCast(dummy) );

  dummy = DummyControl::New();

  DALI_TEST_CHECK( Control::DownCast(dummy) );
  END_TEST;
}

int UtcDaliControlNew(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control;

  DALI_TEST_CHECK( !Control::DownCast(control) );

  control = Control::New();

  DALI_TEST_CHECK( Control::DownCast(control) );
  END_TEST;
}


int UtcDaliControlRegister(void)
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
  END_TEST;
}

int UtcDaliControlCopyAndAssignment(void)
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
  END_TEST;
}

int UtcDaliControlDownCast(void)
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
  END_TEST;
}

int UtcDaliControlDownCastTemplate(void)
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
  END_TEST;
}

int UtcDaliControlKeyInputFocus(void)
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
  END_TEST;
}

int UtcDaliControlGetImplementationN(void)
{
  ToolkitTestApplication application;
  DummyControl control;

  // Get Empty
  {
    try
    {
      Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationConstN(void)
{
  ToolkitTestApplication application;
  DummyControl control;

  // Get Const Empty
  {
    try
    {
      const DummyControl constControl(control);
      const Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( constControl );
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_FAIL);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_PASS);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationP(void)
{
  ToolkitTestApplication application;
  DummyControl control = DummyControl::New();

  // Get
  {
    try
    {
      Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_FAIL);
    }
  }
  END_TEST;
}

int UtcDaliControlGetImplementationConstP(void)
{
  ToolkitTestApplication application;
  DummyControl control = DummyControl::New();
  // Get Const
  {
    try
    {
      const DummyControl constControl(control);
      const Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( constControl );
      (void)controlImpl; // Avoid unused warning
      tet_result(TET_PASS);
    }
    catch (DaliException &exception)
    {
      tet_result(TET_FAIL);
    }
  }
  END_TEST;
}

int UtcDaliControlSignalConnectDisconnect(void)
{
  ToolkitTestApplication application;

  {
    DummyControl dummy = DummyControlImpl::New();

    Actor actor = Actor::New();
    DALI_TEST_EQUALS( actor.OnStageSignal().GetConnectionCount(), 0u, TEST_LOCATION );
    Toolkit::Internal::Control& control = Toolkit::Internal::GetImplementation( dummy );
    DummyControlImpl* dummyImpl = dynamic_cast<DummyControlImpl*>(&control);

    if( dummyImpl == NULL )
    {
      tet_result( TET_FAIL );
      END_TEST;
    }

    actor.OnStageSignal().Connect( dummyImpl, &DummyControlImpl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.OnStageSignal().GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( dummyImpl->mCustomSlot1Called, false, TEST_LOCATION );

    Stage::GetCurrent().Add( actor );
    DALI_TEST_EQUALS( dummyImpl->mCustomSlot1Called, true, TEST_LOCATION );

    dummyImpl->mCustomSlot1Called = false;
    actor.OnStageSignal().Disconnect( dummyImpl, &DummyControlImpl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.OnStageSignal().GetConnectionCount(), 0u, TEST_LOCATION );
    Stage::GetCurrent().Remove( actor );
    Stage::GetCurrent().Add( actor );
    DALI_TEST_EQUALS( dummyImpl->mCustomSlot1Called, false, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliControlSignalAutomaticDisconnect(void)
{
  ToolkitTestApplication application;

  Actor actor = Actor::New();

  {
    DummyControl dummy = DummyControlImpl::New();
    Toolkit::Internal::Control& control = Toolkit::Internal::GetImplementation( dummy );
    DummyControlImpl* dummyImpl = dynamic_cast<DummyControlImpl*>(&control);

    if( dummyImpl == NULL )
    {
      tet_result( TET_FAIL );
      END_TEST;
    }

    actor.OnStageSignal().Connect( dummyImpl, &DummyControlImpl::CustomSlot1 );
    DALI_TEST_EQUALS( actor.OnStageSignal().GetConnectionCount(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( dummyImpl->mCustomSlot1Called, false, TEST_LOCATION );

    Stage::GetCurrent().Add( actor );
    DALI_TEST_EQUALS( dummyImpl->mCustomSlot1Called, true, TEST_LOCATION );
    Stage::GetCurrent().Remove( actor );
  }
  // dummyControl automatically disconnects

  DALI_TEST_EQUALS( actor.OnStageSignal().GetConnectionCount(), 0u, TEST_LOCATION );

  const Vector3 ignoredSize( 20, 20, 0 );
  actor.SetSize( ignoredSize );
  END_TEST;
}

int UtcDaliControlTestParameters(void)
{
  ToolkitTestApplication application;
  DummyControl test = DummyControl::New();

  test.SetSize( 0.7f, 0.7f, 0.7f );

  Stage::GetCurrent().Add( test );

  application.SendNotification();
  application.Render();

  float width = 640.0f;
  float height = test.GetHeightForWidth( width );
  DALI_TEST_EQUALS( 640.0f, height, TEST_LOCATION );
  DALI_TEST_EQUALS( 640.0f, test.GetWidthForHeight( height ), TEST_LOCATION );

  test.KeyEventSignal();

  // Provide coverage for pointer destructor
  Control* testControlPtr = new Control;
  DALI_TEST_CHECK( testControlPtr );
  delete testControlPtr;
  END_TEST;
}

int UtcDaliControlBackgroundColor(void)
{
  ToolkitTestApplication application;
  Control control = Control::New();

  DALI_TEST_EQUALS( control.GetBackgroundColor(), Color::TRANSPARENT, TEST_LOCATION );

  control.SetBackgroundColor( Color::RED );

  Property::Value propValue = control.GetProperty( Control::Property::BACKGROUND );
  Property::Map* resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "rendererType" ) );
  DALI_TEST_CHECK( resultMap->Find( "rendererType" )->Get<std::string>() == "color" );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" ) );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" )->Get<Vector4>() == Color::RED );

  DALI_TEST_EQUALS( control.GetBackgroundColor(), Color::RED, TEST_LOCATION );

  control.SetBackgroundColor( Color::YELLOW );

  propValue = control.GetProperty( Control::Property::BACKGROUND );
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "blendColor" ) );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" )->Get<Vector4>() == Color::YELLOW );

  DALI_TEST_EQUALS( control.GetBackgroundColor(), Color::YELLOW, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlBackgroundImage(void)
{
  ToolkitTestApplication application;
  Control control = Control::New();

  DALI_TEST_EQUALS( control.GetBackgroundColor(), Color::TRANSPARENT, TEST_LOCATION );

  Image image = ResourceImage::New("TestImage");
  control.SetBackgroundImage( image );

  Property::Value propValue = control.GetProperty( Control::Property::BACKGROUND );
  Property::Map* resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "rendererType" ) );
  DALI_TEST_CHECK( resultMap->Find( "rendererType" )->Get<std::string>() == "image" );
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" ) );
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" )->Get<std::string>() == "TestImage" );

  image = ResourceImage::New("TestImage2");
  control.SetBackgroundImage( image );

  propValue = control.GetProperty( Control::Property::BACKGROUND );
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" ) );
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" )->Get<std::string>() == "TestImage2" );

  END_TEST;
}

int UtcDaliControlBackgroundProperties(void)
{
  ToolkitTestApplication application;
  Control control = Control::New();

  DALI_TEST_EQUALS( control.GetBackgroundColor(), Color::TRANSPARENT, TEST_LOCATION );
  DALI_TEST_CHECK( control.GetProperty( Control::Property::BACKGROUND ).Get< Property::Map >().Empty() );

  Property::Map colorMap;
  colorMap["color"] = Color::RED;
  control.SetProperty( Control::Property::BACKGROUND, colorMap );
  Property::Value propValue = control.GetProperty( Control::Property::BACKGROUND );
  Property::Map* resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "rendererType" ) );
  DALI_TEST_CHECK( resultMap->Find( "rendererType" )->Get<std::string>() == "color" );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" ) );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" )->Get<Vector4>() == Color::RED );

  Property::Map imageMap;
  imageMap[ "filename" ] = "TestImage";
  control.SetProperty( Control::Property::BACKGROUND, imageMap );
  propValue = control.GetProperty( Control::Property::BACKGROUND );
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "rendererType" ) );
  DALI_TEST_CHECK( resultMap->Find( "rendererType" )->Get<std::string>() == "image" );
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" ) );
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" )->Get<std::string>() == "TestImage" );

  Property::Map rendererMap;
  rendererMap["rendererType"] = "color";
  rendererMap["blendColor"] = Color::CYAN;
  control.SetProperty( Control::Property::BACKGROUND, rendererMap );
  propValue = control.GetProperty( Control::Property::BACKGROUND );
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "rendererType" ) );
  DALI_TEST_CHECK( resultMap->Find( "rendererType" )->Get<std::string>() == "color" );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" ) );
  DALI_TEST_CHECK( resultMap->Find( "blendColor" )->Get<Vector4>() == Color::CYAN );

  Property::Map emptyMap;
  control.SetProperty( Control::Property::BACKGROUND, emptyMap );
  DALI_TEST_CHECK( control.GetProperty( Control::Property::BACKGROUND ).Get< Property::Map >().Empty() );

  // Deprecated Properties
  control.SetProperty( Control::Property::BACKGROUND_COLOR, Color::YELLOW );
  DALI_TEST_CHECK( control.GetProperty( Control::Property::BACKGROUND_COLOR ).Get< Vector4 >() == Color::YELLOW );
  DALI_TEST_CHECK( control.GetProperty( Control::Property::BACKGROUND_COLOR ).Get< Vector4 >() == control.GetBackgroundColor() );
  control.ClearBackground();
  control.SetProperty( Control::Property::BACKGROUND_IMAGE, imageMap );
  propValue = control.GetProperty( Control::Property::BACKGROUND_IMAGE );
  resultMap = propValue.GetMap();
  DALI_TEST_CHECK( resultMap->Find( "imageUrl" )->Get< std::string >() == "TestImage" );
  control.SetProperty( Control::Property::BACKGROUND_IMAGE, emptyMap );
  DALI_TEST_CHECK( control.GetProperty( Control::Property::BACKGROUND_IMAGE ).Get< Property::Map >().Empty() );

  END_TEST;
}

int UtcDaliControlKeyProperties(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  Stage::GetCurrent().Add( control );

  DALI_TEST_EQUALS( control.HasKeyInputFocus(), control.GetProperty( Control::Property::KEY_INPUT_FOCUS ).Get< bool >(), TEST_LOCATION );

  control.SetKeyInputFocus();
  DALI_TEST_EQUALS( true, control.GetProperty( Control::Property::KEY_INPUT_FOCUS ).Get< bool >(), TEST_LOCATION );

  control.ClearKeyInputFocus();
  DALI_TEST_EQUALS( false, control.GetProperty( Control::Property::KEY_INPUT_FOCUS ).Get< bool >(), TEST_LOCATION );

  control.SetProperty( Control::Property::KEY_INPUT_FOCUS, true );
  DALI_TEST_EQUALS( true, control.HasKeyInputFocus(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlGestureSignals(void)
{
  ToolkitTestApplication application;
  ConnectionTracker connectionTracker;
  Control control = Control::New();

  // Each gesture detector gets created when connecting to the gesture signals
  DALI_TEST_CHECK( !control.GetTapGestureDetector() );
  control.ConnectSignal( &connectionTracker, "tapped", &TestVoidCallback );
  DALI_TEST_CHECK( control.GetTapGestureDetector() );

  DALI_TEST_CHECK( !control.GetPanGestureDetector() );
  control.ConnectSignal( &connectionTracker, "panned", &TestVoidCallback );
  DALI_TEST_CHECK( control.GetPanGestureDetector() );

  DALI_TEST_CHECK( !control.GetPinchGestureDetector() );
  control.ConnectSignal( &connectionTracker, "pinched", &TestVoidCallback );
  DALI_TEST_CHECK( control.GetPinchGestureDetector() );

  DALI_TEST_CHECK( !control.GetLongPressGestureDetector() );
  control.ConnectSignal( &connectionTracker, "longPressed",  &TestVoidCallback );
  DALI_TEST_CHECK( control.GetLongPressGestureDetector() );

  END_TEST;
}

int UtcDaliControlImplKeyInputFocusGainedSignal(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  Stage::GetCurrent().Add( control );

  gKeyInputFocusCallBackCalled = false;
  control.KeyInputFocusGainedSignal().Connect(&TestKeyInputFocusCallback);

  application.SendNotification();
  application.Render();

  control.SetKeyInputFocus();

  DALI_TEST_CHECK( control.HasKeyInputFocus() );

  DALI_TEST_CHECK( gKeyInputFocusCallBackCalled );

  END_TEST;
}

int UtcDaliControlImplKeyInputFocusLostSignal(void)
{
  ToolkitTestApplication application;

  Control control = Control::New();
  Stage::GetCurrent().Add( control );

  gKeyInputFocusCallBackCalled = false;
  control.KeyInputFocusLostSignal().Connect(&TestKeyInputFocusCallback);

  application.SendNotification();
  application.Render();

  control.SetKeyInputFocus();

  DALI_TEST_CHECK( control.HasKeyInputFocus() );

  control.ClearKeyInputFocus();

  DALI_TEST_CHECK( gKeyInputFocusCallBackCalled );

  END_TEST;
}

int UtcDaliControlImplGetControlExtensionP(void)
{
  ToolkitTestApplication application;
  Control control = Control::New();

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );

  DALI_TEST_CHECK( NULL == controlImpl.GetControlExtension() );

  END_TEST;
}
