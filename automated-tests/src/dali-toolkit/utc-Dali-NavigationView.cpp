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
#include <sstream>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>

using namespace Dali;
using namespace Toolkit;

void dali_navigationView_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_navigationView_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliNavigationTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "NavigationView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  NavigationView view = NavigationView::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}

int UtcDaliNavigationViewNew(void)
{
  ToolkitTestApplication application;

  NavigationView navigationView;
  DALI_TEST_CHECK( !navigationView );

  navigationView = NavigationView::New();
  DALI_TEST_CHECK( navigationView );

  Stage::GetCurrent().Add( navigationView );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliNavigationViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  NavigationView view = NavigationView::New();
  DALI_TEST_CHECK( view );

  NavigationView copy( view );
  DALI_TEST_CHECK( copy == view );

  NavigationView assign;
  DALI_TEST_CHECK( !assign );
  assign = view;
  DALI_TEST_CHECK( assign == view );

  // Self assignment
  assign = assign;
  DALI_TEST_CHECK( assign );
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliNavigationViewDownCast(void)
{
  ToolkitTestApplication application;

  BaseHandle view = NavigationView::New();
  DALI_TEST_CHECK( NavigationView::DownCast( view ) );

  BaseHandle empty;
  DALI_TEST_CHECK( ! NavigationView::DownCast( empty ) );

  BaseHandle another = Actor::New();
  DALI_TEST_CHECK( ! NavigationView::DownCast( another ) );

  END_TEST;
}

int UtcDaliNavigationViewPush(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  // 1 Create and Add Navigation View to stage, actor count should be zero
  NavigationView naviView = NavigationView::New();
  stage.Add( naviView );

  DALI_TEST_EQUALS( naviView.GetChildCount(), 0,  TEST_LOCATION );

  // 2 Add Actor to Navigation View, actor count should increase to 1

  Actor TestParentActor1 = Actor::New();
  naviView.Push( TestParentActor1 );

  DALI_TEST_EQUALS( naviView.GetChildCount(), 1,  TEST_LOCATION );

  END_TEST;
}

int UtcDaliNavigationViewPop(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  // 1 Create Navigation View
  NavigationView naviView = NavigationView::New();
  stage.Add( naviView );

  // 2 Push initial Actor
  Actor testParentActor1 = Actor::New();
  testParentActor1.SetName("TestParentActor1");
  naviView.Push( testParentActor1 );
  DALI_TEST_EQUALS( naviView.GetChildCount(), 1 ,  TEST_LOCATION );

  // 3 Push Second Actor which contains a child actor
  Actor testParentActor2 = Actor::New();
  testParentActor2.SetName("TestParentActor2");
  Actor testChildActor1 = Actor::New();
  testParentActor2.Add( testChildActor1 );
  naviView.Push( testParentActor2 );


  // 4 Pop head actor, it should be TestParentActor2
  Actor poppedActor = naviView.Pop();
  DALI_TEST_EQUALS( poppedActor.GetName() ,  "TestParentActor2", TEST_LOCATION );

  // 5 Navigation View child count should be 1
  DALI_TEST_EQUALS( naviView.GetChildCount(), 1 ,  TEST_LOCATION );


  END_TEST;
}

int UtcDaliNavigationViewPushAndPop(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  // 1 Create Navigation View
  NavigationView naviView = NavigationView::New();
  stage.Add( naviView );

  // 2 Push initial Actor
  Actor testParentActor1 = Actor::New();
  testParentActor1.SetName("TestParentActor1");
  naviView.Push( testParentActor1 );
  DALI_TEST_EQUALS( naviView.GetChildCount(), 1 ,  TEST_LOCATION );

  // 3 Push Second Actor which contains a child actor
  Actor testParentActor2 = Actor::New();
  testParentActor2.SetName("TestParentActor2");
  Actor testChildActor1 = Actor::New();
  testParentActor2.Add( testChildActor1 );
  naviView.Push( testParentActor2 );

  // 3 Push third Actor which contains a child actor
  Actor testParentActor3 = Actor::New();
  testParentActor3.SetName("TestParentActor3");
  Actor testChildActor2 = Actor::New();
  testParentActor2.Add( testChildActor2 );
  naviView.Push( testParentActor3 );

  // 4 Pop head actor,  it should be TestParentActor3
  Actor poppedActor = naviView.Pop();
  DALI_TEST_EQUALS( poppedActor.GetName() ,  "TestParentActor3", TEST_LOCATION );

  // 5 Pop head actor,  it should be TestParentActor2
  Actor poppedActor2 = naviView.Pop();
  DALI_TEST_EQUALS( poppedActor2.GetName() ,  "TestParentActor2", TEST_LOCATION );


  END_TEST;
}

int UtcDaliNavigationViewPreventLastPop(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  // 1 Create Navigation View
  NavigationView naviView = NavigationView::New();
  stage.Add( naviView );

  // 2 Push initial Actor
  Actor testParentActor1 = Actor::New();
  testParentActor1.SetName("TestParentActor1");
  naviView.Push( testParentActor1 );
  DALI_TEST_EQUALS( naviView.GetChildCount(), 1 ,  TEST_LOCATION );

  // 3 Push Second Actor which contains a child actor
  Actor testParentActor2 = Actor::New();
  testParentActor2.SetName("TestParentActor2");
  Actor testChildActor1 = Actor::New();
  testParentActor2.Add( testChildActor1 );
  naviView.Push( testParentActor2 );

  // 4 Pop head actor, it should be TestParentActor2
  Actor poppedActor1 = naviView.Pop();
  DALI_TEST_EQUALS( poppedActor1.GetName() ,  "TestParentActor2", TEST_LOCATION );


  // 5 Try to Pop head actor, Should be empty hence can not get name of Actor
  Actor poppedActorEmpty = naviView.Pop();

  try
  {
    const std::string hasNoName = poppedActorEmpty.GetName();
    tet_infoline( hasNoName.c_str() );
    DALI_TEST_CHECK( false ); // should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }


  END_TEST;
}
