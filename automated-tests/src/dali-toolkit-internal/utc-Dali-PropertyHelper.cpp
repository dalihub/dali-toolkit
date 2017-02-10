/*
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/helpers/property-helper.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_property_helper_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_property_helper_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyHelperGetStringFromPropertyWithString(void)
{
  tet_infoline( "Test to check if a simple string is parsed correctly" );

  const std::string inputString = "Hello World";
  Property::Value value( inputString );

  std::string output;
  DALI_TEST_CHECK( Toolkit::Internal::GetStringFromProperty( value, output ) );
  DALI_TEST_EQUALS( output, inputString, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyHelperGetStringFromPropertyWithEmptyValue(void)
{
  tet_infoline( "Test to ensure if an empty value returns false" );

  std::string output;
  DALI_TEST_CHECK( ! Toolkit::Internal::GetStringFromProperty( Property::Value(), output ) );

  END_TEST;
}

int UtcDaliPropertyHelperGetStringFromPropertyWithStringArray(void)
{
  tet_infoline( "Test to check if a string array is parsed correctly and adds new line characters too" );

  Property::Value value( Property::Array().Add( "Hello World" )
                                          .Add( "The Quick Brown Fox" )
                                          .Add( "Jumps over the lazy dog" ) );

  std::string output;
  DALI_TEST_CHECK( Toolkit::Internal::GetStringFromProperty( value, output ) );
  DALI_TEST_CHECK( output.find( "Hello World\n" ) != std::string::npos );
  DALI_TEST_CHECK( output.find( "The Quick Brown Fox\n" ) != std::string::npos );
  DALI_TEST_CHECK( output.find( "Jumps over the lazy dog\n" ) != std::string::npos );

  END_TEST;
}

int UtcDaliPropertyHelperGetStringFromPropertyWithEmptyArray(void)
{
  tet_infoline( "Test to check if an empty array returns false" );

  Property::Array array;

  std::string output;
  DALI_TEST_CHECK( ! Toolkit::Internal::GetStringFromProperty( Property::Value( array ), output ) );

  END_TEST;
}

int UtcDaliPropertyHelperGetStringFromPropertyWithMultipleTypesInArray(void)
{
  tet_infoline( "Test to ensure an array with multiple types returns false" );

  Property::Value value( Property::Array().Add( "Hello World" )
                                          .Add( "The Quick Brown Fox" )
                                          .Add( 1 )
                                          .Add( "Jumps" )
                                          .Add( 25 )
                                          .Add( "Over" ) );

  std::string output;
  DALI_TEST_CHECK( ! Toolkit::Internal::GetStringFromProperty( value, output ) );
  DALI_TEST_CHECK( output.empty() );

  END_TEST;
}
