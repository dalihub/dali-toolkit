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

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliCarouselEffectUninitialized();
static void UtcDaliCarouselEffectPropertyNames();
static void UtcDaliCarouselEffectDefaultValues();
static void UtcDaliCarouselEffectCustomValues();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliCarouselEffectUninitialized, NEGATIVE_TC_IDX },
    { UtcDaliCarouselEffectPropertyNames, POSITIVE_TC_IDX },
    { UtcDaliCarouselEffectDefaultValues, POSITIVE_TC_IDX },
    { UtcDaliCarouselEffectCustomValues, POSITIVE_TC_IDX },
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

static void UtcDaliCarouselEffectUninitialized()
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
}

static void UtcDaliCarouselEffectPropertyNames()
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();

  // Check the names, these names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetRadiusPropertyName(), "uRadius", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName(), "uCenter", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetAnglePerUnitPropertyName(), "uAnglePerUnit", TEST_LOCATION );
}

static void UtcDaliCarouselEffectDefaultValues()
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliCarouselEffectCustomValues()
{
  ToolkitTestApplication application;

  Toolkit::CarouselEffect effect = Toolkit::CarouselEffect::New();
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}
