#ifndef __DALI_TOOLKIT_SCROLL_VIEW_CUSTOM_EFFECT_H__
#define __DALI_TOOLKIT_SCROLL_VIEW_CUSTOM_EFFECT_H__

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
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>

namespace Dali DALI_IMPORT_API
{

class Actor;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ScrollViewCustomEffect;
}

/**
 * ScrollView Inner Cube-Effect.
 *
 * This effect cause each page in a scroll-view to rotate round an inner 3D cube.
 * It should be used on the following Actor hierarchy:
 *
 * ScrollView
 * |
 * Page (1..n)
 *
 * You should ensure ScrollView's default constraints have been removed,
 * by calling ScrollView::RemoveConstraintsFromChildren() before applying
 * this effect to ScrollView.
 *
 * Manual operation:
 * ApplyToPage(...) method should be called on every page.
 *
 * Automatic operation:
 * not implemented.
 */
class ScrollViewCustomEffect : public ScrollViewEffect
{
public:
  enum EFlag
  {
    FlagTranslate             = 0x0001,             ///< indicates that translation is wanted
    FlagTranslateIn           = 0x0002,             ///< translating onto the screen is a separate value
    FlagTranslateOut          = 0x0004,             ///< translating off the screen is a separate value
    FlagTranslateMask         = FlagTranslate | FlagTranslateIn | FlagTranslateOut,
    FlagRotate                = 0x0008,             ///< indicates that a positional rotation is wanted (rotate all pages around a single point like inner cube effect)
    FlagRotateIn              = 0x0010,             ///< rotating onto the screen is a separate value
    FlagRotateOut             = 0x0020,             ///< rotating off the screen is a separate value
    FlagRotateAngleForcedOrigin = 0x0040,
    FlagRotateMask            = FlagRotate | FlagRotateIn | FlagRotateOut | FlagRotateAngleForcedOrigin,
    FlagRotateOrigin          = 0x0080,             ///< indicates to use a global origin to rotate all pages around
    FlagRotateOriginIn        = 0x0100,             ///<
    FlagRotateOriginOut       = 0x0200,             ///<
    FlagRotateOriginMask      = FlagRotateOrigin | FlagRotateOriginIn | FlagRotateOriginOut,
    FlagSwingAngle            = 0x0400,             ///< indicates that a SwingAngle is wanted (rotate all pages around a single point like inner cube effect)
    FlagSwingAngleIn          = 0x0800,             ///< SwingAngle onto the screen is a separate value
    FlagSwingAngleOut         = 0x1000,             ///< SwingAngle off the screen is a separate value
    FlagSwingAngleMask        = FlagSwingAngle | FlagSwingAngleIn | FlagSwingAngleOut,
    FlagSwingAnchor           = 0x2000,             ///< indicates that a swing requires a specified anchor point, otherwise swings around centre of actor (rotate all pages around a single point like inner cube effect)
    FlagSwingAnchorIn         = 0x4000,             ///< Swing anchor onto the screen is a separate value
    FlagSwingAnchorOut        = 0x8000,             ///< Swing anchor off the screen is a separate value
    FlagSwingAnchorMask       = FlagSwingAnchor | FlagSwingAnchorIn | FlagSwingAnchorOut,
    FlagOpacityThreshold      = 0x00010000,             ///<
    FlagOpacityThresholdIn    = 0x00020000,       ///<
    FlagOpacityThresholdOut   = 0x00040000,      ///<
    FlagOpacityThresholdMask  = FlagOpacityThreshold | FlagOpacityThresholdIn | FlagOpacityThresholdOut,
    FlagTranslationAlphaFunctionIn      = 0x00080000,
    FlagTranslationAlphaFunctionOut     = 0x00100000,
    FlagTranslationAlphaFunctionMask    = FlagTranslationAlphaFunctionIn | FlagTranslationAlphaFunctionOut,
    FlagRotateAlphaFunctionIn           = 0x00200000,
    FlagRotateAlphaFunctionOut          = 0x00400000,
    FlagRotateAlphaFunctionMask         = FlagRotateAlphaFunctionIn | FlagRotateAlphaFunctionOut,
    FlagRotateOriginAlphaFunctionIn     = 0x00800000,
    FlagRotateOriginAlphaFunctionOut    = 0x01000000,
    FlagRotateOriginAlphaFunctionMask   = FlagRotateOriginAlphaFunctionIn | FlagRotateOriginAlphaFunctionOut,
    FlagSwingAngleAlphaFunctionIn       = 0x02000000,
    FlagSwingAngleAlphaFunctionOut      = 0x04000000,
    FlagSwingAngleAlphaFunctionMask     = FlagSwingAngleAlphaFunctionIn | FlagSwingAngleAlphaFunctionOut,
    FlagSwingAnchorAlphaFunctionIn      = 0x08000000,
    FlagSwingAnchorAlphaFunctionOut     = 0x10000000,
    FlagSwingAnchorAlphaFunctionMask    = FlagSwingAnchorAlphaFunctionIn | FlagSwingAnchorAlphaFunctionOut,
    FlagOpacityAlphaFunctionIn          = 0x20000000,
    FlagOpacityAlphaFunctionOut         = 0x40000000,
    FlagOpacityAlphaFunctionMask        = FlagOpacityAlphaFunctionIn | FlagOpacityAlphaFunctionOut
  };

