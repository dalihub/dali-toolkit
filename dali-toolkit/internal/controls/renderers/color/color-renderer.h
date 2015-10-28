#ifndef __DALI_TOOLKIT_INTERNAL_COLOR_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_COLOR_RENDERER_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/renderers/control-renderer-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders a solid color to the control's quad
 *
 * The following properties are required for create a ColorRender
 *
 * | %Property Name   | Type        |
 * |------------------|-------------|
 * | blend-color      | VECTOR4     |
 */
class ColorRenderer: public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  ColorRenderer( RendererFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ColorRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc ControlRenderer::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc ControlRenderer::SetOffset
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc ControlRenderer::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @copydoc ControlRenderer::DoInitialize
   */
  virtual void DoInitialize( const Property::Map& propertyMap );

  /**
   * @copydoc ControlRenderer::InitializeRenderer
   */
  virtual void InitializeRenderer( Renderer& renderer );

public:

  /**
   * Set the color for rendering.
   * @param[in] color The color to be rendered.
   */
  void SetColor( const Vector4& color );

private:

  // Undefined
  ColorRenderer( const ColorRenderer& colorRenderer );

  // Undefined
  ColorRenderer& operator=( const ColorRenderer& colorRenderer );

private:

  Vector4 mBlendColor;
  Property::Index mBlendColorIndex;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_COLOR_RENDERER_H__ */
