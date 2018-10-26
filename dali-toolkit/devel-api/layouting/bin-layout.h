#ifndef DALI_TOOLKIT_LAYOUTING_BIN_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_BIN_LAYOUT_H

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
class BinLayout;
}

/**
 * This class implements a bin layout, providing a simple layout to accept
 * children and perform ResizePolicy mapping.
 *
 * Children will be positioned with their AnchorPoint and position if provided.
 * If multiple children are added then they could overlap.
 */
class DALI_TOOLKIT_API BinLayout : public LayoutGroup
{
public:

  /**
   * @brief Creates an uninitialized BinLayout handle.
   *
   * Initialize it using BinLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  BinLayout();

  /**
   * @brief Creates a BinLayout object.
   */
  static BinLayout New();

  /**
   * @brief Downcasts a handle to a BinLayout handle.
   *
   * If handle points to a BinLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a BinLayout or an uninitialized handle
   */
  static BinLayout DownCast( BaseHandle handle );

  /**
   * @brief Copy constructor
   */
  BinLayout( const BinLayout& other );

  /**
   * @brief Assignment operator
   */
  BinLayout& operator=( const BinLayout& other );

  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~BinLayout()=default;


public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by BinLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL BinLayout( Internal::BinLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_BIN_LAYOUT_H
