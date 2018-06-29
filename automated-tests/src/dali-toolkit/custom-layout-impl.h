#ifndef TEST_INTERNAL_CUSTOM_LAYOUT_H
#define TEST_INTERNAL_CUSTOM_LAYOUT_H

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

// INTERNAL INCLUDES
#include "custom-layout.h"

namespace Test
{

namespace Internal
{

using Dali::Toolkit::MeasureSpec;
using Dali::Toolkit::LayoutLength;

class CustomLayout;
using CustomLayoutPtr = Dali::IntrusivePtr< CustomLayout >;

/**
 * @brief The implementation of our custom layout.
 *
 * Here we will override the methods that we require to mimic a very simple horizontal layout.
 */
class CustomLayout final : public Dali::Toolkit::Internal::LayoutGroup
{
public:

  /**
   * @brief Create a new CustomLayout object.
   * @return An intrusive pointer to the created CustomLayout object
   */
  static CustomLayoutPtr New();

  // Movable but not copyable
  CustomLayout( const CustomLayout& other ) = delete;
  CustomLayout& operator=( const CustomLayout& ) = delete;
  CustomLayout( CustomLayout&& other ) = default;
  CustomLayout& operator=( CustomLayout&& other ) = default;

private:

  /**
   * @brief Default Constructor
   */
  CustomLayout() = default;

  /**
   * Virtual Destructor
   */
  virtual ~CustomLayout() = default;

  /**
   * @copydoc LayoutItem::OnMeasure
   *
   * Overriding this method so that we can calculate the size we require using our children's sizes
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, Dali::Toolkit::MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   *
   * Overriding this method so that we can layout our children as required.
   */
  virtual void OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom ) override;

};

} // namespace Internal

inline Internal::CustomLayout& GetImplementation( Test::CustomLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "CustomLayout handle is empty" );
  Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::CustomLayout&>( object );
}

inline const Internal::CustomLayout& GetImplementation( const Test::CustomLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "CustomLayout handle is empty" );
  const Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::CustomLayout&>( object );
}

} // namespace Test

#endif // TEST_INTERNAL_CUSTOM_LAYOUT_H
