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


void utc_dali_toolkit_swirl_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_swirl_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSwirlUninitializedEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::SwirlEffect effect;

  try
  {
    // New() must be called to create a SwirlEffect or it wont be valid.
    effect.SetRadius( 0.5f );
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

int UtcDaliSwirlPropertyNamesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::SwirlEffect effect = Toolkit::SwirlEffect::New(false);

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetAnglePropertyName(), "uAngle", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName(), "uCenter", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetRadiusPropertyName(), "uRadius", TEST_LOCATION );
  END_TEST;
}

int UtcDaliSwirlDefaultValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::SwirlEffect effect = Toolkit::SwirlEffect::New(true);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Gets converted to opengl viewport coordinates
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetAnglePropertyName().c_str(),
          0.0f ) );

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName().c_str(),
          Vector2(0.5f, 0.5f) ) );

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetRadiusPropertyName().c_str(),
          1.0f ) );
  END_TEST;
}

int UtcDaliSwirlCustomValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::SwirlEffect effect = Toolkit::SwirlEffect::New(false);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  effect.SetAngle( 1.0f );
  effect.SetCenter( Vector2(0.3f, 0.7f) );
  effect.SetRadius( 2.0f );

  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Gets converted to opengl viewport coordinates
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetAnglePropertyName().c_str(),
          1.0f ) );

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName().c_str(),
          Vector2(0.3f, 0.7f) ) );

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetRadiusPropertyName().c_str(),
          2.0f ) );
  END_TEST;
}
