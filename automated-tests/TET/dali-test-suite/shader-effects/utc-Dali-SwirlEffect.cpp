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

static void UtcDaliSwirlUninitializedEffect();
static void UtcDaliSwirlPropertyNamesEffect();
static void UtcDaliSwirlDefaultValuesEffect();
static void UtcDaliSwirlCustomValuesEffect();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliSwirlUninitializedEffect, NEGATIVE_TC_IDX },
    { UtcDaliSwirlPropertyNamesEffect, POSITIVE_TC_IDX },
    { UtcDaliSwirlDefaultValuesEffect, POSITIVE_TC_IDX },
    { UtcDaliSwirlCustomValuesEffect, POSITIVE_TC_IDX },
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

static void UtcDaliSwirlUninitializedEffect()
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
}

static void UtcDaliSwirlPropertyNamesEffect()
{
  ToolkitTestApplication application;

  Toolkit::SwirlEffect effect = Toolkit::SwirlEffect::New(false);

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetAnglePropertyName(), "uAngle", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName(), "uCenter", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetRadiusPropertyName(), "uRadius", TEST_LOCATION );
}

static void UtcDaliSwirlDefaultValuesEffect()
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
}

static void UtcDaliSwirlCustomValuesEffect()
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
}
