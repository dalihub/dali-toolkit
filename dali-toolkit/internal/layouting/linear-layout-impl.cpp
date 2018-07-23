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
#include "linear-layout-impl.h"

//PUBLIC INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/actors/actor.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>

namespace
{
#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LinearLayoutPtr LinearLayout::New()
{
  LinearLayoutPtr layout( new LinearLayout() );
  return layout;
}

LinearLayout::LinearLayout()
: LayoutGroup(),
  mCellPadding( 0, 0 ),
  mOrientation( Dali::Toolkit::LinearLayout::Orientation::HORIZONTAL ),
  mTotalLength( 0 )
{
}

LinearLayout::~LinearLayout()
{
}

void LinearLayout::SetCellPadding( LayoutSize size )
{
  if ( mCellPadding != size )
  {
    mCellPadding = size;
    RequestLayout();
  }
}

LayoutSize LinearLayout::GetCellPadding()
{
  return mCellPadding;
}

void LinearLayout::SetOrientation( Dali::Toolkit::LinearLayout::Orientation orientation )
{
  if ( mOrientation != orientation )
  {
    mOrientation = orientation;
    RequestLayout();
  }
}

Dali::Toolkit::LinearLayout::Orientation LinearLayout::GetOrientation()
{
  return mOrientation;
}

void LinearLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "LinearLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  if( mOrientation == Dali::Toolkit::LinearLayout::Orientation::HORIZONTAL )
  {
    MeasureHorizontal( widthMeasureSpec, heightMeasureSpec );
  }
  else
  {
    MeasureVertical( widthMeasureSpec, heightMeasureSpec );
  }
}

void LinearLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "LinearLayout::OnLayout  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "left:" << left << " top:" << top << " right:" << right << " bottom:" << bottom << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  if( mOrientation == Dali::Toolkit::LinearLayout::Orientation::HORIZONTAL )
  {
    LayoutHorizontal( left, top, right, bottom );
  }
  else
  {
    LayoutVertical( left, top, right, bottom );
  }
}

void LinearLayout::MeasureHorizontal( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
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

  // Reset total length
  mTotalLength = 0;

  // measure children, and determine if further resolution is required
  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childMargin = childLayout->GetMargin();

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LinearLayout::OnMeasure childWidth(%d)\n", MeasureSpec::IntType( childWidth ) );

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

void LinearLayout::ForceUniformHeight( int count, MeasureSpec widthMeasureSpec )
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

void LinearLayout::LayoutHorizontal( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
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
  auto childSpace = height - (int)padding.top - (int)padding.bottom;

  auto count = GetChildCount();

  int start = 0;
  int dir = 1;

  // In case of RTL, start drawing from the last child and apply right alignment.
  // @TODO Should we have also support Actor HorizontalAlignment|VerticalAlignment in general for LinearLayout?
  if( isLayoutRtl ) {
    start = count - 1;
    dir = -1;
    childLeft = padding.start + right - left - mTotalLength;
  }

  for( unsigned int i = 0; i < count; i++)
  {
    int childIndex = start + dir * i;
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();
      auto childMargin = childLayout->GetMargin();

      childTop = LayoutLength(padding.top) + (int)((childSpace - childHeight) / 2) + (int)childMargin.top - (int)childMargin.bottom;

      childLeft += childMargin.start;
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childLeft += childWidth + (int)childMargin.end + mCellPadding.width;
    }
  }
}

void LinearLayout::MeasureVertical( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  auto widthMode = widthMeasureSpec.GetMode();

  bool matchWidth = false;
  bool allFillParent = true;
  LayoutLength maxWidth = 0;
  LayoutLength alternativeMaxWidth = 0;

  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // Reset total length
  mTotalLength = 0;

  // measure children, and determine if further resolution is required
  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );

      MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, heightMeasureSpec, 0 );
      auto childHeight = childLayout->GetMeasuredHeight();
      auto childMargin = childLayout->GetMargin();
      auto length = childHeight + LayoutLength::IntType(childMargin.top + childMargin.bottom );

      auto cellPadding = i<GetChildCount()-1 ? mCellPadding.height : 0;
      auto totalLength = mTotalLength;
      mTotalLength = std::max( totalLength, totalLength + length + cellPadding);

      bool matchWidthLocally = false;
      if( widthMode != MeasureSpec::Mode::EXACTLY && desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchWidth = true;
        matchWidthLocally = true;
      }

      auto marginWidth = LayoutLength( childMargin.start + childMargin.end );
      auto childWidth = childLayout->GetMeasuredWidth() + marginWidth;

      // was combineMeasuredStates()
      if( childLayout->GetMeasuredWidthAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.widthState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }
      if( childLayout->GetMeasuredHeightAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.heightState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }

      maxWidth = std::max( maxWidth, childWidth );
      allFillParent = ( allFillParent && desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT );
      alternativeMaxWidth = std::max( alternativeMaxWidth, matchWidthLocally ? marginWidth : childWidth );
    }
  }
  Extents padding = GetPadding();
  mTotalLength += padding.top + padding.bottom;
  auto heightSize = mTotalLength;
  heightSize = std::max( heightSize, GetSuggestedMinimumHeight() );
  MeasuredSize heightSizeAndState = ResolveSizeAndState( heightSize, heightMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  heightSize = heightSizeAndState.GetSize();

  if( !allFillParent && widthMode != MeasureSpec::Mode::EXACTLY )
  {
    maxWidth = alternativeMaxWidth;
  }
  maxWidth += padding.start + padding.end;
  maxWidth = std::max( maxWidth, GetSuggestedMinimumWidth() );

  heightSizeAndState.SetState( childState.heightState );

  SetMeasuredDimensions( ResolveSizeAndState( maxWidth, widthMeasureSpec, childState.widthState ),
                         heightSizeAndState );

  if( matchWidth )
  {
    ForceUniformWidth( GetChildCount(), heightMeasureSpec );
  }
}

void LinearLayout::ForceUniformWidth( int count, MeasureSpec heightMeasureSpec )
{
  // Pretend that the linear layout has an exact size.
  auto uniformMeasureSpec = MeasureSpec( GetMeasuredWidth(), MeasureSpec::Mode::EXACTLY );
  for (int i = 0; i < count; ++i)
  {
    LayoutItemPtr childLayout = GetChildAt(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured height
        int oldHeight = desiredHeight;
        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, childLayout->GetMeasuredHeight().mValue );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, uniformMeasureSpec, 0, heightMeasureSpec, 0 );

        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, oldHeight );
      }
    }
  }
}

void LinearLayout::LayoutVertical( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  Extents padding = GetPadding();

  LayoutLength childTop( 0 );
  LayoutLength childLeft( padding.start );

  // Where bottom of child should go
  auto width = right - left;

  // Space available for child
  auto childSpace = width - (int)padding.start - (int)padding.end;
  auto count = GetChildCount();

  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();
      auto childMargin = childLayout->GetMargin();

      childTop += childMargin.top;
      childLeft = ( childSpace - childWidth ) / 2 + (int)childMargin.start - (int)childMargin.end;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childTop += childHeight + (int)childMargin.bottom + mCellPadding.height;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
