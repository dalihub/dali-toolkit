/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include "visual-base-impl.h"

// EXTERNAL HEADER
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>
#include <dali/public-api/rendering/visual-renderer.h>

// INTERNAL HEARDER
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/helpers/property-helper.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/primitive-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVisualBaseLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VISUAL_BASE");
#endif

// visual string constants contains OFFSET_SIZE_MODE instead
const char* const OFFSET_POLICY("offsetPolicy");
const char* const SIZE_POLICY("sizePolicy");

} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(VISUAL_FITTING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, FIT_KEEP_ASPECT_RATIO)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, FILL)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, OVER_FIT_KEEP_ASPECT_RATIO)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, FIT_HEIGHT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, FIT_WIDTH)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Visual::FittingMode, DONT_CARE)
DALI_ENUM_TO_STRING_TABLE_END(VISUAL_FITTING_MODE)

/**
 * @brief Check whether this visual type can use corner radius feature or not.
 * @param type VisualType that want to checkup
 * @return true if type can use corner radius feature.
 */
static bool IsTypeAvailableForCornerRadius(Toolkit::Visual::Type type)
{
  switch(static_cast<Toolkit::DevelVisual::Type>(type))
  {
    case Toolkit::Visual::Type::COLOR:
    case Toolkit::Visual::Type::GRADIENT:
    case Toolkit::Visual::Type::IMAGE:
    case Toolkit::Visual::Type::SVG:
    case Toolkit::Visual::Type::ANIMATED_IMAGE:
    case Toolkit::DevelVisual::Type::ANIMATED_VECTOR_IMAGE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

/**
 * @brief Check whether this visual type can use borderline feature or not.
 * @param type VisualType that want to checkup
 * @return true if type can use borderline feature.
 */
static bool IsTypeAvailableForBorderline(Toolkit::Visual::Type type)
{
  switch(static_cast<Toolkit::DevelVisual::Type>(type))
  {
    case Toolkit::Visual::Type::COLOR:
    case Toolkit::Visual::Type::GRADIENT:
    case Toolkit::Visual::Type::IMAGE:
    case Toolkit::Visual::Type::SVG:
    case Toolkit::Visual::Type::ANIMATED_IMAGE:
    case Toolkit::DevelVisual::Type::ANIMATED_VECTOR_IMAGE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

struct StringProperty
{
  const char* const name;
  Property::Index   index;
};
StringProperty PROPERTY_NAME_INDEX_TABLE[] =
  {
    {CUSTOM_SHADER, Toolkit::Visual::Property::SHADER},
    {TRANSFORM, Toolkit::Visual::Property::TRANSFORM},
    {PREMULTIPLIED_ALPHA, Toolkit::Visual::Property::PREMULTIPLIED_ALPHA},
    {MIX_COLOR, Toolkit::Visual::Property::MIX_COLOR},
    {OPACITY, Toolkit::Visual::Property::OPACITY},
    {VISUAL_FITTING_MODE, Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE},
    {BORDERLINE_WIDTH, Toolkit::DevelVisual::Property::BORDERLINE_WIDTH},
    {BORDERLINE_COLOR, Toolkit::DevelVisual::Property::BORDERLINE_COLOR},
    {BORDERLINE_OFFSET, Toolkit::DevelVisual::Property::BORDERLINE_OFFSET},
    {CORNER_RADIUS, Toolkit::DevelVisual::Property::CORNER_RADIUS},
    {CORNER_RADIUS_POLICY, Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY},
    {CORNER_SQUARENESS, Toolkit::DevelVisual::Property::CORNER_SQUARENESS},
};
const uint16_t PROPERTY_NAME_INDEX_TABLE_COUNT = sizeof(PROPERTY_NAME_INDEX_TABLE) / sizeof(PROPERTY_NAME_INDEX_TABLE[0]);

Property::Index GetVisualPropertyIndex(Property::Key key)
{
  if(key.type == Property::Key::STRING)
  {
    for(auto tableId = 0u; tableId < PROPERTY_NAME_INDEX_TABLE_COUNT; ++tableId)
    {
      if(key == PROPERTY_NAME_INDEX_TABLE[tableId].name)
      {
        return PROPERTY_NAME_INDEX_TABLE[tableId].index;
        break;
      }
    }
  }
  return key.indexKey;
}

} // namespace

Visual::Base::Base(VisualFactoryCache& factoryCache, FittingMode fittingMode, Toolkit::Visual::Type type)
: mImpl(new Impl(fittingMode, type)),
  mFactoryCache(factoryCache)
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Visual::Base[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }
}

Visual::Base::~Base()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Visual::~Base[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }
  delete mImpl;
}

void Visual::Base::Initialize()
{
  // The Renderer should be created inside derived class here.
  OnInitialize();

  if(mImpl->mRenderer)
  {
    RegisterMixColor();
    RegisterDecoration();

    if(IsBorderlineRequired())
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);
    }
    else if(IsRoundedCornerRequired())
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
    }
  }
}

void Visual::Base::SetCustomShader(const Property::Map& shaderMap)
{
  if(IsUsingCustomShader())
  {
    mImpl->ClearCustomShader();
  }
  mImpl->AddCustomShader(shaderMap);

  // Let derived class know
  UpdateShader();
}

void Visual::Base::SetCustomShader(const Property::Array& shaderArray)
{
  if(IsUsingCustomShader())
  {
    mImpl->ClearCustomShader();
  }

  uint32_t arraySize = shaderArray.Count();
  for(uint32_t i = 0; i < arraySize; ++i)
  {
    const Dali::Property::Map* shaderMap = shaderArray.GetElementAt(i).GetMap();
    if(shaderMap)
    {
      mImpl->AddCustomShader(*shaderMap);
    }
  }

  // Let derived class know
  UpdateShader();
}

