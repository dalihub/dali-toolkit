#ifndef __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_DATA_IMPL_H__
#define __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_DATA_IMPL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>
#include <dali/devel-api/rendering/renderer.h>

#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

struct Internal::ControlRenderer::Impl
{
  struct CustomShader
  {
    std::string mVertexShader;
    std::string mFragmentShader;
    Dali::ImageDimensions mGridSize;
    Dali::Shader::ShaderHints mHints; //(bitfield) values from enum Shader::Hints

    CustomShader( const Property::Map& map );
    void SetPropertyMap( const Property::Map& map );
    void CreatePropertyMap( Property::Map& map ) const;
  };

  Geometry mGeometry;
  Shader   mShader;
  Renderer mRenderer;

  CustomShader* mCustomShader;

  Vector2   mSize;
  Vector2   mOffset;
  Rect<int> mClipRect;
  float     mDepthIndex;
  bool      mIsOnStage;

  Impl();
  ~Impl();
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_CONTROL_RENDERER_DATA_IMPL_H__ */
