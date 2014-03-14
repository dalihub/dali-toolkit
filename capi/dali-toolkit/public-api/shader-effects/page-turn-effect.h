#ifndef __DALI_PAGE_TURN_EFFECT_H_
#define __DALI_PAGE_TURN_EFFECT_H_

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_TOOLKIT_SHADER_EFFECTS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/**
 * @brief PageTurnEffect implementation class.
 */
class PageTurnEffect;

} // namespace Internal

/**
 * @brief PageTurnEffect is a custom shader effect to achieve page turn effect for Image actors.
 *
 * Usage example:-
 *
 * // create shader used for doing page-turn effect\n
 * PageTurnEffect pageTurnEffect = PageTurnEffect::New();
 *
 * // set image actor shader to the page-turn one\n
 * // for portrait view, one image actor for each page\n
 * // for landscape view, the page turned over is still visible, so back image is needed \n
 * //     in this case, create another image Actor using the back image and added to the page actor \n
 * ImageActor pageActor = ImageActor::New(....); \n
 * ImageActor backImageActor = ImageActor::New(....); \n
 * pageActor.Add(backPageActor);\n
 * pageActor.SetShaderEffect ( pageTurnEffect ); \n
 *
 * //set initial values
 * pageTurnEffect.SetPageSize();\n
 * pageTurnEffect.SetOriginalCenter();\n
 * pageTurnEffect.SetIsTurningBack();\n
 * pageTurnEffect.SetCurrentCenter();\n
 *
 * //Animate it with the current center property\n
 * Animation animation[mAnimationIndex] = Animation::New( ... );\n
 * animation.AnimateTo(Property( pageTurnEffect, pageTurnEffect.PageTurnEffect::GetCurrentCenterPropertyName() ),
 *                            currentCenter,
 *                            AlphaFunctions::...);\n
 * animation[mAnimationIndex].Play(); \n
 */

class PageTurnEffect : public ShaderEffect
{
public:
  /**
   * @brief Create an uninitialized PageTurnEffect; this can be initialized with PageTurnEffect::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PageTurnEffect();

  /**
   * @brief Virtual destructor.
   */
  virtual ~PageTurnEffect();

  /**
   * @brief Create an initialized PageTurnEffect.
   *
   * If fake shadow is used, need to apply the ShaderEffect::HINT_BLENDING
   * @param[in] enableBlending If true, apply HINT_BLENDING when creating the shader object; If false, disable the HINT_BLENDING
   * @return A handle to a newly allocated Dali resource.
   */
  static PageTurnEffect New( bool enableBlending = true );

  /**
   * @brief Set the page size of the PageTurnEffect.
   *
   * @param [in] pageSize The page size.
   */
  void SetPageSize(const Vector2& pageSize);

  /**
   * @brief Set the origin point of the PageTurnEffect, the position where the mouse/finger is pushed from.
   *
   * @param [in] originalCenter The new origin point.
   */
  void SetOriginalCenter(const Vector2& originalCenter);

  /**
   * @brief Set the center point of the PageTurnEffect, the current position of touch motion.
   *
   * @param [in] currentCenter The new center point.
   */
  void SetCurrentCenter(const Vector2& currentCenter);

  /**
   * @brief Set whether the current page is turning forward or backward.
   *
   * @param [in] isTurningBack True for turning backward or False for turning forward
   */
  void SetIsTurningBack(bool isTurningBack);

  /**
   * @brief Set the width of shadow to be pageSize * shadowWidth.
   *
   * @param [in] shadowWidth The width for the simulated shadow
   */
  void SetShadowWidth(float shadowWidth);

  /**
   * @brief Set the spine shadow parameter.
   *
   * The two parameters are the major&minor radius (in pixels) to form an ellipse shape
   * The top-left quarter of this ellipse is used to calculate spine normal for simulating shadow
   * @param [in] spineShadowParameter The major&minor ellipse radius for the simulated spine shadow
   */
  void SetSpineShadowParameter(const Vector2& spineShadowParameter);

  /**
   * @brief Get the name for the page size property.
   *
   * @return A std::string containing the property name
   */
  const std::string& GetPageSizePropertyName() const;

  /**
  * @brief Get the name for the origin center property.
  *
  * @return A std::string containing the property name
  */
  const std::string& GetOriginalCenterPropertyName() const;

  /**
   * @brief Get the name for the current center property.
   *
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetCurrentCenterPropertyName() const;

public: // Not intended for application developers

  PageTurnEffect( ShaderEffect handle, Internal::PageTurnEffect* shaderExtension );

};  //end of PageTurnEffect class

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif /* __DALI_TOOLKIT_SC_CURVE_EFFECT_H_ */
