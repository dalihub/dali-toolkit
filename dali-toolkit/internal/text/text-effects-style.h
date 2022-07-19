#ifndef DALI_TOOLKIT_INTERNAL_TEXT_EFFECTS_STYLE_H
#define DALI_TOOLKIT_INTERNAL_TEXT_EFFECTS_STYLE_H

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
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali/devel-api/scripting/scripting.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
const Scripting::StringEnum UNDERLINE_TYPE_STRING_TABLE[] =
  {
    {"solid", Text::Underline::SOLID},
    {"dashed", Text::Underline::DASHED},
    {"double", Text::Underline::DOUBLE}};
const unsigned int UNDERLINE_TYPE_STRING_TABLE_COUNT = sizeof(UNDERLINE_TYPE_STRING_TABLE) / sizeof(UNDERLINE_TYPE_STRING_TABLE[0]);
namespace EffectStyle
{
enum Type
{
  DEFAULT, ///< The default text effect style.
  INPUT    ///< The input text effect style.
};
};

/**
 * @brief Parses the shadow properties.
 *
 * @param[in] shadowProperties The map with the shadow properties.
 * @param[out] colorDefined Whether the shadow's color is defined.
 * @param[out] color The shadow's color.
 * @param[out] offsetDefined Whether the shadow's offset is defined.
 * @param[out] offset The shadow's offset.
 */
bool ParseShadowProperties(const Property::Map& shadowProperties,
                           bool&                colorDefined,
                           Vector4&             color,
                           bool&                offsetDefined,
                           Vector2&             offset);

/**
 * @brief Parses the underline properties.
 *
 * @param[in] underlineProperties The map with the underline properties.
 * @param[out] enabled Whether the underline is enabled.
 * @param[out] colorDefined Whether the underline's color is defined.
 * @param[out] color The underline's color.
 * @param[out] heightDefined Whether the underline's height is defined.
 * @param[out] height The underline's height.
 * @param[out] type The underline's type; DASHED, DOUBLE, etc. Default is a solid underline.
 * @param[out] dashWidth The dashed underline's width.
 * @param[out] dashGap The dashed underline's gap.
 */
bool ParseUnderlineProperties(const Property::Map&   underlineProperties,
                              bool&                  enabled,
                              bool&                  colorDefined,
                              Vector4&               color,
                              bool&                  heightDefined,
                              float&                 height,
                              bool&                  typeDefined,
                              Text::Underline::Type& type,
                              bool&                  widthDefined,
                              float&                 dashWidth,
                              bool&                  dashGapDefined,
                              float&                 dashGap);

/**
 * @brief Parses the outline properties.
 *
 * @param[in] outlineProperties The map with the outline properties.
 * @param[out] colorDefined Whether the outline's color is defined.
 * @param[out] color The outline's color.
 * @param[out] widthDefined Whether the outline's width is defined.
 * @param[out] width The outline's width.
 */
bool ParseOutlineProperties(const Property::Map& outlineProperties,
                            bool&                colorDefined,
                            Vector4&             color,
                            bool&                widthDefined,
                            unsigned int&        width);

/**
 * @brief Parses the background properties.
 *
 * @param[in] backgroundProperties The map with the background properties.
 * @param[out] enabled Whether the background is enabled.
 * @param[out] colorDefined Whether the background color is defined.
 * @param[out] color The background color.
 */
bool ParseBackgroundProperties(const Property::Map& backgroundProperties,
                               bool&                enabled,
                               bool&                colorDefined,
                               Vector4&             color);

/**
 * @brief Parses the strikethrough properties.
 *
 * @param[in] strikethroughProperties The map with the strikethrough properties.
 * @param[out] enabled Whether the strikethrough is enabled.
 * @param[out] colorDefined Whether the strikethrough's color is defined.
 * @param[out] color The strikethrough's color.
 * @param[out] heightDefined Whether the strikethrough's height is defined.
 * @param[out] height The strikethrough's height.
 */
bool ParseStrikethroughProperties(const Property::Map& strikethroughProperties,
                                  bool&                enabled,
                                  bool&                colorDefined,
                                  Vector4&             color,
                                  bool&                heightDefined,
                                  float&               height);

/**
 * @brief Sets the underline properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the underline's properties.
 * @param[in] type Whether the property is for the default underline or the input underline.
 *
 * @return Whether the underline properties have been updated.
 */
bool SetUnderlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Sets the strikethrough properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the strikethrough's properties.
 * @param[in] type Whether the property is for the default strikethrough or the input strikethrough.
 *
 * @return Whether the strikethrough properties have been updated.
 */
bool SetStrikethroughProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the underline's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the underline's properties.
 * @param[in] type Whether the property is for the default underline or the input underline.
 */
void GetUnderlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the strikethrough's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the strikethrough's properties.
 * @param[in] type Whether the property is for the default strikethrough or the input strikethrough.
 */
void GetStrikethroughProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Sets the shadow properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the shadow's style.
 * @param[in] type Whether the property is for the default shadow's style or the input shadow's style.
 *
 * @return Whether the shadow properties have been updated.
 */
bool SetShadowProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the shadow's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the shadow's properties.
 * @param[in] type Whether the property is for the default shadow or the input shadow.
 */
void GetShadowProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Sets the emboss properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the emboss's properties.
 * @param[in] type Whether the property is for the default emboss or the input emboss.
 *
 * @return Whether the emboss properties have been updated.
 */
bool SetEmbossProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the emboss's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the emboss's properties.
 * @param[in] type Whether the property is for the default emboss or the input emboss.
 */
void GetEmbossProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Sets the outline properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the outline's properties.
 * @param[in] type Whether the property is for the default outline or the input outline.
 *
 * @return Whether the outline properties have been updated.
 */
bool SetOutlineProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the outline's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the outline's properties.
 * @param[in] type Whether the property is for the default outline or the input outline.
 */
void GetOutlineProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Sets the background properties.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The values of the background's properties.
 * @param[in] type Whether the property is for the default background or the input background.
 *
 * @return Whether the background properties have been updated.
 */
bool SetBackgroundProperties(ControllerPtr controller, const Property::Value& value, EffectStyle::Type type);

/**
 * @brief Retrieves the background's properties.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the underline's properties.
 * @param[in] type Whether the property is for the default background or the input background.
 */
void GetBackgroundProperties(ControllerPtr controller, Property::Value& value, EffectStyle::Type type);

/**
 * @brief Converts a underline type string into @e Underline::Type
 *
 * @param[in] underlineTypeStr The underline type string. Must end with '\0'.
 *
 * @return The @e Underline::Type value corresponding to the string.
 */
Underline::Type StringToUnderlineType(const char* const underlineTypeStr);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_EFFECTS_STYLE_H
