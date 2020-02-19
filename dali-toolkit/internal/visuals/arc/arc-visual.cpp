/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/arc/arc-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-actions-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// cap
DALI_ENUM_TO_STRING_TABLE_BEGIN( CAP )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelArcVisual::Cap, BUTT )
DALI_ENUM_TO_STRING_WITH_SCOPE( DevelArcVisual::Cap, ROUND )
DALI_ENUM_TO_STRING_TABLE_END( CAP )

const char* VERTEX_SHADER =
  "INPUT mediump vec2 aPosition;\n"
  "OUTPUT mediump vec2 vPosition;\n"

  "uniform highp mat4 uMvpMatrix;\n"
  "uniform highp vec3 uSize;\n"

  "//Visual size and offset\n"
  "uniform mediump vec2 offset;\n"
  "uniform highp vec2 size;\n"
  "uniform mediump vec4 offsetSizeMode;\n"
  "uniform mediump vec2 origin;\n"
  "uniform mediump vec2 anchorPoint;\n"

  "vec4 ComputeVertexPosition()\n"
  "{\n"
  "  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n"
  "  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n"
  "  vPosition = aPosition* visualSize;\n"
  "  return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n"
  "}\n"

  "void main()\n"
  "{\n"
  "  gl_Position = uMvpMatrix * ComputeVertexPosition();\n"
  "}\n";

const char* FRAGMENT_SHADER_BUTT_CAP =
  "INPUT mediump vec2 vPosition;\n"

  "uniform lowp vec4 uColor;\n"
  "uniform lowp vec3 mixColor;\n"
  "uniform mediump float thickness;\n"
  "uniform mediump float radius;\n"
  "uniform mediump float startAngle;\n"
  "uniform mediump float sweepAngle;\n"

  "const mediump float M_PI_OVER_2 = 1.57079632679;\n"
  "const mediump float M_PI = 3.14159265359;\n"
  "const mediump float M_PI_2 = 6.28318530718;\n"

  "mediump float GetOpacity()\n"
  "{\n"
  "  mediump float start = radians( mod( startAngle, 360.0 ) );\n"
  "  mediump float angle = mod( atan( vPosition.y, vPosition.x ) + M_PI_OVER_2 - start, M_PI_2 );\n"
  "  mediump float dist = length( vPosition );\n"
  "  if( angle <= radians( sweepAngle ) )\n"
  "  {\n"
  "    return smoothstep( -1.0, 1.0, thickness / 2.0 - ( abs( dist - radius ) ) );\n"
  "  }\n"
  "  mediump float end = radians( mod( startAngle + sweepAngle, 360.0 ) );\n"
  "  mediump vec2 q0 = vec2( dist * cos( start - M_PI_OVER_2 ), dist * sin( start - M_PI_OVER_2 ) );\n"
  "  mediump vec2 q1 = vec2( dist * cos( end - M_PI_OVER_2 ), dist * sin( end - M_PI_OVER_2 ) );\n"
  "  mediump float opacity = 1.0 - smoothstep( 0.0, 2.0, min( length( vPosition - q0 ), length( vPosition - q1 ) ) );\n"
  "  opacity *= step( 0.0, thickness / 2.0 - abs( dist - radius ) );\n"
  "  return opacity;\n"
  "}\n"

  "void main()\n"
  "{\n"
  "  OUT_COLOR = vec4( mixColor, 1.0 ) * uColor;\n"
  "  OUT_COLOR.a *= GetOpacity();\n"
  "}\n";

const char* FRAGMENT_SHADER_ROUND_CAP =
  "INPUT mediump vec2 vPosition;\n"

  "uniform lowp vec4 uColor;\n"
  "uniform lowp vec3 mixColor;\n"
  "uniform mediump float thickness;\n"
  "uniform mediump float radius;\n"
  "uniform mediump float startAngle;\n"
  "uniform mediump float sweepAngle;\n"

  "const mediump float M_PI_OVER_2 = 1.57079632679;\n"
  "const mediump float M_PI_2 = 6.28318530718;\n"

  "mediump float GetOpacity()\n"
  "{\n"
  "  mediump float start = radians( mod( startAngle, 360.0 ) );\n"
  "  mediump float angle = mod( atan( vPosition.y, vPosition.x ) + M_PI_OVER_2 - start, M_PI_2 );\n"
  "  mediump float dist = length( vPosition );\n"
  "  if( angle <= radians( sweepAngle ) )\n"
  "  {\n"
  "    return smoothstep( -1.0, 1.0, thickness / 2.0 - ( abs( dist - radius ) ) );\n"
  "  }\n"
  "  mediump float end = radians( mod( startAngle + sweepAngle, 360.0 ) );\n"
  "  mediump vec2 q0 = vec2( radius * cos( start - M_PI_OVER_2 ), radius * sin( start - M_PI_OVER_2 ) );\n"
  "  mediump vec2 q1 = vec2( radius * cos( end - M_PI_OVER_2 ), radius * sin( end - M_PI_OVER_2 ) );\n"
  "  return smoothstep( -1.0, 1.0, thickness / 2.0 - min( length( vPosition - q0 ), length( vPosition - q1 ) ) );\n"
  "}\n"

  "void main()\n"
  "{\n"
  "  OUT_COLOR = vec4( mixColor, 1.0 ) * uColor;\n"
  "  OUT_COLOR.a *= GetOpacity();\n"
  "}\n";

}

