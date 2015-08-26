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
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | gradient-start-position   | VECTOR2          |
 * | gradient-end-position     | VECTOR2          |
 * | gradient-stop-offset      | ARRAY of FLOAT   |
 * | gradient-stop-color       | ARRAY of VECTOR4 |
 *
 * The following properties are essential for create a RADIAL GradientRender
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | gradient-center           | VECTOR2          |
 * | gradient-radius           | FLOAT            |
 * | gradient-stop-offset      | ARRAY of FLOAT   |
 * | gradient-stop-color       | ARRAY of VECTOR4 |
 *
 * The following properties are optional for both LINEAR and RADIAL GradientRender.
 *
 * | %Property Name            | Type             |
 * |---------------------------|------------------|
 * | gradient-units            | STRING           |
 * | gradient-spread-method    | STRING           |
 *
 * Valid values for gradient-units are 'user-space' and 'object-bounding-box'.
 * Valid values for gradient-spread-method are 'pad', 'repeat' and 'reflect.'
 * If not provided, 'objectBoundingBox' is used as default gradient units, and 'pad' is used as default spread method.
 */
class GradientRenderer: public ControlRenderer
{
public:

  /**
   * @brief Constructor.
   */
  GradientRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~GradientRenderer();

public:  // from ControlRenderer

  /**
   * @copydoc ControlRenderer::Initialize
   */
  virtual void Initialize( RendererFactoryCache& factoryCache, const Property::Map& propertyMap );

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
   * @copydoc ControlRenderer::SetOnStage
   */
  virtual void SetOnStage( Actor& actor );

private:

  /**
   * Types of the gradient
   */
  enum Type
  {
    LINEAR,
    RADIAL
  };

  /**
   * New a gradient object with the given property map.
   *
   * @return True if the property map provides valid properties to create a gradient. Otherwise, returns false.
   */
  bool NewGradient(Type gradientType, const Property::Map& propertyMap);

  // Undefined
  GradientRenderer( const GradientRenderer& gradientRenderer );

  // Undefined
  GradientRenderer& operator=( const GradientRenderer& gradientRenderer );

private:

  Matrix3 mGradientTransform;
  Property::Index mGradientTransformIndex;
  IntrusivePtr<Gradient> mGradient;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_GRADIENT_RENDERER_H__ */
