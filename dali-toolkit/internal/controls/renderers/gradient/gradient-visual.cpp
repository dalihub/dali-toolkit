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

// CLASS HEADER
#include "gradient-visual.h"

// EXTERNAL INCLUDES
#include <typeinfo>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/object/property-array.h>

//INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/visual-factory-impl.h>
#include <dali-toolkit/internal/controls/renderers/visual-factory-cache.h>
#include <dali-toolkit/internal/controls/renderers/visual-data-impl.h>
#include <dali-toolkit/internal/controls/renderers/visual-string-constants.h>
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
// properties: linear gradient
const char * const START_POSITION_NAME("startPosition"); // Property::VECTOR2
const char * const END_POSITION_NAME("endPosition"); // Property::VECTOR2

// properties: radial gradient
const char * const CENTER_NAME("center"); // Property::VECTOR2
const char * const RADIUS_NAME("radius"); // Property::FLOAT

// properties: linear&radial gradient
const char * const STOP_OFFSET_NAME("stopOffset"); // Property::Array FLOAT
const char * const STOP_COLOR_NAME("stopColor"); // Property::Array VECTOR4
const char * const UNITS_NAME("units"); // Property::String  "userSpaceOnUse | objectBoundingBox"
const char * const SPREAD_METHOD_NAME("spreadMethod"); // Property::String  "pad | reflect | repeat"

// string values
const char * const UNIT_USER_SPACE("USER_SPACE");
const char * const UNIT_BOUNDING_BOX("OBJECT_BOUNDING_BOX");
const char * const SPREAD_PAD("PAD");
const char * const SPREAD_REFLECT("REFLECT");
const char * const SPREAD_REPEAT("REPEAT");

// uniform names
const char * const UNIFORM_ALIGNMENT_MATRIX_NAME( "uAlignmentMatrix" );

// default offset value
const unsigned int DEFAULT_OFFSET_MINIMUM = 0.0f;
const unsigned int DEFAULT_OFFSET_MAXIMUM = 1.0f;

VisualFactoryCache::ShaderType GetShaderType( GradientVisual::Type type, Gradient::GradientUnits units)
{
  if( type==GradientVisual::LINEAR )
  {
   if( units == Gradient::USER_SPACE_ON_USE )
   {
     return VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE;
   }
   return VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX;
  }
  else if( units == Gradient::USER_SPACE_ON_USE )
  {
    return VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE;
  }

  return VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX;
}

const char* VERTEX_SHADER[] =
{
// vertex shader for gradient units as USER_SPACE_ON_USE
DALI_COMPOSE_SHADER(
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
),

// vertex shader for gradient units as OBJECT_BOUNDING_BOX
 DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump mat3 uAlignmentMatrix;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vTexCoord = (uAlignmentMatrix*vertexPosition.xyw).xy;\n
    \n
    vertexPosition.xyz *= uSize;\n
    gl_Position = uMvpMatrix * vertexPosition;\n
  }\n
)
};

const char* FRAGMENT_SHADER[] =
{
// fragment shader for linear gradient
DALI_COMPOSE_SHADER(
  uniform sampler2D sTexture;\n // sampler1D?
  uniform lowp vec4 uColor;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vec2( vTexCoord.y, 0.5 ) ) * uColor;\n
  }\n
),

// fragment shader for radial gradient
DALI_COMPOSE_SHADER(
  uniform sampler2D sTexture;\n // sampler1D?
  uniform lowp vec4 uColor;\n
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vec2( length(vTexCoord), 0.5 ) ) * uColor;\n
  }\n
)
};

Dali::WrapMode::Type GetWrapMode( Gradient::SpreadMethod spread )
{
  switch(spread)
  {
    case Gradient::REPEAT:
    {
      return Dali::WrapMode::REPEAT;
    }
    case Gradient::REFLECT:
    {
      return Dali::WrapMode::MIRRORED_REPEAT;
    }
    case Gradient::PAD:
    default:
    {
      return Dali::WrapMode::CLAMP_TO_EDGE;
    }
  }
}

}


