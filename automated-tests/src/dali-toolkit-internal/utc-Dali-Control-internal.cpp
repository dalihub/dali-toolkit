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
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <dummy-visual.h>
#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/internal/controls/control/control-debug.h>


using namespace Dali;
using namespace Toolkit;


int UtcDaliControlActionOnVisual(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Register an ImageVisual and perform image reload Action on it. Tests Actions are completed." );
  Vector2 controlSize( 20.f, 30.f );

  //Created DummyVisual
  Property::Map settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New( settings );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline( "Register test visual and stage control" );

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base( dummyVisualPtr.Get() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visualBaseHandle );
  dummyControl.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check action counter is 0 before DoAction" );
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  tet_infoline( "Perform TEST_ACTION action on registered test visual. Should increase the action counter" );

  Property::Map attributes;
  DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes );

  application.SendNotification();
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlDebugHierarchy(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Create a control hierarchy, and test that the debug produces output" );

  auto tableView = Toolkit::TableView::New(1, 2);
  tableView.AddChild( ImageView::New( TEST_RESOURCE_DIR "/gallery-small-1.jpg" ), TableView::CellPosition( 1, 1 ) );
  tableView.AddChild( TextLabel::New("Stuff"), TableView::CellPosition( 1, 2 ) );

  Stage::GetCurrent().Add( tableView );

  Property::Value v(Matrix3::IDENTITY);
  tableView.RegisterProperty( "SomeMatrix3", v);

  std::ostringstream oss;
  Dali::Toolkit::Internal::DumpControlHierarchy( oss, Stage::GetCurrent().GetRootLayer() );
  DALI_TEST_CHECK( oss.str().length() != 0 );
  tet_printf("Control hierarchy: \n%s\n", oss.str().c_str() );

  END_TEST;
}
