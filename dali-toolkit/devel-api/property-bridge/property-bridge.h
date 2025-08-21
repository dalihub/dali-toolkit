#ifndef DALI_TOOLKIT_PROPERTY_BRIDGE_H
#define DALI_TOOLKIT_PROPERTY_BRIDGE_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class PropertyBridge;
}

using StringGetterDelegate = void(*)(void* obj, const char* propertyName, std::string* result);

/**
 * PropertyBridge
 */
class DALI_TOOLKIT_API PropertyBridge : public BaseHandle
{
public:
  /**
   * Create a PropertyBridge handle; this can be initialised with PropertyBridge::Get()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  PropertyBridge();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PropertyBridge();

  /**
   * Get the singleton of PropertyBridge object.
   * @return A handle to the PropertyBridge.
   */
  static PropertyBridge Get();

  /**
   * Get the string property.
   * @return A string property value.
   */
  std::string GetStringProperty(Actor actor, const std::string& propertyName);

public:
  explicit DALI_INTERNAL PropertyBridge(Internal::PropertyBridge* impl);

}; // class PropertyBridge

/**
 * @brief Register String Getter.
 */
DALI_TOOLKIT_API void PropertyBridgeRegisterStringGetter(StringGetterDelegate getter);

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_PROPERTY_BRIDGE_H
