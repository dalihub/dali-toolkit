/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelTextLabel
{
AnchorClickedSignalType& AnchorClickedSignal(TextLabel textLabel)
{
  return GetImpl(textLabel).AnchorClickedSignal();
}

TextFitChangedSignalType& TextFitChangedSignal(TextLabel textLabel)
{
  return GetImpl(textLabel).TextFitChangedSignal();
}

AsyncTextRenderedSignalType& AsyncTextRenderedSignal(TextLabel textLabel)
{
  return GetImpl(textLabel).AsyncTextRenderedSignal();
}

AsyncNaturalSizeComputedSignalType& AsyncNaturalSizeComputedSignal(TextLabel textLabel)
{
  return GetImpl(textLabel).AsyncNaturalSizeComputedSignal();
}

AsyncHeightForWidthComputedSignalType& AsyncHeightForWidthComputedSignal(TextLabel textLabel)
{
  return GetImpl(textLabel).AsyncHeightForWidthComputedSignal();
}

Vector<Vector2> GetTextSize(TextLabel textLabel, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textLabel).GetTextSize(startIndex, endIndex);
}

Vector<Vector2> GetTextPosition(TextLabel textLabel, const uint32_t startIndex, const uint32_t endIndex)
{
  return GetImpl(textLabel).GetTextPosition(startIndex, endIndex);
}

Rect<> GetTextBoundingRectangle(TextLabel textLabel, uint32_t startIndex, uint32_t endIndex)
{
  return GetImpl(textLabel).GetTextBoundingRectangle(startIndex, endIndex);
}

void SetTextFitArray(TextLabel textLabel, const bool enable, std::vector<FitOption>& fitOptions)
{
  GetImpl(textLabel).SetTextFitArray(enable, fitOptions);
}

std::vector<FitOption>& GetTextFitArray(TextLabel textLabel)
{
  return GetImpl(textLabel).GetTextFitArray();
}

bool IsTextFitArrayEnabled(TextLabel textLabel)
{
  return GetImpl(textLabel).IsTextFitArrayEnabled();
}

void SetRemoveFrontInset(TextLabel textLabel, const bool remove)
{
  GetImpl(textLabel).SetRemoveFrontInset(remove);
}

bool IsRemoveFrontInset(TextLabel textLabel)
{
  return GetImpl(textLabel).IsRemoveFrontInset();
}

void SetRemoveBackInset(TextLabel textLabel, const bool remove)
{
  GetImpl(textLabel).SetRemoveBackInset(remove);
}

bool IsRemoveBackInset(TextLabel textLabel)
{
  return GetImpl(textLabel).IsRemoveBackInset();
}

void RequestAsyncRenderWithFixedSize(TextLabel textLabel, float width, float height)
{
  GetImpl(textLabel).RequestAsyncRenderWithFixedSize(width, height);
}

void RequestAsyncRenderWithFixedWidth(TextLabel textLabel, float width, float heightConstraint)
{
  GetImpl(textLabel).RequestAsyncRenderWithFixedWidth(width, heightConstraint);
}

void RequestAsyncRenderWithConstraint(TextLabel textLabel, float widthConstraint, float heightConstraint)
{
  GetImpl(textLabel).RequestAsyncRenderWithConstraint(widthConstraint, heightConstraint);
}

void RequestAsyncNaturalSize(TextLabel textLabel)
{
  GetImpl(textLabel).RequestAsyncNaturalSize();
}

void RequestAsyncHeightForWidth(TextLabel textLabel, float width)
{
  GetImpl(textLabel).RequestAsyncHeightForWidth(width);
}

int GetLineCount(TextLabel textLabel, float width)
{
  return GetImpl(textLabel).GetLineCount(width);
}

Dali::Property::Index RegisterFontVariationProperty(TextLabel textLabel, std::string tag)
{
  return GetImpl(textLabel).RegisterFontVariationProperty(tag);
}

void SetMaskEffect(TextLabel textLabel, Toolkit::Control control)
{
  GetImpl(textLabel).SetMaskEffect(control);
}

} // namespace DevelTextLabel

} // namespace Toolkit

} // namespace Dali