bool Visual::Base::IsUsingCustomShader() const
{
  return !mImpl->mCustomShaders.empty();
}

void Visual::Base::SetProperties(const Property::Map& propertyMap)
{
  bool needUpdateShader = false;
  for(size_t i = 0; i < propertyMap.Count(); ++i)
  {
    const KeyValuePair&    pair  = propertyMap.GetKeyValue(i);
    const Property::Key&   key   = pair.first;
    const Property::Value& value = pair.second;

    switch(GetVisualPropertyIndex(key))
    {
      case Toolkit::Visual::Property::SHADER:
      {
        if(value.GetType() == Property::MAP)
        {
          const Dali::Property::Map* map = value.GetMap();
          if(map)
          {
            SetCustomShader(*map);
          }
        }
        else if(value.GetType() == Property::ARRAY)
        {
          const Dali::Property::Array* array = value.GetArray();
          if(array)
          {
            SetCustomShader(*array);
          }
        }
        break;
      }

      case Toolkit::Visual::Property::TRANSFORM:
      {
        Property::Map map;
        if(value.Get(map))
        {
          if(!map.Empty() &&
             (!mImpl->mTransformMapUsingDefault ||
              map.GetHash() != Impl::Transform::GetDefaultTransformMap().GetHash() ||
              DALI_UNLIKELY(map != Impl::Transform::GetDefaultTransformMap())))
          {
            if(DALI_UNLIKELY(mImpl->mRenderer))
            {
              // Unusual case. SetProperty called after OnInitialize().
              // Assume that DoAction call UPDATE_PROPERTY.
              mImpl->mTransformMapChanged = true;
              if(mImpl->mTransformMapUsingDefault)
              {
                mImpl->mTransformMapUsingDefault = false;
                mImpl->mRenderer.RegisterVisualTransformUniform();
              }

              if(!mImpl->mTransformMapUsingDefault)
              {
                mImpl->GetOrCreateTransform().UpdatePropertyMap(map);

                // Set Renderer uniforms, and change logics for subclasses.
                OnSetTransform();
              }

              // Reset flag here.
              mImpl->mTransformMapChanged = false;
            }
            else
            {
              mImpl->mTransformMapUsingDefault = false;
              mImpl->GetOrCreateTransform().SetPropertyMap(map);
            }
          }
        }
        break;
      }

      case Toolkit::Visual::Property::PREMULTIPLIED_ALPHA:
      {
        bool premultipliedAlpha = false;
        if(value.Get(premultipliedAlpha))
        {
          EnablePreMultipliedAlpha(premultipliedAlpha);
        }
        break;
      }

      case Toolkit::Visual::Property::MIX_COLOR:
      {
        Vector4 mixColor;
        if(value.Get(mixColor))
        {
          if(value.GetType() == Property::VECTOR4)
          {
            SetMixColor(mixColor);
          }
          else
          {
            Vector3 mixColor3(mixColor);
            SetMixColor(mixColor3);
          }
        }
        break;
      }
      case Toolkit::Visual::Property::OPACITY:
      {
        float opacity;
        if(value.Get(opacity))
        {
          mImpl->mMixColor.a = opacity;
          SetMixColor(mImpl->mMixColor);
        }
        break;
      }
      case Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE:
      {
        Scripting::GetEnumerationProperty<Visual::FittingMode>(
          value, VISUAL_FITTING_MODE_TABLE, VISUAL_FITTING_MODE_TABLE_COUNT, mImpl->mFittingMode);
        break;
      }
      case Toolkit::DevelVisual::Property::BORDERLINE_WIDTH:
      {
        float width;
        if(value.Get(width) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(width)))
        {
          mImpl->SetBorderlineWidth(width);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBorderlineUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingBorderline && IsBorderlineRequired())
          {
            // Required to change shader mean, we didn't setup BORDERLINE_COLOR and BORDERLINE_OFFSET into mRenderer before. Set property now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());

            // Make Blend mode ON_WITHOUT_CULL for transparent mix color.
            mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);

            // Change the shader must not be occured many times. we always have to use borderline feature.
            mImpl->mAlwaysUsingBorderline = true;

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }
        break;
      }
      case Toolkit::DevelVisual::Property::BORDERLINE_COLOR:
      {
        Vector4 color;
        if(value.Get(color) && (mImpl->mDecorationData != nullptr || color != Vector4::ZERO))
        {
          mImpl->SetBorderlineColor(color);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
        }
        break;
      }
      case Toolkit::DevelVisual::Property::BORDERLINE_OFFSET:
      {
        float offset;
        if(value.Get(offset) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(offset)))
        {
          mImpl->SetBorderlineOffset(offset);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());
        }
        break;
      }
      case Toolkit::DevelVisual::Property::CORNER_RADIUS:
      {
        if(value.GetType() == Property::VECTOR4)
        {
          // If CORNER_RADIUS Property is Vector4,
          // Each values mean the radius of
          // (top-left, top-right, bottom-right, bottom-left)
          Vector4 radius;
          if(value.Get(radius) && (mImpl->mDecorationData != nullptr || radius != Vector4::ZERO))
          {
            mImpl->SetCornerRadius(radius);
          }
        }
        else
        {
          // If CORNER_RADIUS Property is float,
          // Every corner radius have same value
          float radius;
          if(value.Get(radius) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(radius)))
          {
            mImpl->SetCornerRadius(Vector4(radius, radius, radius, radius));
          }
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForCornerRadius(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerRadiusUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingCornerRadius && IsRoundedCornerRequired())
          {
            // Required to change shader mean, we didn't setup CORNER_RADIUS_POLICY into mRenderer before. Set property now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));

            // Change the shader must not be occured many times. we always have to use corner radius feature.
            mImpl->mAlwaysUsingCornerRadius = true;

            if(!IsBorderlineRequired())
            {
              // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
              mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
            }

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }

        break;
      }
      case Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          switch(policy)
          {
            case Toolkit::Visual::Transform::Policy::RELATIVE:
            case Toolkit::Visual::Transform::Policy::ABSOLUTE:
            {
              mImpl->SetCornerRadiusPolicy(policy);
              if(DALI_UNLIKELY(mImpl->mRenderer))
              {
                // Unusual case. SetProperty called after OnInitialize().
                // Assume that DoAction call UPDATE_PROPERTY.
                mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));
              }
              break;
            }
            default:
            {
              DALI_LOG_ERROR("Unsupported policy: %d\n", policy);
              break;
            }
          }
        }
        break;
      }
      case Toolkit::DevelVisual::Property::CORNER_SQUARENESS:
      {
        if(value.GetType() == Property::VECTOR4)
        {
          // If CORNER_SQUARENESS Property is Vector4,
          // Each values mean the squareness of
          // (top-left, top-right, bottom-right, bottom-left)
          Vector4 squareness;
          if(value.Get(squareness) && (mImpl->mDecorationData != nullptr || squareness != Vector4::ZERO))
          {
            mImpl->SetCornerSquareness(squareness);
          }
        }
        else
        {
          // If CORNER_RADIUS Property is float,
          // Every corner radius have same value
          float squareness;
          if(value.Get(squareness) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(squareness)))
          {
            mImpl->SetCornerSquareness(Vector4(squareness, squareness, squareness, squareness));
          }
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForCornerRadius(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerSquarenessUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingCornerSquareness && IsSquircleCornerRequired())
          {
            // Required to change shader mean, we didn't setup CORNER_RADIUS_POLICY into mRenderer before. Set property now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, static_cast<Vector4>(mImpl->GetCornerRadius()));
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));

            // Change the shader must not be occured many times. we always have to use corner squreness feature.
            mImpl->mAlwaysUsingCornerSquareness = true;

            if(!IsBorderlineRequired())
            {
              // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
              mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
            }

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }

        break;
      }
    }
  }

  DoSetProperties(propertyMap);

  if(DALI_UNLIKELY(needUpdateShader))
  {
    UpdateShader();
  }
}

void Visual::Base::SetTransformAndSize(const Property::Map& transform, Size controlSize)
{
  mImpl->mControlSize = controlSize;
  mImpl->mTransformMapChanged |= !transform.Empty();
  if(mImpl->mTransformMapChanged && mImpl->mTransformMapUsingDefault)
  {
    mImpl->mTransformMapUsingDefault = false;
    mImpl->mRenderer.RegisterVisualTransformUniform();
  }
  if(!mImpl->mTransformMapUsingDefault)
  {
    mImpl->GetOrCreateTransform().UpdatePropertyMap(transform);
  }

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << transform;
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "Visual::Base::SetTransformAndSize(%s) - [\e[1;32mtransform: %s  controlSize: (%3.1f, %3.1f)]\e[0m\n", GetName().c_str(), oss.str().c_str(), controlSize.x, controlSize.y);
#endif

  OnSetTransform();

  // Reset flag here.
  mImpl->mTransformMapChanged = false;
}

void Visual::Base::SetName(const std::string& name)
{
  mImpl->mName = name;
}

const std::string& Visual::Base::GetName() const
{
  return mImpl->mName;
}

float Visual::Base::GetHeightForWidth(float width)
{
  float   aspectCorrectedHeight = 0.f;
  Vector2 naturalSize;
  GetNaturalSize(naturalSize);
  if(naturalSize.width > 0.0f)
  {
    aspectCorrectedHeight = naturalSize.height * width / naturalSize.width;
  }
  return aspectCorrectedHeight;
}

float Visual::Base::GetWidthForHeight(float height)
{
  float   aspectCorrectedWidth = 0.f;
  Vector2 naturalSize;
  GetNaturalSize(naturalSize);
  if(naturalSize.height > 0.0f)
  {
    aspectCorrectedWidth = naturalSize.width * height / naturalSize.height;
  }
  return aspectCorrectedWidth;
}

void Visual::Base::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = Vector2::ZERO;
}

void Visual::Base::DoAction(const Property::Index actionId, const Property::Value& attributes)
{
  OnDoAction(actionId, attributes);

  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case DevelVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if(map)
      {
        SetProperties(*map);
      }
      break;
    }
  }
}

void Visual::Base::DoActionExtension(const Dali::Property::Index actionId, const Dali::Any& attributes)
{
  OnDoActionExtension(actionId, attributes);
}

void Visual::Base::SetDepthIndex(int index)
{
  // Clamp input value as valid range.
  Dali::ClampInPlace(index, static_cast<int>(Toolkit::DepthIndex::MINIMUM_DEPTH_INDEX), static_cast<int>(Toolkit::DepthIndex::MAXIMUM_DEPTH_INDEX));

  if(mImpl->mDepthIndex != index)
  {
    mImpl->mDepthIndex = index;
    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
    }
  }
}

