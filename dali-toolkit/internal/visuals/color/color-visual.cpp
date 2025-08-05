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
#include <dali-toolkit/internal/visuals/color/color-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/toolkit-constraint-tag-ranges.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const int CUSTOM_PROPERTY_COUNT(0); ///< Note : cutout policy property will be registered only of the cutout view is used.
                                    ///<        We don't need to reserve that property memory always.

// cutout policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(CUTOUT_POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelColorVisual::CutoutPolicy, NONE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelColorVisual::CutoutPolicy, CUTOUT_VIEW)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelColorVisual::CutoutPolicy, CUTOUT_VIEW_WITH_CORNER_RADIUS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelColorVisual::CutoutPolicy, CUTOUT_OUTSIDE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::Toolkit::DevelColorVisual::CutoutPolicy, CUTOUT_OUTSIDE_WITH_CORNER_RADIUS)
DALI_ENUM_TO_STRING_TABLE_END(CUTOUT_POLICY)

static constexpr uint32_t CUTOUT_CORNER_RADIUS_CONSTRAINT_TAG(Dali::Toolkit::ConstraintTagRanges::TOOLKIT_CONSTRAINT_TAG_START + 20);

} // unnamed namespace

ColorVisualPtr ColorVisual::New(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory, const Property::Map& properties)
{
  ColorVisualPtr colorVisualPtr(new ColorVisual(factoryCache, shaderFactory));
  colorVisualPtr->SetProperties(properties);
  colorVisualPtr->Initialize();
  return colorVisualPtr;
}

ColorVisual::ColorVisual(VisualFactoryCache& factoryCache, ColorVisualShaderFactory& shaderFactory)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::COLOR),
  mBlurRadius(0.0f),
  mCuroutCornerRadiusIndex(Property::INVALID_INDEX),
  mCutoutPolicy(DevelColorVisual::CutoutPolicy::NONE),
  mAlwaysUsingBlurRadius(false),
  mColorVisualShaderFactory(shaderFactory)
{
  // Make we always use premultiplied alpha.
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

ColorVisual::~ColorVisual()
{
}

void ColorVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // By virtue of DoSetProperties being called last, this will override
  // anything set by Toolkit::Visual::Property::MIX_COLOR
  Property::Value* colorValue = propertyMap.Find(Toolkit::ColorVisual::Property::MIX_COLOR, MIX_COLOR);
  if(colorValue)
  {
    Vector4 color;
    if(colorValue->Get(color))
    {
      Property::Type type = colorValue->GetType();
      if(type == Property::VECTOR4)
      {
        SetMixColor(color);
      }
      else if(type == Property::VECTOR3)
      {
        Vector3 color3(color);
        SetMixColor(color3);
      }
    }
    else
    {
      DALI_LOG_ERROR("ColorVisual: mixColor property has incorrect type\n");
    }
  }

  Property::Value* blurRadiusValue = propertyMap.Find(Toolkit::DevelColorVisual::Property::BLUR_RADIUS, BLUR_RADIUS_NAME);
  if(blurRadiusValue)
  {
    if(!blurRadiusValue->Get(mBlurRadius))
    {
      DALI_LOG_ERROR("ColorVisual:DoSetProperties:: BLUR_RADIUS property has incorrect type: %d\n", blurRadiusValue->GetType());
    }

    if(DALI_UNLIKELY(mImpl->mRenderer))
    {
      // Unusual case. SetProperty called after OnInitialize().
      // Assume that DoAction call UPDATE_PROPERTY.
      DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBlurRadiusUniform();
      mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, mBlurRadius);

      // Check whether we must update shader.
      if(!mAlwaysUsingBlurRadius && IsBlurRequired())
      {
        // Change the shader must not be occured many times. we always have to use blur feature.
        mAlwaysUsingBlurRadius = true;

        if(!IsBorderlineRequired())
        {
          // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
          mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
        }

        // Change shader
        if(!mImpl->mCustomShader)
        {
          UpdateShader();
        }
      }
    }
  }

  Property::Value* cutoutPolicyValue = propertyMap.Find(Toolkit::DevelColorVisual::Property::CUTOUT_POLICY, CUTOUT_POLICY_NAME);
  if(cutoutPolicyValue)
  {
    int cutoutPolicy = static_cast<int>(DevelColorVisual::CutoutPolicy::NONE) - 1; ///< Make always invalid
    if(DALI_UNLIKELY(!Scripting::GetEnumerationProperty(*cutoutPolicyValue, CUTOUT_POLICY_TABLE, CUTOUT_POLICY_TABLE_COUNT, cutoutPolicy)))
    {
      std::ostringstream oss;
      oss << *cutoutPolicyValue;
      DALI_LOG_ERROR("ColorVisual:DoSetProperties:: CUTOUT_POLICY property has incorrect type : %d, value : %s\n", cutoutPolicyValue->GetType(), oss.str().c_str());
    }
    else
    {
      mCutoutPolicy = static_cast<DevelColorVisual::CutoutPolicy::Type>(cutoutPolicy);
    }
  }
}

void ColorVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  if(mCuroutCornerRadiusIndex != Property::INVALID_INDEX)
  {
    // If cutout policy is CUTOUT_VIEW_WITH_CORNER_RADIUS or CUTOUT_OUTSIDE_WITH_CORNER_RADIUS, we need to apply equal constraint to it with control's corner radius.
    Toolkit::Control control = Toolkit::Control::DownCast(actor);
    DALI_ASSERT_ALWAYS(control && "ColorVisual must be used with Control");

    // Get the corner radius from control
    mCutoutCornerRadiusConstraint = Constraint::New<Vector4>(mImpl->mRenderer, mCuroutCornerRadiusIndex, Dali::EqualToConstraint());
    mCutoutCornerRadiusConstraint.AddSource(Source(control, Toolkit::DevelControl::Property::CORNER_RADIUS));
    Dali::Integration::ConstraintSetInternalTag(mCutoutCornerRadiusConstraint, CUTOUT_CORNER_RADIUS_CONSTRAINT_TAG);

    // Apply the constraint to renderer
    mCutoutCornerRadiusConstraint.Apply();
  }

  // Color Visual generated and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void ColorVisual::DoSetOffScene(Actor& actor)
{
  if(mCuroutCornerRadiusIndex != Property::INVALID_INDEX)
  {
    // Remove the constraint
    mCutoutCornerRadiusConstraint.Remove();
    mCutoutCornerRadiusConstraint.Reset();
  }
  actor.RemoveRenderer(mImpl->mRenderer);
}

void ColorVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  map.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, mImpl->mMixColor);
  map.Insert(Toolkit::DevelColorVisual::Property::CUTOUT_POLICY, mCutoutPolicy);

  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    float blurRadius = mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS);
    map.Insert(Toolkit::DevelColorVisual::Property::BLUR_RADIUS, blurRadius);
  }
  else
  {
    map.Insert(Toolkit::DevelColorVisual::Property::BLUR_RADIUS, mBlurRadius);
  }
}

void ColorVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void ColorVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  // Make always enable pre multiplied alpha whether preMultiplied value is false.
  if(!preMultiplied)
  {
    DALI_LOG_WARNING("Note : ColorVisual cannot disable PreMultipliedAlpha\n");
  }
}

void ColorVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
  }
}

void ColorVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void ColorVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  Shader shader = GenerateShader();

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);

  if(mAlwaysUsingBlurRadius || !EqualsZero(mBlurRadius))
  {
    DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBlurRadiusUniform();
    mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, mBlurRadius);
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  }

  if(IsCutoutRequired())
  {
    int cutoutWithCornerRadius = ((mCutoutPolicy == DevelColorVisual::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS) || (mCutoutPolicy == DevelColorVisual::CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS));
    int cutoutOutside          = ((mCutoutPolicy == DevelColorVisual::CutoutPolicy::CUTOUT_OUTSIDE) || (mCutoutPolicy == DevelColorVisual::CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS));

    mImpl->mRenderer.ReserveCustomProperties(2 + cutoutWithCornerRadius);
    mImpl->mRenderer.RegisterUniqueProperty("uCutoutWithCornerRadius", cutoutWithCornerRadius);
    mImpl->mRenderer.RegisterUniqueProperty("uCutoutOutside", cutoutOutside);

    if(cutoutWithCornerRadius)
    {
      // Register cutout policy property
      mCuroutCornerRadiusIndex = mImpl->mRenderer.RegisterUniqueProperty(CUTOUT_CORNER_RADIUS_UNIFORM_NAME, Vector4::ZERO);
    }
  }

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Shader ColorVisual::GenerateShader() const
{
  Shader shader;

  const bool useStandardShader = !mImpl->mCustomShader;

  if(useStandardShader)
  {
    shader = mColorVisualShaderFactory.GetShader(
      mFactoryCache,
      ColorVisualShaderFeature::FeatureBuilder()
        .EnableBlur(IsBlurRequired())
        .EnableBorderLine(IsBorderlineRequired())
        .EnableRoundCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
        .EnableCutout(IsCutoutRequired()));
  }
  else
  {
    const bool hasVertexShader   = !mImpl->mCustomShader->mVertexShader.empty();
    const bool hasFragmentShader = !mImpl->mCustomShader->mFragmentShader.empty();

    std::string_view vertexShaderView;
    std::string_view fragmentShaderView;

    if(hasVertexShader)
    {
      vertexShaderView = mImpl->mCustomShader->mVertexShader;
    }
    else
    {
      vertexShaderView = mColorVisualShaderFactory.GetVertexShaderSource();
    }

    if(hasFragmentShader)
    {
      fragmentShaderView = mImpl->mCustomShader->mFragmentShader;
    }
    else
    {
      fragmentShaderView = mColorVisualShaderFactory.GetFragmentShaderSource();
    }

    shader = Shader::New(vertexShaderView, fragmentShaderView, mImpl->mCustomShader->mHints);

    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.RegisterVisualTransformUniform();
    }
  }
  return shader;
}

Dali::Property ColorVisual::OnGetPropertyObject(Dali::Property::Key key)
{
  if(!mImpl->mRenderer)
  {
    Handle handle;
    return Dali::Property(handle, Property::INVALID_INDEX);
  }

  if((key.type == Property::Key::INDEX && key.indexKey == DevelColorVisual::Property::BLUR_RADIUS) ||
     (key.type == Property::Key::STRING && key.stringKey == BLUR_RADIUS_NAME))
  {
    const bool updateShader = !mImpl->mCustomShader && !IsBlurRequired();

    // Blur is animated now. we always have to use blur feature.
    mAlwaysUsingBlurRadius = true;

    if(updateShader)
    {
      // Update each values to renderer
      DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBlurRadiusUniform();
      mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, mBlurRadius);

      // Change shader
      UpdateShader();
    }
    if(!IsBorderlineRequired())
    {
      // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite it.
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
    }
    return Dali::Property(mImpl->mRenderer, DecoratedVisualRenderer::Property::BLUR_RADIUS);
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

bool ColorVisual::IsBlurRequired() const
{
  float blurRadius = mBlurRadius;
  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    blurRadius = mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BLUR_RADIUS);
  }
  return mAlwaysUsingBlurRadius || !EqualsZero(blurRadius);
}

bool ColorVisual::IsCutoutRequired() const
{
  return (mCutoutPolicy != DevelColorVisual::CutoutPolicy::NONE);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
