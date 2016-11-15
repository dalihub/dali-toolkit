#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/devel-api/align-enums.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace Visual
{

struct Base::Impl
{
  enum Flags
  {
    IS_ON_STAGE = 1,
    IS_FROM_CACHE = 1 << 1,
    IS_ATLASING_APPLIED = 1<<2,
    IS_PREMULTIPLIED_ALPHA = 1 << 3,
    IS_SYNCHRONOUS_RESOURCE_LOADING = 1 << 4
  };

  struct CustomShader
  {
    std::string mVertexShader;
    std::string mFragmentShader;
    Dali::ImageDimensions mGridSize;
    Dali::Shader::Hint::Value mHints; //(bitfield) values from enum Shader::Hint

    CustomShader( const Property::Map& map );
    void SetPropertyMap( const Property::Map& map );
    void CreatePropertyMap( Property::Map& map ) const;
  };

  struct Transform
  {
    Vector2 mOffset;
    Vector2 mSize;
    Vector4 mOffsetSizeMode;
    Toolkit::Align::Type mOrigin;
    Toolkit::Align::Type mAnchorPoint;

    Transform();
    void SetPropertyMap( const Property::Map& map );
    void GetPropertyMap( Property::Map& map ) const;
    void RegisterUniforms( Renderer renderer, Toolkit::Direction::Type direction );
  };

  Renderer      mRenderer;
  CustomShader* mCustomShader;
  std::string   mName;
  Transform     mTransform;
  Vector2       mSize;
  float         mDepthIndex;
  int           mFlags;

  Impl();
  ~Impl();
};

} // namespace Visual

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
