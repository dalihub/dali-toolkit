/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

// INTERAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{
namespace Toolkit
{
Visual::Base::Base()
{
}

Visual::Base::~Base()
{
}

Visual::Base::Base(const Visual::Base& handle)
: BaseHandle(handle)
{
}

Visual::Base& Visual::Base::operator=(const Visual::Base& handle)
{
  BaseHandle::operator=(handle);
  return *this;
}

Visual::Base::Base(Internal::Visual::Base* impl)
: BaseHandle(impl)
{
}

void Visual::Base::SetName(const std::string& name)
{
  GetImplementation(*this).SetName(name);
}

const std::string& Visual::Base::GetName() const
{
  return GetImplementation(*this).GetName();
}

void Visual::Base::SetTransformAndSize(const Dali::Property::Map& transform, Size controlSize)
{
  GetImplementation(*this).SetTransformAndSize(transform, controlSize);
}

float Visual::Base::GetHeightForWidth(float width)
{
  return GetImplementation(*this).GetHeightForWidth(width);
}

float Visual::Base::GetWidthForHeight(float height)
{
  return GetImplementation(*this).GetWidthForHeight(height);
}

void Visual::Base::GetNaturalSize(Vector2& naturalSize)
{
  GetImplementation(*this).GetNaturalSize(naturalSize);
}

void Visual::Base::SetDepthIndex(int index)
{
  GetImplementation(*this).SetDepthIndex(index);
}

int Visual::Base::GetDepthIndex() const
{
  return GetImplementation(*this).GetDepthIndex();
}

void Visual::Base::CreatePropertyMap(Dali::Property::Map& map) const
{
  GetImplementation(*this).CreatePropertyMap(map);
}

void Visual::Base::DoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  GetImplementation(*this).DoAction(actionId, attributes);
}

void Visual::Base::DoActionExtension(const Dali::Property::Index actionId, const Dali::Any& attributes)
{
  GetImplementation(*this).DoActionExtension(actionId, attributes);
}

Visual::Type Visual::Base::GetType() const
{
  return GetImplementation(*this).GetType();
}

} // namespace Toolkit

} // namespace Dali
