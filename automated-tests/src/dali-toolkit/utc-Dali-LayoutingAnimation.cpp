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
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>
#include <dali-toolkit/devel-api/layouting/grid.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

#include <../custom-layout.h>
#include <dummy-control.h>

#include <layout-utils.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_layouting_animation_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_layouting_animation_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

// Functor to test whether a Finish signal is emitted
struct LayoutTransitionFinishCheck
{
  LayoutTransitionFinishCheck( bool& signalReceived )
  : mSignalReceived( signalReceived )
  {
  }

  void operator()( LayoutTransitionData::Type type, LayoutTransitionData& layoutTransitionData )
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

} // anon namespace

int UtcDaliLayouting_LayoutTransitionDataConstructorP(void)
{
  TestApplication application;

  LayoutTransitionData layoutTransitionData;

  DALI_TEST_CHECK( !layoutTransitionData );
  END_TEST;
}

int UtcDaliLayouting_LayoutTransitionDataNewP(void)
{
  TestApplication application;

  LayoutTransitionData layoutTransitionData = LayoutTransitionData::New();

  DALI_TEST_CHECK(layoutTransitionData);
  END_TEST;
}

int UtcDaliLayouting_LayoutTransitionDataDownCastP(void)
{
  TestApplication application;

  LayoutTransitionData layoutTransitionData = LayoutTransitionData::New();
  BaseHandle object(layoutTransitionData);

  LayoutTransitionData layoutTransitionData2 = LayoutTransitionData::DownCast(object);
  DALI_TEST_CHECK(layoutTransitionData2);

  LayoutTransitionData layoutTransitionData3 = DownCast< LayoutTransitionData >(object);
  DALI_TEST_CHECK(layoutTransitionData2);
  END_TEST;
}

int UtcDaliLayouting_LayoutTransitionDataDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  LayoutTransitionData layoutTransitionData1 = LayoutTransitionData::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !layoutTransitionData1 );

  LayoutTransitionData layoutTransitionData2 = DownCast< LayoutTransitionData >( unInitializedObject );
  DALI_TEST_CHECK( !layoutTransitionData2 );
  END_TEST;
}

int UtcDaliLayouting_LayoutTransitionDataSetGetTransition(void)
{
  TestApplication application;

  auto layout = LinearLayout::New();
  auto layoutTransitionData = LayoutTransitionData::New();

  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_ADD ) == layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_OWNER_SET ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE ) == LayoutTransitionData() );

  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, LayoutTransitionData() );
  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE, layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_ADD ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE ) == layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_OWNER_SET ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE ) == LayoutTransitionData() );

  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE, LayoutTransitionData() );
  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS, layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_ADD ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS ) == layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_OWNER_SET ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE ) == LayoutTransitionData() );

  layout.SetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS, LayoutTransitionData() );
  layout.SetTransitionData( LayoutTransitionData::ON_OWNER_SET, layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_ADD ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_OWNER_SET ) == layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE ) == LayoutTransitionData() );

  layout.SetTransitionData( LayoutTransitionData::ON_OWNER_SET, LayoutTransitionData() );
  layout.SetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE, layoutTransitionData );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_ADD ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_OWNER_SET ) == LayoutTransitionData() );
  DALI_TEST_CHECK( layout.GetTransitionData( LayoutTransitionData::ON_LAYOUT_CHANGE ) == layoutTransitionData );

  END_TEST;
}

