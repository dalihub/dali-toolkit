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

using namespace Dali;


void carousel_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void carousel_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliCarouselEffectUninitialized(void)
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect;

  try
  {
    // New() must be called to create a CarouselEffect or it wont be valid.
    effect.SetRadius( 100.0f );
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

int UtcDaliCarouselEffectPropertyNames(void)
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();

  // Check the names, these names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetRadiusPropertyName(), "uRadius", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName(), "uCenter", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetAnglePerUnitPropertyName(), "uAnglePerUnit", TEST_LOCATION );
  END_TEST;
}

int UtcDaliCarouselEffectDefaultValues(void)
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  const float radiusValue(0.0f);
  const Vector2 centerValue(0.0f, 0.0f);
  const Vector2 anglePerUnitValue(0.0f, 0.0f);

  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetRadiusPropertyName().c_str(), radiusValue ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetCenterPropertyName().c_str(), centerValue ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetAnglePerUnitPropertyName().c_str(), anglePerUnitValue ) );
  END_TEST;
}

int UtcDaliCarouselEffectCustomValues(void)
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  const float radiusValue(100.0f);
  const Vector2 centerValue(150.0f, 200.0f);
  const Vector2 anglePerUnitValue(0.1f, 0.25f);

  effect.SetRadius( radiusValue );
  effect.SetCenter( centerValue );
  effect.SetAnglePerUnit( anglePerUnitValue );

  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetRadiusPropertyName().c_str(), radiusValue ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetCenterPropertyName().c_str(), centerValue ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( effect.GetAnglePerUnitPropertyName().c_str(), anglePerUnitValue ) );
  END_TEST;
}
