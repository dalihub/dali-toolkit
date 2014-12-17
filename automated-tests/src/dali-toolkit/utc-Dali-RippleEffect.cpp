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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

void utc_dali_toolkit_ripple_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_ripple_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliRippleUninitializedEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::RippleEffect effect;

  try
  {
    // New() must be called to create a RippleEffect or it wont be valid.
    effect.SetAmplitude( 0.5f );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(!effect);
  }
  END_TEST;
}

int UtcDaliRipplePropertyNamesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::RippleEffect effect = Toolkit::RippleEffect::New();

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetAmplitudePropertyName(), "uAmplitude", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName(), "uCenter", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetTimePropertyName(), "uTime", TEST_LOCATION );

  END_TEST;
}

int UtcDaliRippleDefaultValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::RippleEffect effect = Toolkit::RippleEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetAmplitudePropertyName().c_str(),
          0.0f ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName().c_str(),
          Vector2( 0.0f, 0.0f ) ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetTimePropertyName().c_str(),
          0.0f ) );
  END_TEST;
}

int UtcDaliRippleCustomValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::RippleEffect effect = Toolkit::RippleEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  effect.SetAmplitude( 0.5f );
  effect.SetCenter( Vector2( 10.0f, 10.0f ) );
  effect.SetTime( 2.0f );

  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetAmplitudePropertyName().c_str(),
          0.5f ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName().c_str(),
          Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetTimePropertyName().c_str(),
          2.0f ) );
  END_TEST;
}
