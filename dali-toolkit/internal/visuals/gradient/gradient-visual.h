#ifndef DALI_TOOLKIT_INTERNAL_GRADIENT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_GRADIENT_VISUAL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/gradient/gradient.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{
class Vector2;

namespace Toolkit
{
namespace Internal
{
class Gradient;
class GradientVisual;
typedef IntrusivePtr<GradientVisual> GradientVisualPtr;

/**
 * The visual which renders smooth transition of colors to the control's quad.
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
class GradientVisual : public Visual::Base
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
   * @brief Create a new gradient visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static GradientVisualPtr New(VisualFactoryCache& factoryCache, const Property::Map& properties);

public: // from Visual
  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::EnablePreMultipliedAlpha
   */
  void EnablePreMultipliedAlpha(bool preMultiplied) override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  GradientVisual(VisualFactoryCache& factoryCache);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~GradientVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

  /**
   * @copydoc Visual::Base::GenerateShader
   */
  Shader GenerateShader() const override;

private:
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
  GradientVisual(const GradientVisual& gradientVisual);

  // Undefined
  GradientVisual& operator=(const GradientVisual& gradientVisual);

private:
  Matrix3                mGradientTransform;
  IntrusivePtr<Gradient> mGradient;
  Type                   mGradientType;
  Dali::Property::Index  mStartOffsetIndex;
  bool                   mIsOpaque; ///< Set to false if any of the stop colors are not opaque
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_GRADIENT_VISUAL_H */