int UtcDaliLayouting_SetLayoutTransition01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_SetLayoutTransition01" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  auto verticalLayout = LinearLayout::New();
  verticalLayout.SetAnimateLayout( false );
  verticalLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );
  for( auto&& iter : controls )
  {
    container.Add( iter );
  }
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map["property"] = "size";
    map["targetValue"] = Property::Value(); // capture from layout update
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
        .Add("delay", 0.0f)
        .Add("duration", 0.0f));
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    Property::Map map;
    map["property"] = "opacity";
    map["initialValue"] = 0.0f;
    map["targetValue"] = 1.0f;
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
        .Add("delay", 0.0f)
        .Add("duration", 0.5f));
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for children
    Property::Map map;
    map["property"] = "position";
    map["animator"] = Property::Map()
       .Add("alphaFunction", "LINEAR")
       .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 0.0f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Grow children from (0,0) size to full size (captured)
    Property::Map map;
    map["property"] = "size";
    map["initialValue"] = Vector3( 0.0f, 0.0f, 0 );
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
        .Add("delay", 0.0f)
        .Add("duration", 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map );
  }

  // Ensure layouting happens
  application.SendNotification();
  application.Render();

  // First round, no animation
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  horizontalLayout.SetAnimateLayout( true );
  verticalLayout.SetAnimateLayout( true );

  // Change a layout, start transition
  verticalLayout.SetTransitionData( LayoutTransitionData::ON_OWNER_SET, layoutTransitionData );
  DevelControl::SetLayout( container, verticalLayout );

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // Second round, animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( container.GetCurrentOpacity(), 0.0f, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 0.0f, 0.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 0.0f, 0.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // Third round, animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( container.GetCurrentOpacity(), 1.0f, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  finishCheck.CheckSignalReceived();

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  // Transition back now with default transition
  DevelControl::SetLayout( container, horizontalLayout );

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( container.GetCurrentOpacity(), 1.0f, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 300.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 0.0f, 400.0f, 0.0f ), 1.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( container.GetCurrentOpacity(), 1.0f, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for complete default animation */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_AddChildLayoutTransition01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AddChildLayoutTransition01" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( true );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = "size";
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for a child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::NONE;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = "position";
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Grow a child from (0,0) size to full size (captured)
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_ADD;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = "size";
    map[ LayoutTransitionData::AnimatorKey::INITIAL_VALUE ] = Vector3( 0.0f, 0.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to an added child
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  container.Add( controls[0] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // The animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // The animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_RemoveChildLayoutTransition01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_RemoveChildLayoutTransition01" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  stage.Add( container );
  container.Add( controls[0] );
  container.Add( controls[1] );

  // Initial rendering done
  application.SendNotification();
  application.Render();

  horizontalLayout.SetAnimateLayout( true );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent width
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_WIDTH;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant resize for parent height
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_HEIGHT;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for parent X position
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION_X;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for parent Y position
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION_Y;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Shrink the children
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_WIDTH;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 0.0f;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map );
  }
  {
    // Shrink the children
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_HEIGHT;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 0.0f;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map );
  }
  {
    // Shrink the removed child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_REMOVE;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Size( 0.0f, 0.0f );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map );
  }
  {
    // Instant position for children
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION_X;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Instant position for children
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION_Y;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_TO" )
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_REMOVE, layoutTransitionData );
  container.Remove( controls[1] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // Animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );
  // this control is already removed from the tree.
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // Animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  // this control is already removed from the tree.
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 0.0f, 0.0f, 0.0f ), 1.0f, TEST_LOCATION );
  // this control is already removed from the tree.
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  // this control is already removed from the tree.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  // this control is already removed from the tree.
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_FocusChildLayoutTransition01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FocusChildLayoutTransition01" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  stage.Add( container );
  container.Add( controls[0] );
  container.Add( controls[1] );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  manager.SetFocus( controls[0] );

  // Initial rendering done
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  horizontalLayout.SetAnimateLayout( true );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Shrink the lost focus child
    Property::Map map;
    map[ "affectsSiblings" ] = false;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_LOST;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 80.0f, 80.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus lost child
  }
  {
    // Grow the gained focus child
    Property::Map map;
    map[ "affectsSiblings" ] = false;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_GAINED;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 120.0f, 120.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus gained child
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS, layoutTransitionData );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);
  manager.SetFocus( controls[1] );

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // Animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // Animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 80.0f, 80.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 120.0f, 120.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_FocusChildLayoutTransition02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FocusChildLayoutTransition02" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  stage.Add( container );
  container.Add( controls[0] );
  container.Add( controls[1] );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  manager.SetFocus( controls[0] );

  // Initial rendering done
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  horizontalLayout.SetAnimateLayout( true );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Shrink the lost focus child width
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = false;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // move all children
  }
  {
    // Shrink the lost focus child width
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_WIDTH;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_LOST;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 80.0f;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus lost child
  }
  {
    // Shrink the lost focus child height
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE_HEIGHT;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_LOST;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 80.0f;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus lost child
  }
  {
    // Grow the gained focus child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_GAINED;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 120.0f, 120.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus gained child
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS, layoutTransitionData );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);
  manager.SetFocus( controls[1] );

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // Animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // Animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 80.0f, 340.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 80.0f, 80.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 120.0f, 120.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 360.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 80.0f, 340.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 120.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_FocusChildLayoutTransition03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_FocusChildLayoutTransition03" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  stage.Add( container );
  container.Add( controls[0] );
  container.Add( controls[1] );

  KeyInputFocusManager manager = KeyInputFocusManager::Get();
  manager.SetFocus( controls[0] );

  // Initial rendering done
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  horizontalLayout.SetAnimateLayout( true );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Shrink the lost focus child width
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = false;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // move all children
  }
  {
    // Shrink the lost focus child width
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SCALE_X;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_LOST;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 0.8f;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus lost child
  }
  {
    // Shrink the lost focus child height
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SCALE_Y;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_LOST;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = 0.8f;
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus lost child
  }
  {
    // Grow the gained focus child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS ] = true;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SCALE;
    map[ LayoutTransitionData::AnimatorKey::CONDITION ] = LayoutTransitionData::Condition::ON_FOCUS_GAINED;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 1.2f, 1.2f, 1.0f );
    map[ LayoutTransitionData::AnimatorKey::TYPE ] = LayoutTransitionData::Animator::ANIMATE_TO;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
         .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
           .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
           .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to on focus gained child
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_FOCUS, layoutTransitionData );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);
  manager.SetFocus( controls[1] );

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // Animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 100.0f, 350.0f, 0.0f ), 1.0f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // Animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( -10.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 90.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize() * controls[0].GetCurrentScale(), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize() * controls[1].GetCurrentScale(), Vector3( 120.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( -10.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 90.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ) * controls[0].GetProperty<Vector3>( Actor::Property::SCALE ), Vector3( 80.0f, 80.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ) * controls[1].GetProperty<Vector3>( Actor::Property::SCALE ), Vector3( 120.0f, 120.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_AddChildLayoutTransition02_KeyFrames(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AddChildLayoutTransition02_KeyFrames" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( true );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for a child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Grow a child from (0,0) size to full size with key frames
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add( 0.0f, Vector3( 0.0f, 0.0f, 0 ) );
    keyFrames.Add( 0.5f, Vector3( 100.0f, 100.0f, 0 ) );

    Property::Map map;
    map[ "property" ] = Actor::Property::SIZE;
    map[ "condition" ] = LayoutTransitionData::Condition::ON_ADD;
    map[ "animator" ] = Property::Map()
      .Add( "type", "ANIMATE_BETWEEN")
      .Add( "alphaFunction", "LINEAR")
      .Add( "timePeriod", Property::Map()
        .Add( "delay", 0.0f)
        .Add( "duration", 0.5f));
    layoutTransitionData.AddPropertyAnimator( Actor(), map, keyFrames, Animation::Interpolation::Linear );
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  container.Add( controls[0] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // The animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // The animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_AddChildLayoutTransition03_Path(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AddChildLayoutTransition03_Path" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( true );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }

  Dali::Path path = Dali::Path::New();
  {
    // Curve position for a child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_PATH")
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f ) );

    // Build the path
    Vector3 position0( 30.0,  80.0,  0.0 );
    Vector3 position1( 70.0,  120.0, 0.0 );
    Vector3 position2( 0.0, 350.0, 0.0 );

    //Dali::Path path = Dali::Path::New();
    path.AddPoint( position0 );
    path.AddPoint( position1 );
    path.AddPoint( position2 );

    // Control points for first segment
    path.AddControlPoint( Vector3( 39.0,  90.0, 0.0 ) );
    path.AddControlPoint( Vector3( 56.0, 119.0, 0.0 ) );

    // Control points for second segment
    path.AddControlPoint( Vector3( 78.0, 120.0, 0.0 ) );
    path.AddControlPoint( Vector3( 93.0, 104.0, 0.0 ) );

    layoutTransitionData.AddPropertyAnimator( controls[0], map, path, Vector3::XAXIS );
  }
  {
    // Grow a child from (0,0) size to full size (captured)
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = "size";
    map[ LayoutTransitionData::AnimatorKey::INITIAL_VALUE ] = Vector3( 0.0f, 0.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( controls[0], map );
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  container.Add( controls[0] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 0 );

  finishCheck.CheckSignalNotReceived();
  // The animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  Vector3 position, tangent;
  Quaternion rotation;
  path.Sample( 0.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), position,  0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentOrientation(), rotation, 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  path.Sample( 1.0f, position, tangent );
  rotation = Quaternion( Vector3::XAXIS, tangent );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), position, 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentOrientation(), rotation, 0.0001f, TEST_LOCATION );

  // The animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_AddChildLayoutTransition04_AnimateBy(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AddChildLayoutTransition04_AnimateBy" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( true );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for a child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Vector3( 10.0f, 10.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::TYPE, "ANIMATE_BY")
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR")
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f)
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.5f));
    layoutTransitionData.AddPropertyAnimator( controls[0], map );
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  container.Add( controls[0] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // The animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // The animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 110.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 110.0f, 110.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_AddChildLayoutTransition05(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_AddChildLayoutTransition05" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( true );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container");

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  stage.Add( container );

  auto layoutTransitionData = LayoutTransitionData::New();
  {
    // Instant resize for parent
    Property::Map map;
    map[ "property" ] = Actor::Property::SIZE;
    map[ "targetValue" ] = Property::Value(); // capture from layout update
    map[ "animator" ] = Property::Map()
      .Add( "name", "InstantAnimator" )
      .Add( "type", "ANIMATE_TO")
      .Add( "alphaFunction", "LINEAR" )
      .Add( "timePeriod", Property::Map()
        .Add( "delay", 0.0f )
        .Add( "duration", 0.0f ) );
    layoutTransitionData.AddPropertyAnimator( container, map );
  }
  {
    // Instant position for a child
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::COLOR_ALPHA;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = "InstantAnimator";
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  {
    // Grow a child from (0,0) size to full size (captured)
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::INITIAL_VALUE ] = Vector3( 0.0f, 0.0f, 0 );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = std::string();
    layoutTransitionData.AddPropertyAnimator( controls[0], map );
  }
  {
    // Instant opacity for a child, for testing
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = "InstantAnimator";
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }

  // Just throw all other alpha functions in
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "WRONG" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "LINEAR" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "REVERSE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN_SQUARE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_OUT_SQUARE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_OUT" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN_OUT" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN_OUT_SINE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_IN_SINE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_OUT_SINE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "BOUNCE" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "SIN" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, "EASE_OUT_BACK" );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, Vector4( 0.0f, 1.0f, 2.0f, 3.0f ) );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    // valid
    Property::Array array;
    array.Reserve( 4 );
    array.PushBack( 0.0f );
    array.PushBack( 1.0f );
    array.PushBack( 2.0f );
    array.PushBack( 3.0f );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, array );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    // invalid
    Property::Array array;
    array.Reserve( 3 );
    array.PushBack( 0.0f );
    array.PushBack( 1.0f );
    array.PushBack( 2.0f );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, array );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    // invalid
    Property::Array array;
    array.Reserve( 4 );
    array.PushBack( 0.0f );
    array.PushBack( 10 );
    array.PushBack( 2.0f );
    array.PushBack( 3.0f );
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, array );
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }
  { // no property, not going to be used, but going to be parsed
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = 0; // invalid
    layoutTransitionData.AddPropertyAnimator( Actor(), map ); // apply to all children except parent
  }

  horizontalLayout.SetTransitionData( LayoutTransitionData::ON_CHILD_ADD, layoutTransitionData );
  container.Add( controls[0] );

  bool signalReceived(false);
  LayoutTransitionFinishCheck finishCheck(signalReceived);
  layoutTransitionData.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render( 1u /*just very beginning of the animation*/ );

  finishCheck.CheckSignalNotReceived();
  // The animation just started
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 0.0f, 0.0f, 0.0f ), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  // The animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  application.SendNotification();
  application.Render( 10u /* wait a bit more for a signal */ );

  // Now sizes and positions are finally set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  finishCheck.CheckSignalReceived();

  END_TEST;
}

