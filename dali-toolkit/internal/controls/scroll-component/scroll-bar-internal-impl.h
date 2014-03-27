#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_INTERNAL_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_INTERNAL_H__

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
#include <dali-toolkit/internal/controls/scroll-component/scroll-component-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scrollable-impl.h>
#include <dali-toolkit/internal/controls/scroll-component/scroll-bar-internal.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * ScrollBarInternal is a UI component that can be added to the sides of the ScrollView
 * indicating the current scroll position within the domain.
 */
class ScrollBarInternal : public ScrollComponent
{

public:

  /**
   * ScrollBarInternal constructor.
   * @param[in] container Reference to the container of scroll bar
   * @param[in] vertical Whether ScrollBarInternal should be oriented vertically (true)
   * or horizontally (false)
   */
  ScrollBarInternal(Toolkit::Scrollable& container, bool vertical);

  /**
   * Virtual destructor
   */
  virtual ~ScrollBarInternal();

  /**
   * Create an initialized ScrollBarInternal
   * @param[in] container Reference to the container of scroll bar
   * @param[in] vertical Whether ScrollBarInternal should be oriented vertically (true)
   * or horizontally (false)
   * @return A pointer to the created ScrollBarInternal.
   */
  static Toolkit::ScrollBarInternal New(Toolkit::Scrollable& container, bool vertical);

  /**
   * Show ScrollBarInternal
   */
  void Show();

  /**
   * Hide ScrollBarInternal
   */
  void Hide();

private:

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::ScrollComponent::OnDisconnect()
   */
  virtual void OnDisconnect();

  /**
   * Called when scrollbar is dragged.
   * @param[in] actor Actor under touch
   * @param[in] gesture The pan gesture data.
   */
  void OnPanGesture(Actor actor, PanGesture gesture);

  /**
   * Called when scrolling starts (scroll bars should extend out)
   * @param[in] position current scroll position.
   */
  void OnStarted(const Vector3& position);

  /**
   * Called when scrolling ends (scroll bars should contract)
   * @param[in] position current scroll position.
   */
  void OnCompleted(const Vector3& position);

  /**
   * Called when timer is finished. This time guarantee contract animation time.
   * @return Timer is used or not.
   */
  bool OnContractDelayExpired(void);

  /**
   * Create timer. This timer used for wating contract.
   */
  void CreateTimer();

  /**
   * Destruct timer.
   */
  void DestructTimer();

  /**
   * Wait ContractTime
   */
  void WaitingContractDelay();

private:
  Scrollable& mContainer;                                 ///< Container of scroll bar
  ImageActor mSlider;                                     ///< Scroll Slider.
  ImageActor mSliderWrap;                                 ///< Scroll Slider (wrapped view).
  Actor mHitArea;                                         ///< Hit Area for dragging scroll slider.
  PanGestureDetector mPanGesture;                         ///< Pan Gesture detector for dragging scrollbar.
  Animation mAnimation;                                   ///< Scroll Contract/Expand Animation.
  bool mVertical;                                         ///< Scroll Axis (Vertical or Horizontal)
  Vector3 mAxisMask;                                      ///< Scroll Axis mask (Vector3::YAXIS for vert. or Vector3::XAXIS for horiz.)
  Vector3 mScrollStart;                                   ///< Scroll Start position (start of drag)
  Vector3 mGestureDisplacement;                           ///< Gesture Displacement.
  bool mDragMode;                                         ///< Flag indicating whether currently dragging or not.
  Timer mTimer;                                           ///< Timer guarantee contract delay time.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ScrollBarInternal& GetImpl(Toolkit::ScrollBarInternal& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<Toolkit::Internal::ScrollBarInternal&>(handle);
}

inline const Toolkit::Internal::ScrollBarInternal& GetImpl(const Toolkit::ScrollBarInternal& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  const Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<const Toolkit::Internal::ScrollBarInternal&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_INTERNAL_H__
