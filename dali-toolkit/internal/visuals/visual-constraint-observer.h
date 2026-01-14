#ifndef DALI_INTERNAL_TOOLKIT_VISUAL_CONSTRAINT_OBSERVER_H
#define DALI_INTERNAL_TOOLKIT_VISUAL_CONSTRAINT_OBSERVER_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>

#include <unordered_set>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace Visual
{
class Base;

/**
 * Observer to be informed when visuals have events.
 */
class ConstraintObserver
{
public:
  /**
   * Query to observer whether any of properties are animate or not.
   * @param[in] properties The properties whether any of them are animate or not.
   * @return True if at least one of property animate. False otherwise.
   */
  virtual bool IsAnyPropertyAnimate(const std::unordered_set<Property::Index>& properties) const = 0;

protected:
  /**
   * constructor
   */
  ConstraintObserver()
  {
  }

  /**
   * virtual destructor
   */
  virtual ~ConstraintObserver()
  {
  }

  // Undefined copy constructor.
  ConstraintObserver(const ConstraintObserver&) = delete;

  // Undefined assignment operator.
  ConstraintObserver& operator=(const ConstraintObserver&) = delete;
};

} // namespace Visual
} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_INTERNAL_TOOLKIT_VISUAL_CONSTRAINT_OBSERVER_H
