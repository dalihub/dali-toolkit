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
#include <dali-toolkit/internal/text/spannable/spans/strikethrough-span-impl.h>

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
struct StrikethroughSpan::Impl
{
  StrikethroughStyleProperties mStrikethroughProperties; ///< The properties of strikethrough style.
};

StrikethroughSpan::StrikethroughSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::STRIKETHROUGH)
{
  mImpl = std::make_unique<Impl>();
}

StrikethroughSpan ::~StrikethroughSpan()
{
}

Dali::Toolkit::Text::StrikethroughSpan StrikethroughSpan::New()
{
  StrikethroughSpanPtr                   object = new StrikethroughSpan();
  Dali::Toolkit::Text::StrikethroughSpan handle = Dali::Toolkit::Text::StrikethroughSpan(object.Get());
  return handle;
}

Dali::Toolkit::Text::StrikethroughSpan StrikethroughSpan::New(Vector4 color, float height)
{
  StrikethroughSpanPtr object = new StrikethroughSpan();
  object->SetColor(color);
  object->SetHeight(height);
  Dali::Toolkit::Text::StrikethroughSpan handle = Dali::Toolkit::Text::StrikethroughSpan(object.Get());

  return handle;
}

//Methods
Vector4 StrikethroughSpan::GetColor() const
{
  return mImpl->mStrikethroughProperties.color;
}

bool StrikethroughSpan::IsColorDefined() const
{
  return mImpl->mStrikethroughProperties.colorDefined;
}

void StrikethroughSpan::SetColor(const Vector4& color)
{
  mImpl->mStrikethroughProperties.color        = color;
  mImpl->mStrikethroughProperties.colorDefined = true;
}

float StrikethroughSpan::GetHeight() const
{
  return mImpl->mStrikethroughProperties.height;
}

bool StrikethroughSpan::IsHeightDefined() const
{
  return mImpl->mStrikethroughProperties.heightDefined;
}

void StrikethroughSpan::SetHeight(const float& height)
{
  mImpl->mStrikethroughProperties.height        = height;
  mImpl->mStrikethroughProperties.heightDefined = true;
}

void StrikethroughSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const

{
  StrikethroughCharacterRun strikethroughCharacterRun;
  strikethroughCharacterRun.characterRun.characterIndex     = range.GetStartIndex();
  strikethroughCharacterRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();
  strikethroughCharacterRun.properties                      = mImpl->mStrikethroughProperties;
  logicalModel->mStrikethroughCharacterRuns.PushBack(strikethroughCharacterRun);
  logicalModel->mStrikethroughRunsUpdated = true;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali