#ifndef DALI_TOOLKIT_TEXT_ENUMERATION_IMPL_H
#define DALI_TOOLKIT_TEXT_ENUMERATION_IMPL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-value.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Get the alignment from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting alignment from the given source
 * @return true if the resulting alignment has been updated
 */
bool GetHorizontalAlignmentEnumeration( const Property::Value& propertyValue, Toolkit::Text::HorizontalAlignment::Type& alignment );

/**
 * @brief Get the alignment from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting alignment from the given source
 * @return true if the resulting alignment has been updated
 */
bool GetVerticalAlignmentEnumeration( const Property::Value& propertyValue, Toolkit::Text::VerticalAlignment::Type& alignment );

/**
 * @brief Get the line-wrap-mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting lineWrapMode from the given source
 * @return true if the resulting lineWrapMode has been updated
 */
bool GetLineWrapModeEnumeration( const Property::Value& propertyValue, Toolkit::Text::LineWrap::Mode& lineWrapMode );

/**
 * @brief Get the alignment string from the provided alignment string.
 * @param[in] alignment the Text::HORIZONTAL enum source
 * @return the string equivalent
 */
const char* GetHorizontalAlignmentString( const Toolkit::Text::HorizontalAlignment::Type& alignment );

/**
 * @brief Get the alignment string from the provided alignment string.
 * @param[in] alignment the Text::VerticalAlignment enum source
 * @return the string equivalent
 */
const char* GetVerticalAlignmentString( const Toolkit::Text::VerticalAlignment::Type& alignment );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ENUMERATION_IMPL_H
