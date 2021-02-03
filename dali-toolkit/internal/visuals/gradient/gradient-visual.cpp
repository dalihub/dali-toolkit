/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-array.h>
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/generated/gradient-visual-bounding-box-rounded-corner-shader-vert.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-bounding-box-shader-vert.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-linear-rounded-corner-shader-frag.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-linear-shader-frag.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-radial-rounded-corner-shader-frag.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-radial-shader-frag.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-user-space-rounded-corner-shader-vert.h>
#include <dali-toolkit/internal/graphics/generated/gradient-visual-user-space-shader-vert.h>
#include <dali-toolkit/internal/visuals/gradient/linear-gradient.h>
#include <dali-toolkit/internal/visuals/gradient/radial-gradient.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/gradient-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(UNITS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::GradientVisual::Units, OBJECT_BOUNDING_BOX)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::GradientVisual::Units, USER_SPACE)
DALI_ENUM_TO_STRING_TABLE_END(UNITS)

DALI_ENUM_TO_STRING_TABLE_BEGIN(SPREAD_METHOD)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::GradientVisual::SpreadMethod, PAD)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::GradientVisual::SpreadMethod, REFLECT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Toolkit::GradientVisual::SpreadMethod, REPEAT)
DALI_ENUM_TO_STRING_TABLE_END(SPREAD_METHOD)

// uniform names
const char* const UNIFORM_ALIGNMENT_MATRIX_NAME("uAlignmentMatrix");

// default offset value
const unsigned int DEFAULT_OFFSET_MINIMUM = 0.0f;
const unsigned int DEFAULT_OFFSET_MAXIMUM = 1.0f;

VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[][4] =
  {
    {VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE,
     VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX,
     VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_CORNER,
     VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_CORNER},
    {VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE,
     VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX,
     VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_CORNER,
     VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_CORNER}};

const std::string_view VERTEX_SHADER[] =
  {
    // vertex shader for gradient units as OBJECT_BOUNDING_BOX
    SHADER_GRADIENT_VISUAL_BOUNDING_BOX_SHADER_VERT,

    // vertex shader for gradient units as USER_SPACE
    SHADER_GRADIENT_VISUAL_USER_SPACE_SHADER_VERT,

    // vertex shader for gradient units as OBJECT_BOUNDING_BOX with corner radius
    SHADER_GRADIENT_VISUAL_BOUNDING_BOX_ROUNDED_CORNER_SHADER_VERT,

    // vertex shader for gradient units as USER_SPACE with corner radius
    SHADER_GRADIENT_VISUAL_USER_SPACE_ROUNDED_CORNER_SHADER_VERT};

const std::string_view FRAGMENT_SHADER[] =
  {
    // fragment shader for linear gradient
    SHADER_GRADIENT_VISUAL_LINEAR_SHADER_FRAG,

    // fragment shader for radial gradient
    SHADER_GRADIENT_VISUAL_RADIAL_SHADER_FRAG,

    // fragment shader for linear gradient with corner radius
    SHADER_GRADIENT_VISUAL_LINEAR_ROUNDED_CORNER_SHADER_FRAG,

    // fragment shader for radial gradient with corner radius
    SHADER_GRADIENT_VISUAL_RADIAL_ROUNDED_CORNER_SHADER_FRAG};

Dali::WrapMode::Type GetWrapMode(Toolkit::GradientVisual::SpreadMethod::Type spread)
{
  switch(spread)
  {
    case Toolkit::GradientVisual::SpreadMethod::REPEAT:
    {
      return Dali::WrapMode::REPEAT;
    }
    case Toolkit::GradientVisual::SpreadMethod::REFLECT:
    {
      return Dali::WrapMode::MIRRORED_REPEAT;
    }
    case Toolkit::GradientVisual::SpreadMethod::PAD:
    default:
    {
      return Dali::WrapMode::CLAMP_TO_EDGE;
    }
  }
}

} // unnamed namespace

GradientVisualPtr GradientVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  GradientVisualPtr gradientVisualPtr(new GradientVisual(factoryCache));
  gradientVisualPtr->SetProperties(properties);
  gradientVisualPtr->Initialize();
  return gradientVisualPtr;
}

