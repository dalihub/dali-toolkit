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
#include <dali-toolkit/devel-api/controls/magnifier/magnifier.h>

using namespace Dali;
using namespace Toolkit;

void dali_magnifier_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_magnifier_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliMagnifierNew(void)
{
  ToolkitTestApplication application;

  Magnifier magnifier;
  DALI_TEST_CHECK( !magnifier );

  magnifier = Magnifier::New();
  DALI_TEST_CHECK( magnifier );

  Stage::GetCurrent().Add( magnifier );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliMagnifierCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Magnifier view = Magnifier::New();
  DALI_TEST_CHECK( view );

  Magnifier copy( view );
  DALI_TEST_CHECK( copy == view );

  Magnifier assign;
  DALI_TEST_CHECK( !assign );
  assign = view;
  DALI_TEST_CHECK( assign == view );

  // Self assignment
  assign = assign;
  DALI_TEST_CHECK( assign );
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliMagnifierDownCast(void)
{
  ToolkitTestApplication application;

  BaseHandle view = Magnifier::New();
  DALI_TEST_CHECK( Magnifier::DownCast( view ) );

  BaseHandle empty;
  DALI_TEST_CHECK( ! Magnifier::DownCast( empty ) );

  BaseHandle another = Actor::New();
  DALI_TEST_CHECK( ! Magnifier::DownCast( another ) );

  END_TEST;
}

int UtcDaliMagnifierTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Magnifier" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  Magnifier view = Magnifier::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}

int UtcDaliMagnifierSetSourceActorP(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  Magnifier view = Magnifier::New();
  stage.Add( view );

  application.SendNotification();
  application.Render();

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_CHECK( renderTaskList.GetTaskCount() > 1 );

  Actor actor = Actor::New();
  stage.Add( actor );
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTask( 1 ).GetSourceActor() != actor );

  view.SetSourceActor( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( stage.GetRenderTaskList().GetTask( 1 ).GetSourceActor(), actor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMagnifierSetSourceActorN(void)
{
  ToolkitTestApplication application;

  Magnifier view;

  try
  {
    view.SetSourceActor( Actor::New() );
    DALI_TEST_CHECK( false ); // should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliMagnifierFrameVisibility(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  Magnifier view = Magnifier::New();
  stage.Add( view );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::FRAME_VISIBILITY ).Get< bool >(), true, TEST_LOCATION );

  view.SetProperty( Magnifier::Property::FRAME_VISIBILITY, false );
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::FRAME_VISIBILITY ).Get< bool >(), false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::FRAME_VISIBILITY ).Get< bool >(), false, TEST_LOCATION );

  view.SetProperty( Magnifier::Property::FRAME_VISIBILITY, true );
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::FRAME_VISIBILITY ).Get< bool >(), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::FRAME_VISIBILITY ).Get< bool >(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMagnifierMagnificationFactor(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  Magnifier view = Magnifier::New();
  stage.Add( view );

  application.SendNotification();
  application.Render();

  float magnificationFactor( 200.0f );

  DALI_TEST_CHECK( view.GetProperty( Magnifier::Property::MAGNIFICATION_FACTOR ).Get< float >() != magnificationFactor );

  view.SetProperty( Magnifier::Property::MAGNIFICATION_FACTOR, magnificationFactor );
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::MAGNIFICATION_FACTOR ).Get< float >(), magnificationFactor, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::MAGNIFICATION_FACTOR ).Get< float >(), magnificationFactor, TEST_LOCATION );

  view.SetProperty( Magnifier::Property::MAGNIFICATION_FACTOR, 1.0f );
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::MAGNIFICATION_FACTOR ).Get< float >(), 1.0f, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::MAGNIFICATION_FACTOR ).Get< float >(), 1.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMagnifierSourcePosition(void)
{
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  Magnifier view = Magnifier::New();
  stage.Add( view );

  application.SendNotification();
  application.Render();

  Vector3 position( 100.0f, 200.0f, 300.0f );

  DALI_TEST_CHECK( view.GetProperty( Magnifier::Property::SOURCE_POSITION ).Get< Vector3 >() != position );

  view.SetProperty( Magnifier::Property::SOURCE_POSITION, position );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::SOURCE_POSITION ).Get< Vector3 >(), position, TEST_LOCATION );

  view.SetProperty( Magnifier::Property::SOURCE_POSITION, Vector3::ONE );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( view.GetProperty( Magnifier::Property::SOURCE_POSITION ).Get< Vector3 >(), Vector3::ONE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliMagnifierOnSizeSet(void)
{
  ToolkitTestApplication application;

  Magnifier view = Magnifier::New();

  Stage::GetCurrent().Add( view );

  application.SendNotification();
  application.Render();

  Vector3 size( 200.0f, 300.0f, 0.0f );
  view.SetSize( size );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( view.GetCurrentSize(), size, TEST_LOCATION );

  END_TEST;
}
