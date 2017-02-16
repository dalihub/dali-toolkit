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
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>

using namespace Dali;
using namespace Toolkit;

void dali_textselectionpopup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textselectionpopup_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliToolkitTextSelectionPopupNewP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;

  DALI_TEST_CHECK( !textSelectionPopup );

  textSelectionPopup = TextSelectionPopup::New( NULL );

  DALI_TEST_CHECK( textSelectionPopup );
  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupConstructorP(void)
{
  TextSelectionPopup textSelectionPopup;

  DALI_TEST_CHECK( !textSelectionPopup );

  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupCopyConstructorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;

  textSelectionPopup = TextSelectionPopup::New( NULL );
  TextSelectionPopup copy( textSelectionPopup );

  DALI_TEST_CHECK( copy == textSelectionPopup );

  END_TEST;
}


int UtcDaliToolkitTextSelectionPopupDestructorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup* textSelectionPopup = new TextSelectionPopup;
  delete textSelectionPopup;

  DALI_TEST_CHECK( true );

  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;
  textSelectionPopup = TextSelectionPopup::New(  NULL );
  TextSelectionPopup copy;
  copy = textSelectionPopup;

  DALI_TEST_CHECK( copy == textSelectionPopup );
  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupDownCastP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;
  textSelectionPopup = TextSelectionPopup::New( NULL );

  TextSelectionPopup cast = TextSelectionPopup::DownCast( textSelectionPopup );

  DALI_TEST_CHECK( cast );

  END_TEST;
}

// TextSelectionToolBar is used TextSelectionPopup, below tests it individually

int UtcDaliToolkitTextSelectionToolBarP(void)
{
  // Creates Toolbar, adds 2 options and a divider then resizes divider
  ToolkitTestApplication application;

  TextSelectionToolbar toolbar = TextSelectionToolbar::New();

  toolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, Size( 100.0f, 60.0f) );

  Toolkit::PushButton option = Toolkit::PushButton::New();
  option.SetName( "test-option" );
  option.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  toolbar.AddOption( option );

  Toolkit::Control divider = Toolkit::Control::New();
  divider.SetSize( 2.0f, 0.0f );
  divider.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
  toolbar.AddDivider( divider );

  Toolkit::PushButton option2 = Toolkit::PushButton::New();
  option2.SetName( "test-option-2" );
  option2.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  toolbar.AddOption( option2 );

  Size newSize =  Size(3.0f, 0.0f);
  toolbar.ResizeDividers( newSize );

  DALI_TEST_CHECK( toolbar );
  END_TEST;
}

int UtcDaliToolkitTextSelectionToolBarScrollBarP(void)
{
  // Creates Toolbar, adds 2 options and a divider then resizes divider
  ToolkitTestApplication application;

  TextSelectionToolbar toolbar = TextSelectionToolbar::New();

  toolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::MAX_SIZE, Size( 100.0f, 60.0f) );

  Toolkit::PushButton option = Toolkit::PushButton::New();
  option.SetName( "test-option" );
  option.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  toolbar.AddOption( option );

  // Add a scroll-bar
  toolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::ENABLE_SCROLL_BAR, true );

  bool enabled = toolbar.GetProperty<bool>( Toolkit::TextSelectionToolbar::Property::ENABLE_SCROLL_BAR );
  DALI_TEST_CHECK( enabled );

  DALI_TEST_CHECK( toolbar );
  END_TEST;
}
