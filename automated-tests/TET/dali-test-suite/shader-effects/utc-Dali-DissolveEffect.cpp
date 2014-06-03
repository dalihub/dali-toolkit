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
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliDissolveUninitializedEffect();
static void UtcDaliDissolvePropertyNamesEffect();
static void UtcDaliDissolveDefaultValuesEffect();
static void UtcDaliDissolveCustomValuesEffect();
static void UtcDaliSetEffectImageEffect();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliDissolveUninitializedEffect, NEGATIVE_TC_IDX },
    { UtcDaliDissolvePropertyNamesEffect, POSITIVE_TC_IDX },
    { UtcDaliDissolveDefaultValuesEffect, POSITIVE_TC_IDX },
    { UtcDaliDissolveCustomValuesEffect, POSITIVE_TC_IDX },
    { UtcDaliSetEffectImageEffect, POSITIVE_TC_IDX },
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
BitmapImage CreateBitmapImage()
{
  BitmapImage image = BitmapImage::New(4,4,Pixel::RGBA8888);

  PixelBuffer* pixbuf = image.GetBuffer();

  // Using a 4x4 image gives a better blend with the GL implementation
  // than a 3x3 image
  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = 0xFF;
    pixbuf[i*4+1] = 0xFF;
    pixbuf[i*4+2] = 0xFF;
    pixbuf[i*4+3] = 0xFF;
  }

  return image;
}

static void UtcDaliDissolveUninitializedEffect()
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
}

static void UtcDaliDissolvePropertyNamesEffect()
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effectHighPrecision = Toolkit::DissolveEffect::New();
  Toolkit::DissolveEffect effectMediumPrecision = Toolkit::DissolveEffect::New( false );

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effectHighPrecision.GetDistortionPropertyName(), "uPercentage", TEST_LOCATION );
  DALI_TEST_EQUALS( effectMediumPrecision.GetDistortionPropertyName(), "uPercentage", TEST_LOCATION );
}

static void UtcDaliDissolveDefaultValuesEffect()
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliDissolveCustomValuesEffect()
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliSetEffectImageEffect()
{
  ToolkitTestApplication application;

  Toolkit::DissolveEffect effect = Toolkit::DissolveEffect::New();
  DALI_TEST_CHECK( effect );

  Image effectImage = CreateBitmapImage();
  effect.SetEffectImage(effectImage);

  BitmapImage image = CreateBitmapImage();

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
}
