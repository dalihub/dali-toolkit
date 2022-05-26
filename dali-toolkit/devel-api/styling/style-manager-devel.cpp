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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelStyleManager
{
const Property::Map& GetConfigurations(StyleManager styleManager)
{
  return GetImpl(styleManager).GetConfigurations();
}

void SetBrokenImageUrl(StyleManager styleManager, DevelStyleManager::BrokenImageType brokenImageType, const std::string& brokenImageUrl)
{
  return GetImpl(styleManager).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
}

std::string GetBrokenImageUrl(StyleManager styleManager, DevelStyleManager::BrokenImageType brokenImageType)
{
  return GetImpl(styleManager).GetBrokenImageUrl(brokenImageType);
}

std::vector<std::string> GetBrokenImageUrlList(StyleManager styleManager)
{
  return GetImpl(styleManager).GetBrokenImageUrlList();
}

BrokenImageChangedSignalType& BrokenImageChangedSignal(StyleManager styleManager)
{
  return GetImpl(styleManager).BrokenImageChangedSignal();
}

} // namespace DevelStyleManager

} // namespace Toolkit

} // namespace Dali
