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
#include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>


using namespace Dali;
using namespace Dali::Toolkit;


void model_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_OBJ_FILE_NAME = TEST_RESOURCE_DIR "/Cube.obj";
const char* TEST_MTL_FILE_NAME = TEST_RESOURCE_DIR "/ToyRobot-Metal.mtl";
const char* TEST_RESOURCE_LOCATION = TEST_RESOURCE_DIR "/";
}

// Negative test case for a method
int UtcDaliModelViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModel3dViewUninitialized");

  Toolkit::Model3dView view;

  try
  {
    // New() must be called to create a Model3dView or it wont be valid.
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
int UtcDaliModelViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModel3dViewNew");

  Toolkit::Model3dView view = Toolkit::Model3dView::New();
  DALI_TEST_CHECK( view );

  Toolkit::Model3dView view2 = Toolkit::Model3dView::New("","","");
  DALI_TEST_CHECK( view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelViewDownCast");

  Toolkit::Model3dView view = Toolkit::Model3dView::New();
  BaseHandle handle(view);

  Toolkit::Model3dView modelView = Toolkit::Model3dView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( modelView );
  DALI_TEST_CHECK( modelView == view );
  END_TEST;
}


// Positive test case for a method
int UtcDaliModelViewPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModel3dViewPropertyNames");

  Toolkit::Model3dView view = Toolkit::Model3dView::New();
  DALI_TEST_CHECK( view );

  view.SetProperty( Model3dView::Property::GEOMETRY_URL, Dali::Property::Value( TEST_OBJ_FILE_NAME ) );
  Property::Value val = view.GetProperty( Model3dView::Property::GEOMETRY_URL );
  std::string obj_file_name;
  DALI_TEST_CHECK( val.Get( obj_file_name ) );
  DALI_TEST_EQUALS( obj_file_name, TEST_OBJ_FILE_NAME, TEST_LOCATION );

  view.SetProperty( Model3dView::Property::MATERIAL_URL, Dali::Property::Value( TEST_MTL_FILE_NAME ) );
  val = view.GetProperty( Model3dView::Property::MATERIAL_URL );
  DALI_TEST_CHECK( val.Get( obj_file_name ) );
  DALI_TEST_EQUALS( obj_file_name, TEST_MTL_FILE_NAME, TEST_LOCATION );

  view.SetProperty( Model3dView::Property::IMAGES_URL, Dali::Property::Value( TEST_RESOURCE_LOCATION ) );
  val = view.GetProperty( Model3dView::Property::IMAGES_URL );
  DALI_TEST_CHECK( val.Get( obj_file_name ) );
  DALI_TEST_EQUALS( obj_file_name, TEST_RESOURCE_LOCATION, TEST_LOCATION );

  Stage::GetCurrent().Add(view);

  END_TEST;
}

// Positive test case for a method
int UtcDaliModelViewAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModel3dViewAddRemove");

  Toolkit::Model3dView view = Toolkit::Model3dView::New();
  DALI_TEST_CHECK( view );

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );


  view.SetParentOrigin(ParentOrigin::CENTER);
  view.SetSize(Stage::GetCurrent().GetSize());
  view.Add(actor);
  Stage::GetCurrent().Add(view);

  DALI_TEST_CHECK( actor.OnStage() );

  view.Remove(actor);

  DALI_TEST_CHECK( !actor.OnStage() );
  END_TEST;
}


int UtcDaliModelCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Model3dView view = Toolkit::Model3dView::New();
  DALI_TEST_CHECK( view );

  Model3dView copy( view );
  DALI_TEST_CHECK( view == copy );

  Model3dView assign;
  DALI_TEST_CHECK( ! assign );

  assign = copy;
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliModelTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "Model3dView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  Model3dView view = Model3dView::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}

int UtcDaliModelOnSizeSet(void)
{
  ToolkitTestApplication application;

  Model3dView view = Toolkit::Model3dView::New();

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
