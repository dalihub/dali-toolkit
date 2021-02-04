#ifndef DALI_TOOLKIT_INTERNAL_CONTROL_RENDERERS_H
#define DALI_TOOLKIT_INTERNAL_CONTROL_RENDERERS_H

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

#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali/dali.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#define BASIC_VERTEX_SOURCE SHADER_CONTROL_RENDERERS_VERT
#define BASIC_FRAGMENT_SOURCE SHADER_CONTROL_RENDERERS_FRAG

/**
 * Helper method for rendering an image with custom shader.
 * @param[in] vertextSrc The custom vertex shader.
 * @param[in] fragmentSrc The custom fragment shader.
 * @return A newly created renderer.
 */
Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc);

/**
 * Helper method for rendering an image with custom shader.
 * @param[in] vertextSrc The custom vertex shader.
 * @param[in] fragmentSrc The custom fragment shader.
 * @param[in] gridSize The number of grid sub-divisions required.
 * @return A newly created renderer.
 */
Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc, Dali::Shader::Hint::Value hints, Dali::Uint16Pair gridSize);

/**
 * Helper method for setting the first texture passed to a renderer.
 * @param[in] renderer The renderer using the texture.
 * @param[in] texture The texture to set.
 */
void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture);

/**
 * Helper method for setting the first texture passed to a renderer.
 * @param[in] renderer The renderer using the texture.
 * @param[in] framebuffer A frame buffer color texture attached.
 */
void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer);

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CONTROL_RENDERERS_H
