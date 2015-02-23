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

void utc_distance_field_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_distance_field_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Create buffer image
BufferImage CreateDistanceField()
{
  BufferImage image = BufferImage::New(256, 256, Pixel::RGBA8888);
  BufferImage distanceFieldImage = BufferImage::New(256, 256, Pixel::L8);

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
}

int UtcDaliDistanceFieldEffectUninitialized(void)
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
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_CHECK(!effect);
  }
  END_TEST;
}

int UtcDaliDistanceFieldEffectPropertyNames(void)
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

  END_TEST;
}

int UtcDaliDistanceFieldEffectDefaultValues(void)
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateDistanceField();

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
  END_TEST;
}

int UtcDaliDistanceFieldEffectCustomValues(void)
{
  ToolkitTestApplication application;

  Toolkit::DistanceFieldEffect effect = Toolkit::DistanceFieldEffect::New();
  DALI_TEST_CHECK( effect );

  BufferImage image = CreateDistanceField();

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
  END_TEST;
}
