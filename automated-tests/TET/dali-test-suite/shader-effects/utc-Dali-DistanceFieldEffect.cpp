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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/public-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliDistanceFieldEffectUninitialized();
static void UtcDaliDistanceFieldEffectPropertyNames();
static void UtcDaliDistanceFieldEffectDefaultValues();
static void UtcDaliDistanceFieldEffectCustomValues();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliDistanceFieldEffectUninitialized, NEGATIVE_TC_IDX },
    { UtcDaliDistanceFieldEffectPropertyNames, POSITIVE_TC_IDX },
    { UtcDaliDistanceFieldEffectDefaultValues, POSITIVE_TC_IDX },
    { UtcDaliDistanceFieldEffectCustomValues, POSITIVE_TC_IDX },
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

// Create bitmap image
BitmapImage CreateDistanceField()
{
  BitmapImage image = BitmapImage::New(256, 256, Pixel::RGBA8888);
  BitmapImage distanceFieldImage = BitmapImage::New(256, 256, Pixel::L8);

  PixelBuffer* pixbuf = image.GetBuffer();

  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = 0xFF;
    pixbuf[i*4+1] = 0xFF;
    pixbuf[i*4+2] = 0xFF;
    pixbuf[i*4+3] = 0xFF;
  }

  // GenerateDistanceFieldMap(distanceFieldImage.GetBuffer(), Size(256, 256), pixbuf, Size(256, 256), 8, 4);

  return distanceFieldImage;
}

static void UtcDaliDistanceFieldEffectUninitialized()
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect;

  try
  {
    // New() must be called to create a DistanceField effect or it wont be valid.
    effect.SetShadow( true );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
}

static void UtcDaliDistanceFieldEffectPropertyNames()
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetColorPropertyName(), "uColor", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetSmoothingPropertyName(), "uSmoothing", TEST_LOCATION );

  // control flags
  DALI_TEST_EQUALS( effect.GetOutlineEnablePropertyName(), "uDoOutline", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetGlowEnablePropertyName(), "uDoGlow", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetShadowEnablePropertyName(), "uDoShadow", TEST_LOCATION );

  DALI_TEST_EQUALS( effect.GetGlowBoundaryPropertyName(), "uGlowBoundary", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetGlowColorPropertyName(), "uGlowColor", TEST_LOCATION );

  DALI_TEST_EQUALS( effect.GetOutlineColorPropertyName(), "uOutlineColor", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetOutlineSizePropertyName(), "uOutlineParams", TEST_LOCATION );

  DALI_TEST_EQUALS( effect.GetShadowColorPropertyName(), "uShadowColor", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetShadowOffsetPropertyName(), "uShadowOffset", TEST_LOCATION );

}

static void UtcDaliDistanceFieldEffectDefaultValues()
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateDistanceField();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetOutlineEnablePropertyName().c_str(),
          0.0f ));

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetGlowEnablePropertyName().c_str(),
          0.0f ));

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetShadowEnablePropertyName().c_str(),
          0.0f ));
}

static void UtcDaliDistanceFieldEffectCustomValues()
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateDistanceField();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );

  effect.SetShadowColor(Color::YELLOW);
  effect.SetGlowColor(Color::BLUE);

  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // Gets converted to opengl viewport coordinates
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetShadowColorPropertyName().c_str(),
          Color::YELLOW ) );

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetGlowColorPropertyName().c_str(),
          Color::BLUE ) );
}