int Visual::Base::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void Visual::Base::SetOnScene(Actor& actor)
{
  if(!IsOnScene())
  {
    // To display the actor correctly, renderer should not be added to actor until all required resources are ready.
    // Thus the calling of actor.AddRenderer() should happen inside derived class as base class does not know the exact timing.
    DoSetOnScene(actor);

    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
      if(mImpl->mDepthIndex == static_cast<int>(DepthIndex::AUTO_INDEX))
      {
        mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, static_cast<int>(DepthIndex::CONTENT));
      }
      else
      {
        mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
      }
    }

    mImpl->mFlags |= Impl::IS_ON_SCENE;
  }
}

void Visual::Base::SetOffScene(Actor& actor)
{
  if(IsOnScene())
  {
    DoSetOffScene(actor);
    mImpl->mFlags &= ~Impl::IS_ON_SCENE;
  }
}

void Visual::Base::CreatePropertyMap(Property::Map& map) const
{
  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    mImpl->mMixColor = mImpl->mRenderer.GetProperty<Vector4>(Renderer::Property::MIX_COLOR);

    const auto& rendererOffset = mImpl->mRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
    const auto& rendererSize   = mImpl->mRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);

    if(rendererSize != Vector2::ZERO || rendererSize != Vector2::ONE)
    {
      mImpl->GetOrCreateTransform().mOffset = rendererOffset;
      mImpl->GetOrCreateTransform().mSize   = rendererSize;
    }

    if(IsRoundedCornerRequired())
    {
      mImpl->SetCornerRadius(mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS));
    }
    if(IsSquircleCornerRequired())
    {
      mImpl->SetCornerSquareness(mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS));
    }
    if(IsBorderlineRequired())
    {
      mImpl->SetBorderlineWidth(mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH));
      mImpl->SetBorderlineColor(mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR));
      mImpl->SetBorderlineOffset(mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET));
    }
  }

  DoCreatePropertyMap(map);

  if(IsUsingCustomShader())
  {
    mImpl->CreateCustomShaderPropertyMap(map);
  }

  Property::Map transform;
  if(mImpl->mTransform)
  {
    mImpl->mTransform->GetPropertyMap(transform);
  }
  else
  {
    transform = Impl::Transform::GetDefaultTransformMap();
  }
  map.Insert(Toolkit::Visual::Property::TRANSFORM, transform);

  bool premultipliedAlpha(IsPreMultipliedAlphaEnabled());
  map.Insert(Toolkit::Visual::Property::PREMULTIPLIED_ALPHA, premultipliedAlpha);

  // Note, Color and Primitive will also insert their own mix color into the map
  // which is ok, because they have a different key value range, but uses same cached value anyway.
  map.Insert(Toolkit::Visual::Property::MIX_COLOR, mImpl->mMixColor); // vec4
  map.Insert(Toolkit::Visual::Property::OPACITY, mImpl->mMixColor.a);

  auto fittingModeString = Scripting::GetLinearEnumerationName<FittingMode>(
    mImpl->mFittingMode, VISUAL_FITTING_MODE_TABLE, VISUAL_FITTING_MODE_TABLE_COUNT);
  map.Insert(Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE, fittingModeString);

  if(IsTypeAvailableForBorderline(mImpl->mType))
  {
    map.Insert(Toolkit::DevelVisual::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());
    map.Insert(Toolkit::DevelVisual::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
    map.Insert(Toolkit::DevelVisual::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());
  }

  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    map.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
    map.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, mImpl->GetCornerRadiusPolicy());
    map.Insert(Toolkit::DevelVisual::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());
  }
}

void Visual::Base::CreateInstancePropertyMap(Property::Map& map) const
{
  DoCreateInstancePropertyMap(map);

  if(IsUsingCustomShader())
  {
    mImpl->CreateCustomShaderPropertyMap(map);
  }
}

void Visual::Base::EnablePreMultipliedAlpha(bool preMultiplied)
{
  if(preMultiplied)
  {
    mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Impl::IS_PREMULTIPLIED_ALPHA;
  }

  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, preMultiplied);
  }
}

bool Visual::Base::IsPreMultipliedAlphaEnabled() const
{
  return mImpl->mFlags & Impl::IS_PREMULTIPLIED_ALPHA;
}

void Visual::Base::DoSetOffScene(Actor& actor)
{
  actor.RemoveRenderer(mImpl->mRenderer);
}

bool Visual::Base::IsOnScene() const
{
  return mImpl->mFlags & Impl::IS_ON_SCENE;
}

bool Visual::Base::IsRoundedCornerRequired() const
{
  // If VisualType doesn't support rounded corner, always return false.
  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS);

      Vector4 retValue = Vector4::ZERO;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || retValue != Vector4::ZERO)
        {
          mImpl->SetCornerRadius(retValue);
        }
      }
    }
    return mImpl->mAlwaysUsingCornerRadius || !(mImpl->GetCornerRadius() == Vector4::ZERO);
  }
  return false;
}

bool Visual::Base::IsSquircleCornerRequired() const
{
  // If VisualType doesn't support rounded corner, always return false.
  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS);

      Vector4 retValue = Vector4::ZERO;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || retValue != Vector4::ZERO)
        {
          mImpl->SetCornerSquareness(retValue);
        }
      }
    }
    return IsRoundedCornerRequired() && (mImpl->mAlwaysUsingCornerSquareness || !(mImpl->GetCornerSquareness() == Vector4::ZERO));
  }
  return false;
}

