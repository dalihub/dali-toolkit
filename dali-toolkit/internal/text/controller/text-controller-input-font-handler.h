#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Controller::InputFontHandler
{
  static void               SetInputFontFamily(Controller& controller, const std::string& fontFamily);
  static const std::string& GetInputFontFamily(const Controller& controller);
  static void               SetInputFontWeight(const Controller& controller, FontWeight weight);
  static bool               IsInputFontWeightDefined(const Controller& controller);
  static FontWeight         GetInputFontWeight(const Controller& controller);
  static void               SetInputFontWidth(Controller& controller, FontWidth width);
  static bool               IsInputFontWidthDefined(const Controller& controller);
  static FontWidth          GetInputFontWidth(const Controller& controller);
  static void               SetInputFontSlant(Controller& controller, FontSlant slant);
  static bool               IsInputFontSlantDefined(const Controller& controller);
  static FontSlant          GetInputFontSlant(const Controller& controller);
  static void               SetInputFontPointSize(Controller& controller, float size);
  static float              GetInputFontPointSize(const Controller& controller);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_INPUT_FONT_HANDLER_H
