#ifndef __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-component-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-connector.h>
#include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ScrollBar;

typedef IntrusivePtr<ScrollBar> ScrollBarPtr;

/**
 * ScrollBar is a UI component that can be added to the scrollable controls
 * indicating the current scroll position of the scrollable content.
 */
class ScrollBar : public ScrollComponentImpl
{

public:

  typedef Toolkit::ScrollBar::ScrollPositionNotifiedSignalType ScrollPositionNotifiedSignalType;

public:

  /**
   * @copydoc Toolkit::ScrollBar::ScrollBar()
   */
  ScrollBar();

  /**
   * @copydoc Toolkit::ScrollBar::~ScrollBar()
   */
  virtual ~ScrollBar();

  /**
   * @copydoc Toolkit::ScrollBar::New()
   */
  static Toolkit::ScrollBar New();

  /**
   * @copydoc Toolkit::ScrollComponentImpl::OnScrollConnectorSet()
   */
  void OnScrollConnectorSet( Toolkit::ScrollConnector connector );

  /**
   * @copydoc Toolkit::ScrollBar::SetBackgroundImage()
   */
  void SetBackgroundImage( Image image, const Vector4& border );

  /**
   * @copydoc Toolkit::ScrollBar::SetIndicatorImage()
   */
  void SetIndicatorImage( Image image, const Vector4& border );

  /**
   * @copydoc Toolkit::ScrollBar::GetScrollIndicator()
   */
  Actor GetScrollIndicator();

  /**
   * @copydoc Toolkit::ScrollBar::SetPositionNotifications()
   */
  void SetPositionNotifications( const std::vector<float>& positions );

  /**
   * @copydoc Toolkit::ScrollBar::Show()
   */
  void Show();

  /**
   * @copydoc Toolkit::ScrollBar::Hide()
   */
 void Hide();

 /**
  * Signal emitted after the SetScrollDomain() method has been called.
  */
 ScrollPositionNotifiedSignalType& ScrollPositionNotifiedSignal()
 {
   return mScrollPositionNotifiedSignal;
 }

private: // from ControlImpl

  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Control::OnPan
   */
  virtual void OnPan( PanGesture gesture );

private:

  /**
   * Apply constraints for background and indicator.
   * These constraints are based on values from the scroll connector.
   */
  void ApplyConstraints();

  /**
   * Callback when the start & end position and size of the scrollable content are changed.
   * @param[in] minPosition The minimum position.
   * @param[in] maxPosition The maximum position.
   * @param[in] contentSize The size of scrollable content.
   */
  void OnScrollDomainChanged(float minPosition, float maxPosition, float contentSize);

  /**
   * Callback when the current scroll position of the scrollable content goes above or
   * below the values specified by SetPositionNotifications().
   * @param[in] source the property notification that triggered this callback
   */
  void OnScrollPositionNotified(PropertyNotification& source);

private:

  Constrainable mScrollPositionObject;                    ///< From mScrollConnector

  ImageActor mBackground;                                 ///< Background image of scroll bar.
  ImageActor mIndicator;                                  ///< Image of scroll indicator.
  Animation mAnimation;                                   ///< Scroll indicator Show/Hide Animation.

  float mScrollStart;                                     ///< Scroll Start position (start of drag)
  Vector3 mGestureDisplacement;                           ///< Gesture Displacement.

  Property::Index mPropertyIndicatorPosition;             ///< Indicatore Position ("indicator-position")

  PropertyNotification mPositionNotification;             ///< Stores the property notification used for scroll position changes

  ScrollPositionNotifiedSignalType mScrollPositionNotifiedSignal;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ScrollBar& GetImpl(Toolkit::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<Toolkit::Internal::ScrollBar&>(handle);
}

inline const Toolkit::Internal::ScrollBar& GetImpl(const Toolkit::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  const Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<const Toolkit::Internal::ScrollBar&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCROLL_BAR_H__