int UtcDaliLayouting_DefaultTransition01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_DefaultTransition01" );

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto horizontalLayout = LinearLayout::New();
  horizontalLayout.SetAnimateLayout( false );
  horizontalLayout.SetOrientation( LinearLayout::Orientation::HORIZONTAL );

  DevelControl::SetLayout( container, horizontalLayout );
  container.SetName( "Container" );

  std::vector< Control > controls;
  controls.push_back( CreateLeafControl( 100, 100 ) );
  controls.push_back( CreateLeafControl( 100, 100 ) );

  stage.Add( container );
  container.Add( controls[0] );
  container.Add( controls[1] );

  // Initial rendering done
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 100.0f, 350.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  horizontalLayout.SetAnimateLayout( true );

  auto layoutTransitionData0 = LayoutTransitionData::New();
  {
    // Default instant resize
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData0.AddPropertyAnimator( controls[0], map );
  }
  {
    // Instant instant position
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData0.AddPropertyAnimator( controls[0], map );
  }
  DevelControl::GetLayout( controls[0] ).SetTransitionData( LayoutTransitionData::Type::ON_LAYOUT_CHANGE, layoutTransitionData0 );

  auto layoutTransitionData1 = LayoutTransitionData::New();
  {
    // Default instant resize
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::SIZE;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
      .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
      .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
        .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
        .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData1.AddPropertyAnimator( controls[1], map );
  }
  {
    // Instant instant position
    Property::Map map;
    map[ LayoutTransitionData::AnimatorKey::PROPERTY ] = Actor::Property::POSITION;
    map[ LayoutTransitionData::AnimatorKey::TARGET_VALUE ] = Property::Value(); // capture from layout update
    map[ LayoutTransitionData::AnimatorKey::ANIMATOR ] = Property::Map()
       .Add( LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION, AlphaFunction::LINEAR )
       .Add( LayoutTransitionData::AnimatorKey::TIME_PERIOD, Property::Map()
         .Add( LayoutTransitionData::AnimatorKey::DELAY, 0.0f )
         .Add( LayoutTransitionData::AnimatorKey::DURATION, 0.0f ) );
    layoutTransitionData1.AddPropertyAnimator( controls[1], map );
  }
  DevelControl::GetLayout( controls[1] ).SetTransitionData( LayoutTransitionData::Type::ON_LAYOUT_CHANGE, layoutTransitionData1 );

  horizontalLayout.SetOrientation( LinearLayout::Orientation::VERTICAL );

  application.SendNotification();
  application.Render( 10u /*just very beginning of the animation*/ );

  // Animation just finished
  DALI_TEST_EQUALS( container.GetCurrentPosition(), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentPosition(), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentPosition(), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetCurrentSize(), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetCurrentSize(), Vector3( 100.0f, 100.0f, 0.0f ), 1.0f, TEST_LOCATION );

  // Now sizes and positions are set
  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 0.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 300.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::POSITION ), Vector3( 0.0f, 400.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  DALI_TEST_EQUALS( container.GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 480.0f, 800.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[0].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );
  DALI_TEST_EQUALS( controls[1].GetProperty<Vector3>( Actor::Property::SIZE ), Vector3( 100.0f, 100.0f, 0.0f ), 0.0001f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayouting_CheckResourceLeak01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliLayouting_CheckResourceLeak01 - Remove animating layout and add child to stage" );

  Dali::Toolkit::Impl::DummyControl::constructorCount = 0;
  Dali::Toolkit::Impl::DummyControl::destructorCount = 0;

  Stage stage = Stage::GetCurrent();
  auto container = Control::New();
  auto linearLayout = LinearLayout::New();
  linearLayout.SetAnimateLayout( true );

  DevelControl::SetLayout( container, linearLayout );
  container.SetName( "Container" );

  stage.Add( container );

  DummyControl control = DummyControl::New( true );
  control.SetName( "DummyControl01" );
  control.SetSize( 100, 100 );
  container.Add( control );

  control = DummyControl::New( true );
  control.SetName( "DummyControl02" );
  control.SetSize( 100, 100 );
  container.Add( control );

  linearLayout.SetAnimateLayout( true );

  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::constructorCount, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::destructorCount, 0, TEST_LOCATION );

  // Initial rendering done
  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::constructorCount, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::destructorCount, 0, TEST_LOCATION );

  stage.Remove( container );
  container.Reset();

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::constructorCount, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::destructorCount, 1, TEST_LOCATION );

  Stage::GetCurrent().Add( control );

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::constructorCount, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::destructorCount, 1, TEST_LOCATION );

  stage.Remove( control );
  control.Reset();

  application.SendNotification();
  application.Render(static_cast<unsigned int>( 0.5f * 1000.0f ) + 1u /*just after the end of the animation*/ );

  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::constructorCount, 2, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Toolkit::Impl::DummyControl::destructorCount, 2, TEST_LOCATION );

  END_TEST;
}

