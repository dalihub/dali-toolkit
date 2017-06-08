
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

// CLASS HEADER
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>

// INTERNAL HEADER
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace DevelTextEditor
{

ScrollStateChangedSignalType& ScrollStateChangedSignal( TextEditor textEditor )
{
  return Dali::Toolkit::GetImpl( textEditor ).ScrollStateChangedSignal();
}

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali
