#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_HBOX_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_HBOX_LAYOUT_H

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
#include <dali-toolkit/devel-api/layouting/hbox-layout.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class HboxLayout;
using HboxLayoutPtr = IntrusivePtr<HboxLayout>;

class HboxLayout final : public LayoutGroup
{
public:
  static HboxLayoutPtr New();

public:
  void SetCellPadding( LayoutSize size );
  LayoutSize GetCellPadding();

protected:
  HboxLayout();
  virtual ~HboxLayout();

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
  HboxLayout( const HboxLayout& other ) = delete;
  HboxLayout& operator=( const HboxLayout& other ) = delete;

  void ForceUniformHeight( int count, MeasureSpec widthMeasureSpec );

private:
  LayoutSize mCellPadding;
  LayoutLength mTotalLength;
};

} // namespace Internal

inline Internal::HboxLayout& GetImplementation( Dali::Toolkit::HboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "HboxLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::HboxLayout&>( object );
}

inline const Internal::HboxLayout& GetImplementation( const Dali::Toolkit::HboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "HboxLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::HboxLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_HBOX_LAYOUT_H
