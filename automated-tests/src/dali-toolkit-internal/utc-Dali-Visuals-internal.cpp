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
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dummy-visual.h>
#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliVisualAction(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register an ImageVisual and and perform an Action on Visual directly" );
  Vector2 controlSize( 20.f, 30.f );

  //Created DummyVisual
  Property::Map settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New( settings );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline( "Register visual and stage control" );

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base( dummyVisualPtr.Get() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visualBaseHandle );
  dummyControl.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check action counter is 0 before DoAction" );
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  tet_infoline( "Perform TEST_ACTION action on Visual. Should increase the action counter" );

  Property::Map attributes;
  Toolkit::Internal::Visual::Base& internalVisualBase =  GetImplementation( visualBaseHandle );
  internalVisualBase.DoAction( Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes );
  application.SendNotification();
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualActionNotImplemented(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register an ImageVisual and and perform an Action on a Visual which does not support any Actions" );
  Vector2 controlSize( 20.f, 30.f );

  //Created DummyVisual
  Property::Map settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New( settings );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline( "Register visual and stage control" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  dummyControl.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check action counter is 0 before DoAction" );
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  tet_infoline( "Perform TEST_ACTION action on Color Visual which does not support it.. Should not increment the action counter" );
  Property::Map attributes;
  GetImplementation( visual ).DoAction( Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes );
  application.SendNotification();
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualSetProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register a visual and SetProperties" );

  Toolkit::Internal::VisualFactoryCache* factoryCache = new Toolkit::Internal::VisualFactoryCache(false);

  //Created ColorVisual
  Property::Map propertyMap1;
  propertyMap1.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap1.Insert(ColorVisual::Property::MIX_COLOR,  Color::RED);
  Toolkit::Internal::ColorVisualPtr colorVisualPtr = Toolkit::Internal::ColorVisual::New( *factoryCache, propertyMap1 );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast< Impl::DummyControl& >( dummyControl.GetImplementation() );

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base( colorVisualPtr.Get() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visualBaseHandle );
  dummyControl.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::MIX_COLOR,  Color::BLUE );

  colorVisualPtr->SetProperties( propertyMap );
  application.SendNotification();
  application.Render();

  Property::Map resultMap;
  visualBaseHandle.CreatePropertyMap( resultMap );

  Property::Value* colorValue = resultMap.Find( Visual::Property::MIX_COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get< Vector4 >() == Color::BLUE );

  delete factoryCache;

  END_TEST;
}
