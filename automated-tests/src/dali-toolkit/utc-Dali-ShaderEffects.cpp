/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/devel-api/shader-effects/alpha-discard-effect.h>
#include <dali-toolkit/devel-api/shader-effects/bendy-effect.h>
#include <dali-toolkit/devel-api/shader-effects/blind-effect.h>
#include <dali-toolkit/devel-api/shader-effects/bouncing-effect.h>
#include <dali-toolkit/devel-api/shader-effects/carousel-effect.h>
#include <dali-toolkit/devel-api/shader-effects/displacement-effect.h>
#include <dali-toolkit/devel-api/shader-effects/dissolve-effect.h>
#include <dali-toolkit/devel-api/shader-effects/dissolve-local-effect.h>
#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>
#include <dali-toolkit/devel-api/shader-effects/iris-effect.h>
#include <dali-toolkit/devel-api/shader-effects/mask-effect.h>
#include <dali-toolkit/devel-api/shader-effects/mirror-effect.h>
#include <dali-toolkit/devel-api/shader-effects/motion-blur-effect.h>
#include <dali-toolkit/devel-api/shader-effects/motion-stretch-effect.h>
#include <dali-toolkit/devel-api/shader-effects/nine-patch-mask-effect.h>
#include <dali-toolkit/devel-api/shader-effects/overlay-effect.h>
#include <dali-toolkit/devel-api/shader-effects/quadratic-bezier.h>
#include <dali-toolkit/devel-api/shader-effects/ripple-effect.h>
#include <dali-toolkit/devel-api/shader-effects/ripple2d-effect.h>
#include <dali-toolkit/devel-api/shader-effects/shear-effect.h>
#include <dali-toolkit/devel-api/shader-effects/soft-button-effect.h>
#include <dali-toolkit/devel-api/shader-effects/spot-effect.h>
#include <dali-toolkit/devel-api/shader-effects/square-dissolve-effect.h>
#include <dali-toolkit/devel-api/shader-effects/swirl-effect.h>

using namespace Dali;

int UtcDaliCreateAlphaDiscardEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateAlphaDiscardEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateBendyEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateBendyEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateBlindEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateBlindEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateBouncingEffect(void)
{
  ToolkitTestApplication application;

  Vector4 color(1.0f,1.0f,1.0f,1.0f);

  ShaderEffect effect = Toolkit::CreateBouncingEffect(color);
  DALI_TEST_CHECK( effect );

  Property::Value value = effect.GetProperty( effect.GetPropertyIndex("uAssignedColor"));
  DALI_TEST_EQUALS( value.Get<Vector4>(), color, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCreateCarouselEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateCarouselEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateDisplacementEffectDisplaced(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateDisplacementEffect(Toolkit::DISPLACEMENT_EFFECT_DISPLACED);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateDisplacementEffectFixed(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateDisplacementEffect(Toolkit::DISPLACEMENT_EFFECT_FIXED);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateDissolveEffect( bool highPrecision )
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateDissolveEffect( highPrecision );
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( "shader" );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( "vertex-shader" );
  DALI_TEST_CHECK( vertexShaderValue );

  std::string vertexShader;
  DALI_TEST_CHECK( vertexShaderValue->Get( vertexShader ) );
  DALI_TEST_CHECK( !vertexShader.empty() );

  Property::Value* fragmentShaderValue = customShader.Find( "fragment-shader" );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( "subdivide-grid-x" );
  DALI_TEST_CHECK( gridXValue );

  int gridX = 0;
  DALI_TEST_CHECK( gridXValue->Get( gridX ) );
  DALI_TEST_CHECK( gridX > 1 );

  Property::Value* gridYValue = customShader.Find( "subdivide-grid-y" );
  DALI_TEST_CHECK( gridYValue );

  int gridY = 0;
  DALI_TEST_CHECK( gridYValue->Get( gridY ) );
  DALI_TEST_CHECK( gridY > 1 );

  Property::Value* hintsValue = customShader.Find( "hints" );
  DALI_TEST_CHECK( hintsValue );

  std::string hints;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == "output-is-transparent" );

  Actor actor = Actor::New();
  Toolkit::DissolveEffectSetCentralLine( actor, Vector2::ONE, Vector2::ONE, 0.0f );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uPercentage" ) != Property::INVALID_INDEX );

  END_TEST;
}

int UtcDaliCreateDissolveEffectHighPrecision(void)
{
  return UtcDaliCreateDissolveEffect(true);
}

int UtcDaliCreateDissolveEffectMediumPrecision(void)
{
  return UtcDaliCreateDissolveEffect(false);
}

int UtcDaliCreateDissolveLocalEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateDissolveLocalEffect(0);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateDissolveEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateDistanceFieldEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( "shader" );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( "vertex-shader" );
  DALI_TEST_CHECK( !vertexShaderValue );

  Property::Value* fragmentShaderValue = customShader.Find( "fragment-shader" );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( "subdivide-grid-x" );
  DALI_TEST_CHECK( !gridXValue );

  Property::Value* hintsValue = customShader.Find( "hints" );
  DALI_TEST_CHECK( hintsValue );

  std::string hints;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == "output-is-transparent" );

  END_TEST;
}

int UtcDaliCreateImageRegionEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateImageRegionEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateIrisEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateIrisEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateMaskEffect(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();
  ShaderEffect effect = Toolkit::CreateMaskEffect(image);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateMirrorEffect(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();
  ShaderEffect effect = Toolkit::CreateMirrorEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateMotionBlurEffect(void)
{
  ToolkitTestApplication application;

  unsigned int sampleCount(4);
  ShaderEffect effect = Toolkit::CreateMotionBlurEffect(sampleCount);
  DALI_TEST_CHECK( effect );

  Property::Value value = effect.GetProperty( effect.GetPropertyIndex("uNumSamples"));
  DALI_TEST_EQUALS( value.Get<float>(), (float)sampleCount, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCreateMotionStretchEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateMotionStretchEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateOverlayEffect(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();
  ShaderEffect effect = Toolkit::CreateOverlayEffect(image);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateQuadraticBezier(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateQuadraticBezier(3,false);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateQuadraticBezierFilled(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateQuadraticBezier(3,true);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateRipple2DEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateRipple2DEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateRippleEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateRippleEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateShearEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateShearEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliSoftButtonEffectElliptical(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSoftButtonEffect(Toolkit::SOFT_BUTTON_ELLIPTICAL);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliSoftButtonEffectRectangular(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSoftButtonEffect(Toolkit::SOFT_BUTTON_RECTANGULAR);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliSoftButtonEffectFixed(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSoftButtonEffect(Toolkit::SOFT_BUTTON_FIXED);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateSpotEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSpotEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliCreateSquareDissolveEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSquareDissolveEffect();
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliSwirlEffect(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSwirlEffect(false);
  DALI_TEST_CHECK( effect );

  END_TEST;
}

int UtcDaliSwirlEffectWrap(void)
{
  ToolkitTestApplication application;

  ShaderEffect effect = Toolkit::CreateSwirlEffect(true);
  DALI_TEST_CHECK( effect );

  END_TEST;
}
