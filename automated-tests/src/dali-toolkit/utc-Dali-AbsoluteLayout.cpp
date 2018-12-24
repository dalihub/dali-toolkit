/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_absolute_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_absolute_layoutg_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliLayouting_AbsoluteLayoutDownCast(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutDownCast - Testing Downcast");

  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();

  LayoutGroup layoutGroup( absoluteLayout );

  AbsoluteLayout absoluteLayoutCandidate = AbsoluteLayout::DownCast( layoutGroup );
  DALI_TEST_CHECK( absoluteLayoutCandidate );

  END_TEST;
}

int UtcDaliLayouting_AbsoluteLayoutAssignment(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutAssignment - Testing operator=");

  AbsoluteLayout absoluteLayout = AbsoluteLayout::New();
  AbsoluteLayout absoluteLayout2;

  absoluteLayout2 = absoluteLayout;

  DALI_TEST_CHECK( absoluteLayout2 == absoluteLayout );

  END_TEST;
}


int UtcDaliLayouting_AbsoluteLayout01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayout01 - Position an item with Actor::Property::POSITION");

  Stage stage = Stage::GetCurrent();
  auto absoluteLayout = Control::New();
  auto layout = AbsoluteLayout::New();
  layout.SetAnimateLayout( true );
  DevelControl::SetLayout( absoluteLayout, layout );
  absoluteLayout.SetName( "AsoluteLayout");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  // Position one of the  controls using the actor property.
  controls[1].SetProperty(Actor::Property::POSITION, Vector3( 100.0f, 0.0f, 0.0f) );

  for( auto&& iter : controls )
  {
    absoluteLayout.Add( iter );
  }
  absoluteLayout.SetParentOrigin( ParentOrigin::CENTER );
  absoluteLayout.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( absoluteLayout );

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // AbsoluteLayout renders items at the positions given by their Actor::Property::POSITION relative to the top left of the container.
  // Items can overlap or spill out of their parent container.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // The controls[1] was the only control to have a defiend position
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Items size should not change regardless of parent's size.
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[2].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[3].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

namespace
{

// Functor to test whether a Finish signal is emitted
struct AnimationFinishCheck
{
  AnimationFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Animation& animation)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if (!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if (mSignalReceived)
    {
      tet_printf("Unexpected Finish signal was received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

}

int UtcDaliLayouting_AbsoluteLayoutWithStandaloneAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutWithStandaloneAnimation ");

  Stage stage = Stage::GetCurrent();
  auto root = Control::New();
  auto layout = AbsoluteLayout::New();
  layout.SetAnimateLayout( false );
  DevelControl::SetLayout( root, layout );
  root.SetName( "AsoluteLayout");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );

  controls[0].SetProperty( Actor::Property::POSITION, Vector3( 100.0f, 100.0f, 0.0f ) );

  for( auto&& iter : controls )
  {
    root.Add( iter );
  }

  root.SetParentOrigin( ParentOrigin::CENTER );
  root.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( root );

  float durationSeconds = 1.0f;
  Animation animation = Animation::New( durationSeconds );
  Vector3 targetPosition( 200.0f, 200.0f, 0.0f );
  animation.AnimateTo(Property( controls[0], Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR );
  animation.Play();

  DALI_TEST_EQUALS( true, DevelActor::IsPositionOrSizeCurrentlyAnimating( controls[0] ), TEST_LOCATION );

  bool signalReceived( false );
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );

  application.SendNotification();
  //  0.1 of a second, 10 pixel delta
  application.Render( 100u );

  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 110.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( durationSeconds * 1000.0f ) - 100u + 1u );

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_EQUALS( false, DevelActor::IsPositionOrSizeCurrentlyAnimating( controls[0] ), TEST_LOCATION);
  DALI_TEST_EQUALS( targetPosition, controls[0].GetCurrentPosition(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_AbsoluteLayoutAndLayoutGroupWithStandaloneAnimation(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AbsoluteLayoutAndLayoutGroupWithStandaloneAnimation ");

  Stage stage = Stage::GetCurrent();
  auto root = Control::New();
  auto layout = AbsoluteLayout::New();
  layout.SetAnimateLayout( false );
  DevelControl::SetLayout( root, layout );
  root.SetName( "AsoluteLayout" );

  Control container = CreateLeafControl( 200, 200 );
  DevelControl::SetLayoutingRequired( container, true );
  container.SetName( "Container" );
  root.Add( container );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls[0].SetProperty( Actor::Property::POSITION, Vector3( 100.0f, 100.0f, 0.0f ) );

  for( auto&& iter : controls )
  {
    container.Add( iter );
  }

  root.SetParentOrigin( ParentOrigin::CENTER );
  root.SetAnchorPoint( AnchorPoint::CENTER );
  stage.Add( root );

  float durationSeconds = 1.0f;
  Animation animation = Animation::New( durationSeconds );
  Vector3 targetPosition( 200.0f, 200.0f, 0.0f );
  animation.AnimateTo(Property( controls[0], Actor::Property::POSITION ), targetPosition, AlphaFunction::LINEAR );
  animation.Play();

  DALI_TEST_EQUALS( true, DevelActor::IsPositionOrSizeCurrentlyAnimating( controls[0] ), TEST_LOCATION );

  bool signalReceived( false );
  AnimationFinishCheck finishCheck( signalReceived );
  animation.FinishedSignal().Connect( &application, finishCheck );

  application.SendNotification();
  //  0.1 of a second, 10 pixel delta
  application.Render( 100u );

  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 110.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( durationSeconds * 1000.0f ) - 100u + 1u );

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();

  DALI_TEST_EQUALS( false, DevelActor::IsPositionOrSizeCurrentlyAnimating( controls[0] ), TEST_LOCATION);
  DALI_TEST_EQUALS( targetPosition, controls[0].GetCurrentPosition(), TEST_LOCATION );

  END_TEST;
}
