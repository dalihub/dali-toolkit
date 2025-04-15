/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/table-view/table-view.h>
#include <dali-toolkit/internal/controls/control/control-debug.h>
#include <dummy-visual.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliControlActionOnVisual(void)
{
  ToolkitTestApplication application;

  tet_infoline("Register an ImageVisual and perform image reload Action on it. Tests Actions are completed.");
  Vector2 controlSize(20.f, 30.f);

  //Created DummyVisual
  Property::Map                     settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New(settings);

  DummyControl        dummyControl = DummyControl::New(true);
  Impl::DummyControl& dummyImpl    = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline("Register test visual and stage control");

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base(dummyVisualPtr.Get());
  dummyImpl.RegisterVisual(DummyControl::Property::TEST_VISUAL, visualBaseHandle);
  dummyControl.SetProperty(Actor::Property::SIZE, Vector2(200.f, 200.f));
  application.GetScene().Add(dummyControl);

  application.SendNotification();
  application.Render();

  tet_infoline("Check action counter is 0 before DoAction");
  DALI_TEST_EQUALS(dummyVisualPtr->GetActionCounter(), 0, TEST_LOCATION);

  tet_infoline("Perform TEST_ACTION action on registered test visual. Should increase the action counter");

  Property::Map attributes;
  DevelControl::DoAction(dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes);

  application.SendNotification();
  DALI_TEST_EQUALS(dummyVisualPtr->GetActionCounter(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliControlDebugHierarchy(void)
{
  ToolkitTestApplication application;
  tet_infoline("Create a control hierarchy, and test that the debug produces output");

  auto tableView = Toolkit::TableView::New(1, 2);
  tableView.AddChild(ImageView::New(TEST_RESOURCE_DIR "/gallery-small-1.jpg"), TableView::CellPosition(1, 1));
  tableView.AddChild(TextLabel::New("Stuff"), TableView::CellPosition(1, 2));

  application.GetScene().Add(tableView);

  Property::Value v(Matrix3::IDENTITY);
  tableView.RegisterProperty("SomeMatrix3", v);

  std::ostringstream oss;
  Dali::Toolkit::Internal::DumpControlHierarchy(oss, application.GetScene().GetRootLayer());
  DALI_TEST_CHECK(oss.str().length() != 0);
  tet_printf("Control hierarchy: \n%s\n", oss.str().c_str());

  END_TEST;
}

int UtcDaliControlOverrideCornerProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline("Override corner properties of control to target visual.");

  DummyControl      dummy     = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(dummy.GetImplementation());

  dummy.SetProperty(Actor::Property::SIZE, Vector2(10.0f, 10.0f));
  dummy.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Vector4 cornerRadius       = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
  int     cornerRadiusPolicy = (int)Visual::Transform::Policy::Type::RELATIVE;
  Vector4 cornerSquareness   = Vector4(0.3f, 0.3f, 0.3f, 0.3f);

  dummy.SetProperty(DevelControl::Property::CORNER_RADIUS, cornerRadius);
  dummy.SetProperty(DevelControl::Property::CORNER_RADIUS_POLICY, cornerRadiusPolicy);
  dummy.SetProperty(DevelControl::Property::CORNER_SQUARENESS, cornerSquareness);

  DALI_TEST_EQUALS(dummy.GetProperty<Vector4>(DevelControl::Property::CORNER_RADIUS), cornerRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(dummy.GetProperty<int>(DevelControl::Property::CORNER_RADIUS_POLICY), cornerRadiusPolicy, TEST_LOCATION);
  DALI_TEST_EQUALS(dummy.GetProperty<Vector4>(DevelControl::Property::CORNER_SQUARENESS), cornerSquareness, TEST_LOCATION);
  tet_infoline("Control properties are all set");

  Property::Map imagePropertyMap;
  imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
  imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg");
  Toolkit::Visual::Base visual = Toolkit::VisualFactory::Get().CreateVisual(imagePropertyMap);

  dummyImpl.RegisterVisual(Toolkit::DummyControl::Property::TEST_VISUAL, visual);
  tet_infoline("Register image visual into derived dummy control");

  DevelControl::EnableCornerPropertiesOverridden(dummy, visual, true);

  Property::Map map;
  visual.CreatePropertyMap(map);

  Vector4 visualCornerRadius;
  int     visualCornerRadiusPolicy;
  Vector4 visualCornerSquareness;

  map[DevelVisual::Property::CORNER_RADIUS].Get(visualCornerRadius);
  map[DevelVisual::Property::CORNER_RADIUS_POLICY].Get(visualCornerRadiusPolicy);
  map[DevelVisual::Property::CORNER_SQUARENESS].Get(visualCornerSquareness);

  DALI_TEST_EQUALS(cornerRadius, visualCornerRadius, TEST_LOCATION);
  DALI_TEST_EQUALS(cornerRadiusPolicy, visualCornerRadiusPolicy, TEST_LOCATION);
  DALI_TEST_EQUALS(cornerSquareness, visualCornerSquareness, TEST_LOCATION);

  END_TEST;
}
