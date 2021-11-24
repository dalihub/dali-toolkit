#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_PLACEHOLDER_HANDLER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_PLACEHOLDER_HANDLER_H

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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Controller::PlaceholderHandler
{
  static void               SetPlaceholderTextElideEnabled(Controller& controller, bool enabled);
  static bool               IsPlaceholderTextElideEnabled(const Controller& controller);
  static void               SetPlaceholderText(Controller& controller, PlaceholderType type, const std::string& text);
  static void               GetPlaceholderText(const Controller& controller, PlaceholderType type, std::string& text);
  static void               SetPlaceholderFontFamily(Controller& controller, const std::string& placeholderTextFontFamily);
  static const std::string& GetPlaceholderFontFamily(const Controller& controller);
  static void               SetPlaceholderTextFontWeight(Controller& controller, FontWeight weight);
  static bool               IsPlaceholderTextFontWeightDefined(const Controller& controller);
  static FontWeight         GetPlaceholderTextFontWeight(const Controller& controller);
  static void               SetPlaceholderTextFontWidth(Controller& controller, FontWidth width);
  static bool               IsPlaceholderTextFontWidthDefined(const Controller& controller);
  static FontWidth          GetPlaceholderTextFontWidth(const Controller& controller);
  static void               SetPlaceholderTextFontSlant(Controller& controller, FontSlant slant);
  static bool               IsPlaceholderTextFontSlantDefined(const Controller& controller);
  static FontSlant          GetPlaceholderTextFontSlant(const Controller& controller);
  static void               SetPlaceholderTextFontSize(Controller& controller, float fontSize, FontSizeType type);
  static float              GetPlaceholderTextFontSize(const Controller& controller, FontSizeType type);
  static void               SetPlaceholderTextColor(Controller& controller, const Vector4& textColor);
  static const Vector4&     GetPlaceholderTextColor(const Controller& controller);
  static void               SetPlaceholderProperty(Controller& controller, const Property::Map& map);
  static void               GetPlaceholderProperty(Controller& controller, Property::Map& map);
  static void               ShowPlaceholderText(Controller::Impl& impl);
  static void               CreatePlaceholderFont(Controller& controller);
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_PLACEHOLDER_HANDLER_H
