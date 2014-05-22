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
using namespace Dali::Toolkit;

namespace
{
const char* TEST_IMAGE_FILE_NAME = "gallery_image_01.jpg";
} // namespace


void utc_displacement_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_displacement_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Negative test case for a method
int UtcDaliDisplacementEffectUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDisplacementEffectUninitialized");

  Toolkit::DisplacementEffect effect;

  try
  {
    // New() must be called to create a GaussianBlurView or it wont be valid.
    effect.SetStateProperty( 1.0f );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliDisplacementEffectNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDisplacementEffectNew");

  Toolkit::DisplacementEffect effect = Toolkit::DisplacementEffect::New(Toolkit::DisplacementEffect::DISPLACED);
  DALI_TEST_CHECK( effect );

  Toolkit::DisplacementEffect effect2 = Toolkit::DisplacementEffect::New(Toolkit::DisplacementEffect::FIXED);
  DALI_TEST_CHECK( effect2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliDisplacementEffectPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDisplacementEffectPropertyNames");

  Toolkit::DisplacementEffect effect = Toolkit::DisplacementEffect::New(Toolkit::DisplacementEffect::DISPLACED);
  DALI_TEST_CHECK( effect );

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetLightDirectionPropertyName(), "uLightDirection", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetAmbientLightColorPropertyName(), "uAmbientLightColor", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetDiffuseLightColorPropertyName(), "uDiffuseLightColor", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetLightingMultiplierPropertyName(), "uLightMultiplier", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetStatePropertyName(), "uState", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetHeightScalePropertyName(), "uHightScale", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetFixedNormalPropertyName(), "uFixedNormal", TEST_LOCATION );
  END_TEST;
}

// Positive test case for a method
int UtcDaliDisplacementEffectTestSetProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliDisplacementEffectTestSetProperty");

  Toolkit::DisplacementEffect effect = Toolkit::DisplacementEffect::New(Toolkit::DisplacementEffect::DISPLACED);
  DALI_TEST_CHECK( effect );

  ImageActor actor = ImageActor::New( Image::New(TEST_IMAGE_FILE_NAME) );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  Toolkit::DisplacementEffect effect2 = Toolkit::DisplacementEffect::New(Toolkit::DisplacementEffect::FIXED);
  DALI_TEST_CHECK( effect );

  ImageActor actor2 = ImageActor::New( Image::New(TEST_IMAGE_FILE_NAME) );
  actor2.SetSize( 100.0f, 100.0f );
  actor2.SetShaderEffect( effect2 );
  Stage::GetCurrent().Add( actor2 );

  Vector3 testVector3 = Vector3(45.0f, 55.0f, 65.0f);
  float testFloat = 0.623f;
  effect.SetLightDirection(testVector3);
  effect.SetAmbientLightColorProperty(testVector3);
  effect.SetDiffuseLightColorProperty(testVector3);
  effect.SetStateProperty(testFloat);
  effect.SetLightingMultiplierProperty(testFloat);
  effect.SetHeightScaleProperty(testFloat);

  effect2.SetFixedNormalProperty(testVector3);

  application.SendNotification();
  application.Render(0);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetLightDirectionPropertyName() ) ).Get<Vector3>(), testVector3, TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetAmbientLightColorPropertyName() ) ).Get<Vector3>(), testVector3, TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetDiffuseLightColorPropertyName() ) ).Get<Vector3>(), testVector3, TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetStatePropertyName().c_str() ) ).Get<float>(), testFloat, TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetLightingMultiplierPropertyName().c_str() ) ).Get<float>(), testFloat, TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetProperty( effect.GetPropertyIndex( effect.GetHeightScalePropertyName().c_str() ) ).Get<float>(), testFloat, TEST_LOCATION );

  Vector3 normalizedVector3(testVector3);
  normalizedVector3.Normalize();
  DALI_TEST_EQUALS( effect2.GetProperty( effect2.GetPropertyIndex( effect2.GetFixedNormalPropertyName() ) ).Get<Vector3>(), normalizedVector3, TEST_LOCATION );
  END_TEST;
}
