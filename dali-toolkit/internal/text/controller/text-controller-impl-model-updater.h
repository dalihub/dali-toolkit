#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_MODEL_UPDATER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_MODEL_UPDATER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/controller/text-controller-impl.h>

namespace Dali::Toolkit::Text
{

/**
 * Contains methods for updating the models in the TextController
 */
struct ControllerImplModelUpdater
{
  using OperationsMask = Controller::OperationsMask;

  /**
   * @brief Updates the logical and visual models. Updates the style runs in the visual model when the text's styles changes.
   *
   * @param[in] impl A reference to the Controller::Impl class
   * @param[in] operationsRequired The operations required
   * @return true if mode has been modified.
   */
  static bool Update(Controller::Impl& impl, OperationsMask operationsRequired);
};

} // namespace Dali::Toolkit::Text

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_MODEL_UPDATER_H