  /**
   * Create an initialized ScrollViewPageCubeEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ScrollViewCustomEffect New();

  /**
   * Create an uninitialized ScrollViewPageCubeEffect; this can be initialized with ScrollViewPageCubeEffect::New()
   * Calling member functions with an uninitialized Toolkit::ScrollViewPageCubeEffect is not allowed.
   */
  ScrollViewCustomEffect();

  /**
   * Downcast an Object handle to ScrollViewCustomEffect. If handle points to a ScrollViewCustomEffect the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollViewCustomEffect or an uninitialized handle
   */
  static ScrollViewCustomEffect DownCast( BaseHandle handle );

  /**
   * @brief SetPageSpacing
   * @param spacing
   */
  void SetPageSpacing(const Vector2& spacing);

  /**
   * @brief SetPageTranslation sets a simple translate on/off value
   * @param translation
   */
  void SetPageTranslation(const Vector3& translation);

  /**
   * @brief SetPageTranslation
   * @param translationIn
   * @param translationOut
   */
  void SetPageTranslation(const Vector3& translationIn, const Vector3& translationOut);

  /**
   * @brief SetPageTranslationIn
   * @param translation
   */
  void SetPageTranslationIn(const Vector3& translation);

  /**
   * @brief SetPageTranslationOut
   * @param translation
   */
  void SetPageTranslationOut(const Vector3& translation);

  /**
   * @brief SetPageTranslateAlphaFunction
   * @param func
   */
  void SetPageTranslateAlphaFunction(AlphaFunction func);

