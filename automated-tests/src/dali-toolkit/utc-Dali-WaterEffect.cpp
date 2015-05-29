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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/shader-effects/water-effect.h>

using namespace Dali;

void dali_water_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_water_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}



int UtcDaliWaterEffectUninitialized(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect;

  try
  {
    // New() must be called to create a RippleEffect or it wont be valid.
    effect.SetAmplitude( 0, 0.5f );
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

int UtcDaliWaterEffectPropertyNames(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetAmplitudePropertyName( 0 ), "uDrops[0].amplitude", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName( 0 ), "uDrops[0].center", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetPropagationPropertyName( 0 ), "uDrops[0].radius", TEST_LOCATION );
  END_TEST;
}

int UtcDaliWaterEffectOutOfBounds(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  try
  {
    // the highest index acceptable is (GetNumberOfWaves() - 1)
    effect.SetAmplitude( effect.GetNumberOfWaves(), 0 );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK( true );
  }
  END_TEST;
}

int UtcDaliWaterEffectDefaultValues(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  // Check that the effect has the number of waves it was requested
  DALI_TEST_CHECK( effect.GetNumberOfWaves() == 4 );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  Vector2 topLeft( Stage::GetCurrent().GetSize() * 0.5f );
  topLeft.y = -topLeft.y;

  for ( unsigned int i = 0; i < effect.GetNumberOfWaves(); ++i )
  {
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetAmplitudePropertyName(i).c_str(),
            0.0f ) );
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetCenterPropertyName(i).c_str(),
            topLeft ) );
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetPropagationPropertyName(i).c_str(),
            0.0f ) );
  }
  END_TEST;
}

int UtcDaliWaterEffectCustomValues(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  effect.SetAmplitude( 0, 0.5f );
  effect.SetCenter( 0, Vector2 ( 10.0f, 10.0f ) );
  effect.SetPropagation( 0, 2.0f );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetAmplitudePropertyName(0).c_str(),
          0.5f ) );

  Vector2 centerPoint( Stage::GetCurrent().GetSize() * 0.5f );
  centerPoint.y = -centerPoint.y;

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName(0).c_str(),
          Vector2( centerPoint.x - 10.0f, centerPoint.y + 10.0f ) ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetPropagationPropertyName(0).c_str(),
          2.0f ) );
  END_TEST;
}

int UtcDaliWaterEffectGetAmplitudePositive(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  float amplitude(0.5f);
  DALI_TEST_CHECK(effect.GetAmplitude(0) != amplitude);
  effect.SetAmplitude( 0, amplitude );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(amplitude, effect.GetAmplitude(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliWaterEffectGetAmplitudeNegative(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetAmplitude(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "index < mNumberOfWaves", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliWaterEffectGetCenterPositive(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  Vector2 center(10.0f, 20.0f);
  DALI_TEST_CHECK(effect.GetCenter(0) != center);
  effect.SetCenter( 0, center );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(center, effect.GetCenter(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliWaterEffectGetCenterNegative(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetCenter(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "index < mNumberOfWaves", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliWaterEffectGetPropagationPositive(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  float propagation(0.5f);
  DALI_TEST_CHECK(effect.GetPropagation(0) != propagation);
  effect.SetPropagation( 0, propagation );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(propagation, effect.GetPropagation(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliWaterEffectGetPropagationNegative(void)
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetPropagation(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "index < mNumberOfWaves", TEST_LOCATION );
  }
  END_TEST;
}
