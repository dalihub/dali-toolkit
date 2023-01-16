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
#include <dali-toolkit/internal/text/spannable/spans/foreground-color-span-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
struct ForegroundColorSpan::Impl
{
  Vector4 mForegroundColor;            ///< The foreground-color of the characters.
  bool    mForegroundColorDefined : 1; ///< Whether the foreground-color is defined.
};

ForegroundColorSpan::ForegroundColorSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::FOREGROUND_COLOR)
{
  mImpl = std::make_unique<Impl>();
}

ForegroundColorSpan ::~ForegroundColorSpan()
{
}

Dali::Toolkit::Text::ForegroundColorSpan ForegroundColorSpan::New(Vector4 color)
{
  ForegroundColorSpanPtr object = new ForegroundColorSpan();
  object->SetForegroundColor(color);

  Dali::Toolkit::Text::ForegroundColorSpan handle = Dali::Toolkit::Text::ForegroundColorSpan(object.Get());

  return handle;
}

//Methods
const Vector4 ForegroundColorSpan::GetForegroundColor() const
{
  return mImpl->mForegroundColor;
}

bool ForegroundColorSpan::IsForegroundColorDefined() const
{
  return mImpl->mForegroundColorDefined;
}

void ForegroundColorSpan::SetForegroundColor(Vector4 color)
{
  mImpl->mForegroundColor        = color;
  mImpl->mForegroundColorDefined = true;
}

void ForegroundColorSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  ColorRun colorRun;
  colorRun.characterRun.characterIndex     = range.GetStartIndex();
  colorRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();

  colorRun.color = mImpl->mForegroundColor;
  logicalModel->mColorRuns.PushBack(colorRun);
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
