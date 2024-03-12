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
#include "color-visual.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-actions-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
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
const int CUSTOM_PROPERTY_COUNT(0);

VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[6] =
  {
    VisualFactoryCache::COLOR_SHADER,
    VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER,
    VisualFactoryCache::COLOR_SHADER_BORDERLINE,
    VisualFactoryCache::COLOR_SHADER_ROUNDED_BORDERLINE,
    VisualFactoryCache::COLOR_SHADER_BLUR_EDGE,
    VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
};

// enum of required list when we select shader
enum ColorVisualRequireFlag
{
  DEFAULT        = 0,
  ROUNDED_CORNER = 1 << 0,
  BORDERLINE     = 1 << 1,
  BLUR           = 1 << 2,
};

constexpr uint32_t MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR = 300;
} // unnamed namespace
ColorVisualPtr ColorVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  ColorVisualPtr colorVisualPtr(new ColorVisual(factoryCache));
  colorVisualPtr->SetProperties(properties);
  colorVisualPtr->Initialize();
  return colorVisualPtr;
}

ColorVisual::ColorVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::DONT_CARE, Toolkit::Visual::COLOR),
  mBlurRadius(0.0f),
  mAlwaysUsingBlurRadius(false)
{
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

        mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
        // Change shader
        if(!mImpl->mCustomShader)
        {
          UpdateShader();
        }
      }
    }
  }
}

void ColorVisual::DoSetOnScene(Actor& actor)
{
  actor.AddRenderer(mImpl->mRenderer);

  // Color Visual generated and ready to display
  ResourceReady(Toolkit::Visual::ResourceStatus::READY);
}

void ColorVisual::DoSetOffScene(Actor& actor)
{
  actor.RemoveRenderer(mImpl->mRenderer);
}

void ColorVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR);
  map.Insert(Toolkit::ColorVisual::Property::MIX_COLOR, mImpl->mMixColor);

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
  // Make always disable pre multiplied alpha whether preMultiplied value is true.
  if(preMultiplied)
  {
    DALI_LOG_WARNING("Note : ColorVisual cannot enable PreMultipliedAlpha\n");
  }
}

void ColorVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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

  mImpl->mRenderer.SetProperty(VisualRenderer::Property::VISUAL_MIX_COLOR, Vector3(mImpl->mMixColor));

  if(mAlwaysUsingBlurRadius || !EqualsZero(mBlurRadius))
  {
    DownCast<DecoratedVisualRenderer>(mImpl->mRenderer).RegisterBlurRadiusUniform();
    mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BLUR_RADIUS, mBlurRadius);
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  }

  // Register transform properties
  mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Shader ColorVisual::GenerateShader() const
{
  Shader                         shader;
  VisualFactoryCache::ShaderType shaderType;

  bool roundedCorner  = IsRoundedCornerRequired();
  bool borderline     = IsBorderlineRequired();
  bool blur           = IsBlurRequired();
  int  shaderTypeFlag = ColorVisualRequireFlag::DEFAULT;

  if(blur)
  {
    // If we use blur, just ignore borderline
    borderline = false;
    shaderTypeFlag |= ColorVisualRequireFlag::BLUR;
  }
  if(roundedCorner)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::ROUNDED_CORNER;
  }
  if(borderline)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::BORDERLINE;
  }

  shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  shader     = mFactoryCache.GetShader(shaderType);
  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;
    if(roundedCorner)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    }
    if(blur)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_BLUR\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_BLUR\n";

      // If shader version doesn't support latest blur with corner radius, Let we use legacy code.
      if(DALI_UNLIKELY(Dali::Shader::GetShaderLanguageVersion() < MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR))
      {
        fragmentShaderPrefixList += "#define SL_VERSION_LOW\n";
      }
    }
    if(borderline)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }

    shader = mFactoryCache.GenerateAndSaveShader(shaderType,
                                                 Dali::Shader::GetVertexShaderPrefix() + vertexShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_VERT.data(),
                                                 Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_FRAG.data());
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

  if((key.type == Property::Key::INDEX && key.indexKey == DevelColorVisual::Property::BLUR_RADIUS) || (key.type == Property::Key::STRING && key.stringKey == BLUR_RADIUS_NAME))
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
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
