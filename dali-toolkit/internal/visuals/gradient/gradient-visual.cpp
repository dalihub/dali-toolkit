/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-array.h>
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/gradient/conic-gradient.h>
#include <dali-toolkit/internal/visuals/gradient/linear-gradient.h>
#include <dali-toolkit/internal/visuals/gradient/radial-gradient.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/gradient-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

#include "dali/integration-api/string-utils.h"

using Dali::Integration::ToDaliStringView;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr int32_t CUSTOM_PROPERTY_COUNT(3); // alignment, scalefactor, offset

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
static constexpr std::string_view UNIFORM_START_ANGLE_NAME("uStartAngle");
static constexpr std::string_view UNIFORM_START_OFFSET_NAME("uGradientOffset");

// default offset value
static constexpr float DEFAULT_OFFSET_MINIMUM = 0.0f;
static constexpr float DEFAULT_OFFSET_MAXIMUM = 1.0f;

VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[] = {
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_BOUNDING_BOX_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_LINEAR_USER_SPACE_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_BOUNDING_BOX_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_RADIAL_USER_SPACE_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_BOUNDING_BOX_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE_ROUNDED_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE_SQUIRCLE_CORNER,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE_ROUNDED_BORDERLINE,
  VisualFactoryCache::GRADIENT_SHADER_CONIC_USER_SPACE_SQUIRCLE_BORDERLINE,
};
#if defined(DEBUG_ENABLED)
constexpr uint32_t SHADER_TYPE_TABLE_COUNT = sizeof(SHADER_TYPE_TABLE) / sizeof(SHADER_TYPE_TABLE[0]);
#endif

// enum of required list when we select shader
enum GradientVisualRequireFlag
{
  DEFAULT         = 0,
  ROUNDED_CORNER  = 1,
  SQUIRCLE_CORNER = 2,

  BORDERLINE = (1 << 0) * 3,
  USER_SPACE = (1 << 1) * 3,
  RADIAL     = (1 << 2) * 3,
  CONIC      = (2 << 2) * 3
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
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::GRADIENT),
  mGradientTransform(),
  mGradient(nullptr),
  mGradientType(LINEAR),
  mStartOffsetIndex(Property::INVALID_INDEX)
{
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

GradientVisual::~GradientVisual()
{
}

void GradientVisual::DoSetProperties(const Property::Map& propertyMap)
{
  bool needShaderUpdated  = false;
  bool needTextureUpdated = false;
  // Mutable properties. Allow to call as UpdateProperty action
  {
    // TODO : Currently we cannot change gradient type as mutable.
    // Can we change it as mutable in future?
    if(!mGradient)
    {
      mGradientType = Type::LINEAR;
      if(propertyMap.Find(Toolkit::GradientVisual::Property::RADIUS, RADIUS_NAME))
      {
        mGradientType = Type::RADIAL;
      }
      else if(propertyMap.Find(Toolkit::GradientVisual::Property::START_ANGLE, CONIC_START_ANGLE_NAME))
      {
        mGradientType = Type::CONIC;
      }

      if(NewGradient(mGradientType, propertyMap))
      {
        mGradientTransform = mGradient->GetAlignmentTransform();

        if(mImpl->mRenderer)
        {
          mImpl->mRenderer.RegisterProperty(ToDaliStringView(UNIFORM_ALIGNMENT_MATRIX_NAME), mGradientTransform);
          if(DALI_LIKELY(mGradient) && mGradientType == Type::CONIC)
          {
            ConicGradient* gradient = static_cast<ConicGradient*>(mGradient.Get());
            mImpl->mRenderer.RegisterProperty(ToDaliStringView(UNIFORM_START_ANGLE_NAME), gradient->GetStartAngle().radian);
          }
        }
        needShaderUpdated  = true;
        needTextureUpdated = true;
      }
    }

    if(ApplyStopNodes(propertyMap))
    {
      needTextureUpdated = true;
    }

    Property::Value* startOffsetValue = propertyMap.Find(Toolkit::GradientVisual::Property::START_OFFSET, START_OFFSET_NAME);
    float            startOffset;
    if(startOffsetValue && startOffsetValue->Get(startOffset))
    {
      if(DALI_LIKELY(mGradient))
      {
        mGradient->SetStartOffset(startOffset);
      }
      if(mImpl->mRenderer && mStartOffsetIndex != Property::INVALID_INDEX)
      {
        mImpl->mRenderer.SetProperty(mStartOffsetIndex, startOffset);
      }
    }

    if(DALI_LIKELY(mGradient))
    {
      Property::Value*                            spreadValue  = propertyMap.Find(Toolkit::GradientVisual::Property::SPREAD_METHOD, SPREAD_METHOD_NAME);
      Toolkit::GradientVisual::SpreadMethod::Type spreadMethod = mGradient->GetSpreadMethod();
      if(spreadValue && Scripting::GetEnumerationProperty(*spreadValue, SPREAD_METHOD_TABLE, SPREAD_METHOD_TABLE_COUNT, spreadMethod))
      {
        mGradient->SetSpreadMethod(spreadMethod);

        needTextureUpdated = true;
      }
      Property::Value*                     unitsValue    = propertyMap.Find(Toolkit::GradientVisual::Property::UNITS, UNITS_NAME);
      Toolkit::GradientVisual::Units::Type gradientUnits = mGradient->GetGradientUnits();
      if(unitsValue && Scripting::GetEnumerationProperty(*unitsValue, UNITS_TABLE, UNITS_TABLE_COUNT, gradientUnits))
      {
        mGradient->SetGradientUnits(gradientUnits);

        needShaderUpdated = true;
      }
    }
  }

  if(DALI_LIKELY(mGradient) && mImpl->mRenderer)
  {
    if(needShaderUpdated)
    {
      UpdateShader();
    }
    if(needTextureUpdated)
    {
      // Set up the texture set
      TextureSet    textureSet    = TextureSet::New();
      Dali::Texture lookupTexture = mGradient->GenerateLookupTexture();
      textureSet.SetTexture(0u, lookupTexture);
      Dali::WrapMode::Type wrap = GetWrapMode(mGradient->GetSpreadMethod());
      if(wrap != Dali::WrapMode::DEFAULT)
      {
        Sampler sampler = Sampler::New();
        sampler.SetWrapMode(wrap, wrap);
        textureSet.SetSampler(0u, sampler);
      }

      mImpl->mRenderer.SetTextures(textureSet);

      float textureSize = static_cast<float>(lookupTexture.GetWidth());
      mImpl->mRenderer.RegisterProperty(ToDaliStringView(UNIFORM_TEXTURE_COORDINATE_SCALE_FACTOR_NAME),
                                        (textureSize - 1.0f) / textureSize);
    }
  }
}

void GradientVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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

  if(DALI_LIKELY(mGradient))
  {
    if(mImpl->mRenderer && mStartOffsetIndex != Property::INVALID_INDEX)
    {
      mGradient->SetStartOffset(mImpl->mRenderer.GetProperty<float>(mStartOffsetIndex));
    }
    map.Insert(Toolkit::GradientVisual::Property::START_OFFSET, mGradient->GetStartOffset());
    map.Insert(Toolkit::GradientVisual::Property::UNITS, mGradient->GetGradientUnits());
    map.Insert(Toolkit::GradientVisual::Property::SPREAD_METHOD, mGradient->GetSpreadMethod());

    const Vector<Gradient::GradientStop>& stops(mGradient->GetStops());
    Property::Array                       offsets;
    Property::Array                       colors;
    offsets.Reserve(stops.Count());
    colors.Reserve(stops.Count());
    for(unsigned int i = 0; i < stops.Count(); i++)
    {
      offsets.PushBack(stops[i].mOffset);
      colors.PushBack(stops[i].mStopColor);
    }

    map.Insert(Toolkit::GradientVisual::Property::STOP_OFFSET, offsets);
    map.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, colors);

    switch(mGradientType)
    {
      case Type::LINEAR:
      {
        LinearGradient* gradient = static_cast<LinearGradient*>(mGradient.Get());
        map.Insert(Toolkit::GradientVisual::Property::START_POSITION, gradient->GetStartPosition());
        map.Insert(Toolkit::GradientVisual::Property::END_POSITION, gradient->GetEndPosition());
        break;
      }
      case Type::RADIAL:
      {
        RadialGradient* gradient = static_cast<RadialGradient*>(mGradient.Get());
        map.Insert(Toolkit::GradientVisual::Property::CENTER, gradient->GetCenter());
        map.Insert(Toolkit::GradientVisual::Property::RADIUS, gradient->GetRadius());
        break;
      }
      case Type::CONIC:
      {
        ConicGradient* gradient = static_cast<ConicGradient*>(mGradient.Get());
        map.Insert(Toolkit::GradientVisual::Property::CENTER, gradient->GetCenter());
        map.Insert(Toolkit::GradientVisual::Property::START_ANGLE, gradient->GetStartAngle().radian);
        break;
      }
    }
  }
  else
  {
    if(mImpl->mRenderer && mStartOffsetIndex != Property::INVALID_INDEX)
    {
      map.Insert(Toolkit::GradientVisual::Property::START_OFFSET, mImpl->mRenderer.GetProperty<float>(mStartOffsetIndex));
    }
    else
    {
      map.Insert(Toolkit::GradientVisual::Property::START_OFFSET, 0.0f);
    }
    map.Insert(Toolkit::GradientVisual::Property::UNITS, Toolkit::GradientVisual::Units::OBJECT_BOUNDING_BOX);
    map.Insert(Toolkit::GradientVisual::Property::SPREAD_METHOD, Toolkit::GradientVisual::SpreadMethod::PAD);
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

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT + (mGradientType == Type::CONIC ? 1 : 0));

  // Set up the texture set
  if(DALI_LIKELY(mGradient))
  {
    TextureSet    textureSet    = TextureSet::New();
    Dali::Texture lookupTexture = mGradient->GenerateLookupTexture();
    textureSet.SetTexture(0u, lookupTexture);
    Dali::WrapMode::Type wrap = GetWrapMode(mGradient->GetSpreadMethod());
    if(wrap != Dali::WrapMode::DEFAULT)
    {
      Sampler sampler = Sampler::New();
      sampler.SetWrapMode(wrap, wrap);
      textureSet.SetSampler(0u, sampler);
    }
    mImpl->mRenderer.SetTextures(textureSet);

    float textureSize = static_cast<float>(lookupTexture.GetWidth());
    mImpl->mRenderer.RegisterUniqueProperty(ToDaliStringView(UNIFORM_TEXTURE_COORDINATE_SCALE_FACTOR_NAME), (textureSize - 1.0f) / textureSize);
  }

  mImpl->mRenderer.RegisterUniqueProperty(ToDaliStringView(UNIFORM_ALIGNMENT_MATRIX_NAME), mGradientTransform);
  if(DALI_LIKELY(mGradient) && mGradientType == Type::CONIC)
  {
    ConicGradient* gradient = static_cast<ConicGradient*>(mGradient.Get());
    mImpl->mRenderer.RegisterUniqueProperty(ToDaliStringView(UNIFORM_START_ANGLE_NAME), gradient->GetStartAngle().radian);
  }

  float startOffset = mGradient->GetStartOffset();
  mStartOffsetIndex = mImpl->mRenderer.RegisterUniqueProperty(Toolkit::GradientVisual::Property::START_OFFSET, ToDaliStringView(UNIFORM_START_OFFSET_NAME), startOffset);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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
  else if(gradientType == Type::RADIAL)
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
  else // if(gradientType == Type::CONIC)
  {
    Property::Value* centerValue     = propertyMap.Find(Toolkit::GradientVisual::Property::CENTER, CENTER_NAME);
    Property::Value* startAngleValue = propertyMap.Find(Toolkit::GradientVisual::Property::START_ANGLE, CONIC_START_ANGLE_NAME);
    Vector2          center;
    float            startAngle;
    if(centerValue && centerValue->Get(center) && startAngleValue && startAngleValue->Get(startAngle))
    {
      mGradient = new ConicGradient(center, Dali::Radian(startAngle));
    }
    else
    {
      return false;
    }
  }

  return true;
}

