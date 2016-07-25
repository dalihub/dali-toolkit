#ifndef DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H

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
#include <dali-toolkit/internal/visuals/visual-impl.h>

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
 * | %Property Name  | Type        |
 * |-----------------|-------------|
 * | mixColor        | VECTOR4     |
 */
class ColorVisual: public Visual
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  ColorVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ColorVisual();

public:  // from Visual

  /**
   * @copydoc Visual::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Visual::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc Visual::SetOffset
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc Visual::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @copydoc Visual::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Visual::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

public:

  /**
   * Set the color for rendering.
   * @param[in] color The color to be rendered.
   */
  void SetColor( const Vector4& color );

private:
  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

private:

  // Undefined
  ColorVisual( const ColorVisual& colorRenderer );

  // Undefined
  ColorVisual& operator=( const ColorVisual& colorRenderer );

private:

  Vector4 mMixColor;
  Property::Index mMixColorIndex;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H */
