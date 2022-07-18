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
#include "dali-scene3d/public-api/loader/customization.h"
#include "dali/devel-api/common/map-wrapper.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
struct Customization::Map::Impl
{
  std::map<Tag, Customization> mCustomizations;
};

Customization::Map::Map()
: mImpl{new Impl}
{
}

Customization::Map::~Map() = default;

Customization* Customization::Map::Set(Tag tag, Customization&& customization)
{
  auto& result = (mImpl->mCustomizations[tag] = customization);
  return &result;
}

const Customization* Customization::Map::Get(Tag tag) const
{
  auto& customizations = mImpl->mCustomizations;
  auto  iFind          = customizations.find(tag);
  if(iFind != customizations.end())
  {
    return &iFind->second;
  }
  return nullptr;
}

Customization* Customization::Map::Get(Tag tag)
{
  auto& customizations = mImpl->mCustomizations;
  auto  iFind          = customizations.find(tag);
  if(iFind != customizations.end())
  {
    return &iFind->second;
  }
  return nullptr;
}

uint32_t Customization::Map::Size() const
{
  return mImpl->mCustomizations.size();
}

void Customization::Map::Clear()
{
  mImpl->mCustomizations.clear();
}

struct Customization::Choices::Impl
{
  std::map<Tag, OptionType> mOptions;
};

Customization::Choices::Choices()
: mImpl{new Impl}
{
}

Customization::Choices::~Choices() = default;

void Customization::Choices::Set(Tag tag, OptionType option)
{
  mImpl->mOptions[tag] = option;
}

Customization::OptionType Customization::Choices::Get(Tag tag) const
{
  auto& options = mImpl->mOptions;
  auto  iFind   = options.find(tag);
  if(iFind != options.end())
  {
    return iFind->second;
  }
  return NONE;
}

uint32_t Customization::Choices::Size() const
{
  return mImpl->mOptions.size();
}

void Customization::Choices::Clear()
{
  mImpl->mOptions.clear();
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
