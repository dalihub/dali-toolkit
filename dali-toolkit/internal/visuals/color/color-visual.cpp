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
#include "color-visual.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
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
} // unnamed namespace
ColorVisualPtr ColorVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  ColorVisualPtr colorVisualPtr(new ColorVisual(factoryCache));
  colorVisualPtr->SetProperties(properties);
  colorVisualPtr->Initialize();
  return colorVisualPtr;
}

ColorVisual::ColorVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::COLOR),
  mBlurRadius(0.0f),
  mBlurRadiusIndex(Property::INVALID_INDEX),
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

  if(mImpl->mRenderer && mBlurRadiusIndex != Property::INVALID_INDEX)
  {
    // Update values from Renderer
    float blurRadius = mImpl->mRenderer.GetProperty<float>(mBlurRadiusIndex);
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

void ColorVisual::OnSetTransform()
{
  if(mImpl->mRenderer)
  {
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
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

  mImpl->mRenderer = Renderer::New(geometry, shader);

  // ColorVisual has it's own index key for mix color - use this instead
  // of using the new base index to avoid changing existing applications
  // String keys will get to this property.
  mImpl->mMixColorIndex = mImpl->mRenderer.RegisterProperty(Toolkit::ColorVisual::Property::MIX_COLOR, MIX_COLOR, Vector3(mImpl->mMixColor));

  if(!EqualsZero(mBlurRadius))
  {
    mBlurRadiusIndex = mImpl->mRenderer.RegisterProperty(DevelColorVisual::Property::BLUR_RADIUS, BLUR_RADIUS_NAME, mBlurRadius);
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  }

  // Register transform properties
  mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
}

Shader ColorVisual::GenerateShader() const
{
  Shader shader;
  VisualFactoryCache::ShaderType shaderType;

  bool roundedCorner = IsRoundedCornerRequired();
  bool borderline    = IsBorderlineRequired();
  bool blur          = !EqualsZero(mBlurRadius) || mAlwaysUsingBlurRadius;
  int shaderTypeFlag = ColorVisualRequireFlag::DEFAULT;

  if(roundedCorner)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::ROUNDED_CORNER;
  }
  if(blur)
  {
    // If we use blur, just ignore borderline
    borderline = false;
    shaderTypeFlag |= ColorVisualRequireFlag::BLUR;
  }
  if(borderline)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::BORDERLINE;
  }

  shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  shader = mFactoryCache.GetShader(shaderType);
  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;
    if(roundedCorner)
    {
      vertexShaderPrefixList   += "#define IS_REQUIRED_ROUNDED_CORNER 1\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER 1\n";
    }
    if(blur)
    {
      vertexShaderPrefixList   += "#define IS_REQUIRED_BLUR 1\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_BLUR 1\n";
    }
    if(borderline)
    {
      vertexShaderPrefixList   += "#define IS_REQUIRED_BORDERLINE 1\n";
      fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE 1\n";
    }
    shader = Shader::New(Dali::Shader::GetVertexShaderPrefix()   + vertexShaderPrefixList   + SHADER_COLOR_VISUAL_SHADER_VERT.data(),
                         Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_FRAG.data());
    mFactoryCache.SaveShader(shaderType, shader);
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
    mBlurRadiusIndex = mImpl->mRenderer.RegisterProperty(DevelColorVisual::Property::BLUR_RADIUS, BLUR_RADIUS_NAME, mBlurRadius);

    // Blur is animated now. we always have to use blur feature.
    mAlwaysUsingBlurRadius = true;

    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

    // Change shader
    UpdateShader();

    return Dali::Property(mImpl->mRenderer, mBlurRadiusIndex);
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
