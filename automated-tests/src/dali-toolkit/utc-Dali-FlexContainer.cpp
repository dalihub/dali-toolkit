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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void dali_flexflexContainer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_flexflexContainer_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* const PROPERTY_NAME_CONTENT_DIRECTION = "contentDirection";
const char* const PROPERTY_NAME_FLEX_DIRECTION = "flexDirection";
const char* const PROPERTY_NAME_FLEX_WRAP = "flexWrap";
const char* const PROPERTY_NAME_JUSTIFY_CONTENT = "justifyContent";
const char* const PROPERTY_NAME_ALIGN_ITEMS = "alignItems";
const char* const PROPERTY_NAME_ALIGN_CONTENT =  "alignContent";
const char* const CHILD_PROPERTY_NAME_FLEX = "flex";
const char* const CHILD_PROPERTY_NAME_ALIGN_SELF = "alignSelf";
const char* const CHILD_PROPERTY_NAME_FLEX_MARGIN =  "flexMargin";

} // namespace

int UtcDaliToolkitFlexContainerConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerConstructorP");
  FlexContainer flexContainer;
  DALI_TEST_CHECK( !flexContainer );
  END_TEST;
}

int UtcDaliToolkitFlexContainerNewP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerNewP");
  FlexContainer flexContainer = FlexContainer::New();
  DALI_TEST_CHECK( flexContainer );
  END_TEST;
}

int UtcDaliToolkitFlexContainerDownCastP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerDownCastP");
  FlexContainer flexContainer1 = FlexContainer::New();
  BaseHandle object( flexContainer1 );

  FlexContainer flexContainer2 = FlexContainer::DownCast( object );
  DALI_TEST_CHECK( flexContainer2 );

  FlexContainer flexContainer3 = DownCast< FlexContainer >( object );
  DALI_TEST_CHECK( flexContainer3 );
  END_TEST;
}

int UtcDaliToolkitFlexContainerDownCastN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerDownCastN");
  BaseHandle uninitializedObject;
  FlexContainer flexContainer1 = FlexContainer::DownCast( uninitializedObject );
  DALI_TEST_CHECK( !flexContainer1 );

  FlexContainer flexContainer2 = DownCast< FlexContainer >( uninitializedObject );
  DALI_TEST_CHECK( !flexContainer2 );
  END_TEST;
}

int UtcDaliToolkitFlexContainerCopyConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerCopyConstructorP");
  FlexContainer flexContainer = FlexContainer::New();
  flexContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FlexContainer::ROW_REVERSE );

  FlexContainer copy( flexContainer );
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<int>( FlexContainer::Property::FLEX_DIRECTION ) == flexContainer.GetProperty<int>( FlexContainer::Property::FLEX_DIRECTION ) );

  END_TEST;
}

int UtcDaliToolkitFlexContainerAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerAssingmentOperatorP");
  FlexContainer flexContainer = FlexContainer::New();
  flexContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FlexContainer::ROW_REVERSE );

  FlexContainer copy = flexContainer;
  DALI_TEST_CHECK( copy );
  DALI_TEST_CHECK( copy.GetProperty<int>( FlexContainer::Property::FLEX_DIRECTION ) == flexContainer.GetProperty<int>( FlexContainer::Property::FLEX_DIRECTION ) );
  END_TEST;
}

// Positive test case for a method
int UtcDaliToolkitFlexContainerGetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerGetPropertyP");
  FlexContainer flexContainer = FlexContainer::New();
  DALI_TEST_CHECK( flexContainer );

  // Check Property Indices are correct
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_CONTENT_DIRECTION ) == FlexContainer::Property::CONTENT_DIRECTION );
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_FLEX_DIRECTION ) == FlexContainer::Property::FLEX_DIRECTION );
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_FLEX_WRAP ) == FlexContainer::Property::FLEX_WRAP );
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_JUSTIFY_CONTENT ) == FlexContainer::Property::JUSTIFY_CONTENT );
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_ALIGN_ITEMS ) == FlexContainer::Property::ALIGN_ITEMS );
  DALI_TEST_CHECK( flexContainer.GetPropertyIndex( PROPERTY_NAME_ALIGN_CONTENT ) == FlexContainer::Property::ALIGN_CONTENT );

  END_TEST;
}

int UtcDaliToolkitFlexContainerSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerSetPropertyP");
  FlexContainer flexContainer = FlexContainer::New();
  DALI_TEST_CHECK( flexContainer );

  // Add flex container to the stage
  Stage::GetCurrent().Add( flexContainer );

  // Create two actors and add them to the container
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexContainer.Add(actor1);
  flexContainer.Add(actor2);

  // Check content direction property.
  flexContainer.SetProperty( FlexContainer::Property::CONTENT_DIRECTION, FlexContainer::RTL );
  DALI_TEST_EQUALS( (FlexContainer::ContentDirection)flexContainer.GetProperty<int>( FlexContainer::Property::CONTENT_DIRECTION ), FlexContainer::RTL, TEST_LOCATION );

  // Check flex direction property.
  flexContainer.SetProperty( FlexContainer::Property::FLEX_DIRECTION, FlexContainer::COLUMN_REVERSE );
  DALI_TEST_EQUALS( (FlexContainer::FlexDirection)flexContainer.GetProperty<int>( FlexContainer::Property::FLEX_DIRECTION ), FlexContainer::COLUMN_REVERSE, TEST_LOCATION );

  // Check flex wrap property.
  flexContainer.SetProperty( FlexContainer::Property::FLEX_WRAP, FlexContainer::WRAP );
  DALI_TEST_EQUALS( (FlexContainer::WrapType)flexContainer.GetProperty<int>( FlexContainer::Property::FLEX_WRAP ), FlexContainer::WRAP, TEST_LOCATION );

  // Check justify content property.
  flexContainer.SetProperty( FlexContainer::Property::JUSTIFY_CONTENT, FlexContainer::JUSTIFY_SPACE_BETWEEN );
  DALI_TEST_EQUALS( (FlexContainer::Justification)flexContainer.GetProperty<int>( FlexContainer::Property::JUSTIFY_CONTENT ), FlexContainer::JUSTIFY_SPACE_BETWEEN, TEST_LOCATION );

  // Check align items property.
  flexContainer.SetProperty( FlexContainer::Property::ALIGN_ITEMS, FlexContainer::ALIGN_FLEX_START );
  DALI_TEST_EQUALS( (FlexContainer::Alignment)flexContainer.GetProperty<int>( FlexContainer::Property::ALIGN_ITEMS ), FlexContainer::ALIGN_FLEX_START, TEST_LOCATION );

  // Check align content property.
  flexContainer.SetProperty( FlexContainer::Property::ALIGN_CONTENT, FlexContainer::ALIGN_STRETCH );
  DALI_TEST_EQUALS( (FlexContainer::Alignment)flexContainer.GetProperty<int>( FlexContainer::Property::ALIGN_CONTENT ), FlexContainer::ALIGN_STRETCH, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexContainerSetChildPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexContainerSetChildPropertyP");
  FlexContainer flexContainer = FlexContainer::New();
  DALI_TEST_CHECK( flexContainer );

  // Add flex container to the stage
  Stage::GetCurrent().Add( flexContainer );

  // Create an actor and add it to the container
  Actor actor = Actor::New();
  DALI_TEST_CHECK( actor );

  flexContainer.Add(actor);

  // Check flex child property.
  actor.SetProperty( FlexContainer::ChildProperty::FLEX, 2.0f );
  DALI_TEST_EQUALS( actor.GetProperty<float>( FlexContainer::ChildProperty::FLEX ), 2.0f, TEST_LOCATION );
  DALI_TEST_CHECK( actor.GetPropertyIndex( CHILD_PROPERTY_NAME_FLEX ) == FlexContainer::ChildProperty::FLEX );

  // Check align self child property.
  actor.SetProperty( FlexContainer::ChildProperty::ALIGN_SELF, FlexContainer::ALIGN_FLEX_END );
  DALI_TEST_EQUALS( (FlexContainer::Alignment)actor.GetProperty<int>( FlexContainer::ChildProperty::ALIGN_SELF ), FlexContainer::ALIGN_FLEX_END, TEST_LOCATION );
  DALI_TEST_CHECK( actor.GetPropertyIndex( CHILD_PROPERTY_NAME_ALIGN_SELF ) == FlexContainer::ChildProperty::ALIGN_SELF );

  // Check flex margin child property.
  actor.SetProperty( FlexContainer::ChildProperty::FLEX_MARGIN, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );
  DALI_TEST_EQUALS( actor.GetProperty<Vector4>( FlexContainer::ChildProperty::FLEX_MARGIN ), Vector4( 10.0f, 10.0f, 10.0f, 10.0f ), TEST_LOCATION );
  DALI_TEST_CHECK( actor.GetPropertyIndex( CHILD_PROPERTY_NAME_FLEX_MARGIN ) == FlexContainer::ChildProperty::FLEX_MARGIN );

  END_TEST;
}

