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
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

// global string variable to caching complate vertex shader
static std::string gVertexShader;

// global string variable to caching complate fragment shader (no atlas)
static std::string gFragmentShaderNoAtlas;

} // unnamed namespace

ImageVisualShaderFactory::ImageVisualShaderFactory()
{
}

ImageVisualShaderFactory::~ImageVisualShaderFactory()
{
}

Shader ImageVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, bool atlasing, bool defaultTextureWrapping, bool roundedCorner)
{
  Shader shader;
  if(atlasing)
  {
    if(defaultTextureWrapping)
    {
      shader = factoryCache.GetShader(VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP);
      if(!shader)
      {
        shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data(),
                             Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_ATLAS_CLAMP_SHADER_FRAG.data());
        shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
        factoryCache.SaveShader(VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader);
      }
    }
    else
    {
      shader = factoryCache.GetShader(VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP);
      if(!shader)
      {
        shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data(),
                             Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_ATLAS_VARIOUS_WRAP_SHADER_FRAG.data());
        shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
        factoryCache.SaveShader(VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP, shader);
      }
    }
  }
  else
  {
    if(roundedCorner)
    {
      shader = factoryCache.GetShader(VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER);
      if(!shader)
      {
        shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_ROUNDED_CORNER_SHADER_VERT.data(),
                             Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_ROUNDED_CORNER_SHADER_FRAG.data());
        shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
        factoryCache.SaveShader(VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER, shader);
      }
    }
    else
    {
      shader = factoryCache.GetShader(VisualFactoryCache::IMAGE_SHADER);
      if(!shader)
      {
        shader = Shader::New(Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data(),
                             Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_NO_ATLAS_SHADER_FRAG.data());
        shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
        factoryCache.SaveShader(VisualFactoryCache::IMAGE_SHADER, shader);
      }
    }
  }

  return shader;
}

std::string_view ImageVisualShaderFactory::GetVertexShaderSource()
{
  if(gVertexShader.empty())
  {
    gVertexShader = Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data();
  }

  return gVertexShader;
}

std::string_view ImageVisualShaderFactory::GetFragmentShaderSource()
{
  if(gFragmentShaderNoAtlas.empty())
  {
    gFragmentShaderNoAtlas = Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_NO_ATLAS_SHADER_FRAG.data();
  }
  return gFragmentShaderNoAtlas;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
