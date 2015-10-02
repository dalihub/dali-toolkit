#ifndef __DALI_TOOLKIT_INTERNAL_BORDER_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_BORDER_RENDERER_H__

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
#include <dali/devel-api/rendering/geometry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders a solid color to the control's quad border fixed to a specified size.
 *
 * The following properties are required for create a BorderRender
 *
 * | %Property Name   | Type        |
 * |------------------|-------------|
 * | border-color     | VECTOR4     |
 * | border-size      | FLOAT       |
 */

class BorderRenderer : public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   */
  BorderRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~BorderRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::DoInitialize
   */
  virtual void DoInitialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap );

  /**
   * @copydoc ControlRenderer::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

protected:
  /**
   * @copydoc ControlRenderer::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @copydoc ControlRenderer::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

public:

  /**
   * Request the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  void Initialize( RendererFactoryCache& factoryCache );

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

private:

  /**
   * Create the geometry which presents the border.
   * @return The border geometry
   */
  Geometry CreateBorderGeometry();

  // Undefined
  BorderRenderer( const BorderRenderer& borderRenderer );

  // Undefined
  BorderRenderer& operator=( const BorderRenderer& borderRenderer );

private:

  Vector4 mBorderColor;
  float   mBorderSize;

  Property::Index mBorderColorIndex;
  Property::Index mBorderSizeIndex;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_BORDER_RENDERER_H__ */
