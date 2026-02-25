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
 */

// INTERNAL INCLUDES
#include "control-renderers.h"
#include <dali-toolkit/internal/controls/control/control-renderers.h>

namespace Dali::Toolkit::DevelControl
{

Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc);
}

Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Shader::Hint::Value hints, const std::string& shaderName, Uint16Pair gridSize)
{
  return Internal::CreateRenderer(vertexSrc, fragmentSrc, hints, shaderName, gridSize);
}

void SetRendererTexture(Renderer renderer, Texture texture)
{
  Internal::SetRendererTexture(renderer, texture);
}

void SetRendererTexture(Renderer renderer, FrameBuffer frameBuffer)
{
  Internal::SetRendererTexture(renderer, frameBuffer);
}

} // Dali::Toolkit::DevelControl