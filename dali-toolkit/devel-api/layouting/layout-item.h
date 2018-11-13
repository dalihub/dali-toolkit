#ifndef DALI_TOOLKIT_LAYOUTING_LAYOUT_ITEM_H
#define DALI_TOOLKIT_LAYOUTING_LAYOUT_ITEM_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <memory>
#include <cstdint>
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>
#include <dali-toolkit/devel-api/layouting/layout-transition-data.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class LayoutItem;
}

using LayoutId = unsigned int;

/**
 * Base class for layouts. It is used to layout a control (or visual).
 * It can be laid out by a LayoutGroup.
 */
class DALI_TOOLKIT_API LayoutItem : public BaseHandle
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = CHILD_PROPERTY_REGISTRATION_START_INDEX, ///< Start index is used by child properties
    CHILD_PROPERTY_END_INDEX   = CHILD_PROPERTY_REGISTRATION_START_INDEX+1000 ///< Reserving 1000 property indices
  };

  struct ChildProperty
  {
    enum
    {
      WIDTH_SPECIFICATION = ///< Child property to specify desired width (May use MATCH_PARENT/WRAP_CONTENT)
        CHILD_PROPERTY_START_INDEX,
      HEIGHT_SPECIFICATION ///< Child property to specify desired height (May use MATCH_PARENT/WRAP_CONTENT)
    };
  };

  /**
   * @brief Default constructor which provides an uninitialized Dali::LayoutItem.
   */
  LayoutItem();

  /**
   * @brief Default destructor
   */
  ~LayoutItem()=default;

  /**
   * @brief Create an initialized LayoutItem
   *
   * @param[in] handle A handle to the object that this layout for, e.g. a Control or a Visual::Base
   * @warning This is an interim function, and will be deleted when all controls have layouts
   * @todo Ensure that this warning is implemented
   */
  static LayoutItem New( Handle& handle );

  /**
   * @brief Copy constructor
   * @param[in] copy The LayoutItem to copy.
   */
  LayoutItem(const LayoutItem& copy) = default;

  /**
   * @brief Assignment operator
   * @param[in] rhs The LayoutItem to copy
   */
  LayoutItem& operator=( const LayoutItem& rhs ) = default;

  /**
   * @brief Get a handle to the control or visual this layout represents.
   *
   * @return
   */
  Handle GetOwner() const;

  /**
   * @brief Set whether this layout should be animated or not
   *
   * @param[in] animateLayout True if the layout should be animated when applied
   */
  void SetAnimateLayout( bool animateLayout );

  /**
   * @brief Predicate to determine whether this layout should be animated when applied
   *
   * @return True if the layout should be animated when applied
   */
  bool IsLayoutAnimated() const;

  /**
   * @brief Set the layout transition data
   *
   * @param[in] layoutTransitionType The type of the layout transition
   * @param[in] layoutTransitionData The layout transition data
   */
  void SetTransitionData( LayoutTransitionData::Type layoutTransitionType, LayoutTransitionData layoutTransitionData );

  /**
   * @brief Get the layout transition data
   *
   * @param[in] layoutTransitionType The type of the layout transition
   *
   * @return The layout transition data
   */
  LayoutTransitionData GetTransitionData( LayoutTransitionData::Type layoutTransitionType ) const;

public:
  /// @cond internal
  /**
   * @brief This constructor is used by LayoutItem::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit LayoutItem( Internal::LayoutItem* LayoutItem );
  /// @endcond
};


}//namespace Toolkit
}//namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_LAYOUT_ITEM_H