GradientVisual::GradientVisual( VisualFactoryCache& factoryCache )
: Visual( factoryCache ),
  mGradientType( LINEAR )
{
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

GradientVisual::~GradientVisual()
{
}

void GradientVisual::DoInitialize( Actor& actor, const Property::Map& propertyMap )
{
  Gradient::GradientUnits gradientUnits = Gradient::OBJECT_BOUNDING_BOX;
  Property::Value* unitsValue = propertyMap.Find( UNITS_NAME );
  std::string units;
  // The default unit is OBJECT_BOUNDING_BOX.
  // Only need to set new units if 'USER_SPACE'
  if( unitsValue && unitsValue->Get( units ) && units == UNIT_USER_SPACE )
  {
    gradientUnits = Gradient::USER_SPACE_ON_USE;
  }

  mGradientType = LINEAR;
  if( propertyMap.Find( RADIUS_NAME ))
  {
    mGradientType = RADIAL;
  }

  if( NewGradient( mGradientType, propertyMap ) )
  {
    mGradient->SetGradientUnits( gradientUnits );
    mGradientTransform = mGradient->GetAlignmentTransform();
  }
  else
  {
    DALI_LOG_ERROR( "Fail to provide valid properties to create a GradientVisual object" );
  }
}

void GradientVisual::SetSize( const Vector2& size )
{
  Visual::SetSize( size );
}

void GradientVisual::SetClipRect( const Rect<int>& clipRect )
{
  Visual::SetClipRect( clipRect );

  //ToDo: renderer responds to the clipRect change
}

void GradientVisual::SetOffset( const Vector2& offset )
{
  //ToDo: renderer applies the offset
}

void GradientVisual::DoSetOnStage( Actor& actor )
{
  InitializeRenderer();
}

void GradientVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( RENDERER_TYPE, GRADIENT_RENDERER );

  Gradient::GradientUnits units = mGradient->GetGradientUnits();
  if( units == Gradient::USER_SPACE_ON_USE )
  {
    map.Insert( UNITS_NAME, UNIT_USER_SPACE );
  }
  else // if( units == Gradient::OBJECT_BOUNDING_BOX )
  {
    map.Insert( UNITS_NAME, UNIT_BOUNDING_BOX );
  }

  Gradient::SpreadMethod spread = mGradient->GetSpreadMethod();
  if( spread == Gradient::PAD )
  {
    map.Insert( SPREAD_METHOD_NAME, SPREAD_PAD );
  }
  else if( spread == Gradient::REFLECT )
  {
    map.Insert( SPREAD_METHOD_NAME, SPREAD_REFLECT );
  }
  else // if( units == Gradient::REPEAT )
  {
    map.Insert( SPREAD_METHOD_NAME, SPREAD_REPEAT );
  }

  const Vector<Gradient::GradientStop>& stops( mGradient->GetStops() );
  Property::Array offsets;
  Property::Array colors;
  for( unsigned int i=0; i<stops.Count(); i++ )
  {
    offsets.PushBack( stops[i].mOffset );
    if( EqualsZero(stops[i].mStopColor.a) )
    {
      colors.PushBack( Vector4::ZERO );
    }
    else
    {
      colors.PushBack( Vector4( stops[i].mStopColor.r / stops[i].mStopColor.a,
                                stops[i].mStopColor.g / stops[i].mStopColor.a,
                                stops[i].mStopColor.b / stops[i].mStopColor.a,
                                stops[i].mStopColor.a));
    }
  }

  map.Insert( STOP_OFFSET_NAME, offsets );
  map.Insert( STOP_COLOR_NAME, colors );

  if( &typeid( *mGradient ) == &typeid(LinearGradient) )
  {
    LinearGradient* gradient = static_cast<LinearGradient*>( mGradient.Get() );
    map.Insert( START_POSITION_NAME, gradient->GetStartPosition() );
    map.Insert( END_POSITION_NAME, gradient->GetEndPosition() );
  }
  else // if( &typeid( *mGradient ) == &typeid(RadialGradient) )
  {
    RadialGradient* gradient = static_cast<RadialGradient*>( mGradient.Get() );
    map.Insert( CENTER_NAME, gradient->GetCenter() );
    map.Insert( RADIUS_NAME, gradient->GetRadius() );
  }
}

