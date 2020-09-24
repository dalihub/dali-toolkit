/**
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali/devel-api/common/stage.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>

#include <dali-toolkit/devel-api/controls/scroll-bar/scroll-bar.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

using namespace Dali::Accessibility;

void utc_dali_toolkit_accessibility_value_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_toolkit_accessibility_value_cleanup(void)
{
  test_return_value = TET_PASS;
}

int utcDaliAccessibilityProgressBarGetMinimum(void)
{
  ToolkitTestApplication application;

  Toolkit::ProgressBar progress_bar = Toolkit::ProgressBar::New();
  auto q = Dali::Accessibility::Accessible::Get( progress_bar );
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMinimum(), 0.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityProgressBarGetMaximum(void)
{
  ToolkitTestApplication application;

  Toolkit::ProgressBar progress_bar = Toolkit::ProgressBar::New();
  auto q = Dali::Accessibility::Accessible::Get( progress_bar );
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMaximum(), 1.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityProgressBarGetMinimumIncrement(void)
{
  ToolkitTestApplication application;

  Toolkit::ProgressBar progress_bar = Toolkit::ProgressBar::New();
  auto q = Dali::Accessibility::Accessible::Get(progress_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMinimumIncrement(), 0.001, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityProgressBarGetSetCurrent(void)
{
  ToolkitTestApplication application;

  Toolkit::ProgressBar progress_bar = Toolkit::ProgressBar::New();
  auto q = Dali::Accessibility::Accessible::Get(progress_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.0, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 2.0 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 0.25 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.25, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityScrollBarGetMinimum(void)
{
  ToolkitTestApplication application;

  // Create a source actor that owns the scroll properties required by the scroll bar
  Actor sourceActor = Actor::New();
  Stage::GetCurrent().Add( sourceActor );

  // Register the scroll properties
  Property::Index propertyScrollPosition = sourceActor.RegisterProperty( "sourcePosition",  0.0f );
  Property::Index propertyMinScrollPosition = sourceActor.RegisterProperty( "sourcePositionMin",   10.0f );
  Property::Index propertyMaxScrollPosition = sourceActor.RegisterProperty( "sourcePositionMax",   100.0f );
  Property::Index propertyScrollContentSize = sourceActor.RegisterProperty( "sourceContentSize",   500.0f );

  Toolkit::ScrollBar scroll_bar = Toolkit::ScrollBar::New();

  scroll_bar.SetScrollPropertySource(sourceActor, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);

  auto q = Dali::Accessibility::Accessible::Get(scroll_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMinimum(), 10.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityScrollBarGetMaximum(void)
{
  ToolkitTestApplication application;

  // Create a source actor that owns the scroll properties required by the scroll bar
  Actor sourceActor = Actor::New();
  Stage::GetCurrent().Add( sourceActor );

  // Register the scroll properties
  Property::Index propertyScrollPosition = sourceActor.RegisterProperty( "sourcePosition",  0.0f );
  Property::Index propertyMinScrollPosition = sourceActor.RegisterProperty( "sourcePositionMin",   0.0f );
  Property::Index propertyMaxScrollPosition = sourceActor.RegisterProperty( "sourcePositionMax",   100.0f );
  Property::Index propertyScrollContentSize = sourceActor.RegisterProperty( "sourceContentSize",   500.0f );

  Toolkit::ScrollBar scroll_bar = Toolkit::ScrollBar::New();

  scroll_bar.SetScrollPropertySource(sourceActor, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);

  auto q = Dali::Accessibility::Accessible::Get(scroll_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMaximum(), 100.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityScrollBarGetMinimumIncrement(void)
{
  ToolkitTestApplication application;

  Toolkit::ScrollBar scroll_bar = Toolkit::ScrollBar::New();
  auto q = Dali::Accessibility::Accessible::Get(scroll_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMinimumIncrement(), 1.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityScrollBarGetSetCurrent(void)
{
  ToolkitTestApplication application;

  // Create a source actor that owns the scroll properties required by the scroll bar
  Actor sourceActor = Actor::New();
  Stage::GetCurrent().Add( sourceActor );

  // Register the scroll properties
  Property::Index propertyScrollPosition = sourceActor.RegisterProperty( "sourcePosition",  0.0f );
  Property::Index propertyMinScrollPosition = sourceActor.RegisterProperty( "sourcePositionMin",   0.0f );
  Property::Index propertyMaxScrollPosition = sourceActor.RegisterProperty( "sourcePositionMax",   100.0f );
  Property::Index propertyScrollContentSize = sourceActor.RegisterProperty( "sourceContentSize",   500.0f );

  Toolkit::ScrollBar scroll_bar = Toolkit::ScrollBar::New();

  scroll_bar.SetScrollPropertySource(sourceActor, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);

  //sourceActor.SetProperty(propertyScrollPosition, 20.0f);

  auto q = Dali::Accessibility::Accessible::Get(scroll_bar);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.0, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 1000.0 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 50.0 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilitySliderGetMinimum(void)
{
  ToolkitTestApplication application;

  Toolkit::Slider slider = Toolkit::Slider::New();
  auto q = Dali::Accessibility::Accessible::Get(slider);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMinimum(), 0.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilitySliderGetMaximum(void)
{
  ToolkitTestApplication application;

  Toolkit::Slider slider = Toolkit::Slider::New();
  auto q = Dali::Accessibility::Accessible::Get(slider);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetMaximum(), 1.0, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilitySliderGetMinimumIncrement(void)
{
  ToolkitTestApplication application;

  Toolkit::Slider slider = Toolkit::Slider::New();
  auto q = Dali::Accessibility::Accessible::Get(slider);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS<float>( x->GetMinimumIncrement(), 0.05, TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilitySliderGetSetCurrent(void)
{
  ToolkitTestApplication application;

  Toolkit::Slider slider = Toolkit::Slider::New();
  auto q = Dali::Accessibility::Accessible::Get(slider);
  auto x = dynamic_cast< Dali::Accessibility::Value* >( q );
  DALI_TEST_CHECK( x );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.0, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 2.0 ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( x->SetCurrent( 0.25 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( x->GetCurrent(), 0.25, TEST_LOCATION );

  END_TEST;
}
