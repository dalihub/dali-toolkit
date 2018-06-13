#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_FLEX_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_FLEX_LAYOUT_H

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

//EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/layouting/flex-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/third-party/yoga/Yoga.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class FlexLayout;
using FlexLayoutPtr = IntrusivePtr<FlexLayout>;

class FlexLayout final : public LayoutGroup
{
public:
  static FlexLayoutPtr New();

public:
  /**
   * @copydoc FlexLayout::SetFlexDirection
   */
  void SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection flexDirection );

  /**
   * @copydoc FlexLayout::GetFlexDirection
   */
  Dali::Toolkit::FlexLayout::FlexDirection GetFlexDirection() const;

  /**
   * @copydoc FlexLayout::SetFlexJustification
   */
  void SetFlexJustification( Dali::Toolkit::FlexLayout::Justification flexJustification );

  /**
   * @copydoc FlexLayout::GetFlexJustification
   */
  Dali::Toolkit::FlexLayout::Justification GetFlexJustification() const;

  /**
   * @copydoc FlexLayout::SetFlexWrap
   */
  void SetFlexWrap( Dali::Toolkit::FlexLayout::WrapType wrap );

  /**
   * @copydoc FlexLayout::GetFlexWrap
   */
  Dali::Toolkit::FlexLayout::WrapType GetFlexWrap() const;

  /**
   * @copydoc FlexLayout::SetFlexAlignment
   */
  void SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment );
  /**
   * @copydoc FlexLayout::GetFlexAlignment
   */
  Dali::Toolkit::FlexLayout::Alignment::Type GetFlexAlignment() const;

  /**
   * @copydoc FlexLayout::SetFlexItemsAlignment
   */
  void SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment );

  /**
   * @copydoc FlexLayout::GetFlexItemsAlignment
   */
  Dali::Toolkit::FlexLayout::Alignment::Type GetFlexItemsAlignment() const;

protected:
  /**
   * Constructor
   */
  FlexLayout();

  /**
   * Virtual destructor
   */
  virtual ~FlexLayout();

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
   * @copydoc LayoutItem::OnChildRemove
   */
  virtual void OnChildRemove( LayoutItem& child ) override;

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:
  FlexLayout( const FlexLayout& other ) = delete;
  FlexLayout& operator=( const FlexLayout& other ) = delete;

  static YGSize OnChildMeasure( YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode );
  void SetChildrenStyle();

private:
  YGNodeRef mRoot;
};

} // namespace Internal

inline Internal::FlexLayout& GetImplementation( Dali::Toolkit::FlexLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "FboxLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::FlexLayout&>( object );
}

inline const Internal::FlexLayout& GetImplementation( const Dali::Toolkit::FlexLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "FboxLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::FlexLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_FLEX_LAYOUT_H
