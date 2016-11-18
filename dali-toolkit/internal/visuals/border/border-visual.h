#ifndef DALI_TOOLKIT_INTERNAL_BORDER_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_BORDER_VISUAL_H

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
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class BorderVisual;
typedef IntrusivePtr< BorderVisual > BorderVisualPtr;

/**
 * The visual which renders a solid color to the control's quad border fixed to a specified size.
 *
 * The following properties are required for create a BorderRender
 *
 * | %Property Name  | Type        |
 * |-----------------|-------------|
 * | borderColor     | VECTOR4     |
 * | borderSize      | FLOAT       |
 * | antiAliasing    | BOOLEAN     |
 */
class BorderVisual : public Visual::Base
{
public:

  /**
   * @brief Create a new border visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @return A smart-pointer to the newly allocated visual.
   */
  static BorderVisualPtr New( VisualFactoryCache& factoryCache );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  BorderVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~BorderVisual();

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  virtual void DoSetProperties( const Property::Map& propertyMap );

  /**
   * @copydoc Visual::Base::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  virtual void OnSetTransform();

public:

  /**
   * Set the color of the border.
   * @param[in] color The border color.
   */
  void SetBorderColor( const Vector4& color);

  /**
   * Set the size of the border.
   * @param[in] size The border size.
   */
  void SetBorderSize( float size );

  /**
   * Enable/Disable the anti-aliasing.
   * @param[in] enable Whether the anti-aliasing be enabled or not.
   */
  void RequireAntiAliasing( bool antiAliasing );

private:

  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

  /**
   * Request the border shader from the factory cache. If fail, create tha shader and add it to cache.
   * @return The border shader.
   */
  Shader GetBorderShader();

  /**
   * Create the geometry which presents the border.
   * @return The border geometry
   */
  Geometry CreateBorderGeometry();

  // Undefined
  BorderVisual( const BorderVisual& borderRenderer );

  // Undefined
  BorderVisual& operator=( const BorderVisual& borderRenderer );

private:

  Vector4 mBorderColor;
  float   mBorderSize;

  Property::Index mBorderColorIndex;
  Property::Index mBorderSizeIndex;

  bool mAntiAliasing;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BORDER_VISUAL_H