bool Visual::Base::IsBorderlineRequired() const
{
  // If VisualType doesn't support borderline, always return false.
  if(IsTypeAvailableForBorderline(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH);

      float retValue = 0.0f;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || !Dali::EqualsZero(retValue))
        {
          mImpl->SetBorderlineWidth(retValue);
        }
      }
    }
    return mImpl->mAlwaysUsingBorderline || !EqualsZero(mImpl->GetBorderlineWidth());
  }
  return false;
}

void Visual::Base::OnDoAction(const Property::Index actionId, const Property::Value& attributes)
{
  // May be overriden by derived class
}

void Visual::Base::OnDoActionExtension(const Property::Index actionId, const Dali::Any& attributes)
{
  // May be overriden by derived class
}

void Visual::Base::RegisterMixColor()
{
  if(mImpl->mRenderer)
  {
    // All visual renderers now use same mix color / opacity properties.
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR, mImpl->mMixColor);

    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
  }
}

void Visual::Base::RegisterDecoration()
{
  if(mImpl->mRenderer)
  {
    if(IsTypeAvailableForCornerRadius(mImpl->mType))
    {
      if(mImpl->mAlwaysUsingCornerRadius || !(mImpl->GetCornerRadius() == Vector4::ZERO))
      {
        DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerRadiusUniform();
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));
      }
      if(mImpl->mAlwaysUsingCornerSquareness || !(mImpl->GetCornerSquareness() == Vector4::ZERO))
      {
        DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerSquarenessUniform();
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());
      }
    }
    if(IsTypeAvailableForBorderline(mImpl->mType))
    {
      if(mImpl->mAlwaysUsingBorderline || !EqualsZero(mImpl->GetBorderlineWidth()))
      {
        DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBorderlineUniform();
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());
      }
    }
  }
}

void Visual::Base::SetMixColor(const Vector4& color)
{
  mImpl->mMixColor = color;

  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR, color);
  }
}

void Visual::Base::SetMixColor(const Vector3& color)
{
  mImpl->mMixColor.r = color.r;
  mImpl->mMixColor.g = color.g;
  mImpl->mMixColor.b = color.b;

  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_RED, color.r);
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_GREEN, color.g);
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_BLUE, color.b);
  }
}

void Visual::Base::AddEventObserver(Visual::EventObserver& observer)
{
  mImpl->mEventObserver = &observer;
}

void Visual::Base::RemoveEventObserver(Visual::EventObserver& observer)
{
  mImpl->mEventObserver = NULL;
}

void Visual::Base::ResourceReady(Toolkit::Visual::ResourceStatus resourceStatus)
{
  if(mImpl->mResourceStatus != resourceStatus)
  {
    mImpl->mResourceStatus = resourceStatus;

    if(mImpl->mEventObserver)
    {
      // observer is currently a control impl
      mImpl->mEventObserver->ResourceReady(*this);
    }
  }
}

bool Visual::Base::IsResourceReady() const
{
  return (mImpl->mResourceStatus == Toolkit::Visual::ResourceStatus::READY ||
          mImpl->mResourceStatus == Toolkit::Visual::ResourceStatus::FAILED);
}

bool Visual::Base::IsSynchronousLoadingRequired() const
{
  return (mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING);
}

Toolkit::Visual::Type Visual::Base::GetType() const
{
  return mImpl->mType;
}

Toolkit::Visual::ResourceStatus Visual::Base::GetResourceStatus() const
{
  return mImpl->mResourceStatus;
}

Visual::FittingMode Visual::Base::GetFittingMode() const
{
  return mImpl->mFittingMode;
}

void Visual::Base::SetFittingMode(Visual::FittingMode fittingMode)
{
  mImpl->mFittingMode = fittingMode;
}

bool Visual::Base::IsIgnoreFittingMode() const
{
  return mImpl->mIgnoreFittingMode;
}

bool Visual::Base::IsPixelAreaSetForFittingMode() const
{
  return mImpl->mPixelAreaSetByFittingMode;
}

void Visual::Base::SetPixelAreaForFittingMode(const Vector4& pixelArea)
{
  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, pixelArea);
  }

  mImpl->mPixelAreaSetByFittingMode = (pixelArea != FULL_TEXTURE_RECT) ? true : false;
}

bool Visual::Base::IsTransformMapSetForFittingMode() const
{
  return mImpl->mTransformMapSetForFittingMode;
}

void Visual::Base::SetTransformMapUsageForFittingMode(bool used)
{
  mImpl->mTransformMapSetForFittingMode = used;
}

void Visual::Base::SetControlSize(Size controlSize)
{
  const static Property::Map emptyMap;
  SetTransformAndSize(emptyMap, controlSize);
}

Visual::Base& Visual::Base::GetVisualObject()
{
  return *this;
}

Renderer Visual::Base::GetRenderer()
{
  return mImpl->mRenderer;
}

