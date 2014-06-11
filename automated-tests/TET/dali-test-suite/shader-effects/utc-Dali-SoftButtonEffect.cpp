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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliSoftButtonEffectUninitialized();
static void UtcDaliSoftButtonEffectNew();
static void UtcDaliSoftButtonEffectPropertyNames();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
      { UtcDaliSoftButtonEffectUninitialized, NEGATIVE_TC_IDX },
      { UtcDaliSoftButtonEffectNew, POSITIVE_TC_IDX },
      { UtcDaliSoftButtonEffectPropertyNames, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Negative test case for a method
static void UtcDaliSoftButtonEffectUninitialized()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliSoftButtonEffectUninitialized");

  Toolkit::SoftButtonEffect effect;

  // New() must be called to create a SoftButtonEffect or it wont be valid.

  DALI_TEST_CHECK(!effect);
}

// Positive test case for a method
static void UtcDaliSoftButtonEffectNew()
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewNew");

  Toolkit::SoftButtonEffect effect = Toolkit::SoftButtonEffect::New(Toolkit::SoftButtonEffect::ELLIPTICAL);
  DALI_TEST_CHECK( effect );
}

// Positive test case for a method
static void UtcDaliSoftButtonEffectPropertyNames()
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
}
