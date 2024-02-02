/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/decorated-visual-renderer.h>
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
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
static constexpr int32_t CUSTOM_PROPERTY_COUNT(1); // alignment

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
static constexpr std::string_view UNIFORM_ALIGNMENT_MATRIX_NAME("uAlignmentMatrix");
static constexpr std::string_view UNIFORM_TEXTURE_COORDINATE_SCALE_FACTOR_NAME("uTextureCoordinateScaleFactor");

// default offset value
static constexpr float DEFAULT_OFFSET_MINIMUM = 0.0f;
static constexpr float DEFAULT_OFFSET_MAXIMUM = 1.0f;

VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[16] =
  {
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_CORNER,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_CORNER,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_CORNER,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_CORNER,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_BORDERLINE,
    VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_BORDERLINE,
};

// enum of required list when we select shader
enum GradientVisualRequireFlag
{
  DEFAULT        = 0,
  ROUNDED_CORNER = 1 << 0,
  BORDERLINE     = 1 << 1,
  USER_SPACE     = 1 << 2,
  RADIAL         = 1 << 3,
};

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

  mGradientType = Type::LINEAR;
  if(propertyMap.Find(Toolkit::GradientVisual::Property::RADIUS, RADIUS_NAME))
  {
    mGradientType = Type::RADIAL;
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
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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
    Shader shader = GenerateShader();
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

void GradientVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  // Make always enable pre multiplied alpha whether preMultiplied value is false.
  if(!preMultiplied)
  {
    DALI_LOG_WARNING("Note : GradientVisual cannot disable PreMultipliedAlpha\n");
  }
}

void GradientVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  Shader   shader   = GenerateShader();

  // Set up the texture set
  TextureSet    textureSet    = TextureSet::New();
  Dali::Texture lookupTexture = mGradient->GenerateLookupTexture();
  textureSet.SetTexture(0u, lookupTexture);
  Dali::WrapMode::Type wrap    = GetWrapMode(mGradient->GetSpreadMethod());
  Sampler              sampler = Sampler::New();
  sampler.SetWrapMode(wrap, wrap);
  textureSet.SetSampler(0u, sampler);

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);
  mImpl->mRenderer.SetTextures(textureSet);

  // If opaque and then no need to have blending
  if(mIsOpaque)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::OFF);
  }

  mImpl->mRenderer.RegisterUniqueProperty(UNIFORM_ALIGNMENT_MATRIX_NAME, mGradientTransform);

  float textureSize = static_cast<float>(lookupTexture.GetWidth());
  mImpl->mRenderer.RegisterUniqueProperty(UNIFORM_TEXTURE_COORDINATE_SCALE_FACTOR_NAME, (textureSize - 1.0f) / textureSize);

  // Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

bool GradientVisual::NewGradient(Type gradientType, const Property::Map& propertyMap)
{
  if(gradientType == Type::LINEAR)
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
  else // type==Type::RADIAL
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

Shader GradientVisual::GenerateShader() const
{
  bool userspaceUnit  = (mGradient->GetGradientUnits() == Toolkit::GradientVisual::Units::USER_SPACE);
  bool roundedCorner  = IsRoundedCornerRequired();
  bool borderline     = IsBorderlineRequired();
  bool radialGradient = (mGradientType == Type::RADIAL);

  int shaderTypeFlag = GradientVisualRequireFlag::DEFAULT;
  if(roundedCorner)
  {
    shaderTypeFlag |= GradientVisualRequireFlag::ROUNDED_CORNER;
  }
  if(borderline)
  {
    shaderTypeFlag |= GradientVisualRequireFlag::BORDERLINE;
  }
  if(userspaceUnit)
  {
    shaderTypeFlag |= GradientVisualRequireFlag::USER_SPACE;
  }
  if(radialGradient)
  {
    shaderTypeFlag |= GradientVisualRequireFlag::RADIAL;
  }

  VisualFactoryCache::ShaderType shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  Shader                         shader     = mFactoryCache.GetShader(shaderType);
  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;

    if(roundedCorner)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    }
    if(borderline)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }
    if(radialGradient)
    {
      fragmentShaderPrefixList += "#define RADIAL\n";
    }
    if(userspaceUnit)
    {
      vertexShaderPrefixList += "#define USER_SPACE\n";
    }

    shader = mFactoryCache.GenerateAndSaveShader(shaderType,
                                                 Dali::Shader::GetVertexShaderPrefix() + vertexShaderPrefixList + SHADER_GRADIENT_VISUAL_SHADER_VERT.data(),
                                                 Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_GRADIENT_VISUAL_SHADER_FRAG.data());
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
