/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <toolkit-timer.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali/integration-api/events/hover-event-integ.h>

using namespace Dali;
using namespace Dali::Toolkit;

///////////////////////////////////////////////////////////////////////////////////////////////////

void utc_dali_toolkit_tooltip_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_tooltip_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

Integration::HoverEvent GenerateSingleHover( TouchPoint::State state, const Vector2& screenPosition )
{
  Integration::HoverEvent hoverEvent;
  Integration::Point point;
  point.SetState( static_cast< PointState::Type >( state ) );
  point.SetScreenPosition( screenPosition );
  hoverEvent.points.push_back( point );
  return hoverEvent;
}

} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////

int UtcDaliTooltipGetWithoutSetting(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map is empty" );
  DALI_TEST_EQUALS( true, map->Empty(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithString(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP, "Hello Test" );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map contains the content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );

  tet_infoline( "Check content is a property map" );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );

  tet_infoline( "Check that the map contains the text item" );
  Property::Value* textStringValue = contentMap->Find( TextVisual::Property::TEXT );
  DALI_TEST_CHECK( textStringValue );

  tet_infoline( "Ensure it matches what we set" );
  DALI_TEST_EQUALS( "Hello Test", textStringValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "We sent valid text, so ensure the hover signal has been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithTextVisualMap(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT,
                                            Property::Map().Add( Visual::Property::TYPE, DevelVisual::TEXT )
                                                           .Add( TextVisual::Property::TEXT, "Hello TextVisual Test" ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map contains the content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );

  tet_infoline( "Check content is a property map" );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );

  tet_infoline( "Check that the map contains the text item" );
  Property::Value* textStringValue = contentMap->Find( TextVisual::Property::TEXT );
  DALI_TEST_CHECK( textStringValue );

  tet_infoline( "Ensure it matches what we set" );
  DALI_TEST_EQUALS( "Hello TextVisual Test", textStringValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "We sent a text visual with TEXT property set, so ensure the hover signal has been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithTextVisualMapWithoutString(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT,
                                            Property::Map().Add( Visual::Property::TYPE, DevelVisual::TEXT )
                                                           .Add( TextVisual::Property::POINT_SIZE, 20 ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map contains the content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );

  tet_infoline( "Check content is a property map" );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );

  tet_infoline( "Check that the map contains the point-size item" );
  Property::Value* pointSizeValue = contentMap->Find( TextVisual::Property::POINT_SIZE );
  DALI_TEST_CHECK( pointSizeValue );

  tet_infoline( "Ensure it matches what we set" );
  DALI_TEST_EQUALS( 20, pointSizeValue->Get< int >(), TEST_LOCATION );

  tet_infoline( "We sent a text visual without a TEXT property set, so ensure the hover signal has NOT been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithImageVisualMap(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT,
                                            Property::Map().Add( Visual::Property::TYPE, Visual::IMAGE )
                                                           .Add( ImageVisual::Property::URL, "dummy-url.png" ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map contains the content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );

  tet_infoline( "Check content is a property map" );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );

  tet_infoline( "Check that the map contains the url item" );
  Property::Value* urlValue = contentMap->Find( ImageVisual::Property::URL );
  DALI_TEST_CHECK( urlValue );

  tet_infoline( "Ensure it matches what we set" );
  DALI_TEST_EQUALS( "dummy-url.png", urlValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "We sent an ImageVisual, so ensure the hover signal has been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithArray(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Array().Add( Property::Map().Add( Visual::Property::TYPE, Visual::IMAGE )
                                                             .Add( ImageVisual::Property::URL, "dummy-url.png" ) )
                                        .Add( Property::Map().Add( Visual::Property::TYPE, DevelVisual::TEXT )
                                                             .Add( TextVisual::Property::TEXT, "Hello Array Test" ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Ensure map contains the content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );

  tet_infoline( "Check content is a property array" );
  Property::Array* contentArray = contentValue->GetArray();
  DALI_TEST_CHECK( contentArray );

  tet_infoline( "Ensure the array contains two items" );
  DALI_TEST_EQUALS( 2u, contentArray->Count(), TEST_LOCATION );

  tet_infoline( "Ensure first value is a map and contains the right item" );
  const Property::Value mapValue1 = contentArray->GetElementAt( 0 );
  Property::Map* map1 = mapValue1.GetMap();
  DALI_TEST_CHECK( map1 );
  Property::Value* urlValue = map1->Find( ImageVisual::Property::URL );
  DALI_TEST_CHECK( urlValue );
  DALI_TEST_EQUALS( "dummy-url.png", urlValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "Ensure second value is a map and contains the right item" );
  const Property::Value mapValue2 = contentArray->GetElementAt( 1 );
  Property::Map* map2 = mapValue2.GetMap();
  DALI_TEST_CHECK( map2 );
  Property::Value* textValue = map2->Find( TextVisual::Property::TEXT );
  DALI_TEST_CHECK( textValue );
  DALI_TEST_EQUALS( "Hello Array Test", textValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "We sent an array, so ensure the hover signal has been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithFullMap(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT,
                                            Property::Map().Add( Visual::Property::TYPE, DevelVisual::TEXT )
                                                           .Add( TextVisual::Property::TEXT, "Hello TextVisual Test" ) )
                                      .Add( Tooltip::Property::LAYOUT, Vector2( 1.0f, 2.0f ) )
                                      .Add( Tooltip::Property::WAIT_TIME, 2.5f )
                                      .Add( Tooltip::Property::BACKGROUND, "tooltip-background.png" )
                                      .Add( Tooltip::Property::TAIL, true )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::HOVER_POINT )
                                      .Add( Tooltip::Property::HOVER_POINT_OFFSET, Vector2( 100.0f, 50.f ) )
                                      .Add( Tooltip::Property::MOVEMENT_THRESHOLD, 50 )
                                      .Add( Tooltip::Property::DISAPPEAR_ON_MOVEMENT, true )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Check content" );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );

  tet_infoline( "Check layout" );
  Property::Value* layoutValue = map->Find( Tooltip::Property::LAYOUT );
  DALI_TEST_CHECK( layoutValue );
  DALI_TEST_EQUALS( layoutValue->Get< Vector2 >(), Vector2( 1.0f, 2.0f ), TEST_LOCATION );

  tet_infoline( "Check wait time" );
  Property::Value* waitTimeValue = map->Find( Tooltip::Property::WAIT_TIME );
  DALI_TEST_CHECK( waitTimeValue );
  DALI_TEST_EQUALS( waitTimeValue->Get< float >(), 2.5f, TEST_LOCATION );

  tet_infoline( "Check background" );
  Property::Value* backgroundMapValue = map->Find( Tooltip::Property::BACKGROUND );
  DALI_TEST_CHECK( backgroundMapValue );
  Property::Map* backgroundMap = backgroundMapValue->GetMap();
  DALI_TEST_CHECK( backgroundMap );
  Property::Value* backgroundStringValue = backgroundMap->Find( Tooltip::Background::Property::VISUAL );
  DALI_TEST_CHECK( backgroundStringValue );
  DALI_TEST_EQUALS( backgroundStringValue->Get< std::string >(), "tooltip-background.png", TEST_LOCATION );

  tet_infoline( "Check Tail" );
  Property::Value* tailMapValue = map->Find( Tooltip::Property::TAIL );
  DALI_TEST_CHECK( tailMapValue );
  Property::Map* tailMap = tailMapValue->GetMap();
  DALI_TEST_CHECK( tailMap );
  Property::Value* tailVisibilityValue = tailMap->Find( Tooltip::Tail::Property::VISIBILITY );
  DALI_TEST_CHECK( tailVisibilityValue );
  DALI_TEST_EQUALS( tailVisibilityValue->Get< bool >(), true, TEST_LOCATION );

  tet_infoline( "Check position" );
  Property::Value* positionValue = map->Find( Tooltip::Property::POSITION );
  DALI_TEST_CHECK( positionValue );
  DALI_TEST_EQUALS( positionValue->Get< int >(), static_cast< int >( Tooltip::Position::HOVER_POINT ), TEST_LOCATION );

  tet_infoline( "Check hover point offset" );
  Property::Value* hoverPointOffsetValue = map->Find( Tooltip::Property::HOVER_POINT_OFFSET );
  DALI_TEST_CHECK( hoverPointOffsetValue );
  DALI_TEST_EQUALS( hoverPointOffsetValue->Get< Vector2 >(), Vector2( 100.0f, 50.f ), TEST_LOCATION );

  tet_infoline( "Check movement threshold" );
  Property::Value* movementThresholdValue = map->Find( Tooltip::Property::MOVEMENT_THRESHOLD );
  DALI_TEST_CHECK( movementThresholdValue );
  DALI_TEST_EQUALS( movementThresholdValue->Get< int >(), 50, TEST_LOCATION );

  tet_infoline( "Check disappear on movement" );
  Property::Value* disappearOnMovementValue = map->Find( Tooltip::Property::DISAPPEAR_ON_MOVEMENT );
  DALI_TEST_CHECK( disappearOnMovementValue );
  DALI_TEST_EQUALS( disappearOnMovementValue->Get< bool >(), true, TEST_LOCATION );

  tet_infoline( "We sent a text visual with TEXT property set, so ensure the hover signal has been connected to" );
  DALI_TEST_EQUALS( control.HoveredSignal().GetConnectionCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithBackgroundMap(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Hello TextVisual Test" )
                                      .Add( Tooltip::Property::BACKGROUND,
                                            Property::Map().Add( Tooltip::Background::Property::VISUAL, "tooltip-background.png" )
                                                           .Add( Tooltip::Background::Property::BORDER, Rect< int >( 10, 20, 30, 40 ) ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Check background map" );
  Property::Value* backgroundMapValue = map->Find( Tooltip::Property::BACKGROUND );
  DALI_TEST_CHECK( backgroundMapValue );
  Property::Map* backgroundMap = backgroundMapValue->GetMap();
  DALI_TEST_CHECK( backgroundMap );

  tet_infoline( "Check visual" );
  Property::Value* backgroundStringValue = backgroundMap->Find( Tooltip::Background::Property::VISUAL );
  DALI_TEST_CHECK( backgroundStringValue );
  DALI_TEST_EQUALS( backgroundStringValue->Get< std::string >(), "tooltip-background.png", TEST_LOCATION );

  tet_infoline( "Check border" );
  Property::Value* borderValue = backgroundMap->Find( Tooltip::Background::Property::BORDER );
  DALI_TEST_CHECK( borderValue );
  DALI_TEST_EQUALS( borderValue->Get< Rect< int > >(), Rect< int >( 10, 20, 30, 40 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithBackgroundMapVector4(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Hello TextVisual Test" )
                                      .Add( Tooltip::Property::BACKGROUND,
                                            Property::Map().Add( Tooltip::Background::Property::VISUAL, "tooltip-background.png" )
                                                           .Add( Tooltip::Background::Property::BORDER, Vector4( 40.0f, 30.0f, 20.0f, 10.0f ) ) )
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Check background map" );
  Property::Value* backgroundMapValue = map->Find( Tooltip::Property::BACKGROUND );
  DALI_TEST_CHECK( backgroundMapValue );
  Property::Map* backgroundMap = backgroundMapValue->GetMap();
  DALI_TEST_CHECK( backgroundMap );

  tet_infoline( "Check visual" );
  Property::Value* backgroundStringValue = backgroundMap->Find( Tooltip::Background::Property::VISUAL );
  DALI_TEST_CHECK( backgroundStringValue );
  DALI_TEST_EQUALS( backgroundStringValue->Get< std::string >(), "tooltip-background.png", TEST_LOCATION );

  tet_infoline( "Check border" );
  Property::Value* borderValue = backgroundMap->Find( Tooltip::Background::Property::BORDER );
  DALI_TEST_CHECK( borderValue );
  DALI_TEST_EQUALS( borderValue->Get< Rect< int > >(), Rect< int >( 40, 30, 20, 10 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipCreateWithTailMap(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Hello TextVisual Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ))
                     );

  tet_infoline( "Check if Property::MAP is returned" );
  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );

  tet_infoline( "Ensure map is valid" );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  tet_infoline( "Check Tail" );
  Property::Value* tailMapValue = map->Find( Tooltip::Property::TAIL );
  DALI_TEST_CHECK( tailMapValue );
  Property::Map* tailMap = tailMapValue->GetMap();
  DALI_TEST_CHECK( tailMap );

  tet_infoline( "Check visibility" );
  Property::Value* tailVisibilityValue = tailMap->Find( Tooltip::Tail::Property::VISIBILITY );
  DALI_TEST_CHECK( tailVisibilityValue );
  DALI_TEST_EQUALS( tailVisibilityValue->Get< bool >(), true, TEST_LOCATION );

  tet_infoline( "Check above visual" );
  Property::Value* aboveVisualValue = tailMap->Find( Tooltip::Tail::Property::ABOVE_VISUAL );
  DALI_TEST_CHECK( aboveVisualValue );
  DALI_TEST_EQUALS( aboveVisualValue->Get< std::string >(), "above-visual.png", TEST_LOCATION );

  tet_infoline( "Check below visual" );
  Property::Value* belowVisualValue = tailMap->Find( Tooltip::Tail::Property::BELOW_VISUAL );
  DALI_TEST_CHECK( belowVisualValue );
  DALI_TEST_EQUALS( belowVisualValue->Get< std::string >(), "below-visual.png", TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipDisplay(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP, "Test" );
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  application.ProcessEvent( GenerateSingleHover( TouchPoint::Stationary, centerPoint ) ); // Emit for code coverage, will have no effect

  END_TEST;
}

int UtcDaliTooltipDisplayWithTail(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ))
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipDisplayWithContentArray(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT,
                                            Property::Array().Add( Property::Map().Add( Visual::Property::TYPE, Visual::IMAGE )
                                                                                  .Add( ImageVisual::Property::URL, "dummy-url.png" ) )
                                                             .Add( Property::Map().Add( Visual::Property::TYPE, DevelVisual::TEXT )
                                                                                  .Add( TextVisual::Property::TEXT, "Hello Array Test" ) ))
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ))
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipDisplayBelow(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::BELOW )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip

  tet_infoline( "Ensure tooltip is below control" );
  DALI_TEST_CHECK( ( control.GetCurrentWorldPosition().y + 50.0f /* Half Size */) < tooltip.GetCurrentWorldPosition().y );

  END_TEST;
}

int UtcDaliTooltipDisplayAbove(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::ABOVE )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip

  tet_infoline( "Ensure tooltip is above control" );
  DALI_TEST_CHECK( ( control.GetCurrentWorldPosition().y - 50.0f /* Half Size */) >= ( tooltip.GetCurrentWorldPosition().y + 0.5f * tooltip.GetCurrentSize().height ) );

  END_TEST;
}

int UtcDaliTooltipDisplayAtHoverPoint(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::HOVER_POINT )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  const Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector2 hoverPoint = stageSize * 0.5f;
  hoverPoint.x -= 10.0f;
  hoverPoint.y -= 10.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, hoverPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip

  tet_infoline( "Ensure tooltip is below and to the right of control" );
  DALI_TEST_CHECK( ( hoverPoint.y - stageSize.height * 0.5f ) < tooltip.GetCurrentWorldPosition().y );
  DALI_TEST_CHECK( ( hoverPoint.x - stageSize.width  * 0.5f ) < tooltip.GetCurrentWorldPosition().x );

  END_TEST;
}

int UtcDaliTooltipExceedThreshold(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::MOVEMENT_THRESHOLD, 5 )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  tet_infoline( "Start hover" );
  Vector2 hoverPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Emit a value which exceeds threshold, timer should start again" );
  hoverPoint.x += 10.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Emit Timer signal - timeout at new point which is still within bounds" );
  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipGoOutOfBounds(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP, "Test" );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  tet_infoline( "Start hover" );
  Vector2 hoverPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Emit a value which goes out of bounds" );
  hoverPoint.x += 100.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Emit Timer signal - nothing should happen" );
  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should be the same as before" );
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipHideTooltipWhenOutOfBounds(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP, "Test" );
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 hoverPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, hoverPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  hoverPoint.x += 100.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should be back to what was there before the tooltip was shown" );
  --rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipHideTooltipWhenSetToDisapperOnMovement(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::DISAPPEAR_ON_MOVEMENT, true )
                                      .Add( Tooltip::Property::MOVEMENT_THRESHOLD, 5 )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 hoverPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, hoverPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  hoverPoint.x += 10.0f; // Stay within bounds but exceed threshold
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, hoverPoint ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should be back to what was there before the tooltip was shown" );
  --rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipChangeContent(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Control control = Control::New();
  control.SetProperty( DevelControl::Property::TOOLTIP, "Test" );
  control.SetAnchorPoint( AnchorPoint::CENTER );
  control.SetParentOrigin( ParentOrigin::CENTER );
  control.SetSize( 100.0f, 100.0f );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  int rootChildCount = rootActor.GetChildCount();

  Vector2 centerPoint = Stage::GetCurrent().GetSize() * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  tet_infoline( "Change content while timer is running and ensure it matches the new value" );
  control.SetProperty( DevelControl::Property::TOOLTIP, "Second Value" );

  Property::Value value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );
  Property::Value* contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );
  Property::Map* contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );
  Property::Value* textStringValue = contentMap->Find( TextVisual::Property::TEXT );
  DALI_TEST_CHECK( textStringValue );
  DALI_TEST_EQUALS( "Second Value", textStringValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "Emit signal, nothing should happen as everything has been reset" );
  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, there should NOT be any new actors" );
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  tet_infoline( "More movement at same point, and emit signal, we should get the tooltip" );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, centerPoint ) );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, they should have incremented by one" );
  ++rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  tet_infoline( "Change content while tooltip is showing, current one should be removed from the stage and ensure it matches new value" );
  control.SetProperty( DevelControl::Property::TOOLTIP, "Third Value" );

  value = control.GetProperty( DevelControl::Property::TOOLTIP );
  DALI_TEST_EQUALS( value.GetType(), Property::MAP, TEST_LOCATION );
  map = value.GetMap();
  DALI_TEST_CHECK( map );
  contentValue = map->Find( Tooltip::Property::CONTENT );
  DALI_TEST_CHECK( contentValue );
  contentMap = contentValue->GetMap();
  DALI_TEST_CHECK( contentMap );
  textStringValue = contentMap->Find( TextVisual::Property::TEXT );
  DALI_TEST_CHECK( textStringValue );
  DALI_TEST_EQUALS( "Third Value", textStringValue->Get< std::string >(), TEST_LOCATION );

  tet_infoline( "Emit signal, nothing should happen as everything has been reset" );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Get number of actors on the Stage, there should be one less actor on the stage" );
  --rootChildCount;
  DALI_TEST_EQUALS( rootActor.GetChildCount(), rootChildCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTooltipEnsureRemainsOnStage1(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  tet_infoline( "Create a control and place it at the bottom of the screen, setting the tooltip to appear below" );
  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  control.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  control.SetSize( stageSize );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ) )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::BELOW )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 centerPoint = stageSize * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure tooltip is still on the screen" );
  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip
  DALI_TEST_CHECK( ( tooltip.GetCurrentWorldPosition().y + tooltip.GetCurrentSize().height * 0.5f ) <= centerPoint.height );

  END_TEST;
}

