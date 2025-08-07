#ifndef DALI_TOOLKIT_INTERNAL_PROPERTY_BRIDGE_H
#define DALI_TOOLKIT_INTERNAL_PROPERTY_BRIDGE_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/object-registry.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/property-bridge/property-bridge.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class PropertyBridge;

/**
 * @copydoc Toolkit::PropertyBridge
 */
class PropertyBridge : public Dali::BaseObject
{
public:
  /**
   * Construct a new PropertyBridge.
   */
  PropertyBridge();

  /**
   * Destructor
   */
  ~PropertyBridge();

  /**
   * @copydoc Dali::Toolkit::PropertyBridge::Get()
   */
  static Dali::Toolkit::PropertyBridge Get();

  /**
   * Get the string property.
   * @return A string value.
   */
  std::string GetStringProperty(Actor actor, const std::string& propertyName);

  /**
   * @brief RegisterStringGetter.
   */
  static void RegisterStringGetter(StringGetterDelegate getter);

  /**
   * @brief EnsureStringGetter.
   */
  bool EnsureStringGetter();

private:
  // Undefined
  PropertyBridge(const PropertyBridge&);

  PropertyBridge& operator=(const PropertyBridge& rhs);

private:
  StringGetterDelegate mStringGetter;
};

} // namespace Internal

inline Internal::PropertyBridge& GetImplementation(Dali::Toolkit::PropertyBridge& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::PropertyBridge&>(handle);
}

inline const Internal::PropertyBridge& GetImplementation(const Dali::Toolkit::PropertyBridge& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::PropertyBridge&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_PROPERTY_BRIDGE_H
