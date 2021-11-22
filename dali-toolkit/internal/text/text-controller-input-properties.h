#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_PROPERTIES_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_PROPERTIES_H

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
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/events/gesture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>
//#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
//#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>
//#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
//#include <dali-toolkit/internal/text/decorator/text-decorator.h>
//#include <dali-toolkit/internal/text/hidden-text.h>
//#include <dali-toolkit/internal/text/input-filter.h>
//#include <dali-toolkit/internal/text/layouts/layout-engine.h>
//#include <dali-toolkit/internal/text/text-anchor-control-interface.h>
//#include <dali-toolkit/internal/text/text-model-interface.h>
//#include <dali-toolkit/internal/text/text-selectable-control-interface.h>
//#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali::Toolkit::Text
{
struct Controller::InputProperties
{
  static void SetInputColor(Controller& controller, const Vector4& color);

  static const Vector4& GetInputColor(const Controller& controller);

  static void SetInputLineSpacing(Controller& controller, float lineSpacing);

  static float GetInputLineSpacing(const Controller& controller);

  static void SetInputShadowProperties(Controller& controller, const std::string& shadowProperties);

  static const std::string& GetInputShadowProperties(const Controller& controller);

  static void SetInputUnderlineProperties(Controller& controller, const std::string& underlineProperties);

  static const std::string& GetInputUnderlineProperties(const Controller& controller);

  static void SetInputEmbossProperties(Controller& controller, const std::string& embossProperties);

  static const std::string& GetInputEmbossProperties(const Controller& controller);

  static void SetInputOutlineProperties(Controller& controller, const std::string& outlineProperties);

  static const std::string& GetInputOutlineProperties(const Controller& controller);

  static void SetInputModePassword(Controller& controller, bool passwordInput);

  static bool IsInputModePassword(Controller& controller);
};


} // namespace Dali::Toolkit::Text

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_PROPERTIES_H
