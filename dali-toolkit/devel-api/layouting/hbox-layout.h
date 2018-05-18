#ifndef DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>
#include <dali-toolkit/public-api/toolkit-property-index-ranges.h>
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class HboxLayout;
}

/**
 * This class implements a horizontal box layout, automatically handling
 * right to left or left to right direction change.
 */
class DALI_TOOLKIT_API HboxLayout : public LayoutGroup
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = LayoutGroup::CHILD_PROPERTY_START_INDEX+100,
    CHILD_PROPERTY_END_INDEX   = CHILD_PROPERTY_START_INDEX+1000
  };

  struct Property
  {
    // @todo When we can have event-only properties for BaseObject, this will be useful.
    enum
    {
      CELL_PADDING = PROPERTY_REGISTRATION_START_INDEX + 2000
    };
  };

  struct ChildProperty
  {
    enum
    {
      WEIGHT = CHILD_PROPERTY_START_INDEX
    };
  };

  /**
   * @brief Creates an uninitialized HboxLayout handle.
   *
   * Initialize it using HboxLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  HboxLayout();

  /**
   * @brief Creates a HboxLayout object.
   */
  static HboxLayout New();

  /**
   * @brief Downcasts a handle to a HboxLayout handle.
   *
   * If handle points to a HboxLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a HboxLayout or an uninitialized handle
   */
  static HboxLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  HboxLayout( const HboxLayout& other );

  /**
   * @brief Assigment operator
   */
  HboxLayout& operator=( const HboxLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~HboxLayout()=default;

  /**
   * @brief Set the padding between cells in the layout
   *
   * @param[in] size The padding between cells.
   */
  void SetCellPadding( LayoutSize size );

  /**
   * @brief Get the padding between cells in the layout
   *
   * @return The padding between cells.
   */
  LayoutSize GetCellPadding();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by HboxLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL HboxLayout( Internal::HboxLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H
