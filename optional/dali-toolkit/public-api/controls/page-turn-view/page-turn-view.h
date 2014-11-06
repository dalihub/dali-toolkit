#ifndef __DALI_TOOLKIT_PAGE_TURN_VIEW_H__
#define __DALI_TOOLKIT_PAGE_TURN_VIEW_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 * PageTurnView is a base class of different mode of pageTurnViews ( portrait or landscape )
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
 */
class DALI_IMPORT_API PageTurnView : public Control
{
public:

  /**
   * Creates an empty PageTurnView handle. Only derived versions can be instantiated.
   * Calling member function with an uninitialized handle is not allowed.
   */
  PageTurnView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param[in] handle Handle to copy from
   */
  PageTurnView( const PageTurnView& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  PageTurnView& operator=( const PageTurnView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PageTurnView();

  /**
   * Downcast an Object handle to PageTurnView.
   * If handle points to an PageTurnView the downcast produces valid handle.
   * If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a PageTurnView or an uninitialized handle
   */
  static PageTurnView DownCast( BaseHandle handle );

  /**
   * Set the spine shadow parameter to the shader effects
   * The two parameters are the major&minor radius (in pixels) to form an ellipse shape
   * The top-left quarter of this ellipse is used to calculate spine normal for simulating shadow
   * @param [in] spineShadowParameter The major&minor ellipse radius for the simulated spine shadow
   */
  void SetSpineShadowParameter( const Vector2& spineShadowParameter );

  /**
   * Retrieve the spine shadow parameter of the shader effects
   * @return The major&minor ellipse radius for the simulated spine shadow
   */
  Vector2 GetSpineShadowParameter();

  /*
   * Go to a specific page
   * @param[in] pageId The new current page index
   */
  void GoToPage( unsigned int pageId );

  /**
   * Retrieve the index of the current Page
   * @return The index of the current page
   */
  unsigned int GetCurrentPage();

  /**
   * Enter edit mode
   * Case 1, the page factory passes image actor into the view as page content, do nothing.
   * Case 2, the page factory passes an actor tree into the view as page content,
   *    the actor tree will receive the touch event in edit mode, and set the refresh rate of the off screen render task to always
   * @return an empty actor in case 1; the actor tree root of the current page
   */
  Actor EnterEditMode();

  /**
   * Leave edit mode
   * Case 1, the page factory passes image actor into the view as page content, do nothing.
   * Case 2, the page factory passes an actor tree into the view as page content,
   *   the page actor will receive all the touch event, and set the refresh rage of the off screen render task to once.
   */
  void LeaveEditMode();

  /**
   * Return the actor get hit in the actor tree of the current page by given the touch position on the PageTurnView
   * @param[in] screenCoordinates The hit position of the PageTurnView
   * @param[out] actorCoordinates The local hit position of the hitted actor
   * @return the hitted actor
   */
  Actor GetHitActor( Vector2& screenCoordinates, Vector2& actorCoordinates );

  /**
   * Refresh all the cached pages by calling the render task to refresh.
   */
  void RefreshAll();

  /**
   * Refresh current page by calling the render task to refresh
   */
  void RefreshCurrentPage();

public: //Signal

  // Page Turned signal, with page index and boolean turning direction (true = forward, false = backward)
  typedef SignalV2< void ( PageTurnView, unsigned int, bool ) > PageTurnSignal;
  typedef SignalV2< void ( PageTurnView ) > PagePanSignal;

  /**
   * Signal emitted when a page has started to turn over.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );
   * @endcode
   * @return The signal to connect to
   */
  PageTurnSignal& PageTurnStartedSignal();

  /**
   * Signal emitted when a page has finished turning over.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView, unsigned int pageIndex, bool isTurningForward );
   * @endcode
   * @return The signal to connect to
   */
  PageTurnSignal& PageTurnFinishedSignal();

  /**
   * Signal emitted when a page pan has commenced
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView );
   * @endcode
   * @return The signal to connect to
   */
  PagePanSignal& PagePanStartedSignal();

  /**
   * Signal emitted when a page pan has finished
   * A callback of the following type may be connected:
   * @code
   *   void YourCallBackName( PageTurnView pageTurnView );
   * @endcode
   * @return The signal to connect to
   */
  PagePanSignal& PagePanFinishedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL PageTurnView(Internal::PageTurnView& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL PageTurnView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_PAGE_TURN_VIEW_H__ */
