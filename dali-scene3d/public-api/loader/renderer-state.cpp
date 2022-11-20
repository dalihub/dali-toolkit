/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene3d/public-api/loader/renderer-state.h"
#include "dali-scene3d/public-api/loader/utils.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
// NOTE: values for BlendFactor aren't contiguous, hence we need a mapping.
const Dali::BlendFactor::Type kBlendFactors[] = {
  Dali::BlendFactor::ZERO,
  Dali::BlendFactor::ONE,
  Dali::BlendFactor::SRC_COLOR,
  Dali::BlendFactor::ONE_MINUS_SRC_COLOR,
  Dali::BlendFactor::SRC_ALPHA,
  Dali::BlendFactor::ONE_MINUS_SRC_ALPHA,
  Dali::BlendFactor::DST_ALPHA,
  Dali::BlendFactor::ONE_MINUS_DST_ALPHA,
  Dali::BlendFactor::DST_COLOR,
  Dali::BlendFactor::ONE_MINUS_DST_COLOR,
  Dali::BlendFactor::SRC_ALPHA_SATURATE,
  Dali::BlendFactor::CONSTANT_COLOR,
  Dali::BlendFactor::ONE_MINUS_CONSTANT_COLOR,
  Dali::BlendFactor::CONSTANT_ALPHA,
  Dali::BlendFactor::ONE_MINUS_CONSTANT_ALPHA,
};
} // namespace

namespace RendererState
{
#define RENDERER_SET_PROPERTY(name, value) renderer.SetProperty(Renderer::Property::name, (value))

void Apply(Type rendererState, Renderer& renderer)
{
  RENDERER_SET_PROPERTY(DEPTH_WRITE_MODE, MaskMatch(rendererState, DEPTH_WRITE) ? DepthWriteMode::ON : DepthWriteMode::OFF);
  RENDERER_SET_PROPERTY(DEPTH_TEST_MODE, MaskMatch(rendererState, DEPTH_TEST) ? DepthTestMode::ON : DepthTestMode::OFF);

  RENDERER_SET_PROPERTY(BLEND_MODE, MaskMatch(rendererState, ALPHA_BLEND) ? BlendMode::ON : BlendMode::AUTO);

  const bool cullBack = MaskMatch(rendererState, CULL_BACK);
  RENDERER_SET_PROPERTY(FACE_CULLING_MODE, MaskMatch(rendererState, CULL_FRONT) ? (cullBack ? FaceCullingMode::FRONT_AND_BACK : FaceCullingMode::FRONT) : (cullBack ? FaceCullingMode::BACK : FaceCullingMode::NONE));

  if(auto depthFunc = (rendererState & DEPTH_FUNCTION_MASK) >> DEPTH_FUNCTION_SHIFT)
  {
    RENDERER_SET_PROPERTY(DEPTH_FUNCTION, static_cast<DepthFunction::Type>(depthFunc - 1));
  }

  if(auto blendFactors = (rendererState & BLEND_FACTOR_MASK) >> BLEND_FACTOR_BASE_SHIFT)
  {
    if(auto srcRgb = (blendFactors & BLEND_FACTOR_ITEM_MASK))
    {
      RENDERER_SET_PROPERTY(BLEND_FACTOR_SRC_RGB, kBlendFactors[static_cast<BlendFactor::Type>(srcRgb - 1)]);
    }

    blendFactors >>= BLEND_FACTOR_ITEM_BITS;
    if(auto dstRgb = (blendFactors & BLEND_FACTOR_ITEM_MASK))
    {
      RENDERER_SET_PROPERTY(BLEND_FACTOR_DEST_RGB, kBlendFactors[static_cast<BlendFactor::Type>(dstRgb - 1)]);
    }

    blendFactors >>= BLEND_FACTOR_ITEM_BITS;
    if(auto srcAlpha = (blendFactors & BLEND_FACTOR_ITEM_MASK))
    {
      RENDERER_SET_PROPERTY(BLEND_FACTOR_SRC_ALPHA, kBlendFactors[static_cast<BlendFactor::Type>(srcAlpha - 1)]);
    }

    blendFactors >>= BLEND_FACTOR_ITEM_BITS;
    if(auto dstAlpha = (blendFactors & BLEND_FACTOR_ITEM_MASK))
    {
      RENDERER_SET_PROPERTY(BLEND_FACTOR_DEST_ALPHA, kBlendFactors[static_cast<BlendFactor::Type>(dstAlpha - 1)]);
    }
  }

  if(auto bufferMode = (rendererState & BUFFER_MODE_MASK) >> BUFFER_MODE_SHIFT)
  {
    RENDERER_SET_PROPERTY(RENDER_MODE, static_cast<RenderMode::Type>(bufferMode - 1));
  }
}

} // namespace RendererState

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
