#ifndef __DALI_TOOLKIT_INTERNAL_GRADIENT_RENDERER_H__
#define __DALI_TOOLKIT_INTERNAL_GRADIENT_RENDERER_H__

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
#include <dali-toolkit/internal/controls/renderers/gradient/gradient.h>

namespace Dali
{
class Vector2;

namespace Toolkit
{

namespace Internal
{

class Gradient;

/**
 * The renderer which renders smooth transition of colors to the control's quad.
 * It supports two types of gradients: linear and radial.
 *
 * The following properties are essential for create a LINEAR GradientRender
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | startPosition           | VECTOR2          |
 * | endPosition             | VECTOR2          |
 * | stopColor               | ARRAY of VECTOR4 |
 *
 * The following properties are essential for create a RADIAL GradientRender
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | center                  | VECTOR2          |
 * | radius                  | FLOAT            |
 * | stopColor               | ARRAY of VECTOR4 |
 *
 * The following properties are optional for both LINEAR and RADIAL GradientRender.
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | stopOffset              | ARRAY of FLOAT   |
 * | units                   | STRING           |
 * | spreadMethod            | STRING           |
 *
 * Valid values for units are 'userSpace' and 'objectBoundingBox'.
 * Valid values for spreadMethod are 'pad', 'repeat' and 'reflect.'
 * If not provided, 'objectBoundingBox' is used as default gradient units, and 'pad' is used as default spread method.
 */
class GradientRenderer: public ControlRenderer
{
public:

  /**
   * Types of the gradient
   */
  enum Type
  {
    LINEAR,
    RADIAL
  };

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the RendererFactoryCache object
   */
  GradientRenderer( RendererFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~GradientRenderer();

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
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc ControlRenderer::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

private:

  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

  /**
   * New a gradient object with the given property map.
   *
   * @return True if the property map provides valid properties to create a gradient. Otherwise, returns false.
   */
  bool NewGradient(Type gradientType, const Property::Map& propertyMap);

  /**
   * Get the stop-offsets from the property.
   * The valid property type are ARRAY, VECTOR2, VECTOR3, VECTOR4.
   *
   * @param[in] value The property value of stop-offsets
   * @param[out] stopOffsets The vector contains the stop offset values.
   */
  static void GetStopOffsets(const Property::Value* value, Vector<float>& stopOffsets);

  // Undefined
  GradientRenderer( const GradientRenderer& gradientRenderer );

  // Undefined
  GradientRenderer& operator=( const GradientRenderer& gradientRenderer );

private:

  Matrix3 mGradientTransform;
  IntrusivePtr<Gradient> mGradient;
  Type mGradientType;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_GRADIENT_RENDERER_H__ */
