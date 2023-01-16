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

// CLASS HEADER
#include <dali-toolkit/internal/text/spannable/spans/italic-span-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-description-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
ItalicSpan::ItalicSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::ITALIC)
{
}

ItalicSpan::~ItalicSpan()
{
}

Dali::Toolkit::Text::ItalicSpan ItalicSpan::New()
{
  ItalicSpanPtr object = new ItalicSpan();
  Dali::Toolkit::Text::ItalicSpan handle = Dali::Toolkit::Text::ItalicSpan(object.Get());
  return handle;
}
void ItalicSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  FontDescriptionRun fontRun;
  fontRun.slant        = TextAbstraction::FontSlant::ITALIC;
  fontRun.slantDefined = true;
  fontRun.characterRun.characterIndex     = range.GetStartIndex();
  fontRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();
  logicalModel->mFontDescriptionRuns.PushBack(fontRun);
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali