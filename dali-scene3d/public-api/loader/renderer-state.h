#ifndef DALI_SCENE3D_LOADER_RENDERER_STATE_H
#define DALI_SCENE3D_LOADER_RENDERER_STATE_H
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/renderer.h>

// INTERAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/*
 * @brief Contains values for comparison functions used in depth and stencil testing.
 * @note Relative order of members must match DepthFunction::Type and StencilFunction::Type.
 */
struct DALI_SCENE3D_API Comparison
{
  enum Type
  {
    OMIT, // not specified; will not be set.
    NEVER,
    ALWAYS,
    LESS,
    GREATER,
    EQUAL,
    NOT_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL
  };

  Comparison() = delete;
};

/*
 * @brief Determines the blend factor used.
 * @note Relative order of members must match BlendFactor::Type.
 */
struct DALI_SCENE3D_API BlendFactor
{
  enum Type
  {
    OMIT, // not specified - will not be updated
    ZERO,
    ONE, // default for source alpha
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    SRC_ALPHA,           // default for source RGB
    ONE_MINUS_SRC_ALPHA, // default for destination RGB and destination alpha
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA_SATURATE,
    CONSTANT_COLOR,
    ONE_MINUS_CONSTANT_COLOR,
    CONSTANT_ALPHA,
    ONE_MINUS_CONSTANT_ALPHA,
  };

  BlendFactor() = delete;
};

/*
 * @brief Determines which buffers shall the Renderer write into.
 * @note Relative order of members must match RenderMode::Type.
 */
struct DALI_SCENE3D_API BufferMode
{
  enum Type
  {
    OMIT,         ///< not specified - will not be updated
    NONE,         ///< Don’t write to either color or stencil buffer (But will potentially render to depth buffer).
    AUTO,         ///< Writes are managed by the Actor Clipping API. This is DALi's default.
    COLOR,        ///< Ignore stencil properties.  Write to the color buffer.
    STENCIL,      ///< Use the stencil properties. Do not write to the color buffer.
    COLOR_STENCIL ///< Use the stencil properties AND Write to the color buffer.
  };

  BufferMode() = delete;
};

/*
 * @brief Contains values and functionality for configuring Renderers.
 */
namespace RendererState
{
using Type = uint32_t; // 8 bits reserved for flags, 4 * 4 bit for blend factors, 4 bits for depth function

enum DALI_SCENE3D_API Value : Type
{
  NONE = 0x0,

  DEPTH_WRITE = 0x01,
  DEPTH_TEST  = 0x02,

  CULL_FRONT = 0x04,
  CULL_BACK  = 0x08,

  ALPHA_BLEND = 0x10,

  DEPTH_FUNCTION_SHIFT = 6,
  DEPTH_FUNCTION_BITS  = 4,
  DEPTH_FUNCTION_MASK  = ((1 << DEPTH_FUNCTION_BITS) - 1) << DEPTH_FUNCTION_SHIFT,

  BLEND_FACTOR_BASE_SHIFT = DEPTH_FUNCTION_SHIFT + DEPTH_FUNCTION_BITS,
  BLEND_FACTOR_ITEM_BITS  = 4,
  BLEND_FACTOR_ITEMS      = 4,
  BLEND_FACTOR_BITS       = BLEND_FACTOR_ITEM_BITS * BLEND_FACTOR_ITEMS,
  BLEND_FACTOR_MASK       = ((1 << BLEND_FACTOR_BITS) - 1) << BLEND_FACTOR_BASE_SHIFT,
  BLEND_FACTOR_ITEM_MASK  = (1 << BLEND_FACTOR_ITEM_BITS) - 1, // after rshifting by BLEND_FACTOR_BASE_SHIFT

  // Buffer mode is DALi's RenderMode, just to avoid too much conflation.
  BUFFER_MODE_BITS  = 3u,
  BUFFER_MODE_SHIFT = 32u - BUFFER_MODE_BITS, // from end
  BUFFER_MODE_MASK  = ((1u << BUFFER_MODE_BITS) - 1u) << BUFFER_MODE_SHIFT,

  DEFAULT = DEPTH_WRITE | DEPTH_TEST | CULL_BACK | (Comparison::LESS_EQUAL << DEPTH_FUNCTION_SHIFT),
};

/*
 * @brief Encodes the given blend factors into a RenderMode value, maskable into other options,
 *  passable into ApplyRenderMode().
 */
inline DALI_SCENE3D_API constexpr uint32_t FromBlendFactors(BlendFactor::Type srcRgb, BlendFactor::Type destRgb, BlendFactor::Type srcAlpha, BlendFactor::Type destAlpha)
{
  return (srcRgb | (destRgb << BLEND_FACTOR_ITEM_BITS) | (srcAlpha << (BLEND_FACTOR_ITEM_BITS * 2)) |
          (destAlpha << (BLEND_FACTOR_ITEM_BITS * 3)))
         << BLEND_FACTOR_BASE_SHIFT;
}

/*
 * @brief Applies the settings encoded in @a rendererState, to a @a renderer.
 * @note Depth function is only set if not Comparison::OMIT.
 * @note Blend factors are only set if not BlendFactor::OMIT.
 * @note Buffer mode is only set is not BufferMode::OMIT.
 */
DALI_SCENE3D_API void Apply(Type rendererState, Renderer& renderer);

} // namespace RendererState

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_RENDERER_STATE_H
