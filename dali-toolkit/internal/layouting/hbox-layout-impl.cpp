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
#include <dali/public-api/actors/actor.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>


#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
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
  return layout;
}

HboxLayout::HboxLayout()
: LayoutGroup(),
  mCellPadding( 0, 0 ),
  mTotalLength( 0 )
{
}

HboxLayout::~HboxLayout()
{
}

void HboxLayout::DoInitialize()
{
}

void HboxLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::HboxLayout::ChildProperty::WEIGHT ) == indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight",
                                 Toolkit::HboxLayout::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void HboxLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  owner.SetProperty( Toolkit::HboxLayout::ChildProperty::WEIGHT, 1.0f );
}

void HboxLayout::SetCellPadding( LayoutSize size )
{
  mCellPadding = size;
}

LayoutSize HboxLayout::GetCellPadding()
{
  return mCellPadding;
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
  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, heightMeasureSpec, 0 );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );
      auto length = childWidth + LayoutLength::IntType(childMargin.start + childMargin.end);

      auto cellPadding = i<GetChildCount()-1 ? mCellPadding.width: 0;

      if( isExactly )
      {
        mTotalLength += length;
      }
      else
      {
        auto totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + length + cellPadding );
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
    LayoutItemPtr childLayout = GetChildAt(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured width
        int oldWidth = desiredWidth;
        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, childLayout->GetMeasuredWidth().mValue );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, uniformMeasureSpec, 0);

        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, oldWidth );
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

  // In case of RTL, start drawing from the last child.
  // @todo re-work to draw the first child from the right edge, and move leftwards.
  // (Should have an alignment also)
  if( isLayoutRtl ) {
    start = count - 1;
    dir = -1;
  }

  for( unsigned int i = 0; i < count; i++)
  {
    int childIndex = start + dir * i;
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      auto childOwner = childLayout->GetOwner();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );

      childTop = LayoutLength(padding.top) + ((childSpace - childHeight) / 2) + childMargin.top - childMargin.bottom;

      childLeft += childMargin.start;
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childLeft += childWidth + childMargin.end + mCellPadding.width;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