int UtcDaliTooltipEnsureRemainsOnStage2(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Vector2 stageSize = Stage::GetCurrent().GetSize();

  tet_infoline( "Create a control and place it at the top of the screen, setting the tooltip to appear above" );
  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  control.SetParentOrigin( ParentOrigin::TOP_CENTER );
  control.SetSize( stageSize );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ) )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::ABOVE )
                     );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 centerPoint = stageSize * 0.5f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure tooltip is still on the screen" );
  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip
  DALI_TEST_CHECK( ( tooltip.GetCurrentWorldPosition().y - tooltip.GetCurrentSize().height * 0.5f ) >= -centerPoint.height );

  END_TEST;
}

int UtcDaliTooltipEnsureRemainsOnStage3(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector2 centerPoint = stageSize * 0.5f;

  tet_infoline( "Create a control and adjust it's position so that the tooltip will attempt to appear to the left of the screen" );
  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  control.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  control.SetSize( stageSize );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ) )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::BELOW )
                     );
  control.SetX( -centerPoint.x );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 hoverPoint( centerPoint );
  hoverPoint.x = 1.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure tooltip is still on the screen" );
  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip
  DALI_TEST_CHECK( ( tooltip.GetCurrentWorldPosition().x - tooltip.GetCurrentSize().width * 0.5f ) >= -centerPoint.width );

  END_TEST;
}

