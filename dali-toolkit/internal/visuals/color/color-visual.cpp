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
#include "color-visual.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/devel-api/object/handle-devel.h>

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/color-visual-actions-devel.h>
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

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform highp   mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n

  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
  uniform mediump vec2 extraSize;\n

  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n

  void main()\n
  {\n
    gl_Position = uMvpMatrix * ComputeVertexPosition();\n
  }\n
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = vec4(mixColor, 1.0)*uColor;\n
  }\n
);

const char* VERTEX_SHADER_ROUNDED_CORNER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform highp   mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  varying mediump vec2 vPosition;\n
  varying mediump vec2 vRectSize;\n
  varying mediump float vCornerRadius;\n
  \n
  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec2 extraSize;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
  uniform mediump float cornerRadius;\n
  uniform mediump float cornerRadiusPolicy;\n
  \n
  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    mediump float minSize = min( visualSize.x, visualSize.y );\n
    vCornerRadius = mix( cornerRadius * minSize, cornerRadius, cornerRadiusPolicy);\n
    vCornerRadius = min( vCornerRadius, minSize * 0.5 );\n
    vRectSize = visualSize / 2.0 - vCornerRadius;\n
    vPosition = aPosition* visualSize;\n
    return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n
  \n
  void main()\n
  {\n
    gl_Position = uMvpMatrix * ComputeVertexPosition();\n
  }\n
);

//float distance = length( max( abs( position - center ), size ) - size ) - radius;
const char* FRAGMENT_SHADER_ROUNDED_CORNER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vPosition;\n
  varying mediump vec2 vRectSize;\n
  varying mediump float vCornerRadius;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  \n
  void main()\n
  {\n
      mediump float dist = length( max( abs( vPosition ), vRectSize ) - vRectSize ) - vCornerRadius;\n
      gl_FragColor = uColor * vec4( mixColor, 1.0 );\n
      gl_FragColor.a *= 1.0 - smoothstep( -1.0, 1.0, dist );\n
  }\n
);

const char* VERTEX_SHADER_BLUR_EDGE = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform highp   mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  varying mediump vec2 vPosition;\n
  varying mediump vec2 vRectSize;\n
  \n
  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec2 extraSize;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n
  uniform mediump float blurRadius;\n
  \n
  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize + blurRadius * 2.0;\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    vRectSize = visualSize / 2.0;\n
    vPosition = aPosition* visualSize;\n
    return vec4( vPosition + anchorPoint*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n
  \n
  void main()\n
  {\n
    gl_Position = uMvpMatrix * ComputeVertexPosition();\n
  }\n
);

const char* FRAGMENT_SHADER_BLUR_EDGE = DALI_COMPOSE_SHADER(
  varying mediump vec2 vPosition;\n
  varying mediump vec2 vRectSize;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform mediump float blurRadius;\n
  \n
  void main()\n
  {\n
      mediump vec2 blur = 1.0 - smoothstep( vRectSize - blurRadius * 2.0, vRectSize, abs( vPosition ) );\n
      gl_FragColor = uColor * vec4( mixColor, 1.0 );\n
      gl_FragColor.a *= blur.x * blur.y;\n
  }\n
);

}

ColorVisualPtr ColorVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  ColorVisualPtr colorVisualPtr( new ColorVisual( factoryCache ) );
  colorVisualPtr->SetProperties( properties );
  return colorVisualPtr;
}

ColorVisual::ColorVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::COLOR ),
  mBlurRadius( 0.0f ),
  mRenderIfTransparent( false )
{
}

ColorVisual::~ColorVisual()
{
}

void ColorVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // By virtue of DoSetProperties being called last, this will override
  // anything set by Toolkit::Visual::Property::MIX_COLOR
  Property::Value* colorValue = propertyMap.Find( Toolkit::ColorVisual::Property::MIX_COLOR, MIX_COLOR );
  if( colorValue )
  {
    Vector4 color;
    if( colorValue->Get( color ) )
    {
      Property::Type type = colorValue->GetType();
      if( type == Property::VECTOR4 )
      {
        SetMixColor( color );
      }
      else if( type == Property::VECTOR3 )
      {
        Vector3 color3(color);
        SetMixColor( color3 );
      }
    }
    else
    {
      DALI_LOG_ERROR("ColorVisual: mixColor property has incorrect type\n");
    }
  }

  Property::Value* renderIfTransparentValue = propertyMap.Find( Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT, RENDER_IF_TRANSPARENT_NAME );
  if( renderIfTransparentValue )
  {
    if( ! renderIfTransparentValue->Get( mRenderIfTransparent ) )
    {
      DALI_LOG_ERROR( "ColorVisual: renderIfTransparent property has incorrect type: %d\n", renderIfTransparentValue->GetType() );
    }
  }

  Property::Value* blurRadiusValue = propertyMap.Find( Toolkit::DevelColorVisual::Property::BLUR_RADIUS, BLUR_RADIUS_NAME );
  if( blurRadiusValue )
  {
    if( !blurRadiusValue->Get( mBlurRadius ) )
    {
      DALI_LOG_ERROR( "ColorVisual:DoSetProperties:: BLUR_RADIUS property has incorrect type: %d\n", blurRadiusValue->GetType() );
    }
  }
}

void ColorVisual::DoSetOnScene( Actor& actor )
{
  InitializeRenderer();

  // Only add the renderer if it's not fully transparent
  // We cannot avoid creating a renderer as it's used in the base class
  if( mRenderIfTransparent || mImpl->mMixColor.a > 0.0f )
  {
    actor.AddRenderer( mImpl->mRenderer );
  }

  // Color Visual generated and ready to display
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void ColorVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR );
  map.Insert( Toolkit::ColorVisual::Property::MIX_COLOR, mImpl->mMixColor );
  map.Insert( Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT, mRenderIfTransparent );
  map.Insert( Toolkit::DevelColorVisual::Property::BLUR_RADIUS, mBlurRadius );
}

void ColorVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Do nothing
}


void ColorVisual::OnSetTransform()
{
  if( mImpl->mRenderer )
  {
    mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
  }
}

void ColorVisual::OnDoAction( const Property::Index actionId, const Property::Value& attributes )
{
  // Check if action is valid for this visual type and perform action if possible
  switch( actionId )
  {
    case DevelColorVisual::Action::UPDATE_PROPERTY:
    {
      Property::Map* map = attributes.GetMap();
      if( map )
      {
        DoSetProperties( *map );
      }
      break;
    }
  }
}

void ColorVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  Shader shader;
  if( !EqualsZero( mBlurRadius ) )
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::COLOR_SHADER_BLUR_EDGE );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER_BLUR_EDGE, FRAGMENT_SHADER_BLUR_EDGE );
      mFactoryCache.SaveShader( VisualFactoryCache::COLOR_SHADER_BLUR_EDGE, shader );
    }
  }
  else if( !IsRoundedCornerRequired() )
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::COLOR_SHADER );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
      mFactoryCache.SaveShader( VisualFactoryCache::COLOR_SHADER, shader );
    }
  }
  else
  {
    shader = mFactoryCache.GetShader( VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER );
    if( !shader )
    {
      shader = Shader::New( VERTEX_SHADER_ROUNDED_CORNER, FRAGMENT_SHADER_ROUNDED_CORNER );
      mFactoryCache.SaveShader( VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER, shader );
    }
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  // ColorVisual has it's own index key for mix color - use this instead
  // of using the new base index to avoid changing existing applications
  // String keys will get to this property.
  mImpl->mMixColorIndex = DevelHandle::RegisterProperty( mImpl->mRenderer, Toolkit::ColorVisual::Property::MIX_COLOR, MIX_COLOR, Vector3(mImpl->mMixColor) );

  mImpl->mRenderer.RegisterProperty( BLUR_RADIUS_NAME, mBlurRadius );

  if( mImpl->mMixColor.a < 1.f || !EqualsZero( mBlurRadius ) )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  }

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