Property::Index Visual::Base::GetIntKey(Property::Key key)
{
  if(key.type == Property::Key::INDEX)
  {
    return key.indexKey;
  }

  if(key.stringKey == ANCHOR_POINT)
  {
    return Toolkit::Visual::Transform::Property::ANCHOR_POINT;
  }
  else if(key.stringKey == EXTRA_SIZE)
  {
    return Toolkit::DevelVisual::Transform::Property::EXTRA_SIZE;
  }
  else if(key.stringKey == MIX_COLOR)
  {
    return Toolkit::Visual::Property::MIX_COLOR;
  }
  else if(key.stringKey == OPACITY)
  {
    return Toolkit::Visual::Property::OPACITY;
  }
  else if(key.stringKey == OFFSET)
  {
    return Toolkit::Visual::Transform::Property::OFFSET;
  }
  else if(key.stringKey == OFFSET_POLICY)
  {
    return Toolkit::Visual::Transform::Property::OFFSET_POLICY;
  }
  else if(key.stringKey == ORIGIN)
  {
    return Toolkit::Visual::Transform::Property::ORIGIN;
  }
  else if(key.stringKey == PREMULTIPLIED_ALPHA)
  {
    return Toolkit::Visual::Property::PREMULTIPLIED_ALPHA;
  }
  else if(key.stringKey == CUSTOM_SHADER)
  {
    return Toolkit::Visual::Property::SHADER;
  }
  else if(key.stringKey == SIZE)
  {
    return Toolkit::Visual::Transform::Property::SIZE;
  }
  else if(key.stringKey == SIZE_POLICY)
  {
    return Toolkit::Visual::Transform::Property::SIZE_POLICY;
  }
  else if(key.stringKey == TRANSFORM)
  {
    return Toolkit::Visual::Property::TRANSFORM;
  }
  else if(key.stringKey == VISUAL_FITTING_MODE)
  {
    return Toolkit::DevelVisual::Property::VISUAL_FITTING_MODE;
  }
  else if(key.stringKey == CORNER_RADIUS)
  {
    return Toolkit::DevelVisual::Property::CORNER_RADIUS;
  }
  else if(key.stringKey == CORNER_RADIUS_POLICY)
  {
    return Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY;
  }
  else if(key.stringKey == CORNER_SQUARENESS)
  {
    return Toolkit::DevelVisual::Property::CORNER_SQUARENESS;
  }
  else if(key.stringKey == BORDERLINE_WIDTH)
  {
    return Toolkit::DevelVisual::Property::BORDERLINE_WIDTH;
  }
  else if(key.stringKey == BORDERLINE_COLOR)
  {
    return Toolkit::DevelVisual::Property::BORDERLINE_COLOR;
  }
  else if(key.stringKey == BORDERLINE_OFFSET)
  {
    return Toolkit::DevelVisual::Property::BORDERLINE_OFFSET;
  }

  return Property::INVALID_INDEX;
}

Property::Index Visual::Base::GetPropertyIndex(Property::Key key)
{
  switch(GetIntKey(key))
  {
    case Dali::Toolkit::Visual::Transform::Property::OFFSET:
    {
      return VisualRenderer::Property::TRANSFORM_OFFSET;
    }
    case Dali::Toolkit::Visual::Transform::Property::SIZE:
    {
      return VisualRenderer::Property::TRANSFORM_SIZE;
    }
    case Dali::Toolkit::Visual::Transform::Property::ORIGIN:
    {
      return VisualRenderer::Property::TRANSFORM_ORIGIN;
    }
    case Dali::Toolkit::Visual::Transform::Property::ANCHOR_POINT:
    {
      return VisualRenderer::Property::TRANSFORM_ANCHOR_POINT;
    }
    case Dali::Toolkit::Visual::Property::MIX_COLOR:
    {
      return Renderer::Property::MIX_COLOR;
    }
    case Dali::Toolkit::Visual::Property::OPACITY:
    {
      return Renderer::Property::OPACITY;
    }
    case Dali::Toolkit::Visual::Property::PREMULTIPLIED_ALPHA:
    {
      return Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA;
    }
    case Dali::Toolkit::DevelVisual::Property::CORNER_RADIUS:
    {
      return DecoratedVisualRenderer::Property::CORNER_RADIUS;
    }
    case Dali::Toolkit::DevelVisual::Property::CORNER_SQUARENESS:
    {
      return DecoratedVisualRenderer::Property::CORNER_SQUARENESS;
    }
    case Dali::Toolkit::DevelVisual::Property::BORDERLINE_WIDTH:
    {
      return DecoratedVisualRenderer::Property::BORDERLINE_WIDTH;
    }
    case Dali::Toolkit::DevelVisual::Property::BORDERLINE_COLOR:
    {
      return DecoratedVisualRenderer::Property::BORDERLINE_COLOR;
    }
    case Dali::Toolkit::DevelVisual::Property::BORDERLINE_OFFSET:
    {
      return DecoratedVisualRenderer::Property::BORDERLINE_OFFSET;
    }
  }

  // Fast-out for invalid key.
  if((key.type == Property::Key::INDEX && key.indexKey == Property::INVALID_KEY) ||
     (key.type == Property::Key::STRING && key.stringKey.empty()))
  {
    return Property::INVALID_INDEX;
  }

  Property::Index index = mImpl->mRenderer.GetPropertyIndex(key);

  if(index == Property::INVALID_INDEX)
  {
    // Is it a shader property?
    Shader shader = mImpl->mRenderer.GetShader();
    index         = shader.GetPropertyIndex(key);
    if(index != Property::INVALID_INDEX)
    {
      // Yes - we should register it in the Renderer so it can be set / animated
      // independently, as shaders are shared across multiple renderers.
      std::string     keyName;
      Property::Index keyIndex(Property::INVALID_KEY);
      if(key.type == Property::Key::INDEX)
      {
        keyName  = shader.GetPropertyName(index);
        keyIndex = key.indexKey;
      }
      else
      {
        keyName = key.stringKey;
        // Leave keyIndex as INVALID_KEY - it can still be registered against the string key.
      }
      Property::Value value = shader.GetProperty(index);

      // We already know that mRenderer didn't have property. So we can assume that it is unique.
      index = mImpl->mRenderer.RegisterUniqueProperty(keyIndex, keyName, value);
    }
  }
  return index;
}

