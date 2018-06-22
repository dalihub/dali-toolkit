#ifndef DALI_TOOLKIT_LAYOUTING_ABSOLUTE_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_ABSOLUTE_LAYOUT_H

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

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class AbsoluteLayout;
}

/**
 * This class implements a absolute layout, allowing explict positioning of
 * children.
 * Positions are from the top left of the layout and can be set using the
 * Actor::Property::POSITION and alike.
 */
class DALI_TOOLKIT_API AbsoluteLayout : public LayoutGroup
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = ABSOLUTE_LAYOUT_CHILD_PROPERTY_START_INDEX,
    CHILD_PROPERTY_END_INDEX   = ABSOLUTE_LAYOUT_CHILD_PROPERTY_END_INDEX
  };

  /**
   * @brief Creates an uninitialized AbsoluteLayout handle.
   *
   * Initialize it using AbsoluteLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  AbsoluteLayout();

  /**
   * @brief Creates a AbsoluteLayout object.
   */
  static AbsoluteLayout New();

  /**
   * @brief Downcasts a handle to a AbsoluteLayout handle.
   *
   * If handle points to a AbsoluteLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a AbsoluteLayout or an uninitialized handle
   */
  static AbsoluteLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  AbsoluteLayout( const AbsoluteLayout& other );

  /**
   * @brief Assigment operator
   */
  AbsoluteLayout& operator=( const AbsoluteLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~AbsoluteLayout()=default;


public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by AbsoluteLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL AbsoluteLayout( Internal::AbsoluteLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_ABSOLUTE_LAYOUT_H
