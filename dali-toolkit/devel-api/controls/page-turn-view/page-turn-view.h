#ifndef __DALI_TOOLKIT_PAGE_TURN_VIEW_H__
#define __DALI_TOOLKIT_PAGE_TURN_VIEW_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

// Forward declarations
class PageFactory;

namespace Internal DALI_INTERNAL
{
class PageTurnView;
}

/**
 * @brief PageTurnView is a base class of different mode of pageTurnViews ( portrait or landscape )
 *
 * Page actors are provided from an external PageFactory
 * PanGesture is used to activate the page bending, streching and tuning forward/backward
 *
 * Signal usage: There are four signals. Two matching pairs for panning and page turning:
 * PagePanStarted/PagePanFinished and PageTurnStarted/PageTurnFinished. Panning relates to user interaction with
 * the screen while page turning refers to animation of the page. There are three scenarios for these
 * events: normal page turn (forwards or backwards), aborted page turn (forwards or backwards)
 * and pan with no animation. The order of events is as follows:
 * 1) Normal page turn: PagePanStarted -> PageTurnStarted direction -> PagePanFinished -> PageTurnFinished direction
 * 2) Aborted page turn: PagePanStarted -> PageTurnStarted direction -> PageTurnStarted opposite direction
 *                       -> PagePanFinished -> PageTurnFinished opposite direction
 * 3) Pan with no animation: PagePanStarted -> PagePanFinished
 * Pan with no animation will occur when the user touches the page in an area that does not start the
 * page turning.
 *
 *  Signals
 * | %Signal Name     | Method                        |
 * |------------------|-------------------------------|
 * | pageTurnStarted  | @ref PageTurnStartedSignal()  |
 * | pageTurnFinished | @ref PageTurnFinishedSignal() |
 * | pagePanStarted   | @ref PagePanStartedSignal()   |
 * | pagePanFinished  | @ref PagePanFinishedSignal()  |
 *
 * @SINCE_1_1.4
 */
class DALI_IMPORT_API PageTurnView : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_1.4
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_1.4
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_1.4
  };

  struct Property
  {
    enum
    {
      PAGE_SIZE = PROPERTY_START_INDEX, ///< name "pageSize",        type Vector2 @SINCE_1_1.4
      CURRENT_PAGE_ID,                  ///< name "currentPageId",   type Integer @SINCE_1_1.4

      /**
       * The two values are the major&minor radius (in pixels) to form an ellipse shape.
       * The top-left quarter of this ellipse is used to calculate spine normal for simulating shadow.
       */
      SPINE_SHADOW,                     ///< name "spineShadow",     type Vector2 @SINCE_1_1.4
    };
  };

  /**
   * @brief Creates an empty PageTurnView handle. Only derived versions can be instantiated.
   * Calling member function with an uninitialized handle is not allowed.
   * @SINCE_1_1.4
   */
  PageTurnView();

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object
   * @SINCE_1_1.4
   * @param[in] handle Handle to copy from
   */
  PageTurnView( const PageTurnView& handle );

  /**
   * @brief Assignment operator. Changes this handle to point to another real object
   * @SINCE_1_1.4
   */
  PageTurnView& operator=( const PageTurnView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_1.4
   */
  ~PageTurnView();

  /**
   * @brief Downcast an Object handle to PageTurnView.
   * If handle points to an PageTurnView the downcast produces valid handle.
   * If not the returned handle is left uninitialized.
   * @SINCE_1_1.4
   * @param[in] handle Handle to an object
   * @return handle to a PageTurnView or an uninitialized handle
   */
  static PageTurnView DownCast( BaseHandle handle );

public: //Signal

  // Page Turned signal, with page index and boolean turning direction (true = forward, false = backward)
  typedef Signal< void ( PageTurnView, unsigned int, bool ) > PageTurnSignal;
  typedef Signal< void ( PageTurnView ) > PagePanSignal;

  /**
   * @brief Signal emitted when a page has started to turn over.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );
   * @endcode
   * @SINCE_1_1.4
   * @return The signal to connect to
   */
  PageTurnSignal& PageTurnStartedSignal();

  /**
   * @brief Signal emitted when a page has finished turning over.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );
   * @endcode
   * @SINCE_1_1.4
   * @return The signal to connect to
   */
  PageTurnSignal& PageTurnFinishedSignal();

  /**
   * @brief Signal emitted when a page pan has commenced
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView );
   * @endcode
   * @SINCE_1_1.4
   * @return The signal to connect to
   */
  PagePanSignal& PagePanStartedSignal();

  /**
   * @brief Signal emitted when a page pan has finished
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView );
   * @endcode
   * @SINCE_1_1.4
   * @return The signal to connect to
   */
  PagePanSignal& PagePanFinishedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_1_1.4
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL PageTurnView(Internal::PageTurnView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @SINCE_1_1.4
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL PageTurnView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_PAGE_TURN_VIEW_H__ */