void Visual::Base::SetupTransition(
  Dali::Animation&                    transition,
  Internal::TransitionData::Animator& animator,
  Property::Index                     index,
  const Property::Value&              initialValue,
  const Property::Value&              targetValue)
{
  if(index != Property::INVALID_INDEX)
  {
    if(mImpl->mRenderer)
    {
      if(animator.animate == false)
      {
        mImpl->mRenderer.SetProperty(index, targetValue);
      }
      else
      {
        if(animator.initialValue.GetType() != Property::NONE)
        {
          if(animator.animationType != TransitionData::AnimationType::BETWEEN)
          {
            mImpl->mRenderer.SetProperty(index, initialValue);
          }
        }

        if(!transition)
        {
          transition = Dali::Animation::New(0.1f);
        }

        if(animator.animationType == TransitionData::AnimationType::BETWEEN)
        {
          Dali::KeyFrames keyFrames = Dali::KeyFrames::New();
          keyFrames.Add(0.0f, animator.initialValue);
          keyFrames.Add(1.0f, animator.targetValue);
          transition.AnimateBetween(Property(mImpl->mRenderer, index), keyFrames, TimePeriod(animator.timePeriodDelay, animator.timePeriodDuration));
        }
        else if(animator.animationType == TransitionData::AnimationType::BY)
        {
          // To Do
          DALI_LOG_ERROR("AnimationType::By is not supported yet. \n");
        }
        else
        {
          transition.AnimateTo(Property(mImpl->mRenderer, index),
                               targetValue,
                               animator.alphaFunction,
                               TimePeriod(animator.timePeriodDelay,
                                          animator.timePeriodDuration));
        }
      }
    }
  }
}

void Visual::Base::AnimateProperty(
  Dali::Animation&                    transition,
  Internal::TransitionData::Animator& animator)
{
#if defined(DEBUG_ENABLED)
  if(gVisualBaseLogFilter->IsEnabledFor(Debug::General))
  {
    std::ostringstream oss;
    oss << "Visual::Base::AnimateProperty(Visual:" << mImpl->mName << " Property:" << animator.propertyKey << " Target: " << animator.targetValue << std::endl;
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, oss.str().c_str());
  }
#endif

  if(animator.propertyKey == Toolkit::Visual::Property::MIX_COLOR ||
     animator.propertyKey == MIX_COLOR ||
     (mImpl->mType == Toolkit::Visual::COLOR &&
      animator.propertyKey == ColorVisual::Property::MIX_COLOR) ||
     (mImpl->mType == Toolkit::Visual::PRIMITIVE &&
      animator.propertyKey == PrimitiveVisual::Property::MIX_COLOR))
  {
    AnimateMixColorProperty(transition, animator);
  }
  else if(animator.propertyKey == Toolkit::Visual::Property::OPACITY ||
          animator.propertyKey == OPACITY)
  {
    AnimateOpacityProperty(transition, animator);
  }
  else if(mImpl->mRenderer)
  {
    AnimateRendererProperty(transition, animator);
  }
}

void Visual::Base::AnimateOpacityProperty(
  Dali::Animation&                    transition,
  Internal::TransitionData::Animator& animator)
{
  float targetOpacity;
  if(animator.targetValue.Get(targetOpacity))
  {
    mImpl->mMixColor.a = targetOpacity;
  }

  SetupTransition(transition, animator, Renderer::Property::OPACITY, animator.initialValue, animator.targetValue);
}

void Visual::Base::AnimateRendererProperty(
  Dali::Animation&                    transition,
  Internal::TransitionData::Animator& animator)
{
  // Get actual renderer index (will convert transform keys into visualproperty indices)
  Property::Index index = GetPropertyIndex(animator.propertyKey);

  if(index != Property::INVALID_INDEX)
  {
    if(animator.targetValue.GetType() != Property::NONE)
    {
      // Try writing target value into transform property map
      // if it's not a valid key, then it won't alter mTransform
      Property::Map map;
      if(animator.propertyKey.type == Property::Key::INDEX)
      {
        map.Add(animator.propertyKey.indexKey, animator.targetValue);
      }
      else
      {
        map.Add(animator.propertyKey.stringKey, animator.targetValue);
      }

      // Set flag to ignore fitting mode when we set the transform property map
      mImpl->mIgnoreFittingMode = true;
      mImpl->GetOrCreateTransform().UpdatePropertyMap(map);
    }
    SetupTransition(transition, animator, index, animator.initialValue, animator.targetValue);
  }
}

void Visual::Base::AnimateMixColorProperty(
  Dali::Animation&                    transition,
  Internal::TransitionData::Animator& animator)
{
  bool animateOpacity = false;

  Vector4 initialMixColor;
  Vector4 targetMixColor;
  if(animator.initialValue.Get(initialMixColor))
  {
    if(animator.initialValue.GetType() != Property::VECTOR4)
    {
      // if there is non initial color specifying alpha, get from cached mix color
      initialMixColor.a = mImpl->mMixColor.a;
    }
  }

  // Set target value into data store
  if(animator.targetValue.GetType() != Property::NONE)
  {
    animator.targetValue.Get(targetMixColor);
    if(animator.targetValue.GetType() == Property::VECTOR4)
    {
      mImpl->mMixColor.a = targetMixColor.a;
      animateOpacity     = true;
    }

    mImpl->mMixColor.r = targetMixColor.r;
    mImpl->mMixColor.g = targetMixColor.g;
    mImpl->mMixColor.b = targetMixColor.b;
  }

  if(animateOpacity)
  {
    SetupTransition(transition, animator, Renderer::Property::MIX_COLOR, initialMixColor, targetMixColor);
  }
  else
  {
    SetupTransition(transition, animator, Renderer::Property::MIX_COLOR_RED, initialMixColor.r, targetMixColor.r);
    SetupTransition(transition, animator, Renderer::Property::MIX_COLOR_GREEN, initialMixColor.g, targetMixColor.g);
    SetupTransition(transition, animator, Renderer::Property::MIX_COLOR_BLUE, initialMixColor.b, targetMixColor.b);
  }
}

