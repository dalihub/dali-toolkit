/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/shader-effects/dissolve-effect.h>
#include <dali-toolkit/devel-api/shader-effects/distance-field-effect.h>
#include <dali-toolkit/devel-api/shader-effects/image-region-effect.h>
#include <dali-toolkit/devel-api/shader-effects/motion-blur-effect.h>
#include <dali-toolkit/devel-api/shader-effects/motion-stretch-effect.h>

using namespace Dali;
using namespace Dali::Toolkit;

int UtcDaliCreateAlphaDiscardEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateAlphaDiscardEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( !vertexShaderValue );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( !gridXValue );

  Property::Value* gridYValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_Y );
  DALI_TEST_CHECK( !gridYValue );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( !hintsValue );

  END_TEST;
}

int UtcDaliCreateDissolveEffect( bool highPrecision )
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateDissolveEffect( highPrecision );
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( vertexShaderValue );

  std::string vertexShader;
  DALI_TEST_CHECK( vertexShaderValue->Get( vertexShader ) );
  DALI_TEST_CHECK( !vertexShader.empty() );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( gridXValue );

  int gridX = 0;
  DALI_TEST_CHECK( gridXValue->Get( gridX ) );
  DALI_TEST_CHECK( gridX > 1 );

  Property::Value* gridYValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_Y );
  DALI_TEST_CHECK( gridYValue );

  int gridY = 0;
  DALI_TEST_CHECK( gridYValue->Get( gridY ) );
  DALI_TEST_CHECK( gridY > 1 );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( hintsValue );

  int hints;;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == Shader::Hint::OUTPUT_IS_TRANSPARENT );

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

int UtcDaliCreateDissolveEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateDistanceFieldEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( !vertexShaderValue );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( !gridXValue );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( hintsValue );

  int hints;;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == Shader::Hint::OUTPUT_IS_TRANSPARENT );

  END_TEST;
}

int UtcDaliCreateImageRegionEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateImageRegionEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( vertexShaderValue );

  std::string vertexShader;
  DALI_TEST_CHECK( vertexShaderValue->Get( vertexShader ) );
  DALI_TEST_CHECK( !vertexShader.empty() );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( !fragmentShaderValue );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( !gridXValue );

  Property::Value* gridYValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_Y );
  DALI_TEST_CHECK( !gridYValue );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( !hintsValue );

  END_TEST;
}

int UtcDaliCreateMotionBlurEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateMotionBlurEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( vertexShaderValue );

  std::string vertexShader;
  DALI_TEST_CHECK( vertexShaderValue->Get( vertexShader ) );
  DALI_TEST_CHECK( !vertexShader.empty() );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( gridXValue );

  int gridX = 0;
  DALI_TEST_CHECK( gridXValue->Get( gridX ) );
  DALI_TEST_CHECK( gridX > 1 );

  Property::Value* gridYValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_Y );
  DALI_TEST_CHECK( gridYValue );

  int gridY = 0;
  DALI_TEST_CHECK( gridYValue->Get( gridY ) );
  DALI_TEST_CHECK( gridY > 1 );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( hintsValue );

  int hints;;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == Shader::Hint::OUTPUT_IS_TRANSPARENT );

  unsigned int sampleCount( 4 );
  Actor actor = Actor::New();
  Toolkit::SetMotionBlurProperties( actor, sampleCount );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uBlurTexCoordScale" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uGeometryStretchFactor" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uSpeedScalingFactor" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uObjectFadeStart" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uObjectFadeEnd" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uAlphaScale" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uNumSamples" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uModelLastFrame" ) != Property::INVALID_INDEX );

  END_TEST;
}

int UtcDaliCreateMotionStretchEffect(void)
{
  ToolkitTestApplication application;

  Property::Map effect = Toolkit::CreateMotionStretchEffect();
  DALI_TEST_CHECK( !effect.Empty() );

  Property::Value* customShaderValue = effect.Find( Visual::Property::SHADER );
  DALI_TEST_CHECK( customShaderValue );

  Property::Map customShader;
  DALI_TEST_CHECK( customShaderValue->Get( customShader ) );

  Property::Value* vertexShaderValue = customShader.Find( Visual::Shader::Property::VERTEX_SHADER );
  DALI_TEST_CHECK( vertexShaderValue );

  std::string vertexShader;
  DALI_TEST_CHECK( vertexShaderValue->Get( vertexShader ) );
  DALI_TEST_CHECK( !vertexShader.empty() );

  Property::Value* fragmentShaderValue = customShader.Find( Visual::Shader::Property::FRAGMENT_SHADER );
  DALI_TEST_CHECK( fragmentShaderValue );

  std::string fragmentShader;
  DALI_TEST_CHECK( fragmentShaderValue->Get( fragmentShader ) );
  DALI_TEST_CHECK( !fragmentShader.empty() );

  Property::Value* gridXValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_X );
  DALI_TEST_CHECK( gridXValue );

  int gridX = 0;
  DALI_TEST_CHECK( gridXValue->Get( gridX ) );
  DALI_TEST_CHECK( gridX > 1 );

  Property::Value* gridYValue = customShader.Find( Visual::Shader::Property::SUBDIVIDE_GRID_Y );
  DALI_TEST_CHECK( gridYValue );

  int gridY = 0;
  DALI_TEST_CHECK( gridYValue->Get( gridY ) );
  DALI_TEST_CHECK( gridY > 1 );

  Property::Value* hintsValue = customShader.Find( Visual::Shader::Property::HINTS );
  DALI_TEST_CHECK( hintsValue );

  int hints;;
  DALI_TEST_CHECK( hintsValue->Get( hints ) );
  DALI_TEST_CHECK( hints == Shader::Hint::OUTPUT_IS_TRANSPARENT );

  Actor actor = Actor::New();
  Toolkit::SetMotionStretchProperties( actor );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uGeometryStretchFactor" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uSpeedScalingFactor" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uObjectFadeStart" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uObjectFadeEnd" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uAlphaScale" ) != Property::INVALID_INDEX );
  DALI_TEST_CHECK( actor.GetPropertyIndex( "uModelLastFrame" ) != Property::INVALID_INDEX );

  END_TEST;
}
