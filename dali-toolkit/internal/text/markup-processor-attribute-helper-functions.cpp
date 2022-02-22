/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor-attribute-helper-functions.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

// EXTERNAL INCLUDES
#include <memory.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void CopyAttributeValueFromBuffer(const Attribute& attribute, const Length maxLengthAttributeValue, char* value)
{
  const Length length = attribute.valueLength > maxLengthAttributeValue ? maxLengthAttributeValue : attribute.valueLength;
  memcpy(value, attribute.valueBuffer, length);
  value[length] = 0;
}

float ProcessFloatAttribute(const Attribute& attribute)
{
  return StringToFloat(attribute.valueBuffer);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
