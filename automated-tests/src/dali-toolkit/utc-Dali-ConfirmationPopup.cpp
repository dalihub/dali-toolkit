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

#include <stdlib.h>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/confirmation-popup.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_confirmation_popup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_confirmation_popup_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static bool gSignalReceivedOK;
static bool gSignalReceivedCancel;

/**
 * A connection tracker is required when connecting to a signal with a functor.
 */
class TestConnectionTrackerObject : public ConnectionTracker
{
};

/**
 * This functor is used to test the confirmation popup's OK signal connection.
 */
struct ConfirmationPopupOKTestFunctor
{
  ConfirmationPopupOKTestFunctor()
  {
  }

  void operator()()
  {
    gSignalReceivedOK = true;
  }
};

/**
 * This functor is used to test the confirmation popup's Cancel signal connection.
 */
struct ConfirmationPopupCancelTestFunctor
{
  ConfirmationPopupCancelTestFunctor()
  {
  }

  void operator()()
  {
    gSignalReceivedCancel = true;
  }
};

} // unnamed namespace.


int UtcDaliConfirmationPopupNewP( void )
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliConfirmationPopupNewP" );

  // Create the ConfirmationPopup.
  ConfirmationPopup popup;

  DALI_TEST_CHECK( !popup );

  popup = ConfirmationPopup::New();

  DALI_TEST_CHECK( popup );

  ConfirmationPopup popup2( popup );

  DALI_TEST_CHECK( popup2 == popup );

  // Additional check to ensure object is created by checking if it's registered.
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    ConfirmationPopup popup = ConfirmationPopup::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliConfirmationPopupDestructorP( void )
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliConfirmationPopupDestructorP" );

  ConfirmationPopup* popup = new ConfirmationPopup();
  delete popup;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliConfirmationPopupDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliConfirmationPopupDownCastP" );

  Handle handle = ConfirmationPopup::New();

  ConfirmationPopup popup = ConfirmationPopup::DownCast( handle );

  DALI_TEST_CHECK( popup == handle );
  END_TEST;
}

int UtcDaliConfirmationPopupDynamicSignalGenerationP(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliConfirmationPopupDynamicSignalGenerationP" );

  ConfirmationPopup popup = ConfirmationPopup::New();

  TextLabel titleActor = TextLabel::New( "Title" );
  popup.SetTitle( titleActor );

  TextLabel contentActor = TextLabel::New( "Content" );
  popup.SetContent( contentActor );

  Actor footerActor = Actor::New();

  // The confirmation popup can use any control type for the ok or cancel buttons.
  // It requires that the name is "control-ok" to provide the "control-signal-ok" signal.
  PushButton buttonOK = PushButton::New();
  buttonOK.SetName( "control-ok" );
  footerActor.Add( buttonOK );

  PushButton buttonCancel = PushButton::New();
  buttonCancel.SetName( "control-cancel" );
  footerActor.Add( buttonCancel );

  popup.SetFooter( footerActor );

  // Tell the confirmation popup to connect to the signal in our button called "on-stage".
  popup.SetProperty( Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_OK_SELECTED, "on-stage" );
  std::string resultProperty;
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_OK_SELECTED ).Get( resultProperty ) );
  DALI_TEST_EQUALS( resultProperty, "on-stage", TEST_LOCATION );

  // Connect to the confirmation popup's OK signal. This signal is dynamically created upon connection.
  gSignalReceivedOK = false;
  gSignalReceivedCancel = false;
  TestConnectionTrackerObject* testTracker = new TestConnectionTrackerObject();
  popup.ConnectSignal( testTracker, "control-signal-ok", ConfirmationPopupOKTestFunctor() );

  // Check no signal has occurred yet.
  DALI_TEST_CHECK( !gSignalReceivedOK );
  DALI_TEST_CHECK( !gSignalReceivedCancel );

  // Provoke the signal.
  Stage::GetCurrent().Add( popup );

  // Check the signal has occurred.
  DALI_TEST_CHECK( gSignalReceivedOK );
  DALI_TEST_CHECK( !gSignalReceivedCancel );

  // Remove the popup from the stage, and connect the cancel signal.
  popup.Unparent();
  popup.SetProperty( Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_CANCEL_SELECTED, "on-stage" );
  DALI_TEST_CHECK( popup.GetProperty( Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_CANCEL_SELECTED ).Get( resultProperty ) );
  DALI_TEST_EQUALS( resultProperty, "on-stage", TEST_LOCATION );

  popup.ConnectSignal( testTracker, "control-signal-cancel", ConfirmationPopupCancelTestFunctor() );

  // Check the cancel signal has not occurred yet.
  DALI_TEST_CHECK( gSignalReceivedOK );
  DALI_TEST_CHECK( !gSignalReceivedCancel );

  // Provoke the signal.
  Stage::GetCurrent().Add( popup );

  // Check the cancel signal has occurred.
  DALI_TEST_CHECK( gSignalReceivedOK );
  DALI_TEST_CHECK( gSignalReceivedCancel );

  END_TEST;
}

int UtcDaliConfirmationPopupDynamicSignalGenerationN(void)
{
  ToolkitTestApplication application;
   tet_infoline( " UtcDaliConfirmationPopupDynamicSignalGenerationN" );

   ConfirmationPopup popup = ConfirmationPopup::New();

   TextLabel titleActor = TextLabel::New( "Title" );
   popup.SetTitle( titleActor );

   TextLabel contentActor = TextLabel::New( "Content" );
   popup.SetContent( contentActor );

   Actor footerActor = Actor::New();

   PushButton buttonOK = PushButton::New();
   buttonOK.SetName( "control-ok-misnamed" );
   popup.SetFooter( buttonOK );

   // Tell the confirmation popup to connect to the signal in our button called "on-stage".
   popup.SetProperty( Toolkit::ConfirmationPopup::Property::CONNECT_SIGNAL_OK_SELECTED, "on-stage" );

   // Connect to the confirmation popup's OK signal.
   gSignalReceivedOK = false;

   // The connection will fail at this point as no actor with the name "control-ok" will be located.
   TestConnectionTrackerObject* testTracker = new TestConnectionTrackerObject();
   popup.ConnectSignal( testTracker, "control-signal-ok", ConfirmationPopupOKTestFunctor() );

   // Check no signal has occurred yet.
   DALI_TEST_CHECK( !gSignalReceivedOK );

   // Provoke the signal.
   Stage::GetCurrent().Add( popup );

   // Check the signal has still not occurred, as our button was incorrectly named.
   DALI_TEST_CHECK( !gSignalReceivedOK );

   END_TEST;
}

int UtcDaliConfirmationPopupTypeRegistryCreation(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliConfirmationPopupTypeRegistryCreation" );

  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "ConfirmationPopup" );
  DALI_TEST_CHECK( typeInfo )

  BaseHandle baseHandle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( baseHandle )

  Toolkit::Popup popup = Toolkit::Popup::DownCast( baseHandle );
  popup.SetProperty( Popup::Property::ANIMATION_DURATION, 0.0f );

  Stage::GetCurrent().Add( popup );
  popup.SetDisplayState( Toolkit::Popup::SHOWN );

  application.SendNotification();
  application.Render();

  // Check the popup is shown.
  DALI_TEST_EQUALS( popup.GetDisplayState(), Popup::SHOWN, TEST_LOCATION );

  END_TEST;
}



