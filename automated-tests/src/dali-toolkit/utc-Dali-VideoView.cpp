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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TEST_FILE( "test.mp4" );
const char* const VOLUME_LEFT( "volumeLeft" );
const char* const VOLUME_RIGHT( "volumeRight" );

}

void video_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void video_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Negative test case for a method
int UtcDaliVideoViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewUninitialized");

  Toolkit::VideoView view;

  try
  {
    // New() must be called to create a VideoView or it wont be valid.
    Actor a = Actor::New();
    view.Add( a );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(!view);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewNew");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  Toolkit::VideoView view2 = Toolkit::VideoView::New( "" );
  DALI_TEST_CHECK( view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewDownCast");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  BaseHandle handle(view);

  Toolkit::VideoView view2 = Toolkit::VideoView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( view2 );
  DALI_TEST_CHECK( view == view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewProperty1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty1");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  std::string file;
  view.SetProperty( VideoView::Property::VIDEO, TEST_FILE );
  Property::Value val = view.GetProperty( VideoView::Property::VIDEO );
  DALI_TEST_CHECK( val.Get( file ) );
  DALI_TEST_CHECK( file == TEST_FILE );
  END_TEST;
}

int UtcDaliVideoViewProperty2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty2");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  bool looping;
  Property::Value val = view.GetProperty( VideoView::Property::LOOPING );
  DALI_TEST_CHECK( val.Get( looping ) );
  DALI_TEST_CHECK( !looping );

  view.SetProperty( VideoView::Property::LOOPING, true );
  val = view.GetProperty( VideoView::Property::LOOPING );
  DALI_TEST_CHECK( val.Get( looping ) );
  DALI_TEST_CHECK( looping );
  END_TEST;
}

int UtcDaliVideoViewProperty3(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty3");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  bool muted;
  Property::Value val = view.GetProperty( VideoView::Property::MUTED );
  DALI_TEST_CHECK( val.Get( muted ) );
  DALI_TEST_CHECK( !muted );

  view.SetProperty( VideoView::Property::MUTED, true );
  val = view.GetProperty( VideoView::Property::MUTED );
  DALI_TEST_CHECK( val.Get( muted ) );
  DALI_TEST_CHECK( muted );
  END_TEST;
}

int UtcDaliVideoViewProperty4(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty4");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  float left, right;
  left = right = 0.f;

  Property::Map map;
  map.Insert( VOLUME_LEFT, 1.0f );
  map.Insert( VOLUME_RIGHT, 0.5f );

  Property::Map map2;
  view.SetProperty( VideoView::Property::VOLUME, map );
  Property::Value val4 = view.GetProperty( VideoView::Property::VOLUME );
  DALI_TEST_CHECK( val4.Get( map2 ) );

  Property::Value* volumeLeft = map.Find( VOLUME_LEFT );
  Property::Value* volumeRight = map.Find( VOLUME_RIGHT );

  DALI_TEST_CHECK( volumeLeft && volumeLeft->Get( left ) );
  DALI_TEST_CHECK( volumeRight && volumeRight->Get( right ) );
  DALI_TEST_CHECK( left == 1.0f );
  DALI_TEST_CHECK( right == 0.5f );

  END_TEST;
}

int UtcDaliVideoViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  VideoView copy( view );
  DALI_TEST_CHECK( view == copy );

  VideoView assign;
  DALI_TEST_CHECK( !assign );

  assign = copy;
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliVideoViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "VideoView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  VideoView view = VideoView::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}
