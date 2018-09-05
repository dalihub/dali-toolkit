#ifndef TEST_CUSTOM_LAYOUT_H
#define TEST_CUSTOM_LAYOUT_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>

namespace Test
{

namespace Internal
{
class CustomLayout;
}

/**
 * @brief This is the handle class to a very simple Custom Layout.
 *
 * This class lays out its children horizontally in a very simple manner.
 */
class CustomLayout : public Dali::Toolkit::LayoutGroup
{
public:

  // Behaviour flags
  static const int BEHAVIOUR_FLAG_UNCONSTRAINED_CHILD_WIDTH                = 0x00000001;  // Child width measured without constraint
  static const int BEHAVIOUR_FLAG_UNCONSTRAINED_CHILD_HEIGHT               = 0x00000002;  // Child height measured without constraint

  /**
   * @brief Creates an uninitialized CustomLayout handle.
   *
   * Initialize it using CustomLayout::New().
   * Calling member functions with an uninitialized handle is not allowed.
   */
  CustomLayout() = default;

  /**
   * @brief Creates a CustomLayout object.
   */
  static CustomLayout New();


  /**
   * @brief Default destructor.
   *
   * This is non-virtual, since derived Handle types must not contain data or virtual methods
   */
  ~CustomLayout() = default;

  /**
   * @brief Copy constructor
   */
  CustomLayout( const CustomLayout& ) = default;

  /**
   * @brief Assigment operator
   */
  CustomLayout& operator=( const CustomLayout& ) = default;

  /**
   * @brief Move constructor
   */
  CustomLayout( CustomLayout&& ) = default;

  /**
   * @brief Movable assignment operator
   */
  CustomLayout& operator=( CustomLayout&& ) = default;

  /**
   * @brief Downcasts a handle to a CustomLayout handle.
   *
   * If handle points to a CustomLayout, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.

   * @param[in] handle to an object
   * @return Handle to a CustomLayout or an uninitialized handle
   */
  static CustomLayout DownCast( BaseHandle handle );

  /**
   * Request for a relayout of this layout
   */
  void RequestLayout();

  /**
   * Enables setting of flags corresponding to particular behaviour of this layout
   * @param[in] flag the flag to set
   */
  void SetCustomBehaviourFlag( int flag );

  /**
   * Returns true or false depending on whether flag is set
   * @return bool returns true if flag set
   */
  bool GetCustomBehaviourFlags( int flagToCheck );

  /**
   * Clears the flag if already set.
   * @param[in] flag the flag to clear
   */
  void ClearPrivateFlag( int flag );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by CustomLayout::New() methods.
   *
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  explicit CustomLayout( Internal::CustomLayout* body );
  /// @endcond
};

} // namespace Test

#endif // TEST_CUSTOM_LAYOUT_H
