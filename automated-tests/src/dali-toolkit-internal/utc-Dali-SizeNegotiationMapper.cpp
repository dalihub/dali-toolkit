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
#include <dali-toolkit/internal/layouting/size-negotiation-mapper.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/devel-api/layouting/linear-layout.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliLayoutingSizeNegotiationMapper_01(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingSizeNegotiationMapper_01 - Test mapping Dimension::ALL_DIMENSIONS");

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );
  Toolkit::Internal::Control::Impl& controlDataImpl = Toolkit::Internal::Control::Impl::Get( controlImpl );
  Toolkit::Internal::LayoutItemPtr layout = controlDataImpl.GetLayout();
  // Set defaults which should be overriden by mapper
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Set ALL_DIMENSIONS specifications using mapper
  Toolkit::Internal::SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( control, layout, Dimension::ALL_DIMENSIONS );

  // Test WIDTH and HEIGHT updated
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::WRAP_CONTENT ), TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION ), Property::Value( ChildLayoutData::WRAP_CONTENT ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingSizeNegotiationMapper_02(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingSizeNegotiationMapper_02 - Test mapping for Dimension::WIDTH only");

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );
  Toolkit::Internal::Control::Impl& controlDataImpl = Toolkit::Internal::Control::Impl::Get( controlImpl );
  Toolkit::Internal::LayoutItemPtr layout = controlDataImpl.GetLayout();

  // Set defaults which should be overriden by mapper
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Set WIDTH specifications using mapper
  Toolkit::Internal::SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( control, layout, Dimension::WIDTH );

  // Test WIDTH only updated.
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::WRAP_CONTENT ), TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliLayoutingSizeNegotiationMapper_03(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliLayoutingSizeNegotiationMapper_03 - Test mapping for Dimension::HEIGHT only");

  auto control = Control::New();
  control.SetName( "fitToChildrenControl" );
  DevelControl::SetLayoutingRequired( control, true );
  control.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( control );
  Toolkit::Internal::Control::Impl& controlDataImpl = Toolkit::Internal::Control::Impl::Get( controlImpl );
  Toolkit::Internal::LayoutItemPtr layout = controlDataImpl.GetLayout();

  // Set defaults which should be overriden by mapper
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, ChildLayoutData::MATCH_PARENT );
  control.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, ChildLayoutData::MATCH_PARENT );

  // Set HEIGHT specifications using mapper
  Toolkit::Internal::SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( control, layout, Dimension::HEIGHT );

  // Test HEIGHT only updated.
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION ), Property::Value( ChildLayoutData::MATCH_PARENT ), TEST_LOCATION );
  DALI_TEST_EQUALS( control.GetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION ), Property::Value( ChildLayoutData::WRAP_CONTENT), TEST_LOCATION );

  END_TEST;
}