int UtcDaliTooltipEnsureRemainsOnStage4(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication

  Vector2 stageSize = Stage::GetCurrent().GetSize();
  Vector2 centerPoint = stageSize * 0.5f;

  tet_infoline( "Create a control and adjust it's position so that the tooltip will attempt to appear to the right of the screen" );
  Control control = Control::New();
  control.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  control.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  control.SetSize( stageSize );
  control.SetProperty( DevelControl::Property::TOOLTIP,
                       Property::Map().Add( Tooltip::Property::CONTENT, "Test" )
                                      .Add( Tooltip::Property::TAIL,
                                            Property::Map().Add( Tooltip::Tail::Property::VISIBILITY, true )
                                                           .Add( Tooltip::Tail::Property::ABOVE_VISUAL, "above-visual.png" )
                                                           .Add( Tooltip::Tail::Property::BELOW_VISUAL, "below-visual.png" ) )
                                      .Add( Tooltip::Property::POSITION, Tooltip::Position::BELOW )
                     );
  control.SetX( centerPoint.x );

  Actor rootActor = Stage::GetCurrent().GetRootLayer();
  rootActor.Add( control );

  application.SendNotification();
  application.Render();

  Vector2 hoverPoint( centerPoint );
  hoverPoint.x = 1.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, centerPoint ) );

  Dali::Timer timer = Timer::New( 1u );
  timer.MockEmitSignal();

  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure tooltip is still on the screen" );
  Actor tooltip = rootActor.GetChildAt( rootActor.GetChildCount() - 1 ); // Last actor added will be our tooltip
  DALI_TEST_CHECK( ( tooltip.GetCurrentWorldPosition().x + tooltip.GetCurrentSize().width * 0.5f ) <= centerPoint.width );

  END_TEST;
}
