#ifndef __DALI_TOOLKIT_SCROLL_BAR_H__
#define __DALI_TOOLKIT_SCROLL_BAR_H__

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
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

// Forward declarations
class ScrollConnector;

namespace Internal DALI_INTERNAL
{
// Forward declarations

class ScrollBar;
}

/**
 * ScrollBar is a UI component that can be added to the scrollable controls
 * indicating the current scroll position of the scrollable content.
 */
class ScrollBar : public Control
{
public:

  // Signals
  static const char* const SCROLL_POSITION_NOTIFIED_SIGNAL_NAME;        ///< "scroll-position-notified" signal name
  typedef SignalV2< void ( float ) > ScrollPositionNotifiedSignalType;

public:

  /**
   * @brief Create an uninitialized ScrollBar; this can be initialized with ScrollBar::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   * or horizontally (false)
   */
  ScrollBar();

  /**
   * @brief Copy constructor.
   */
  ScrollBar( const ScrollBar& scrollBar );

  /**
   * @brief Assignment operator.
   */
  ScrollBar& operator=( const ScrollBar& scrollBar );

  /**
   * @brief Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ScrollBar();

  /**
   * @brief Create an initialized ScrollBar
   * @return A pointer to the created ScrollBar.
   */
  static ScrollBar New();

  /**
   * @brief Downcast an Object handle to ScrollBar. If handle points to a ScrollBar the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ScrollBar or an uninitialized handle
   */
  static ScrollBar DownCast( BaseHandle handle );

  /**
   * @brief Sets the scroll connector for the scroll bar.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @param[in] connector Scroll connector used to connect scrollable container with this ScrollBar
   */
 void SetScrollConnector( ScrollConnector connector );

  /**
   * @brief Sets the image for the background of scroll indicator.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * The background image is resized (stretched according to scale settings),
   * to the size of the ScrollBar.
   *
   * @param[in] image The image to cover background
   * @param[in] border The nine patch border for the image.
   */
 void SetBackgroundImage( Image image, const Vector4& border );

  /**
   * @brief Sets the image for the indicator of scroll bar.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * The indicator image is resized (stretched according to scale settings),
   * to reflect the size of the scroll indicator and minimum/maximum limits
   * of the scroll position.
   *
   * @param[in] image The image of indicator that moves to indicate the current scroll position.
   * @param[in] border The nine patch border for the image.
   */
  void SetIndicatorImage( Image image, const Vector4& border );

  /**
   * @brief Gets the indicator of scroll bar.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * The indicator indicates the current scroll position of the scrollable content.
   */
  Actor GetScrollIndicator();

  /**
   * @brief Sets the values to get notification when the current scroll position of the scrollable
   * content goes above or below any of these values.
   *
   * @pre The scroll bar actor has been initialised.
   *
   * @param[in] positions List of values to receive notifications for when the current scroll position crosses them
   */
  void SetPositionNotifications( const std::vector<float>& positions );

  /**
   * @brief Shows the scroll indicator
   */
  void Show();

  /**
   * @brief Hides the scroll indicator
   */
  void Hide();

  /**
   * @brief Signal emitted when the current scroll position of the scrollable content goes above or below the values
   * specified by SetPositionNotifications.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(float currentScrollPosition);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  ScrollBar::ScrollPositionNotifiedSignalType& ScrollPositionNotifiedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ScrollBar( Internal::ScrollBar& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ScrollBar( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SCROLL_BAR_H__
