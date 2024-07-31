#ifndef DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H

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
#include <dali-toolkit/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ColorVisual;
typedef IntrusivePtr<ColorVisual> ColorVisualPtr;

/**
 * The visual which renders a solid color to the control's quad
 *
 * The following properties are required for create a ColorRender
 *
 * | %Property Name  | Type        |
 * |-----------------|-------------|
 * | mixColor        | VECTOR4     |
 */
class ColorVisual : public Visual::Base
{
public:
  /**
   * @brief Create a new color visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static ColorVisualPtr New(VisualFactoryCache& factoryCache, const Property::Map& properties);

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
  ColorVisual(VisualFactoryCache& factoryCache);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ColorVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

  /**
   * @copydoc Visual::Base::GenerateShader
   */
  Shader GenerateShader() const override;

  /**
   * @copydoc Visual::Base::OnGetPropertyObject
   */
  Dali::Property OnGetPropertyObject(Dali::Property::Key key) override;

protected:
  /**
   * @brief Query whether the visual requires to be blur.
   *
   * @return Returns true if the blur is required, false otherwise.
   */
  bool IsBlurRequired() const;

  /**
   * @brief Query whether the visual requires to cutout feature.
   *
   * @return Returns true if the cutout is required, false otherwise.
   */
  bool IsCutoutRequired() const;

private:
  // Undefined
  ColorVisual(const ColorVisual& colorRenderer);

  // Undefined
  ColorVisual& operator=(const ColorVisual& colorRenderer);

private:
  float mBlurRadius; ///< The blur radius

  DevelColorVisual::CutoutPolicy::Type mCutoutPolicy : 3;          ///< The policy of cutout
  bool                                 mAlwaysUsingBlurRadius : 1; ///< Whether we need the blur radius in shader always.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_COLOR_VISUAL_H */
