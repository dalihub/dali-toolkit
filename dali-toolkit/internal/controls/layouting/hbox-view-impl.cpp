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

//CLASS HEADER
#include <dali-toolkit/internal/controls/layouting/hbox-view-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/layouting/hbox-layout-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/devel-api/layouting/margin-layout-data.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/margin-layout-data-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

Dali::Toolkit::HboxView HboxView::New()
{
  HboxViewPtr object( new HboxView() );
  Dali::Toolkit::HboxView handle( *object );
  object->Initialize();
  return handle;
}

void HboxView::AddChild( Actor child )
{
  child.Unparent();
  Self().Add( child );

  LayoutBasePtr childLayout;
  Toolkit::Control control = Toolkit::Control::DownCast( child );
  if( control )
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    childLayout = childControlDataImpl.GetLayout();

    if( ! childLayout )
    {
      childLayout = LayoutBase::New( &childControlImpl );

      CustomActorImpl& customActor = static_cast< CustomActorImpl& >( childControlImpl );
      auto desiredSize = customActor.GetNaturalSize();

      // HBoxLayout will apply default layout data for this object
      auto layoutData = MarginLayoutData::New( int( desiredSize.width ), int( desiredSize.height ), 0, 0, 0, 0 );
      childLayout->SetLayoutData( layoutData );
      childControlDataImpl.SetLayout( *childLayout.Get() );
    }
  }

  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( *this );
  auto layout = controlDataImpl.GetLayout();
  auto layoutGroup = static_cast<LayoutGroup*>(layout.Get());
  layoutGroup->Add( *childLayout.Get() );
}

Actor HboxView::GetChildAt( Dali::Toolkit::HboxView::CellPosition position )
{
  return Actor(0);
}

Actor HboxView::RemoveChildAt( Dali::Toolkit::HboxView::CellPosition position )
{
  return Actor(0);
}

void HboxView::SetCellPadding( LayoutSize size )
{
  GetLayout().SetCellPadding( size );
}

LayoutSize HboxView::GetCellPadding()
{
  return GetLayout().GetCellPadding();
}

void HboxView::SetMode( Dali::Toolkit::HboxView::Mode mode )
{
  GetLayout().SetMode( mode );
}

Dali::Toolkit::HboxView::Mode HboxView::GetMode()
{
  return GetLayout().GetMode();
}

void HboxView::SetCellWidth( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength width )
{
}

void HboxView::SetCellHeight( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength height )
{
}

void HboxView::OnInitialize()
{
  IntrusivePtr<RefObject> handle( static_cast<RefObject*>(this) );

  auto layout = Internal::HboxLayout::New( handle );
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( *this );
  controlDataImpl.SetLayout( *layout.Get() );
}

HboxView::HboxView()
: Control(ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT | DISABLE_SIZE_NEGOTIATION))
{
}

HboxView::~HboxView()
{
}

Toolkit::HboxLayout HboxView::GetLayout()
{
  Control::Impl& controlDataImpl = Control::Impl::Get( *this );
  auto layout = Toolkit::LayoutBase( controlDataImpl.GetLayout().Get() );

  if( layout )
  {
    auto hboxLayout = Toolkit::HboxLayout::DownCast( layout );
    DALI_ASSERT_ALWAYS( hboxLayout && "HboxLayout handle is null" );
    return hboxLayout;
  }
  return Toolkit::HboxLayout();
}

Toolkit::MarginLayoutData HboxView::GetLayoutData()
{
  Control::Impl& controlDataImpl = Control::Impl::Get( *this );
  auto layout = Toolkit::LayoutBase( controlDataImpl.GetLayout().Get() );
  DALI_ASSERT_ALWAYS( layout && "No layout present" );

  auto hboxLayout = Toolkit::HboxLayout::DownCast( layout );
  auto handle = Toolkit::MarginLayoutData::DownCast( hboxLayout.GetLayoutData() );
  return handle;
}

void HboxView::SetLayoutData( Toolkit::MarginLayoutData handle )
{
  Control::Impl& controlDataImpl = Control::Impl::Get( *this );
  auto layout = Toolkit::LayoutBase( controlDataImpl.GetLayout().Get() );
  DALI_ASSERT_ALWAYS( layout && "No layout present" );

  layout.SetLayoutData( handle );
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