Dali::Property Visual::Base::GetPropertyObject(Dali::Property::Key key)
{
  if(!mImpl->mRenderer)
  {
    Handle handle;
    return Dali::Property(handle, Property::INVALID_INDEX);
  }

  switch(GetIntKey(key))
  {
    // Default animatable properties from VisualRenderer
    case Toolkit::Visual::Property::MIX_COLOR:
    {
      return Dali::Property(mImpl->mRenderer, Renderer::Property::MIX_COLOR);
    }
    case Toolkit::Visual::Property::OPACITY:
    {
      return Dali::Property(mImpl->mRenderer, Renderer::Property::OPACITY);
    }
    case Toolkit::Visual::Transform::Property::OFFSET:
    {
      // Need to change visual transform is not default anymore.
      if(mImpl->mTransformMapUsingDefault)
      {
        mImpl->mTransformMapUsingDefault = false;
        mImpl->mRenderer.RegisterVisualTransformUniform();
      }

      return Dali::Property(mImpl->mRenderer, VisualRenderer::Property::TRANSFORM_OFFSET);
    }
    case Toolkit::Visual::Transform::Property::SIZE:
    {
      // Need to change visual transform is not default anymore.
      if(mImpl->mTransformMapUsingDefault)
      {
        mImpl->mTransformMapUsingDefault = false;
        mImpl->mRenderer.RegisterVisualTransformUniform();
      }

      return Dali::Property(mImpl->mRenderer, VisualRenderer::Property::TRANSFORM_SIZE);
    }

    // Default animatable properties from DecoratedVisualRenderer
    case Toolkit::DevelVisual::Property::CORNER_RADIUS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        const bool updateShader = !IsUsingCustomShader() && !IsRoundedCornerRequired();

        // CornerRadius is animated now. we always have to use corner radius feature.
        mImpl->mAlwaysUsingCornerRadius = true;

        if(updateShader)
        {
          // Update each values to renderer
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerRadiusUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));

          if(IsSquircleCornerRequired())
          {
            // Change the shader must not be occured many times. we always have to use corner squreness feature.
            mImpl->mAlwaysUsingCornerSquareness = true;

            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());
          }

          // Change shader
          UpdateShader();
        }
        if(!IsBorderlineRequired())
        {
          // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
          mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
        }
        return Dali::Property(mImpl->mRenderer, DecoratedVisualRenderer::Property::CORNER_RADIUS);
      }
      break;
    }
    case Toolkit::DevelVisual::Property::CORNER_SQUARENESS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        const bool updateShader = !IsUsingCustomShader() && !IsSquircleCornerRequired();

        // CornerSquareness is animated now. we always have to use corner squareness feature.
        mImpl->mAlwaysUsingCornerSquareness = true;

        if(updateShader)
        {
          // Update each values to renderer
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterCornerSquarenessUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY, static_cast<float>(mImpl->GetCornerRadiusPolicy()));
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());

          // Change shader
          UpdateShader();
        }
        if(!IsBorderlineRequired())
        {
          // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
          mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
        }
        return Dali::Property(mImpl->mRenderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS);
      }
      break;
    }
    case Toolkit::DevelVisual::Property::BORDERLINE_WIDTH:
    case Toolkit::DevelVisual::Property::BORDERLINE_COLOR:
    case Toolkit::DevelVisual::Property::BORDERLINE_OFFSET:
    {
      if(IsTypeAvailableForBorderline(mImpl->mType))
      {
        const bool updateShader = !IsUsingCustomShader() && !IsBorderlineRequired();

        // Borderline is animated now. we always have to use borderline feature.
        mImpl->mAlwaysUsingBorderline = true;

        if(updateShader)
        {
          // Update each values to renderer
          DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBorderlineUniform();
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());

          // Change shader
          UpdateShader();
        }
        mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);

        return Dali::Property(mImpl->mRenderer, GetPropertyIndex(key));
      }
      break;
    }
    default:
    {
      // Special case for MIX_COLOR
      if(key.type == Property::Key::INDEX &&
         ((mImpl->mType == Toolkit::Visual::COLOR && key.indexKey == ColorVisual::Property::MIX_COLOR) ||
          (mImpl->mType == Toolkit::Visual::PRIMITIVE && key.indexKey == PrimitiveVisual::Property::MIX_COLOR)))
      {
        return Dali::Property(mImpl->mRenderer, Renderer::Property::MIX_COLOR);
      }

      // Special case for BLUR_RADIUS
      if(mImpl->mType == Toolkit::Visual::COLOR &&
         ((key.type == Property::Key::INDEX && key.indexKey == DevelColorVisual::Property::BLUR_RADIUS) ||
          (key.type == Property::Key::STRING && key.stringKey == BLUR_RADIUS_NAME)))
      {
        // Request to color-visual class
        return OnGetPropertyObject(key);
      }
    }
  }

  // If it is not VisualRenderer property, check registered Renderer and Shader property.
  Property::Index index = GetPropertyIndex(key);
  if(index != Property::INVALID_INDEX)
  {
    return Dali::Property(mImpl->mRenderer, index);
  }

  // We can't find the property in the base class.
  // Request to child class
  return OnGetPropertyObject(key);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
