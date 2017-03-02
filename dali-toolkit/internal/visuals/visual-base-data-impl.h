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
  /**
   * Constructor
   */
  Impl();

  /**
   * Destructor
   */
  ~Impl();

  enum Flags
  {
    IS_ON_STAGE = 1,
    IS_ATLASING_APPLIED = 1<<1,
    IS_PREMULTIPLIED_ALPHA = 1 << 2,
    IS_SYNCHRONOUS_RESOURCE_LOADING = 1 << 3
  };

  struct CustomShader
  {
    CustomShader( const Property::Map& map );
    void SetPropertyMap( const Property::Map& map );
    void CreatePropertyMap( Property::Map& map ) const;

    std::string mVertexShader;
    std::string mFragmentShader;
    Dali::ImageDimensions mGridSize;
    Dali::Shader::Hint::Value mHints; //(bitfield) values from enum Shader::Hint
  };

  struct Transform
  {
    /**
     * Default constructor ensures the visual fills the control
     */
    Transform();

    /**
     * Use the property map to set zero or more of the transform
     * attributes, and sets the remaining attributes to their default
     * values.
     */
    void SetPropertyMap( const Property::Map& map );

    /**
     * Add the transform attributes to the map (using integer keys)
     */
    void GetPropertyMap( Property::Map& map ) const;

    /**
     * Update zero or more attributes from the property map.
     */
    void UpdatePropertyMap( const Property::Map& map );

    /**
     * Register or set the uniform properties onto the renderer
     */
    void RegisterUniforms( Renderer renderer, Toolkit::Direction::Type direction );

    /**
     * Convert the control size and the transform attributes into the actual
     * size of the visual.
     */
    Vector2 GetVisualSize( const Vector2& controlSize );

    Vector2 mOffset;
    Vector2 mSize;
    Vector4 mOffsetSizeMode;
    Toolkit::Align::Type mOrigin;
    Toolkit::Align::Type mAnchorPoint;
  };

  Renderer        mRenderer;
  CustomShader*   mCustomShader;
  SlotDelegate<Visual::Base>* mBlendSlotDelegate; ///< Used to own mix color animation connection
  std::string     mName;
  Transform       mTransform;
  Vector4         mMixColor;
  Size            mControlSize;
  float           mDepthIndex;
  Property::Index mMixColorIndex;
  Property::Index mOpacityIndex;
  int             mFlags;
};

} // namespace Visual

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_BASE_DATA_IMPL_H
