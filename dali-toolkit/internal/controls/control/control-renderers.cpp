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
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-renderers.h>

#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
Shader CreateShader(std::string_view vertexSrc, std::string_view fragmentSrc, Dali::Shader::Hint::Value hints, const std::string& shaderName)
{
  if(Dali::Adaptor::IsAvailable() && !shaderName.empty())
  {
    auto factory = Dali::Toolkit::VisualFactory::Get();
    if(DALI_LIKELY(factory))
    {
      thread_local static std::unordered_map<std::string, VisualFactoryCache::ExternalShaderId> gShaderIdMap;

      auto& visualFactoryCache = GetImplementation(factory).GetFactoryCache();

      auto iter = gShaderIdMap.find(shaderName);
      if(iter != gShaderIdMap.end())
      {
        return visualFactoryCache.GetExternalShader(iter->second);
      }
      Shader shader            = Shader::New(vertexSrc, fragmentSrc, hints, shaderName);
      gShaderIdMap[shaderName] = visualFactoryCache.RegisterExternalShader(shader);
      return shader;
    }
  }

  return Shader::New(vertexSrc, fragmentSrc, hints, shaderName);
}
} // namespace

Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc)
{
  return CreateRenderer(vertexSrc, fragmentSrc, Dali::Shader::Hint::NONE, "", Uint16Pair(1, 1));
}

Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Dali::Shader::Hint::Value hints, const std::string& shaderName, Uint16Pair gridSize)
{
  Dali::Shader shader = CreateShader(vertexSrc, fragmentSrc, hints, shaderName);

  Dali::Geometry gridGeometry = VisualFactoryCache::CreateGridGeometry(gridSize, true);

  Dali::Renderer renderer = Dali::Renderer::New(gridGeometry, shader);

  Dali::TextureSet textureSet = Dali::TextureSet::New();
  renderer.SetTextures(textureSet);

  return renderer;
}

void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture)
{
  if(renderer)
  {
    Dali::TextureSet textureSet = renderer.GetTextures();
    textureSet.SetTexture(0u, texture);
  }
}

void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer)
{
  if(frameBuffer)
  {
    Dali::Texture texture = frameBuffer.GetColorTexture();
    SetRendererTexture(renderer, texture);
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
