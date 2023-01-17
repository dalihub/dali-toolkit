#ifndef DALI_SPAN_ENUMERATIONS_H
#define DALI_SPAN_ENUMERATIONS_H

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
#include <cstdint>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace SpanType
{
/**
 * @brief Enumeration for type of the span.
 */
enum class Value : uint8_t
{
  BASE = 0,
  FOREGROUND_COLOR,
  BACKGROUND_COLOR,
  FONT,
  BOLD,
  ITALIC,
  UNDERLINE,
  STRIKETHROUGH,
  CHARACTER_SPACING

};
} // namespace SpanType

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_SPAN_ENUMERATIONS_H
