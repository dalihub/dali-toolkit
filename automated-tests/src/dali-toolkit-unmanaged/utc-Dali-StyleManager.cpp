//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>


using namespace Dali;
using namespace Dali::Toolkit;

void dali_style_manager_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_style_manager_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStyleManagerGet(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliStyleManagerGet");

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "StyleManager" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  StyleManager manager;

  manager = StyleManager::Get();
  DALI_TEST_CHECK(manager);

  StyleManager newManager = StyleManager::Get();
  DALI_TEST_CHECK(newManager);

  // Check that focus manager is a singleton
  DALI_TEST_CHECK(manager == newManager);
  END_TEST;
}

int UtcDaliStyleManagerSetOrientationValue(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetOrientationValue" );

  StyleManager manager = StyleManager::Get();

  int orientation1 = 0;
  manager.SetOrientationValue( orientation1 );
  DALI_TEST_CHECK( manager.GetOrientationValue() == orientation1 );

  int orientation2 = 180;
  manager.SetOrientationValue( orientation2 );
  DALI_TEST_CHECK( manager.GetOrientationValue() == orientation2 );

  END_TEST;
}

int UtcDaliStyleManagerSetOrientation(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetOrientation" );

  StyleManager manager = StyleManager::Get();

  Orientation orientation;

  manager.SetOrientation( orientation );

  DALI_TEST_CHECK( manager.GetOrientation() == orientation );

  END_TEST;
}

int UtcDaliStyleManagerSetStyleConstant(void)
{
  ToolkitTestApplication application;

  tet_infoline( " UtcDaliStyleManagerSetStyleConstant" );

  StyleManager manager = StyleManager::Get();

  std::string key( "key" );
  Property::Value value( 100 );

  manager.SetStyleConstant( key, value );

  Property::Value returnedValue;
  manager.GetStyleConstant( key, returnedValue );

  DALI_TEST_CHECK( value.Get<int>() == returnedValue.Get<int>() );

  std::string key2( "key2" );
  Property::Value returnedValue2;
  DALI_TEST_CHECK( !manager.GetStyleConstant( key2, returnedValue2 ) );

  END_TEST;
}
