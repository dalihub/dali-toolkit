#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_VBOX_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_VBOX_LAYOUT_H

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

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/devel-api/layouting/vbox-layout.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class VboxLayout;
using VboxLayoutPtr = IntrusivePtr<VboxLayout>;

class VboxLayout final : public LayoutGroup
{
public:
  static VboxLayoutPtr New();

public:

  /**
   * Set the inter-cell padding
   * @param[in] size The size of the inter-cell padding
   */
  void SetCellPadding( LayoutSize size );

  /**
   * Get the inter-cell padding
   * @return The size of the inter-cell padding
   */
  LayoutSize GetCellPadding();

protected:
  /**
   * Constructor
   */
  VboxLayout();

  /**
   * Virtual destructor
   */
  virtual ~VboxLayout();

  /**
   * @copydoc LayoutItem::DoInitialize
   */
  virtual void DoInitialize() override;

  /**
   * @copydoc LayoutItem::DoRegisterChildProperties()
   */
  virtual void DoRegisterChildProperties( const std::string& containerType ) override;

  /**
   * @copydoc LayoutItem::OnChildAdd
   */
  virtual void OnChildAdd( LayoutItem& child ) override;

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:
  VboxLayout( const VboxLayout& other ) = delete;
  VboxLayout& operator=( const VboxLayout& other ) = delete;

  /**
   * Apply a uniform width to the children
   */
  void ForceUniformWidth( int count, MeasureSpec heightMeasureSpec );

private:
  LayoutSize mCellPadding;
  LayoutLength mTotalLength;
};

} // namespace Internal

inline Internal::VboxLayout& GetImplementation( Dali::Toolkit::VboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "VboxLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::VboxLayout&>( object );
}

inline const Internal::VboxLayout& GetImplementation( const Dali::Toolkit::VboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "VboxLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::VboxLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_VBOX_LAYOUT_H
