#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_EVENT_HANDLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_EVENT_HANDLER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-controller-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * Contains all the event handling methods for Text::Controller::Impl
 */
struct ControllerImplEventHandler
{
  /**
   * @brief Processes input events
   *
   * @param[in] impl A reference to Controller::Impl
   * @return True if the decorator has been updated
   */
  static bool ProcessInputEvents(Controller::Impl& impl);

  /**
   * @brief Called by Controller::Impl when a cursor key event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnCursorKeyEvent(Controller::Impl& controllerImpl, const Event& event);

  /**
   * @brief Called by Controller::Impl when a tap event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnTapEvent(Controller::Impl& controllerImpl, const Event& event);

  /**
   * @brief Called by Controller::Impl when a pan event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnPanEvent(Controller::Impl& controllerImpl, const Event& event);

  /**
   * @brief Called by Controller::Impl when a long press event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnLongPressEvent(Controller::Impl& controllerImpl, const Event& event);

  /**
   * @brief Called by Controller::Impl when a handle event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnHandleEvent(Controller::Impl& controllerImpl, const Event& event);

  /**
   * @brief Called by Controller::Impl when a select event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnSelectEvent(Controller::Impl& controllerImpl, const Event& event );

  /**
   * @brief Called by Controller::Impl when a select all event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnSelectAllEvent(Controller::Impl& controllerImpl);

  /**
   * @brief Called by Controller::Impl when a select none event is received.
   *
   * @param controllerImpl A reference to Controller::Impl
   * @param event The event
   */
  static void OnSelectNoneEvent(Controller::Impl& controllerImpl);

private:

  /**
   * @brief Called by OnHandleEvent when we are in the Pressed state.
   *
   * @param impl A reference to Controller::Impl
   * @param event The event
   * @param isSmoothHandlePanEnabled Whether smooth handle pan is enabled
   */
  static void OnHandlePressed(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled);

  /**
   * @brief Called by OnHandleEvent when we are in the Released state.
   *
   * @param impl A reference to Controller::Impl
   * @param event The event
   * @param isSmoothHandlePanEnabled Whether smooth handle pan is enabled
   * @param handleStopScrolling Whether we should handle stop scrolling or not
   */
  static void OnHandleReleased(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled, const bool handleStopScrolling);

  /**
   * @brief Called by OnHandleEvent when we are in the Scrolling state.
   *
   * @param impl A reference to Controller::Impl
   * @param event The event
   * @param isSmoothHandlePanEnabled Whether smooth handle pan is enabled
   */
  static void OnHandleScrolling(Controller::Impl& impl, const Event& event, const bool isSmoothHandlePanEnabled);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_EVENT_HANDLER_H