bool GradientVisual::ApplyStopNodes(const Property::Map& propertyMap)
{
  if(DALI_UNLIKELY(!mGradient))
  {
    return false;
  }

  Property::Value* stopOffsetValue = propertyMap.Find(Toolkit::GradientVisual::Property::STOP_OFFSET, STOP_OFFSET_NAME);
  Property::Value* stopColorValue  = propertyMap.Find(Toolkit::GradientVisual::Property::STOP_COLOR, STOP_COLOR_NAME);

  if(stopOffsetValue == nullptr && stopColorValue == nullptr)
  {
    return false;
  }

  if(stopOffsetValue != nullptr && stopColorValue != nullptr)
  {
    mGradient->ClearStops();
  }

  Vector<float>   offsetArray;
  Vector<Vector4> colorArray;
  GetStopOffsets(stopOffsetValue, offsetArray);
  GetStopColors(stopColorValue, colorArray);

  mGradient->ApplyStops(offsetArray, colorArray);

  return true;
}

Shader GradientVisual::GenerateShader() const
{
  bool userspaceUnit  = (mGradient ? mGradient->GetGradientUnits() == Toolkit::GradientVisual::Units::USER_SPACE : false);
  bool roundedCorner  = IsRoundedCornerRequired();
  bool squircleCorner = IsSquircleCornerRequired();
  bool borderline     = IsBorderlineRequired();
  bool radialGradient = (mGradientType == Type::RADIAL);
  bool conicGradient  = (mGradientType == Type::CONIC);

  uint32_t shaderTypeFlag = GradientVisualRequireFlag::DEFAULT;
  if(squircleCorner)
  {
    shaderTypeFlag += GradientVisualRequireFlag::SQUIRCLE_CORNER;
  }
  else if(roundedCorner)
  {
    shaderTypeFlag += GradientVisualRequireFlag::ROUNDED_CORNER;
  }

  if(borderline)
  {
    shaderTypeFlag += GradientVisualRequireFlag::BORDERLINE;
  }
  if(userspaceUnit)
  {
    shaderTypeFlag += GradientVisualRequireFlag::USER_SPACE;
  }

  if(radialGradient)
  {
    shaderTypeFlag += GradientVisualRequireFlag::RADIAL;
  }
  else if(conicGradient)
  {
    shaderTypeFlag += GradientVisualRequireFlag::CONIC;
  }

  DALI_ASSERT_DEBUG(shaderTypeFlag < SHADER_TYPE_TABLE_COUNT && "Invalid gradient shader type generated!");

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
      if(squircleCorner)
      {
        fragmentShaderPrefixList += "#define IS_REQUIRED_SQUIRCLE_CORNER\n";
      }
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
    else if(conicGradient)
    {
      fragmentShaderPrefixList += "#define CONIC\n";
    }

    if(userspaceUnit)
    {
      vertexShaderPrefixList += "#define USER_SPACE\n";
    }

    shader = mFactoryCache.GenerateAndSaveShader(shaderType,
                                                 vertexShaderPrefixList + SHADER_GRADIENT_VISUAL_SHADER_VERT.data(),
                                                 fragmentShaderPrefixList + SHADER_GRADIENT_VISUAL_SHADER_FRAG.data());
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
}

void GradientVisual::GetStopColors(const Property::Value* value, Vector<Vector4>& stopColors)
{
  if(value) // Only check valve type if a valid Property has been passed in
  {
    switch(value->GetType())
    {
      case Property::ARRAY:
      {
        const Property::Array* colorArray = value->GetArray();
        if(colorArray)
        {
          uint32_t numStop = colorArray->Count();
          Vector4  color;
          for(uint32_t i = 0; i < numStop; i++)
          {
            if(colorArray->GetElementAt(i).Get(color))
            {
              stopColors.PushBack(color);
            }
          }
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING("GetStopColors passed unsupported Property Map\n");
        // Unsupported Type
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
