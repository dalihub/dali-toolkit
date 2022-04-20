#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_RELAYOUTER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_RELAYOUTER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * Contains all the relayouting related methods for Text::Controller
 */
struct Controller::Relayouter
{
  /**
   * @brief Called by the Controller to retrieve the natural size.
   *
   * @param[in] controller A reference to the controller class
   * @return
   */
  static Vector3 GetNaturalSize(Controller& controller);

  /**
   * @brief Called by the Controller to check if the text fits.
   *
   * @param[in] controller A reference to the controller class
   * @param[in] pointSize The point size
   * @param[in] layoutSize The layout size
   * @return
   */
  static bool CheckForTextFit(Controller& controller, float pointSize, const Size& layoutSize);

  /**
   * @brief Calculates the point size for text for given layout()
   *
   * @param[in] controller A reference to the controller class
   * @param[in] layoutSize The layout size
   */
  static void FitPointSizeforLayout(Controller& controller, const Size& layoutSize);

  /**
   * @brief Called by the Controller to get the height for a particular width.
   *
   * @param[in] controller A reference to the controller class
   * @param[in] width The width we want the height for
   * @return
   */
  static float GetHeightForWidth(Controller& controller, float width);

  /**
   * @brief Called by the Controller to do the relayout itself.
   *
   * @param[in] controller A reference to the controller class
   * @param[in] size The size to set
   * @param[in] layoutDirection The layout direction
   * @return
   */
  static Controller::UpdateTextType Relayout(Controller& controller, const Size& size, Dali::LayoutDirection::Type layoutDirection);

  /**
   * @brief Called by the Controller to do certain operations when relayouting.
   *
   * @param[in] impl A reference to the controller impl class
   * @param[in] size The size to set
   * @param[in] operationsRequired The operations we need to do
   * @param[in/out] layoutSize The Layout size which can be updated depending on the result of the performed operations
   * @return
   */

  static bool DoRelayout(Controller::Impl& impl, const Size& size, OperationsMask operationsRequired, Size& layoutSize);

  /**
   * @brief Called by the Controller to calculate the veritcal offset give the control size.
   *
   * @param[in] impl A reference to the controller impl class
   * @param[in] controlSize The control size
   */
  static void CalculateVerticalOffset(Controller::Impl& impl, const Size& controlSize);

  /**
  * @brief Calculates the layout size of control according to @p requestedControllerSize and @p requestedOperationsMask
  *
  * GetNaturalSize() and GetHeightForWidth() calls this method.
  *
  * @param[in] controller The controller to calcualte size on it.
  * @param[in] requestedControllerSize The requested size of controller to calcualte layout size on it.
  * @param[in] requestedOperationsMask The requested operations-mask to calcualte layout size according to it.
  *
  * @return The calculated layout-size.
  */
  static Size CalculateLayoutSizeOnRequiredControllerSize(Controller& controller, const Size& requestedControllerSize, const OperationsMask& requestedOperationsMask);

private:
  /**
   * @brief Called by the DoRelayout to do HorizontalAlignment operation when relayouting.
   *
   * @param[in] impl A reference to the controller impl class
   * @param[in] size The size to set
   * @param[in] startIndex The start index for relayouting
   * @param[in] requestedNumberOfCharacters The number Of characters for relayouting
   */

  static void DoRelayoutHorizontalAlignment(Controller::Impl& impl, const Size& size, const CharacterIndex startIndex, const Length requestedNumberOfCharacters);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_RELAYOUTER_H
