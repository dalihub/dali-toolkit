#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_FBOX_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_FBOX_LAYOUT_H

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
#include <dali-toolkit/devel-api/layouting/fbox-layout.h>
#include <dali-toolkit/third-party/yoga/Yoga.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class FboxLayout;
using FboxLayoutPtr = IntrusivePtr<FboxLayout>;

class FboxLayout final : public LayoutGroup
{
public:
  static FboxLayoutPtr New();

public:
  void SetFlexDirection( Dali::Toolkit::FboxLayout::FlexDirection flexDirection );
  Dali::Toolkit::FboxLayout::FlexDirection GetFlexDirection();
  void SetFlexJustification( Dali::Toolkit::FboxLayout::Justification flexJustification );
  Dali::Toolkit::FboxLayout::Justification GetFlexJustification();
  void SetFlexWrap( Dali::Toolkit::FboxLayout::WrapType flexWrap );
  Dali::Toolkit::FboxLayout::WrapType GetFlexWrap();
  void SetFlexAlignment( Dali::Toolkit::FboxLayout::Alignment flexAlignment );
  Dali::Toolkit::FboxLayout::Alignment GetFlexAlignment();

protected:
  /**
   * Constructor
   */
  FboxLayout();

  /**
   * Virtual destructor
   */
  virtual ~FboxLayout();

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
  FboxLayout( const FboxLayout& other ) = delete;
  FboxLayout& operator=( const FboxLayout& other ) = delete;

  static YGSize OnMeasure( YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode );
  void SetChildrenStyle();

private:
  YGNodeRef mRoot;
};

} // namespace Internal

inline Internal::FboxLayout& GetImplementation( Dali::Toolkit::FboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "FboxLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::FboxLayout&>( object );
}

inline const Internal::FboxLayout& GetImplementation( const Dali::Toolkit::FboxLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "FboxLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::FboxLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_FBOX_LAYOUT_H
