/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/shader-devel.h>

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>

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
#if 0
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n

  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n

  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
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
  uniform lowp float opacity;\n
  \n
  void main()\n
  {\n
    gl_FragColor = vec4(mixColor, opacity)*uColor;\n
  }\n
);
#endif
}

ColorVisualPtr ColorVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  ColorVisualPtr colorVisualPtr( new ColorVisual( factoryCache ) );
  colorVisualPtr->SetProperties( properties );
  return colorVisualPtr;
}

ColorVisual::ColorVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache )
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
}

void ColorVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();

  actor.AddRenderer( mImpl->mRenderer );

  // Color Visual generated and ready to display
  ResourceReady( Toolkit::Visual::ResourceStatus::READY );
}

void ColorVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR );
  map.Insert( Toolkit::ColorVisual::Property::MIX_COLOR, mImpl->mMixColor );
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

void ColorVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );

  Shader shader = mFactoryCache.GetShader( VisualFactoryCache::COLOR_SHADER );
  if( !shader )
  {
    //shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );

    shader = DevelShader::New(
      GraphicsGetBuiltinShaderId( SHADER_COLOR_VISUAL_SHADER_VERT ),
      GraphicsGetBuiltinShaderId( SHADER_COLOR_VISUAL_SHADER_FRAG ),
      DevelShader::ShaderLanguage::SPIRV_1_0,
      Property::Map()
    );
    mFactoryCache.SaveShader( VisualFactoryCache::COLOR_SHADER, shader );
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  // ColorVisual has it's own index key for mix color - use this instead
  // of using the new base index to avoid changing existing applications
  // String keys will get to this property.
  mImpl->mMixColorIndex = DevelHandle::RegisterProperty( mImpl->mRenderer, Toolkit::ColorVisual::Property::MIX_COLOR, MIX_COLOR, Vector3(mImpl->mMixColor) );

  if( mImpl->mMixColor.a < 1.f )
  {
    mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON );
  }

  // Register transform properties
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
