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
#include <dali-toolkit/internal/text/spannable/spans/background-color-span-impl.h>

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
struct BackgroundColorSpan::Impl
{
  Vector4 mBackgroundColor;            ///< The color of the characters.
  bool    mBackgroundColorDefined : 1; ///< Whether the background color is defined.
};

BackgroundColorSpan::BackgroundColorSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::BACKGROUND_COLOR)
{
  mImpl = std::make_unique<Impl>();
}

BackgroundColorSpan ::~BackgroundColorSpan()
{
}

Dali::Toolkit::Text::BackgroundColorSpan BackgroundColorSpan::New(const Vector4 &color)
{
  BackgroundColorSpanPtr object = new BackgroundColorSpan();
  object->SetBackgroundColor(color);
  Dali::Toolkit::Text::BackgroundColorSpan handle = Dali::Toolkit::Text::BackgroundColorSpan(object.Get());

  return handle;
}

//Methods
Vector4 BackgroundColorSpan::GetBackgroundColor() const
{
  return mImpl->mBackgroundColor;
}

bool BackgroundColorSpan::IsBackgroundColorDefined() const
{
  return mImpl->mBackgroundColorDefined;
}

void BackgroundColorSpan::SetBackgroundColor(const Vector4 &color)
{
  mImpl->mBackgroundColor        = color;
  mImpl->mBackgroundColorDefined = true;
}

void BackgroundColorSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  ColorRun backgroundColorRun;
  backgroundColorRun.characterRun.characterIndex     = range.GetStartIndex();
  backgroundColorRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();
  backgroundColorRun.color                           = mImpl->mBackgroundColor;
  logicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
