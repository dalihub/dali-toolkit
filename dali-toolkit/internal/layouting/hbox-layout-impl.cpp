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
#include <dali-toolkit/internal/layouting/hbox-layout-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali-toolkit/devel-api/layouting/layout-base.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>


#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

HboxLayoutPtr HboxLayout::New()
{
  HboxLayoutPtr layout( new HboxLayout() );
  //layout->Initialize( owner );

  auto layoutController = Toolkit::LayoutController::Get();
  layoutController.RegisterLayout( Toolkit::HboxLayout(layout.Get()) );

  return layout;
}

HboxLayout::HboxLayout()
: LayoutGroup(),
  mCellPadding( 20, 20 ),
  mTotalLength( 0 ),
  mSlotDelegate( this )
{
}

HboxLayout::~HboxLayout()
{
}

void HboxLayout::DoInitialize()
{
  auto control = Toolkit::Control::DownCast( GetOwner() );
  if( control )
  {
    DevelActor::ChildAddedSignal( control ).Connect( mSlotDelegate, &HboxLayout::ChildAddedToOwner );
    DevelActor::ChildRemovedSignal( control ).Connect( mSlotDelegate, &HboxLayout::ChildRemovedFromOwner );
    DevelHandle::PropertySetSignal( control ).Connect( mSlotDelegate, &HboxLayout::OnOwnerPropertySet );
  }
}

void HboxLayout::DoRegisterChildProperties( const std::string& containerType )
{
  // Must chain up
  LayoutGroup::DoRegisterChildProperties( containerType );

  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::HboxLayout::ChildProperty::WEIGHT ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight", Toolkit::HboxLayout::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void HboxLayout::SetCellPadding( LayoutSize size )
{
  mCellPadding = size;
}

LayoutSize HboxLayout::GetCellPadding()
{
  return mCellPadding;
}

void HboxLayout::ChildAddedToOwner( Actor child )
{
  LayoutBasePtr childLayout;
  Toolkit::Control control = Toolkit::Control::DownCast( child );

  if( control ) // Can only support adding Controls, not Actors to layout
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    childLayout = childControlDataImpl.GetLayout();

    if( ! childLayout )
    {
      // If the child doesn't already have a layout, then create a LayoutBase for it. (@todo:Why?)
      childLayout = LayoutBase::New( control );
      auto desiredSize = control.GetNaturalSize();
      childControlDataImpl.SetLayout( *childLayout.Get() );

      // HBoxLayout will apply default layout data for this object
      child.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, desiredSize.width );
      child.SetProperty( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION, desiredSize.height );
      child.SetProperty( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION, Extents() );
    }

    Add( *childLayout.Get() );
  }
}

void HboxLayout::ChildRemovedFromOwner( Actor child )
{
  Toolkit::Control control = Toolkit::Control::DownCast( child );
  if( control )
  {
    Internal::Control& childControlImpl = GetImplementation( control );
    Internal::Control::Impl& childControlDataImpl = Internal::Control::Impl::Get( childControlImpl );
    auto childLayout = childControlDataImpl.GetLayout();
    if( childLayout )
    {
      Remove( *childLayout.Get() );
    }
  }
}

void HboxLayout::OnOwnerPropertySet( Handle& handle, Property::Index index, Property::Value value )
{
  auto actor = Actor::DownCast( handle );
  if( actor && index == Actor::Property::LAYOUT_DIRECTION )
  {
    RequestLayout();
  }
}

void HboxLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "HBoxLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
  bool isExactly = (widthMode == MeasureSpec::Mode::EXACTLY);
  bool matchHeight = false;
  bool allFillParent = true;
  LayoutLength maxHeight = 0;
  LayoutLength alternativeMaxHeight = 0;
  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // measure children, and determine if further resolution is required
  for( unsigned int i=0; i<GetChildCount(); ++i ) // @todo assumes layoutId is contiguous... probably need an iterator
  {
    auto childLayout = GetChild( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, heightMeasureSpec, 0 );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );
      auto length = childWidth + LayoutLength::IntType(childMargin.start + childMargin.end);

      if( isExactly )
      {
        mTotalLength += length;
      }
      else
      {
        auto totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + length );
      }

      bool matchHeightLocally = false;
      if( heightMode != MeasureSpec::Mode::EXACTLY && desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchHeight = true;
        matchHeightLocally = true;
      }

      auto marginHeight = LayoutLength( childMargin.top + childMargin.bottom );
      auto childHeight = childLayout->GetMeasuredHeight() + marginHeight;

      if( childLayout->GetMeasuredWidthAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.widthState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }
      if( childLayout->GetMeasuredHeightAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.heightState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }

      maxHeight = std::max( maxHeight, childHeight );
      allFillParent = ( allFillParent && desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT );
      alternativeMaxHeight = std::max( alternativeMaxHeight, matchHeightLocally ? marginHeight : childHeight );
    }
  }

  Extents padding = GetPadding();
  mTotalLength += padding.start + padding.end;
  auto widthSize = mTotalLength;
  widthSize = std::max( widthSize, GetSuggestedMinimumWidth() );
  MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  widthSize = widthSizeAndState.GetSize();

  if( !allFillParent && heightMode != MeasureSpec::Mode::EXACTLY )
  {
    maxHeight = alternativeMaxHeight;
  }
  maxHeight += padding.top + padding.bottom;
  maxHeight = std::max( maxHeight, GetSuggestedMinimumHeight() );

  widthSizeAndState.SetState( childState.widthState );

  SetMeasuredDimensions( widthSizeAndState,
                         ResolveSizeAndState( maxHeight, heightMeasureSpec, childState.heightState ) );

  if( matchHeight )
  {
    ForceUniformHeight( GetChildCount(), widthMeasureSpec );
  }
}

void HboxLayout::ForceUniformHeight( int count, MeasureSpec widthMeasureSpec )
{
  // Pretend that the linear layout has an exact size. This is the measured height of
  // ourselves. The measured height should be the max height of the children, changed
  // to accommodate the heightMeasureSpec from the parent
  auto uniformMeasureSpec = MeasureSpec( GetMeasuredHeight(), MeasureSpec::Mode::EXACTLY );
  for (int i = 0; i < count; ++i)
  {
    LayoutBasePtr childLayout = GetChild(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured width
        int oldWidth = desiredWidth;
        childOwner.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, childLayout->GetMeasuredWidth().mValue );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, uniformMeasureSpec, 0);

        childOwner.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, oldWidth );
      }
    }
  }
}

void HboxLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  bool isLayoutRtl = actor ? actor.GetProperty<bool>( Actor::Property::LAYOUT_DIRECTION ) : false;

  Extents padding = GetPadding();

  LayoutLength childTop( 0 );
  LayoutLength childLeft( padding.start );

  // Where bottom of child should go
  auto height = bottom - top;

  // Space available for child
  auto childSpace = height - padding.top - padding.bottom;

  auto count = GetChildCount();

  int start = 0;
  int dir = 1;

  //In case of RTL, start drawing from the last child.
  if (isLayoutRtl) {
    start = count - 1;
    dir = -1;
  }

  for( unsigned int i = 0; i < count; i++)
  {
    int childIndex = start + dir * i;
    LayoutBasePtr childLayout = GetChild( childIndex );
    if( childLayout != nullptr )
    {
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      auto childOwner = childLayout->GetOwner();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );

      childTop = LayoutLength(padding.top) + ((childSpace - childHeight) / 2) + childMargin.top - childMargin.bottom;

      childLeft += childMargin.start;
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childLeft += childWidth + childMargin.end;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
