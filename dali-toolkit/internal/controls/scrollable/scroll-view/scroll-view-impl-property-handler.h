#ifndef DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_IMPL_PROPERTY_HANDLER_H
#define DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_IMPL_PROPERTY_HANDLER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property.h>

namespace Dali::Toolkit::Internal
{
class ScrollView;

/// Handles the properties in scroll view calling the appropriate scroll view methods
struct ScrollViewPropertyHandler
{
  /**
   * @brief Sets the property on the given scroll-view object.
   *
   * @param object The scrollview object
   * @param index The index to set
   * @param value The value to set
   */
  static void Set(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Retrieves the value of a scroll-view property.
   *
   * @param object The scrollview object
   * @param index The index whose value is to be retrieved
   * @return
   */
  static Property::Value Get(BaseObject* object, Property::Index index);

  /**
   * Set up default rulers using a property map
   * @param[in] scrollView    The scroll view to apply this on
   * @param[in] scrollModeMap A map defining the characteristics of X and Y scrolling
   * using either FixedRuler or DefaultRuler.
   */
  static void SetScrollMode(ScrollView& scrollView, const Property::Map& scrollModeMap);

  /**
   * This is called whenever the Scroll Rulers are modified.
   *
   * This will update the properties: 'scrollPositionMin' * and 'scrollPositionMax' to reflect the changes.
   * @param scrollView The Scroll View to modify
   */
  static void UpdatePropertyDomain(ScrollView& scrollView);
};

} // namespace Dali::Toolkit::Internal

#endif // DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_IMPL_PROPERTY_HANDLER_H
