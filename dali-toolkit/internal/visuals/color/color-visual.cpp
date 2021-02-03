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
#include <dali-toolkit/devel-api/visuals/color-visual-actions-devel.h>
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
  mRenderIfTransparent(false),
  mNeedBlurRadius(false)
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

  Property::Value* renderIfTransparentValue = propertyMap.Find(Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT, RENDER_IF_TRANSPARENT_NAME);
  if(renderIfTransparentValue)
  {
    if(!renderIfTransparentValue->Get(mRenderIfTransparent))
    {
      DALI_LOG_ERROR("ColorVisual: renderIfTransparent property has incorrect type: %d\n", renderIfTransparentValue->GetType());
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
  // Only add the renderer if it's not fully transparent
  // We cannot avoid creating a renderer as it's used in the base class
  if(mRenderIfTransparent || mImpl->mMixColor.a > 0.0f)
  {
    actor.AddRenderer(mImpl->mRenderer);
  }

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
  map.Insert(Toolkit::DevelColorVisual::Property::RENDER_IF_TRANSPARENT, mRenderIfTransparent);

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

void ColorVisual::OnDoAction(const Property::Index actionId, const Property::Value& attributes)
{
  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case DevelColorVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if(map)
      {
        DoSetProperties(*map);
      }
      break;
    }
  }
}

void ColorVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GetShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void ColorVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  Shader shader = GetShader();

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

Shader ColorVisual::GetShader()
{
  Shader shader;
  if(!EqualsZero(mBlurRadius) || mNeedBlurRadius)
  {
    shader = mFactoryCache.GetShader(VisualFactoryCache::COLOR_SHADER_BLUR_EDGE);
    if(!shader)
    {
      shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_COLOR_VISUAL_BLUR_EDGE_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_COLOR_VISUAL_BLUR_EDGE_SHADER_FRAG.data());
      mFactoryCache.SaveShader(VisualFactoryCache::COLOR_SHADER_BLUR_EDGE, shader);
    }
  }
  else if(!IsRoundedCornerRequired())
  {
    shader = mFactoryCache.GetShader(VisualFactoryCache::COLOR_SHADER);
    if(!shader)
    {
      shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_COLOR_VISUAL_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_COLOR_VISUAL_SHADER_FRAG.data());
      mFactoryCache.SaveShader(VisualFactoryCache::COLOR_SHADER, shader);
    }
  }
  else
  {
    shader = mFactoryCache.GetShader(VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER);
    if(!shader)
    {
      shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_COLOR_VISUAL_ROUNDED_CORNER_SHADER_VERT.data(), Dali::Shader::GetFragmentShaderPrefix() + SHADER_COLOR_VISUAL_ROUNDED_CORNER_SHADER_FRAG.data());
      mFactoryCache.SaveShader(VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER, shader);
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

  if((key.type == Property::Key::INDEX && key.indexKey == DevelColorVisual::Property::BLUR_RADIUS) || (key.type == Property::Key::STRING && key.stringKey == BLUR_RADIUS_NAME))
  {
    mBlurRadiusIndex = mImpl->mRenderer.RegisterProperty(DevelColorVisual::Property::BLUR_RADIUS, BLUR_RADIUS_NAME, mBlurRadius);

    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

    mNeedBlurRadius = true;

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
