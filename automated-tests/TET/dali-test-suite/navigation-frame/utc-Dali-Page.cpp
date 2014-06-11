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

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/public-api/controls/navigation-frame/page.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/public-api/controls/popup/popup.h>
#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

namespace
{

static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

}

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliPageNew();
static void UtcDaliPageDownCast();
static void UtcDaliPageSetGetTitle();
static void UtcDaliPageSetGetSubTitle();
static void UtcDaliPageSetGetTitleIcon();
static void UtcDaliPageAddGetToolBarControl();
static void UtcDaliPageAddGetTitleBarControl();
static void UtcDaliPageSetGetPopupMenu();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliPageNew, POSITIVE_TC_IDX },
    { UtcDaliPageDownCast, POSITIVE_TC_IDX },
    { UtcDaliPageSetGetTitle, POSITIVE_TC_IDX },
    { UtcDaliPageSetGetSubTitle, POSITIVE_TC_IDX },
    { UtcDaliPageSetGetTitleIcon, POSITIVE_TC_IDX },
    { UtcDaliPageAddGetToolBarControl, POSITIVE_TC_IDX },
    { UtcDaliPageAddGetTitleBarControl, POSITIVE_TC_IDX },
    { UtcDaliPageSetGetPopupMenu, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


static void UtcDaliPageNew()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliPageNew");

  Page naviItem;
  // Check that this handle is uninitialized
  DALI_TEST_CHECK( !naviItem );

  naviItem = Page::New();
  // Check that the Dali resource is successfully created
  DALI_TEST_CHECK( naviItem );

  Page naviItem2( naviItem );
  DALI_TEST_CHECK( naviItem2 == naviItem );

  // Additional check to ensure the object is created by checking whether it is registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );
  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( TestCallback );
  {
    Page naviItem = Page::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
}

static void UtcDaliPageDownCast()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageDownCast" );

  Page naviItem = Page::New();
  BaseHandle handle( naviItem );

  Page newNaviItem = Page::DownCast( handle );
  DALI_TEST_CHECK( naviItem );
  DALI_TEST_CHECK( newNaviItem == naviItem );
}

static void UtcDaliPageSetGetTitle()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageSetGetTitle" );

  Page naviItem = Page::New();
  DALI_TEST_CHECK( naviItem.GetTitle().empty() );

  std::string str( "ItemTitle" );
  naviItem.SetTitle( str );
  DALI_TEST_CHECK( naviItem.GetTitle() == str );
}

static void UtcDaliPageSetGetSubTitle()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageSetGetSubTitle" );

  Page naviItem = Page::New();
  DALI_TEST_CHECK( naviItem.GetSubTitle().empty() );

  std::string str( "ItemSubTitle" );
  naviItem.SetSubTitle( str );
  DALI_TEST_CHECK( naviItem.GetSubTitle() == str );
}

static void UtcDaliPageSetGetTitleIcon()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageSetGetTitleIcon" );

  Page naviItem = Page::New();
  DALI_TEST_CHECK( !naviItem.GetTitleIcon() );

  Actor titleIcon = Actor::New();
  naviItem.SetTitleIcon( titleIcon );
  DALI_TEST_CHECK( naviItem.GetTitleIcon() == titleIcon );
}

static void UtcDaliPageAddGetToolBarControl()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageAddGetToolBarControl" );

  Page naviItem = Page::New();
  Page::ControlOnBarContainer container = naviItem.GetControlsOnToolBar();
  // Check that the container is empty in the beginning
  DALI_TEST_CHECK( container.empty() );

  // Add control, check whether adding successfully, also check the container size
  PushButton firstControl = PushButton::New();
  DALI_TEST_CHECK( naviItem.AddControlToToolBar(firstControl, Alignment::HorizontalLeft) );
  container = naviItem.GetControlsOnToolBar();
  DALI_TEST_CHECK( container.size() == 1 );

  // Add control, check whether adding successfully, also check the container size
  PushButton secondControl = PushButton::New();
  DALI_TEST_CHECK( naviItem.AddControlToToolBar(secondControl, Alignment::HorizontalCenter) );
  container = naviItem.GetControlsOnToolBar();
  DALI_TEST_CHECK( container.size() == 2 );

  // The control adding fails, as the alignment is not HorizontalLeft/HorizontalCenter/HorizontalRight
  PushButton thirdControl = PushButton::New();
  DALI_TEST_CHECK( !naviItem.AddControlToToolBar(thirdControl, Alignment::VerticalCenter) );
  container = naviItem.GetControlsOnToolBar();
  DALI_TEST_CHECK( container.size() == 2 );

  // Add control, check whether adding successfully, also check the container size
  PushButton fourthControl = PushButton::New();
  DALI_TEST_CHECK( naviItem.AddControlToToolBar(fourthControl, Alignment::HorizontalRight) );
  container = naviItem.GetControlsOnToolBar();
  DALI_TEST_CHECK( container.size() == 3 );

  // The control adding fails, as the control itself is uninitialized
  PushButton fifthControl;
  DALI_TEST_CHECK( !naviItem.AddControlToToolBar(fifthControl, Alignment::HorizontalCenter) );
  container = naviItem.GetControlsOnToolBar();
  DALI_TEST_CHECK( container.size() == 3 );

  // check the content of the three successfully added ControlOnBar objects
  DALI_TEST_CHECK( container[0]->control == firstControl );
  DALI_TEST_CHECK( container[0]->alignment == Alignment::HorizontalLeft );
  DALI_TEST_CHECK( container[1]->control == secondControl );
  DALI_TEST_CHECK( container[1]->alignment == Alignment::HorizontalCenter );
  DALI_TEST_CHECK( container[2]->control == fourthControl );
  DALI_TEST_CHECK( container[2]->alignment == Alignment::HorizontalRight );
}

static void UtcDaliPageAddGetTitleBarControl()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageAddGetTitleBarControl" );

  Page naviItem = Page::New();
  ActorContainer container = naviItem.GetControlsOnTitleBar();
  // Check that the container is empty in the beginning
  DALI_TEST_CHECK( container.empty() );

  // Add control, check whether adding successfully, also check the container size
  PushButton firstControl = PushButton::New();
  DALI_TEST_CHECK( naviItem.AddControlToTitleBar(firstControl) );
  container = naviItem.GetControlsOnTitleBar();
  DALI_TEST_CHECK( container.size() == 1 );

  // The control adding fails, as the control itself is uninitialized
  PushButton secondControl;
  DALI_TEST_CHECK( !naviItem.AddControlToTitleBar(secondControl) );
  container = naviItem.GetControlsOnTitleBar();
  DALI_TEST_CHECK( container.size() == 1 );

  // Add control, check whether adding successfully, also check the container size
  PushButton thirdControl = PushButton::New();
  DALI_TEST_CHECK( naviItem.AddControlToTitleBar(thirdControl) );
  container = naviItem.GetControlsOnTitleBar();
  DALI_TEST_CHECK( container.size() == 2 );

  // check the content of the three successfully added Controls
  DALI_TEST_CHECK( container[0] == firstControl );
  DALI_TEST_CHECK( container[1] == thirdControl );
}

static void UtcDaliPageSetGetPopupMenu()
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliPageSetGetPopupMenu" );

  Page naviItem = Page::New();
  DALI_TEST_CHECK( !naviItem.GetPopupMenu() );

  Toolkit::Popup menu = Toolkit::Popup::New();
  naviItem.SetPopupMenu( menu );
  DALI_TEST_CHECK( menu == naviItem.GetPopupMenu() );
}
