#ifndef DALI_INTERNAL_TOOLKIT_VISUAL_EVENT_OBSERVER_H
#define DALI_INTERNAL_TOOLKIT_VISUAL_EVENT_OBSERVER_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-value.h>

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
class EventObserver
{
public:
  /**
   * Inform the observer of the object when a resource is ready.
   * @param[in] object The connection owner
   */
  virtual void ResourceReady(Visual::Base& object) = 0;

  /**
   * Inform the observer of the object when an event occurs.
   * @param[in] object The connection owner
   * @param[in] signalId The signal to emit. See Visual to find supported signals
   */
  virtual void NotifyVisualEvent(Visual::Base& object, Property::Index signalId) = 0;

  /**
   * Requests a relayout to the observer.
   * @param[in] object The connection owner
   */
  virtual void RelayoutRequest(Visual::Base& object) = 0;

protected:
  /**
   * constructor
   */
  EventObserver()
  {
  }

  /**
   * virtual destructor
   */
  virtual ~EventObserver()
  {
  }

  // Undefined copy constructor.
  EventObserver(const EventObserver&) = delete;

  // Undefined assignment operator.
  EventObserver& operator=(const EventObserver&) = delete;
};

} // namespace Visual
} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_INTERNAL_TOOLKIT_VISUAL_EVENT_OBSERVER_H
