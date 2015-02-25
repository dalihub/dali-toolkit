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

void utc_dali_toolkit_dissolve_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_dissolve_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliDissolveUninitializedEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect;

  try
  {
    // New() must be called to create a DissolveEffect or it wont be valid.
    effect.SetDistortion( 2.0f );
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

int UtcDaliDissolvePropertyNamesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effectHighPrecision = Toolkit::DissolveEffect::New();
  Toolkit::DissolveEffect effectMediumPrecision = Toolkit::DissolveEffect::New( false );

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effectHighPrecision.GetDistortionPropertyName(), "uPercentage", TEST_LOCATION );
  DALI_TEST_EQUALS( effectMediumPrecision.GetDistortionPropertyName(), "uPercentage", TEST_LOCATION );
  END_TEST;
}

int UtcDaliDissolveDefaultValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  effect.SetCentralLine( Vector2(0.0,0.5), Vector2(1.0, -0.1) );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  Property::Index index = effect.GetPropertyIndex( effect.GetDistortionPropertyName());
  float value;
  (effect.GetProperty(index)).Get( value );
  DALI_TEST_EQUALS(value, 0.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDissolveCustomValuesEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  effect.SetDistortion( 0.5f );

  actor.SetShaderEffect(effect);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  Property::Index index = effect.GetPropertyIndex( effect.GetDistortionPropertyName());
  float value;
  (effect.GetProperty(index)).Get( value );
  DALI_TEST_EQUALS(value, 0.5f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSetEffectImageEffect(void)
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  Image effectImage = CreateBufferImage();
  effect.SetEffectImage(effectImage);

  BufferImage image = CreateBufferImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  Property::Index index = effect.GetPropertyIndex( effect.GetDistortionPropertyName());
  float value;
  (effect.GetProperty(index)).Get( value );
  DALI_TEST_EQUALS(value, 0.f, TEST_LOCATION );
  END_TEST;
}
