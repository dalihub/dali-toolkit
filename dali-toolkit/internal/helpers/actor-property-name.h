#ifndef DALI_TOOLKIT_INTERNAL_ACTOR_PROPERTY_NAME_H
#define DALI_TOOLKIT_INTERNAL_ACTOR_PROPERTY_NAME_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/object/handle.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
struct ActorPropertyNamePair
{
  const char* legacyName;
  const char* compatibleName;
};

constexpr ActorPropertyNamePair LEGACY_COLOR_PROPERTY_NAMES[] = {
  {"color", "colorMultiplier"},
  {"colorRed", "colorMultiplierRed"},
  {"colorGreen", "colorMultiplierGreen"},
  {"colorBlue", "colorMultiplierBlue"},
  {"colorAlpha", "colorMultiplierAlpha"},
  {"worldColor", "worldColorMultiplier"},
};
} // unnamed namespace

/**
 * Converts legacy Actor color property names used by JSON/DLI resources to
 * their color multiplier names.
 */
inline Dali::StringView GetCompatibleActorPropertyName(Dali::StringView propertyName)
{
  for(const auto& namePair : LEGACY_COLOR_PROPERTY_NAMES)
  {
    if(propertyName == namePair.legacyName)
    {
      return namePair.compatibleName;
    }
  }
  return propertyName;
}

/**
 * Looks up a property, falling back to legacy Actor color property names.
 */
inline Dali::Property::Index GetPropertyIndexWithCompatibility(const Dali::Handle& handle, const Dali::Property::Key& key)
{
  Dali::Property::Index index = handle.GetPropertyIndex(key);
  if(index == Dali::Property::INVALID_INDEX &&
     key.type == Dali::Property::Key::STRING &&
     Dali::Actor::DownCast(handle))
  {
    const Dali::StringView propertyName(key.stringKey);
    const Dali::StringView compatibleName = GetCompatibleActorPropertyName(propertyName);
    if(!(compatibleName == propertyName))
    {
      index = handle.GetPropertyIndex(compatibleName);
    }
  }
  return index;
}

} // namespace Internal
} // namespace Toolkit
} //namespace DALI_NAMESPACE

#endif // DALI_TOOLKIT_INTERNAL_ACTOR_PROPERTY_NAME_H
