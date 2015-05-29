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
#include <dali-toolkit/devel-api/shader-effects/soft-button-effect.h>

using namespace Dali;
using namespace Dali::Toolkit;

void soft_button_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void soft_button_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Negative test case for a method
int UtcDaliSoftButtonEffectUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSoftButtonEffectUninitialized");

  Toolkit::SoftButtonEffect effect;

  // New() must be called to create a SoftButtonEffect or it wont be valid.

  DALI_TEST_CHECK(!effect);
  END_TEST;
}

// Positive test case for a method
int UtcDaliSoftButtonEffectNew(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewNew");

  Toolkit::SoftButtonEffect effect = Toolkit::SoftButtonEffect::New(Toolkit::SoftButtonEffect::ELLIPTICAL);
  DALI_TEST_CHECK( effect );
  END_TEST;
}

// Positive test case for a method
int UtcDaliSoftButtonEffectPropertyNames(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSoftButtonEffectPropertyNames");

  Toolkit::SoftButtonEffect effect = Toolkit::SoftButtonEffect::New(Toolkit::SoftButtonEffect::ELLIPTICAL);
  DALI_TEST_CHECK( effect );

  // Check the names, this names are used in the shader code,
  // if they change in the shader code, then it has to be updated here.
  DALI_TEST_EQUALS( effect.GetLightingIndentationAmountPropertyName(), "uLightingIndentationAmount", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetTextureDistortionAmountPropertyName(), "uTextureDistortAmount", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetAmbientLightAmountPropertyName(), "uAmbientLight", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetDiffuseLightPropertyName(), "uDiffuseLight", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetLightingMultiplierPropertyName(), "uLightMultiplier", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetInsideShapeSizeScalePropertyName(), "uInsideCircleSizeScale", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetOutsideShapeDepthPropertyName(), "uOutsideCircleDepth", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetEffectPixelAreaPropertyName(), "uEffectRegion", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetRectangleSizeScalePropertyName(), "uRectangleSizeScale", TEST_LOCATION );
  END_TEST;
}