ArcVisualPtr ArcVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  ArcVisualPtr arcVisualPtr( new ArcVisual( factoryCache ) );
  arcVisualPtr->SetProperties( properties );
  return arcVisualPtr;
}

ArcVisual::ArcVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, static_cast<Toolkit::Visual::Type>( Toolkit::DevelVisual::ARC ) ),
  mThickness( 0.0f ),
  mRadius( 0.0f ),
  mStartAngle( 0.0f ),
  mSweepAngle( 360.0f ),
  mRadiusIndex( Property::INVALID_INDEX ),
  mThicknessIndex( Property::INVALID_INDEX ),
  mStartAngleIndex( Property::INVALID_INDEX ),
  mSweepAngleIndex( Property::INVALID_INDEX ),
  mCapType( DevelArcVisual::Cap::BUTT )
{
}

ArcVisual::~ArcVisual()
{
}

void ArcVisual::DoSetProperties( const Property::Map& propertyMap )
{
  Property::Value* thicknessValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::THICKNESS, THICKNESS_NAME );
  if( thicknessValue )
  {
    if( !thicknessValue->Get( mThickness ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: THICKNESS property has incorrect type: %d\n", thicknessValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mThicknessIndex, mThickness );

        // Need to calculate radius again
        OnSetTransform();
      }
    }
  }

  Property::Value* startAngleValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::START_ANGLE, START_ANGLE_NAME );
  if( startAngleValue )
  {
    if( !startAngleValue->Get( mStartAngle ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: START_ANGLE property has incorrect type: %d\n", startAngleValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mStartAngleIndex, mStartAngle );
      }
    }
  }

  Property::Value* sweepAngleValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::SWEEP_ANGLE, SWEEP_ANGLE_NAME );
  if( sweepAngleValue )
  {
    if( !sweepAngleValue->Get( mSweepAngle ) )
    {
      DALI_LOG_ERROR( "ArcVisual:DoSetProperties:: SWEEP_ANGLE property has incorrect type: %d\n", sweepAngleValue->GetType() );
    }
    else
    {
      if( mImpl->mRenderer )
      {
        mImpl->mRenderer.SetProperty( mSweepAngleIndex, mSweepAngle );
      }
    }
  }

  Property::Value* capValue = propertyMap.Find( Toolkit::DevelArcVisual::Property::CAP, CAP_NAME );
  if( capValue )
  {
    int capType = 0;
    Scripting::GetEnumerationProperty( *capValue, CAP_TABLE, CAP_TABLE_COUNT, capType );
    mCapType = Toolkit::DevelArcVisual::Cap::Type( capType );
  }
}

void ArcVisual::DoSetOnScene( Actor& actor )
{
  InitializeRenderer();

  actor.AddRenderer( mImpl->mRenderer );

  // Arc Visual generated and ready to display
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void ArcVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::ARC );
  map.Insert( Toolkit::DevelArcVisual::Property::THICKNESS, mThickness );
  map.Insert( Toolkit::DevelArcVisual::Property::START_ANGLE, mStartAngle );
  map.Insert( Toolkit::DevelArcVisual::Property::SWEEP_ANGLE, mSweepAngle );
  map.Insert( Toolkit::DevelArcVisual::Property::CAP, mCapType );
}

void ArcVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}

void ArcVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );
  mRadius = ( std::min( visualSize.width, visualSize.height ) - mThickness ) / 2.0f;

  if( mImpl->mRenderer )
  {
    mImpl->mRenderer.SetProperty( mRadiusIndex, mRadius );
  }
}

void ArcVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelArcVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if( map )
      {
        DoSetProperties( *map );
      }
      break;
    }
  }
}

void ArcVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  Shader shader;
  if( mCapType == DevelArcVisual::Cap::BUTT )
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::ARC_BUTT_CAP_SHADER );
    if( !shader )
    {
      shader = Shader::New( Dali::Shader::GetVertexShaderPrefix() + VERTEX_SHADER, Dali::Shader::GetFragmentShaderPrefix() + FRAGMENT_SHADER_BUTT_CAP );
      mFactoryCache.SaveShader( VisualFactoryCache::ARC_BUTT_CAP_SHADER, shader );
    }
  }
  else
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::ARC_ROUND_CAP_SHADER );
    if( !shader )
    {
      shader = Shader::New( Dali::Shader::GetVertexShaderPrefix() + VERTEX_SHADER, Dali::Shader::GetFragmentShaderPrefix() + FRAGMENT_SHADER_ROUND_CAP );
      mFactoryCache.SaveShader( VisualFactoryCache::ARC_ROUND_CAP_SHADER, shader );
    }
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  mThicknessIndex = mImpl->mRenderer.RegisterProperty( THICKNESS_NAME, mThickness );
  mStartAngleIndex = mImpl->mRenderer.RegisterProperty( START_ANGLE_NAME, mStartAngle );
  mSweepAngleIndex = mImpl->mRenderer.RegisterProperty( SWEEP_ANGLE_NAME, mSweepAngle );

  mRadiusIndex = mImpl->mRenderer.RegisterProperty( RADIUS_NAME, mRadius );

  mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