GradientVisual::GradientVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::GRADIENT),
  mGradientType(LINEAR),
  mIsOpaque(true)
{
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

GradientVisual::~GradientVisual()
{
}

void GradientVisual::DoSetProperties(const Property::Map& propertyMap)
{
  Toolkit::GradientVisual::Units::Type gradientUnits = Toolkit::GradientVisual::Units::OBJECT_BOUNDING_BOX;

  Property::Value* unitsValue = propertyMap.Find(Toolkit::GradientVisual::Property::UNITS, UNITS_NAME);
  if(unitsValue)
  {
    Scripting::GetEnumerationProperty(*unitsValue, UNITS_TABLE, UNITS_TABLE_COUNT, gradientUnits);
  }

  mGradientType = LINEAR;
  if(propertyMap.Find(Toolkit::GradientVisual::Property::RADIUS, RADIUS_NAME))
  {
    mGradientType = RADIAL;
  }

  if(NewGradient(mGradientType, propertyMap))
  {
    mGradient->SetGradientUnits(gradientUnits);
    mGradientTransform = mGradient->GetAlignmentTransform();
  }
  else
  {
    DALI_LOG_ERROR("Fail to provide valid properties to create a GradientVisual object\n");
  }
}

void GradientVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void GradientVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  // Gradient Visual generated and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void GradientVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GetShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void GradientVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::GRADIENT);
  map.Insert(Toolkit::GradientVisual::Property::UNITS, mGradient->GetGradientUnits());
  map.Insert(Toolkit::GradientVisual::Property::SPREAD_METHOD, mGradient->GetSpreadMethod());

  const Vector<Gradient::GradientStop>& stops(mGradient->GetStops());
  Property::Array                       offsets;
  Property::Array                       colors;
  for(unsigned int i = 0; i < stops.Count(); i++)
  {
    offsets.PushBack(stops[i].mOffset);
    if(EqualsZero(stops[i].mStopColor.a))
    {
      colors.PushBack(Vector4::ZERO);
    }
    else
    {
      colors.PushBack(Vector4(stops[i].mStopColor.r / stops[i].mStopColor.a,
                              stops[i].mStopColor.g / stops[i].mStopColor.a,
                              stops[i].mStopColor.b / stops[i].mStopColor.a,
                              stops[i].mStopColor.a));
    }
  }

  map.Insert(Toolkit::GradientVisual::Property::STOP_OFFSET, offsets);
  map.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, colors);

  if(&typeid(*mGradient) == &typeid(LinearGradient))
  {
    LinearGradient* gradient = static_cast<LinearGradient*>(mGradient.Get());
    map.Insert(Toolkit::GradientVisual::Property::START_POSITION, gradient->GetStartPosition());
    map.Insert(Toolkit::GradientVisual::Property::END_POSITION, gradient->GetEndPosition());
  }
  else // if( &typeid( *mGradient ) == &typeid(RadialGradient) )
  {
    RadialGradient* gradient = static_cast<RadialGradient*>(mGradient.Get());
    map.Insert(Toolkit::GradientVisual::Property::CENTER, gradient->GetCenter());
    map.Insert(Toolkit::GradientVisual::Property::RADIUS, gradient->GetRadius());
  }
}

void GradientVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void GradientVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  Shader   shader   = GetShader();

  //Set up the texture set
  TextureSet    textureSet    = TextureSet::New();
  Dali::Texture lookupTexture = mGradient->GenerateLookupTexture();
  textureSet.SetTexture(0u, lookupTexture);
  Dali::WrapMode::Type wrap    = GetWrapMode(mGradient->GetSpreadMethod());
  Sampler              sampler = Sampler::New();
  sampler.SetWrapMode(wrap, wrap);
  textureSet.SetSampler(0u, sampler);

  mImpl->mRenderer = Renderer::New(geometry, shader);
  mImpl->mRenderer.SetTextures(textureSet);

  // If opaque and then no need to have blending
  if(mIsOpaque)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
  }

  mImpl->mRenderer.RegisterProperty(UNIFORM_ALIGNMENT_MATRIX_NAME, mGradientTransform);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

