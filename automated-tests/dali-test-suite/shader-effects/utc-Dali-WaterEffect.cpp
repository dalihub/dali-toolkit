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

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliWaterEffectUninitialized, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectPropertyNames, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectOutOfBounds, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectDefaultValues, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectCustomValues, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetAmplitudePositive, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetAmplitudeNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetCenterPositive, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetCenterNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetPropagationPositive, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliWaterEffectGetPropagationNegative, NEGATIVE_TC_IDX );

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

static void UtcDaliWaterEffectUninitialized()
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK(!effect);
  }
}

static void UtcDaliWaterEffectPropertyNames()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  // Check the names, this names are used in the shaders code,
  // if they change the shader code has to be updated
  DALI_TEST_EQUALS( effect.GetAmplitudePropertyName( 0 ), "uDrops[0].amplitude", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetCenterPropertyName( 0 ), "uDrops[0].center", TEST_LOCATION );
  DALI_TEST_EQUALS( effect.GetPropagationPropertyName( 0 ), "uDrops[0].radius", TEST_LOCATION );
}

static void UtcDaliWaterEffectOutOfBounds()
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
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_CHECK( true );
  }
}

static void UtcDaliWaterEffectDefaultValues()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  // Check that the effect has the number of waves it was requested
  DALI_TEST_CHECK( effect.GetNumberOfWaves() == 4 );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  Vector2 leftCorner( Stage::GetCurrent().GetSize() * 0.5f );
  leftCorner.x = -leftCorner.x;

  for ( unsigned int i = 0; i < effect.GetNumberOfWaves(); ++i )
  {
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetAmplitudePropertyName(i).c_str(),
            0.0f ) );
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetCenterPropertyName(i).c_str(),
            leftCorner ) );
    DALI_TEST_CHECK(
        application.GetGlAbstraction().CheckUniformValue(
            effect.GetPropagationPropertyName(i).c_str(),
            0.0f ) );
  }
}

static void UtcDaliWaterEffectCustomValues()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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

  Vector2 leftCorner( Stage::GetCurrent().GetSize() * 0.5f );
  leftCorner.x = -leftCorner.x;

  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetCenterPropertyName(0).c_str(),
          Vector2( leftCorner.x + 10.0f, leftCorner.y - 10.0f ) ) );
  DALI_TEST_CHECK(
      application.GetGlAbstraction().CheckUniformValue(
          effect.GetPropagationPropertyName(0).c_str(),
          2.0f ) );
}

static void UtcDaliWaterEffectGetAmplitudePositive()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliWaterEffectGetAmplitudeNegative()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetAmplitude(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& exception)
  {
    if (exception.mCondition == "index < mNumberOfWaves")
    {
      tet_result(TET_PASS);
    }
  }
}

static void UtcDaliWaterEffectGetCenterPositive()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliWaterEffectGetCenterNegative()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetCenter(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& exception)
  {
    if (exception.mCondition == "index < mNumberOfWaves")
    {
      tet_result(TET_PASS);
    }
  }
}

static void UtcDaliWaterEffectGetPropagationPositive()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

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
}

static void UtcDaliWaterEffectGetPropagationNegative()
{
  ToolkitTestApplication application;

  Toolkit::WaterEffect effect = Toolkit::WaterEffect::New(4);
  DALI_TEST_CHECK( effect );

  BitmapImage image = CreateBitmapImage();

  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetShaderEffect( effect );
  Stage::GetCurrent().Add( actor );

  try
  {
    effect.GetPropagation(9999);
    tet_result(TET_FAIL);
  }
  catch(DaliException& exception)
  {
    if (exception.mCondition == "index < mNumberOfWaves")
    {
      tet_result(TET_PASS);
    }
  }
}
