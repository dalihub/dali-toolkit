#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CUSTOM_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CUSTOM_EFFECT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-custom-effect.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>

namespace Dali
{

class Animation;

namespace Toolkit
{

class ScrollGroup;
class ScrollView;

namespace Internal
{

/**
 * @copydoc Toolkit::ScrollViewCustomEffect
 */
class ScrollViewCustomEffect : public ScrollViewEffect
{
  static const std::string SCROLL_AMOUNT_PROPERTY_STRING;
  static const std::string ANCHOR_POINT_PROPERTY_STRING;

public:

  /**
   * Constructor
   */
  ScrollViewCustomEffect();

public:

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
  void SetSwingAngle(const float angle, const Vector3& axis);

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
   * @brief SetSwingAnchor
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
   * ApplyToPage This is the full internal ApplyToPage function and ALL other ApplyToPage
   * functions should call this one. Making this internal allows us to change this function
   * in the future without affecting demo apps
   * @param page
   * @param pageSize
   */
  void ApplyToPage( Actor page, Vector3 pageSize);

  /**
   * @copydoc ScrollViewEffect::OnAttach
   */
  virtual void OnAttach( Toolkit::ScrollView& scrollView );

  /**
   * @copydoc ScrollViewEffect::OnDetach
   */
  virtual void OnDetach( Toolkit::ScrollView& scrollView );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollViewCustomEffect();

private:

  Vector3 mPageSize; ///< The logical page size for the 3D effect.

  uint          mFlags;                         ///< flags describing functionality, set automatically depending on functions called during effect setup
  Vector2       mPageSpacing;                   ///< space between pages... kinda obvious really
  Vector3       mTranslateIn;                   ///< translation offset to use when scrolling a page onto the screen
  Vector3       mTranslateOut;                  ///< translation offset to use when scrolling a page off the screen
  Quaternion    mGlobalRotateIn;                ///< rotates the page's position around a point
  Quaternion    mGlobalRotateOut;               ///< rotates the page's position around a point
  Vector3       mGlobalOriginIn;                ///< the point to rotate a page around when scrolling onto screen
  Vector3       mGlobalOriginOut;               ///< the point to rotate a page around when scrolling off screen
  float         mSwingAngleIn;                  ///< angle to rotate a page around its anchor when scrolling onto screen
  Vector3       mSwingAxisIn;
  float         mSwingAngleOut;                 ///< angle to rotate a page around its anchor when scrolling off screen
  Vector3       mSwingAxisOut;
  Vector3       mSwingAnchorIn;                 ///< the page anchor point to use when scrolling onto screen
  Vector3       mSwingAnchorOut;                ///< the page anchor point to use when scrolling off screen
  float         mOpacityThresholdIn;            ///< the point at which opacity will change as page scrolls onto screen
  float         mOpacityThresholdOut;           ///< the point at which opacity will change as page scrolls off screen
  AlphaFunction mGlobalRotateAlphaFunctionIn;
  AlphaFunction mGlobalRotateAlphaFunctionOut;
  AlphaFunction mSwingAlphaFunctionIn;
  AlphaFunction mSwingAlphaFunctionOut;
  AlphaFunction mSwingAnchorAlphaFunctionIn;
  AlphaFunction mSwingAnchorAlphaFunctionOut;
  AlphaFunction mTranslateAlphaFunctionIn;
  AlphaFunction mTranslateAlphaFunctionOut;
  AlphaFunction mOpacityAlphaFunctionIn;
  AlphaFunction mOpacityAlphaFunctionOut;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewCustomEffect& GetImpl(Dali::Toolkit::ScrollViewCustomEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewCustomEffect&>(handle);
}

inline const Internal::ScrollViewCustomEffect& GetImpl(const Dali::Toolkit::ScrollViewCustomEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewCustomEffect&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CUSTOM_EFFECT_H__