bool GradientVisual::NewGradient(Type gradientType, const Property::Map& propertyMap)
{
  if(gradientType == LINEAR)
  {
    Property::Value* startPositionValue = propertyMap.Find(Toolkit::GradientVisual::Property::START_POSITION, START_POSITION_NAME);
    Property::Value* endPositionValue   = propertyMap.Find(Toolkit::GradientVisual::Property::END_POSITION, END_POSITION_NAME);
    Vector2          startPosition;
    Vector2          endPosition;

    if(startPositionValue && startPositionValue->Get(startPosition) && endPositionValue && endPositionValue->Get(endPosition))
    {
      mGradient = new LinearGradient(startPosition, endPosition);
    }
    else
    {
      return false;
    }
  }
  else // type==RADIAL
  {
    Property::Value* centerValue = propertyMap.Find(Toolkit::GradientVisual::Property::CENTER, CENTER_NAME);
    Property::Value* radiusValue = propertyMap.Find(Toolkit::GradientVisual::Property::RADIUS, RADIUS_NAME);
    Vector2          center;
    float            radius;
    if(centerValue && centerValue->Get(center) && radiusValue && radiusValue->Get(radius))
    {
      mGradient = new RadialGradient(center, radius);
    }
    else
    {
      return false;
    }
  }

  unsigned int     numValidStop    = 0u;
  Property::Value* stopOffsetValue = propertyMap.Find(Toolkit::GradientVisual::Property::STOP_OFFSET, STOP_OFFSET_NAME);
  Property::Value* stopColorValue  = propertyMap.Find(Toolkit::GradientVisual::Property::STOP_COLOR, STOP_COLOR_NAME);
  if(stopColorValue)
  {
    Vector<float>    offsetArray;
    Property::Array* colorArray = stopColorValue->GetArray();
    if(colorArray)
    {
      GetStopOffsets(stopOffsetValue, offsetArray);
      unsigned int numStop = offsetArray.Count() < colorArray->Count() ? offsetArray.Count() : colorArray->Count();
      Vector4      color;
      for(unsigned int i = 0; i < numStop; i++)
      {
        if((colorArray->GetElementAt(i)).Get(color))
        {
          mGradient->AddStop(offsetArray[i], Vector4(color.r * color.a, color.g * color.a, color.b * color.a, color.a));
          numValidStop++;
          if(!Equals(color.a, 1.0f, Math::MACHINE_EPSILON_1))
          {
            mIsOpaque = false;
          }
        }
      }
    }
  }

  if(numValidStop < 1u) // no valid stop
  {
    return false;
  }

  Property::Value* spread = propertyMap.Find(Toolkit::GradientVisual::Property::SPREAD_METHOD, SPREAD_METHOD_NAME);
  // The default spread method is PAD. Only need to set new spread if it's anything else.
  if(spread)
  {
    Toolkit::GradientVisual::SpreadMethod::Type spreadMethod = Toolkit::GradientVisual::SpreadMethod::PAD;
    if(Scripting::GetEnumerationProperty(*spread, SPREAD_METHOD_TABLE, SPREAD_METHOD_TABLE_COUNT, spreadMethod))
    {
      mGradient->SetSpreadMethod(spreadMethod);
    }
  }

  return true;
}

Shader GradientVisual::GetShader()
{
  Toolkit::GradientVisual::Units::Type gradientUnits = mGradient->GetGradientUnits();
  int                                  roundedCorner = IsRoundedCornerRequired() ? 1 : 0;
  VisualFactoryCache::ShaderType       shaderType    = SHADER_TYPE_TABLE[mGradientType][gradientUnits + roundedCorner * 2];
  Shader                               shader        = mFactoryCache.GetShader(shaderType);
  if(!shader)
  {
    shader = Shader::New(VERTEX_SHADER[gradientUnits + roundedCorner * 2], FRAGMENT_SHADER[mGradientType + roundedCorner * 2]);
    mFactoryCache.SaveShader(shaderType, shader);
  }

  return shader;
}

void GradientVisual::GetStopOffsets(const Property::Value* value, Vector<float>& stopOffsets)
{
  if(value) // Only check valve type if a valid Property has been passed in
  {
    switch(value->GetType())
    {
      case Property::VECTOR2:
      {
        Vector2 offset2;
        value->Get(offset2);
        stopOffsets.PushBack(offset2.x);
        stopOffsets.PushBack(offset2.y);
        break;
      }
      case Property::VECTOR3:
      {
        Vector3 offset3;
        value->Get(offset3);
        stopOffsets.PushBack(offset3.x);
        stopOffsets.PushBack(offset3.y);
        stopOffsets.PushBack(offset3.z);
        break;
      }
      case Property::VECTOR4:
      {
        Vector4 offset4;
        value->Get(offset4);
        stopOffsets.PushBack(offset4.x);
        stopOffsets.PushBack(offset4.y);
        stopOffsets.PushBack(offset4.z);
        stopOffsets.PushBack(offset4.w);
        break;
      }
      case Property::ARRAY:
      {
        const Property::Array* offsetArray = value->GetArray();
        if(offsetArray)
        {
          unsigned int numStop = offsetArray->Count();
          float        offset;
          for(unsigned int i = 0; i < numStop; i++)
          {
            if(offsetArray->GetElementAt(i).Get(offset))
            {
              stopOffsets.PushBack(offset);
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

  if(stopOffsets.Empty())
  {
    // Set default offset if none set by Property system, need a minimum and maximum
    stopOffsets.PushBack(DEFAULT_OFFSET_MINIMUM);
    stopOffsets.PushBack(DEFAULT_OFFSET_MAXIMUM);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
