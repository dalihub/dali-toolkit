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

// CLASS HEADER
#include "gradient-renderer.h"

// EXTERNAL INCLUDES
#include <typeinfo>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/object/property-array.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/renderer-factory-impl.h>
#include <dali-toolkit/internal/controls/renderers/renderer-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/control-renderer-data-impl.h>
#include <dali-toolkit/internal/controls/renderers/gradient/linear-gradient.h>
#include <dali-toolkit/internal/controls/renderers/gradient/radial-gradient.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
const char * const RENDERER_TYPE("renderer-type");
const char * const RENDERER_TYPE_VALUE("gradient-renderer");

// properties: linear gradient
const char * const GRADIENT_START_POSITION_NAME("gradient-start-position"); // Property::VECTOR2
const char * const GRADIENT_END_POSITION_NAME("gradient-end-position"); // Property::VECTOR2

// properties: radial gradient
const char * const GRADIENT_CENTER_NAME("gradient-center"); // Property::VECTOR2
const char * const GRADIENT_RADIUS_NAME("gradient-radius"); // Property::FLOAT

// properties: linear&radial gradient
const char * const GRADIENT_STOP_OFFSET_NAME("gradient-stop-offset"); // Property::Array FLOAT
const char * const GRADIENT_STOP_COLOR_NAME("gradient-stop-color"); // Property::Array VECTOR4
const char * const GRADIENT_UNITS_NAME("gradient-units"); // Property::String  "userSpaceOnUse | objectBoundingBox"
const char * const GRADIENT_SPREAD_METHOD_NAME("gradient-spread-method"); // Property::String  "pad | reflect | repeat"

// string values
const char * const UNIT_USER_SPACE("user-space");
const char * const UNIT_BOUNDING_BOX("object-bounding-box");
const char * const SPREAD_PAD("pad");
const char * const SPREAD_REFLECT("reflect");
const char * const SPREAD_REPEAT("repeat");

// uniform names
const char * const UNIFORM_ALIGNMENT_MATRIX_NAME( "uAlignmentMatrix" );
const char * const UNIFORM_TEXTULRE_NAME("sTexture");


const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat3 uAlignmentMatrix;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    gl_Position = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;\n
  }\n
);

const char* FRAGMENT_SHADER_LINEAR = DALI_COMPOSE_SHADER(
  uniform sampler2D sTexture;\n // sampler1D?
  uniform lowp vec4 uColor;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vec2( vTexCoord.y, 0.5 ) ) * uColor;\n
  }\n
);

const char* FRAGMENT_SHADER_RADIAL = DALI_COMPOSE_SHADER(
  uniform sampler2D sTexture;\n // sampler1D?
  uniform lowp vec4 uColor;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vec2( length(vTexCoord), 0.5 ) ) * uColor;\n
  }\n
);

Sampler::WrapMode GetWrapMode( Gradient::SpreadMethod spread )
{
  switch(spread)
  {
    case Gradient::REPEAT:
    {
      return Sampler::REPEAT;
    }
    case Gradient::REFLECT:
    {
      return Sampler::MIRRORED_REPEAT;
    }
    case Gradient::PAD:
    default:
    {
      return Sampler::CLAMP_TO_EDGE;
    }
  }
}

}


GradientRenderer::GradientRenderer()
:mGradientTransformIndex( Property::INVALID_INDEX )
{
}

GradientRenderer::~GradientRenderer()
{
}

void GradientRenderer::DoInitialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap )
{
  mImpl->mGeometry = factoryCache.GetGeometry( RendererFactoryCache::QUAD_GEOMETRY );
  if( !(mImpl->mGeometry) )
  {
    mImpl->mGeometry =  RendererFactoryCache::CreateQuadGeometry();
    factoryCache.SaveGeometry( RendererFactoryCache::QUAD_GEOMETRY, mImpl->mGeometry );
  }

  Type gradientType;
  if( propertyMap.Find( GRADIENT_RADIUS_NAME ))
  {
    mImpl->mShader = factoryCache.GetShader( RendererFactoryCache::GRADIENT_SHADER_RADIAL );
    if( !(mImpl->mShader) )
    {
      mImpl->mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_RADIAL );
      factoryCache.SaveShader( RendererFactoryCache::GRADIENT_SHADER_RADIAL, mImpl->mShader );
    }
    gradientType = RADIAL;
  }
  else
  {
    mImpl->mShader = factoryCache.GetShader( RendererFactoryCache::GRADIENT_SHADER_LINEAR );
    if( !(mImpl->mShader) )
    {
      mImpl->mShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_LINEAR );
      factoryCache.SaveShader( RendererFactoryCache::GRADIENT_SHADER_LINEAR, mImpl->mShader );
    }
    gradientType = LINEAR;
  }

  if( NewGradient(gradientType, propertyMap) )
  {
    mGradientTransform = mGradient->GetAlignmentTransform();
  }
  else
  {
    DALI_LOG_ERROR( "Fail to provide valid properties to create a GradientRenderer object" );
  }
}