  /**
   * @brief SetPageTranslateAlphaFunction
   * @param funcIn
   * @param funcOut
   */
  void SetPageTranslateAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut);

  /**
   * @brief SetPageTranslateAlphaFunctionIn
   * @param func
   */
  void SetPageTranslateAlphaFunctionIn(AlphaFunction func);

  /**
   * @brief SetPageTranslateAlphaFunctionOut
   * @param func
   */
  void SetPageTranslateAlphaFunctionOut(AlphaFunction func);

  /**
   * @brief SetGlobalPageRotation
   * @param angle
   * @param axis
   */
  void SetGlobalPageRotation(float angle, const Vector3& axis);

  /**
   * @brief SetAnglePageRotation uses the angle and page size passed in on creation to create a faked origin (inner cube needs this method)
   * @param angle
   */
  void SetAngledOriginPageRotation(const Vector3& angle);

  /**
   * @brief SetGlobalPageRotation
   * @param angleIn
   * @param axisIn
   * @param angleOut
   * @param axisOut
   */
  void SetGlobalPageRotation(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut);

  /**
   * @brief SetGlobalPageRotationIn
   * @param angle
   * @param axis
   */
  void SetGlobalPageRotationIn(float angle, const Vector3& axis);

  /**
   * @brief SetGlobalPageRotationOut
   * @param angle
   * @param axis
   */
  void SetGlobalPageRotationOut(float angle, const Vector3& axis);

  /**
   * @brief SetPageRotationOrigin Set the origin to rotate all the pages around
   *        - The default value is (0,0,0)
   * @param origin
   */
  void SetGlobalPageRotationOrigin(const Vector3& origin);

  /**
   * @brief SetGlobalPageRotationOrigin
   * @param originIn
   * @param originOut
   */
  void SetGlobalPageRotationOrigin(const Vector3& originIn, const Vector3& originOut);

  /**
   * @brief SetGlobalPageRotationOriginIn
   * @param origin
   */
  void SetGlobalPageRotationOriginIn(const Vector3& origin);

  /**
   * @brief SetGlobalPageRotationOriginOut
   * @param origin
   */
  void SetGlobalPageRotationOriginOut(const Vector3& origin);

  /**
   * @brief SetSwingAngle
   * @param angle
   * @param axis
   */
  void SetSwingAngle(float angle, const Vector3& axis);

  /**
   * @brief SetSwingAngle
   * @param angleIn
   * @param axisIn
   * @param angleOut
   * @param axisOut
   */
  void SetSwingAngle(float angleIn, const Vector3& axisIn, float angleOut, const Vector3& axisOut);

  /**
   * @brief SetSwingAngleIn
   * @param angle
   * @param axis
   */
  void SetSwingAngleIn(float angle, const Vector3& axis);

  /**
   * @brief SetSwingAngleOut
   * @param angle
   * @param axis
   */
  void SetSwingAngleOut(float angle, const Vector3& axis);

  /**
   * @brief SetSwingAngleAlphaFunction
   * @param func
   */
  void SetSwingAngleAlphaFunction(AlphaFunction func);

  /**
   * @brief SetSwingAngleAlphaFunction
   * @param funcIn
   * @param funcOut
   */
  void SetSwingAngleAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut);

  /**
   * @brief SetSwingAngleAlphaFunctionIn
   * @param func
   */
  void SetSwingAngleAlphaFunctionIn(AlphaFunction func);

  /**
   * @brief SetSwingAngleAlphaFunctionOut
   * @param func
   */
  void SetSwingAngleAlphaFunctionOut(AlphaFunction func);

  /**
   * @brief SetPageRotationOrigin Set the origin to rotate all the pages around
   *        - The default value is (0,0,0)
   * @param anchor
   */
  void SetSwingAnchor(const Vector3& anchor);

  /**
   * @brief SetSwingAnchor
   * @param anchorIn
   * @param anchorOut
   */
  void SetSwingAnchor(const Vector3& anchorIn, const Vector3& anchorOut);

  /**
   * @brief SetSwingAnchorIn
   * @param anchor
   */
  void SetSwingAnchorIn(const Vector3& anchor);

  /**
   * @brief SetSwingAnchorOut
   * @param anchor
   */
  void SetSwingAnchorOut(const Vector3& anchor);

  /**
   * @brief SetSwingAnchorAlphaFunction
   * @param func
   */
  void SetSwingAnchorAlphaFunction(AlphaFunction func);

  /**
   * @brief SetSwingAnchorAlphaFunction
   * @param funcIn
   * @param funcOut
   */
  void SetSwingAnchorAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut);

  /**
   * @brief SetSwingAnchorAlphaFunctionIn
   * @param func
   */
  void SetSwingAnchorAlphaFunctionIn(AlphaFunction func);

  /**
   * @brief SetSwingAnchorAlphaFunctionOut
   * @param func
   */
  void SetSwingAnchorAlphaFunctionOut(AlphaFunction func);

  /**
   * @brief SetOpacityThreshold
   * @param thresh
   */
  void SetOpacityThreshold(float thresh);

  /**
   * @brief SetOpacityThreshold
   * @param threshIn
   * @param threshOut
   */
  void SetOpacityThreshold(float threshIn, float threshOut);

  /**
   * @brief SetOpacityThresholdIn
   * @param thresh
   */
  void SetOpacityThresholdIn(float thresh);

  /**
   * @brief SetOpacityThresholdOut
   * @param thresh
   */
  void SetOpacityThresholdOut(float thresh);

  /**
   * @brief SetOpacityAlphaFunction
   * @param func
   */
  void SetOpacityAlphaFunction(AlphaFunction func);

  /**
   * @brief SetOpacityAlphaFunction
   * @param funcIn
   * @param funcOut
   */
  void SetOpacityAlphaFunction(AlphaFunction funcIn, AlphaFunction funcOut);

  /**
   * @brief SetOpacityAlphaFunctionIn
   * @param func
   */
  void SetOpacityAlphaFunctionIn(AlphaFunction func);

  /**
   * @brief SetOpacityAlphaFunctionOut
   * @param func
   */
  void SetOpacityAlphaFunctionOut(AlphaFunction func);

  /**
   * Applies the effect to a page
   * @param page the page to apply this effect to
   * @param pageSize not needed, page size is determined by scroll view

   */
  void ApplyToPage(Actor page, Vector3 pageSize);

protected:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] impl A pointer to a newly allocated Dali resource
   */
  ScrollViewCustomEffect( Internal::ScrollViewCustomEffect *impl );

};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_VIEW_CUSTOM_EFFECT_H__
