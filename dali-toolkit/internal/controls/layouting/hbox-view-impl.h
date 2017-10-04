#ifndef DALI_TOOLKIT_INTERNAL_CONTROL_HBOX_VIEW_H
#define DALI_TOOLKIT_INTERNAL_CONTROL_HBOX_VIEW_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

#include <dali-toolkit/devel-api/controls/layouting/hbox-view.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class HboxView;
using HboxViewPtr = IntrusivePtr<HboxView>;

class HboxView : public Control
{
public:
  static Dali::Toolkit::HboxView New();

public:
  void AddChild( Actor child );
  Actor GetChildAt( Dali::Toolkit::HboxView::CellPosition position );
  Actor RemoveChildAt( Dali::Toolkit::HboxView::CellPosition position );
  void SetCellPadding( LayoutSize size );
  LayoutSize GetCellPadding();
  void SetMode( Dali::Toolkit::HboxView::Mode mode );
  void SetCellWidth( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength width );
  void SetCellHeight( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength height );

protected:
  HboxView();
  virtual ~HboxView();

private:
  HboxView( const HboxView& other ) = delete;
  HboxView& operator=( const HboxView& other ) = delete;

private:
  Uint16Pair mSize;

};

} // namespace Internal

inline Internal::HboxView& GetImpl( Dali::Toolkit::HboxView& handle )
{
  DALI_ASSERT_ALWAYS( handle && "HboxView handle is empty" );
  Dali::RefObject& object = handle.GetImplementation();
  return static_cast<Internal::HboxView&>( object );
}

inline const Internal::HboxView& GetImpl( const Dali::Toolkit::HboxView& handle )
{
  DALI_ASSERT_ALWAYS( handle && "HboxView handle is empty" );
  const Dali::RefObject& object = handle.GetImplementation();
  return static_cast<const Internal::HboxView&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_CONTROL_HBOX_VIEW_H