void GradientRenderer::SetSize( const Vector2& size )
{
  ControlRenderer::SetSize( size );

  if( mGradient->GetGradientUnits() == Gradient::OBJECT_BOUNDING_BOX )
  {
    // Apply scaling
    Matrix3 scaling( 1.f/(size.x+Math::MACHINE_EPSILON_100), 0.f, 0.f,
                     0.f, 1.f/(size.y+Math::MACHINE_EPSILON_100), 0.f, 0.5f, 0.5f, 1.f );
    Matrix3::Multiply( mGradientTransform, scaling, mGradient->GetAlignmentTransform() );

    if( mImpl->mRenderer )
    {
      (mImpl->mRenderer).SetProperty( mGradientTransformIndex, mGradientTransform );
    }
  }
}

void GradientRenderer::SetClipRect( const Rect<int>& clipRect )
{
  ControlRenderer::SetClipRect( clipRect );

  //ToDo: renderer responds to the clipRect change
}

void GradientRenderer::SetOffset( const Vector2& offset )
{
  //ToDo: renderer applies the offset
}

void GradientRenderer::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, RENDERER_TYPE_VALUE );

  Gradient::GradientUnits units = mGradient->GetGradientUnits();
  if( units == Gradient::USER_SPACE_ON_USE )
  {
    map.Insert( GRADIENT_UNITS_NAME, UNIT_USER_SPACE );
  }
  else // if( units == Gradient::OBJECT_BOUNDING_BOX )
  {
    map.Insert( GRADIENT_UNITS_NAME, UNIT_BOUNDING_BOX );
  }

  Gradient::SpreadMethod spread = mGradient->GetSpreadMethod();
  if( spread == Gradient::PAD )
  {
    map.Insert( GRADIENT_SPREAD_METHOD_NAME, SPREAD_PAD );
  }
  else if( spread == Gradient::REFLECT )
  {
    map.Insert( GRADIENT_SPREAD_METHOD_NAME, SPREAD_REFLECT );
  }
  else // if( units == Gradient::REPEAT )
  {
    map.Insert( GRADIENT_SPREAD_METHOD_NAME, SPREAD_REPEAT );
  }

  const Vector<Gradient::GradientStop>& stops( mGradient->GetStops() );
  Property::Array offsets;
  Property::Array colors;
  for( unsigned int i=0; i<stops.Count(); i++ )
  {
    offsets.PushBack( stops[i].mOffset );
    colors.PushBack( stops[i].mStopColor );
  }

  map.Insert( GRADIENT_STOP_OFFSET_NAME, offsets );
  map.Insert( GRADIENT_STOP_COLOR_NAME, colors );

  if( &typeid( *mGradient ) == &typeid(LinearGradient) )
  {
    LinearGradient* gradient = static_cast<LinearGradient*>( mGradient.Get() );
    map.Insert( GRADIENT_START_POSITION_NAME, gradient->GetStartPosition() );
    map.Insert( GRADIENT_END_POSITION_NAME, gradient->GetEndPosition() );
  }
  else // if( &typeid( *mGradient ) == &typeid(RadialGradient) )
  {
    RadialGradient* gradient = static_cast<RadialGradient*>( mGradient.Get() );
    map.Insert( GRADIENT_CENTER_NAME, gradient->GetCenter() );
    map.Insert( GRADIENT_RADIUS_NAME, gradient->GetRadius() );
  }
}

void GradientRenderer::DoSetOnStage( Actor& actor )
{
  mGradientTransformIndex = (mImpl->mRenderer).RegisterProperty( UNIFORM_ALIGNMENT_MATRIX_NAME, mGradientTransform );

  Dali::BufferImage lookupTexture = mGradient->GenerateLookupTexture();
  Sampler sampler = Sampler::New( lookupTexture, UNIFORM_TEXTULRE_NAME );
  Sampler::WrapMode wrap = GetWrapMode( mGradient->GetSpreadMethod() );
  sampler.SetWrapMode(  wrap, wrap  );

  Material material = (mImpl->mRenderer).GetMaterial();
  if( material )
  {
    material.AddSampler( sampler );
  }
}