void GradientVisual::InitializeRenderer()
{
  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  if( !geometry )
  {
    geometry =  VisualFactoryCache::CreateQuadGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY, geometry );
  }

  Gradient::GradientUnits gradientUnits = mGradient->GetGradientUnits();
  VisualFactoryCache::ShaderType shaderType = GetShaderType( mGradientType, gradientUnits );
  Shader shader = mFactoryCache.GetShader( shaderType );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER[gradientUnits], FRAGMENT_SHADER[ mGradientType ] );
    mFactoryCache.SaveShader( shaderType, shader );
  }

  //Set up the texture set
  TextureSet textureSet = TextureSet::New();
  Dali::Texture lookupTexture = mGradient->GenerateLookupTexture();
  textureSet.SetTexture( 0u, lookupTexture );
  Dali::WrapMode::Type wrap = GetWrapMode( mGradient->GetSpreadMethod() );
  Sampler sampler = Sampler::New();
  sampler.SetWrapMode(  wrap, wrap  );
  textureSet.SetSampler( 0u, sampler );

  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetTextures( textureSet );

  mImpl->mRenderer.RegisterProperty( UNIFORM_ALIGNMENT_MATRIX_NAME, mGradientTransform );
}

bool GradientVisual::NewGradient(Type gradientType, const Property::Map& propertyMap)
{
  if( gradientType==LINEAR )
  {
    Property::Value* startPositionValue = propertyMap.Find( START_POSITION_NAME );
    Property::Value* endPositionValue = propertyMap.Find( END_POSITION_NAME );
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
    Property::Value* centerValue = propertyMap.Find( CENTER_NAME );
    Property::Value* radiusValue = propertyMap.Find( RADIUS_NAME );
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
  Property::Value* stopOffsetValue = propertyMap.Find( STOP_OFFSET_NAME );
  Property::Value* stopColorValue = propertyMap.Find( STOP_COLOR_NAME );
  if( stopColorValue )
  {
    Vector<float> offsetArray;
    Property::Array* colorArray = stopColorValue->GetArray();
    if( colorArray )
    {
      GetStopOffsets( stopOffsetValue, offsetArray );
      unsigned int numStop = offsetArray.Count() < colorArray->Count() ?
                             offsetArray.Count() : colorArray->Count();
      Vector4 color;
      for( unsigned int i=0; i<numStop; i++ )
      {
        if( (colorArray->GetElementAt(i)).Get(color) )
        {
          mGradient->AddStop( offsetArray[i], Vector4(color.r*color.a, color.g*color.a, color.b*color.a, color.a));
          numValidStop++;
        }
      }
    }
  }

  if( numValidStop < 1u ) // no valid stop
  {
    return false;
  }

  Property::Value* spread = propertyMap.Find( SPREAD_METHOD_NAME );
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

void GradientVisual::GetStopOffsets(const Property::Value* value, Vector<float>& stopOffsets)
{

  if ( value ) // Only check valve type if a valid Property has been passed in
  {
    switch ( value->GetType() )
    {
      case Property::VECTOR2:
      {
        Vector2 offset2;
        value->Get( offset2 );
        stopOffsets.PushBack( offset2.x );
        stopOffsets.PushBack( offset2.y );
        break;
      }
      case Property::VECTOR3:
      {
        Vector3 offset3;
        value->Get( offset3 );
        stopOffsets.PushBack( offset3.x );
        stopOffsets.PushBack( offset3.y );
        stopOffsets.PushBack( offset3.z );
        break;
      }
      case Property::VECTOR4:
      {
        Vector4 offset4;
        value->Get( offset4 );
        stopOffsets.PushBack( offset4.x );
        stopOffsets.PushBack( offset4.y );
        stopOffsets.PushBack( offset4.z );
        stopOffsets.PushBack( offset4.w );
        break;
      }
      case Property::ARRAY:
      {
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
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING("GetStopOffsets passed unsupported Property Map\n");
        // Unsupported Type
      }
    }
  }

  if ( stopOffsets.Empty() )
  {
    // Set default offset if none set by Property system, need a minimum and maximum
    stopOffsets.PushBack( DEFAULT_OFFSET_MINIMUM );
    stopOffsets.PushBack( DEFAULT_OFFSET_MAXIMUM );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
