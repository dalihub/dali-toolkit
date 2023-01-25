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
#include <dali-toolkit/internal/text/spannable/spans/underline-span-impl.h>

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
struct UnderlineSpan::Impl
{
  UnderlineStyleProperties mUnderlineProperties; ///< The properties of underline style.
};

UnderlineSpan::UnderlineSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::UNDERLINE)
{
  mImpl = std::make_unique<Impl>();
}

UnderlineSpan ::~UnderlineSpan()
{
}

Dali::Toolkit::Text::UnderlineSpan UnderlineSpan::New()
{
  UnderlineSpanPtr object = new UnderlineSpan();

  Dali::Toolkit::Text::UnderlineSpan handle = Dali::Toolkit::Text::UnderlineSpan(object.Get());

  return handle;
}

Dali::Toolkit::Text::UnderlineSpan UnderlineSpan::NewSolid(Vector4 color, float height)
{
  UnderlineSpanPtr object = new UnderlineSpan();
  object->SetType(Text::Underline::SOLID);
  object->SetColor(color);
  object->SetHeight(height);

  Dali::Toolkit::Text::UnderlineSpan handle = Dali::Toolkit::Text::UnderlineSpan(object.Get());

  return handle;
}

Dali::Toolkit::Text::UnderlineSpan UnderlineSpan::NewDashed(Vector4 color, float height, float dashGap, float dashWidth)
{
  UnderlineSpanPtr object = new UnderlineSpan();
  object->SetType(Text::Underline::DASHED);
  object->SetColor(color);
  object->SetHeight(height);
  object->SetDashGap(dashGap);
  object->SetDashWidth(dashWidth);

  Dali::Toolkit::Text::UnderlineSpan handle = Dali::Toolkit::Text::UnderlineSpan(object.Get());

  return handle;
}

Dali::Toolkit::Text::UnderlineSpan UnderlineSpan::NewDouble(Vector4 color, float height)
{
  UnderlineSpanPtr object = new UnderlineSpan();
  object->SetType(Text::Underline::DOUBLE);
  object->SetColor(color);
  object->SetHeight(height);

  Dali::Toolkit::Text::UnderlineSpan handle = Dali::Toolkit::Text::UnderlineSpan(object.Get());

  return handle;
}

//Methods

Text::Underline::Type UnderlineSpan::GetType() const
{
  return mImpl->mUnderlineProperties.type;
}

bool UnderlineSpan::IsTypeDefined() const
{
  return mImpl->mUnderlineProperties.typeDefined;
}

void UnderlineSpan::SetType(const Text::Underline::Type& type)
{
  mImpl->mUnderlineProperties.type        = type;
  mImpl->mUnderlineProperties.typeDefined = true;
}

Vector4 UnderlineSpan::GetColor() const
{
  return mImpl->mUnderlineProperties.color;
}

bool UnderlineSpan::IsColorDefined() const
{
  return mImpl->mUnderlineProperties.colorDefined;
}

void UnderlineSpan::SetColor(const Vector4& color)
{
  mImpl->mUnderlineProperties.color        = color;
  mImpl->mUnderlineProperties.colorDefined = true;
}

float UnderlineSpan::GetHeight() const
{
  return mImpl->mUnderlineProperties.height;
}

bool UnderlineSpan::IsHeightDefined() const
{
  return mImpl->mUnderlineProperties.heightDefined;
}

void UnderlineSpan::SetHeight(const float& height)
{
  mImpl->mUnderlineProperties.height        = height;
  mImpl->mUnderlineProperties.heightDefined = true;
}

float UnderlineSpan::GetDashGap() const
{
  return mImpl->mUnderlineProperties.dashGap;
}

bool UnderlineSpan::IsDashGapDefined() const
{
  return mImpl->mUnderlineProperties.dashGapDefined;
}

void UnderlineSpan::SetDashGap(const float& dashGap)
{
  mImpl->mUnderlineProperties.dashGap        = dashGap;
  mImpl->mUnderlineProperties.dashGapDefined = true;
}

float UnderlineSpan::GetDashWidth() const
{
  return mImpl->mUnderlineProperties.dashWidth;
}

bool UnderlineSpan::IsDashWidthDefined() const
{
  return mImpl->mUnderlineProperties.dashWidthDefined;
}

void UnderlineSpan::SetDashWidth(const float& dashWidth)
{
  mImpl->mUnderlineProperties.dashWidth        = dashWidth;
  mImpl->mUnderlineProperties.dashWidthDefined = true;
}

void UnderlineSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  UnderlinedCharacterRun underlinedCharacterRun;
  underlinedCharacterRun.characterRun.characterIndex     = range.GetStartIndex();
  underlinedCharacterRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();

  underlinedCharacterRun.properties = mImpl->mUnderlineProperties;
  logicalModel->mUnderlinedCharacterRuns.PushBack(underlinedCharacterRun);

  logicalModel->mUnderlineRunsUpdated = true;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali
