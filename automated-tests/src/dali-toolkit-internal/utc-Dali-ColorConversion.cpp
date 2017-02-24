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
#include <dali-toolkit/internal/helpers/color-conversion.h>

using namespace Dali;
using namespace Dali::Toolkit;

void dali_color_conversion_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_color_conversion_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyHelperConvertHtmlStringToColor(void)
{
  tet_infoline( "Test to check whether An HTML style hex string can be converted" );

  const std::string stringColor( "#FF0000" );

  Vector4 result;
  DALI_TEST_CHECK( Toolkit::Internal::ConvertStringToColor( stringColor, result ) );

  DALI_TEST_EQUALS( result, Color::RED, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyHelperConvertStringPropertyToColor(void)
{
  tet_infoline( "Test to check whether A Property value containing a string can be converted" );

  const std::string stringColor( "#00FF00" );
  Property::Value colorProperty( stringColor );

  Vector4 result;
  DALI_TEST_CHECK( Toolkit::Internal::ConvertPropertyToColor( colorProperty, result ) );

  DALI_TEST_EQUALS( result, Color::GREEN, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyHelperConvertVector4PropertyToColor(void)
{
  tet_infoline( "Test to check whether A Property value containing a string can be converted" );

  const Vector4 color( 0.0, 0.0, 1.0, 1.0 );
  Property::Value colorProperty( color );

  Vector4 result;
  DALI_TEST_CHECK( Toolkit::Internal::ConvertPropertyToColor( colorProperty, result ) );

  DALI_TEST_EQUALS( result, Color::BLUE, TEST_LOCATION );

  END_TEST;
}
