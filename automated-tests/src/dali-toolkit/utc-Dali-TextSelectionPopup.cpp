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

namespace
{

const char* TEST_IMAGE_FILE_NAME = "selection-popup-border.9.png";

}

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

int UtcDaliToolkitTextSelectionPopupBackgroundBorderP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;
  textSelectionPopup = TextSelectionPopup::New( NULL );

  textSelectionPopup.SetProperty( TextSelectionPopup::Property::BACKGROUND_BORDER,
                                  Property::Map().Add( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME ) );

  Property::Value value = textSelectionPopup.GetProperty( TextSelectionPopup::Property::BACKGROUND_BORDER );

  Property::Map map;
  value.Get( map );

  Property::Value* returnValue = map.Find( Dali::Toolkit::ImageVisual::Property::URL );
  DALI_TEST_CHECK( NULL != returnValue );

  if( returnValue )
  {
    std::string url;
    returnValue->Get( url );
    DALI_TEST_EQUALS( TEST_IMAGE_FILE_NAME, url, TEST_LOCATION );
  }

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

int UtcDaliToolkitTextSelectionToolBarScrollView(void)
{
  // Configures the ScrollView within the TextSelectionToolbar
  ToolkitTestApplication application;

  TextSelectionToolbar toolbar = TextSelectionToolbar::New();
  DALI_TEST_CHECK( toolbar );
  Stage::GetCurrent().Add( toolbar );

  Property::Map map;
  map["overshootEffectColor"] = Color::RED;
  map["overshootSize"] = Vector2(50.0f, 50.f);
  toolbar.SetProperty( Toolkit::TextSelectionToolbar::Property::SCROLL_VIEW, map );

  application.SendNotification();
  application.Render();

  Actor actor = toolbar.FindChildByName("TextSelectionScrollView");
  DALI_TEST_CHECK( actor );

  ScrollView scrollView = ScrollView::DownCast( actor );
  DALI_TEST_CHECK( scrollView );

  Vector4 color = scrollView.GetProperty<Vector4>( Toolkit::Scrollable::Property::OVERSHOOT_EFFECT_COLOR );
  DALI_TEST_EQUALS( color, Color::RED, TEST_LOCATION );

  Vector2 size = scrollView.GetProperty<Vector2>( Toolkit::Scrollable::Property::OVERSHOOT_SIZE );
  DALI_TEST_EQUALS( size, Vector2(50.0f, 50.f), TEST_LOCATION );

  END_TEST;
}