bool GradientRenderer::NewGradient(Type gradientType, const Property::Map& propertyMap)
{
  if( gradientType==LINEAR )
  {
    Property::Value* startPositionValue = propertyMap.Find( GRADIENT_START_POSITION_NAME );
    Property::Value* endPositionValue = propertyMap.Find( GRADIENT_END_POSITION_NAME );
    Vector2 startPosition;
    Vector2 endPosition;

    if( startPositionValue && startPositionValue->Get(startPosition)
     && endPositionValue && endPositionValue->Get( endPosition ) )
    {
      mGradient = new LinearGradient( startPosition, endPosition );
    }
    else
    {
      return false;
    }
  }
  else // type==RADIAL
  {
    Property::Value* centerValue = propertyMap.Find( GRADIENT_CENTER_NAME );
    Property::Value* radiusValue = propertyMap.Find( GRADIENT_RADIUS_NAME );
    Vector2 center;
    float radius;
    if( centerValue && centerValue->Get(center)
        && radiusValue && radiusValue->Get(radius) )
    {
      mGradient = new RadialGradient( center, radius );
    }
    else
    {
      return false;
    }
  }

  unsigned int numValidStop = 0u;
  Property::Value* stopOffsetValue = propertyMap.Find( GRADIENT_STOP_OFFSET_NAME );
  Property::Value* stopColorValue = propertyMap.Find( GRADIENT_STOP_COLOR_NAME );
  if( stopOffsetValue && stopColorValue )
  {
    Vector<float> offsetArray;
    Property::Array* colorArray = stopColorValue->GetArray();
    if( colorArray && GetStopOffsets( stopOffsetValue, offsetArray ))
    {
      unsigned int numStop = offsetArray.Count() < colorArray->Count() ?
                             offsetArray.Count() : colorArray->Count();
      Vector4 color;
      for( unsigned int i=0; i<numStop; i++ )
      {
        if( (colorArray->GetElementAt(i)).Get(color) )
        {
          mGradient->AddStop( offsetArray[i], color);
          numValidStop++;
        }
      }
    }
  }

  if( numValidStop < 1u ) // no valid stop
  {
    return false;
  }

  Property::Value* unitsValue = propertyMap.Find( GRADIENT_UNITS_NAME );
  std::string units;
  // The default unit is OBJECT_BOUNDING_BOX.
  // Only need to set new units if 'user-space'
  if( unitsValue && unitsValue->Get( units ) && units == UNIT_USER_SPACE )
  {
     mGradient->SetGradientUnits( Gradient::USER_SPACE_ON_USE );
  }

  Property::Value* spread = propertyMap.Find( GRADIENT_SPREAD_METHOD_NAME );
  std::string stringValue ;
  // The default spread method is PAD.
  // Only need to set new spread if 'reflect' or 'repeat"
  if( spread && spread->Get( stringValue ))
  {
    if( stringValue == SPREAD_REFLECT )
    {
      mGradient->SetSpreadMethod( Gradient::REFLECT );
    }
    else if( stringValue == SPREAD_REPEAT )
    {
      mGradient->SetSpreadMethod( Gradient::REPEAT );
    }
  }

  return true;
}

bool GradientRenderer::GetStopOffsets(const Property::Value* value, Vector<float>& stopOffsets)
{
  Vector2 offset2;
  if( value->Get( offset2 ) )
  {
    stopOffsets.PushBack( offset2.x );
    stopOffsets.PushBack( offset2.y );
    return true;
  }

  Vector3 offset3;
  if( value->Get( offset3 ) )
  {
    stopOffsets.PushBack( offset3.x );
    stopOffsets.PushBack( offset3.y );
    stopOffsets.PushBack( offset3.z );
    return true;
  }

  Vector4 offset4;
  if( value->Get( offset4 ) )
  {
    stopOffsets.PushBack( offset4.x );
    stopOffsets.PushBack( offset4.y );
    stopOffsets.PushBack( offset4.z );
    stopOffsets.PushBack( offset4.w );
    return true;
  }

  Property::Array* offsetArray = value->GetArray();
  if( offsetArray )
  {
    unsigned int numStop = offsetArray->Count();
    float offset;
    for( unsigned int i=0; i<numStop; i++ )
    {
      if( offsetArray->GetElementAt(i).Get(offset) )
      {
        stopOffsets.PushBack( offset );
      }
    }
    return true;
  }

  return false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
