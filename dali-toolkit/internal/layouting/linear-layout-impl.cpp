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
#include <dali/devel-api/object/handle-devel.h>

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

const int HORIZONTAL_ALIGNMENT_MASK  = ( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL | Dali::Toolkit::LinearLayout::Alignment::END );
const int VERTICAL_ALIGNMENT_MASK = ( Dali::Toolkit::LinearLayout::Alignment::TOP | Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL | Dali::Toolkit::LinearLayout::Alignment::BOTTOM );

LinearLayoutPtr LinearLayout::New()
{
  LinearLayoutPtr layout( new LinearLayout() );
  return layout;
}

LinearLayout::LinearLayout()
: LayoutGroup(),
  mCellPadding( 0, 0 ),
  mOrientation( Dali::Toolkit::LinearLayout::Orientation::HORIZONTAL ),
  mAlignment( Dali::Toolkit::LinearLayout::Alignment::BEGIN | Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL ),
  mTotalLength( 0 )
{
}

LinearLayout::~LinearLayout()
{
}

void LinearLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::LinearLayout::ChildProperty::WEIGHT ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight", Toolkit::LinearLayout::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void LinearLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  if( !DevelHandle::DoesCustomPropertyExist( owner, Toolkit::LinearLayout::ChildProperty::WEIGHT ) )
  {
    owner.SetProperty( Toolkit::LinearLayout::ChildProperty::WEIGHT, 0.0f );
  }
}

void LinearLayout::SetCellPadding( LayoutSize size )
{
  if ( mCellPadding != size )
  {
    mCellPadding = size;
    RequestLayout();
  }
}

LayoutSize LinearLayout::GetCellPadding() const
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

Dali::Toolkit::LinearLayout::Orientation LinearLayout::GetOrientation() const
{
  return mOrientation;
}

void LinearLayout::SetAlignment( unsigned int alignment )
{
  if ( mAlignment != alignment )
  {
    mAlignment = alignment;
    RequestLayout();
  }
}

unsigned int LinearLayout::GetAlignment() const
{
  return mAlignment;
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
  bool isExactly = ( widthMode == MeasureSpec::Mode::EXACTLY );
  bool matchHeight = false;
  bool allFillParent = true;
  LayoutLength maxHeight = 0;
  LayoutLength alternativeMaxHeight = 0;
  LayoutLength weightedMaxHeight = 0;
  float totalWeight = 0;
  LayoutLength usedExcessSpace = 0;
  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // Reset total length
  mTotalLength = 0;

  // measure children, and determine if further resolution is required

  // 1st phase:
  // We cycle through all children and measure children with weight 0 (non weighted children) according to their specs
  // to accumulate total used space in mTotalLength based on measured sizes and margins.
  // Weighted children are not measured at this phase.
  // Available space for weighted children will be calculated in the phase 2 based on mTotalLength value.
  for( unsigned int i = 0; i < GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      float childWeight = childOwner.GetProperty<float>( Toolkit::LinearLayout::ChildProperty::WEIGHT );
      Extents childMargin = childLayout->GetMargin();

      totalWeight += childWeight;

      const bool useExcessSpace = desiredWidth == 0 && childWeight > 0;
      if( isExactly && useExcessSpace )
      {
        mTotalLength += childMargin.start + childMargin.end;
      }
      else
      {
        LayoutLength childWidth = 0;
        if( useExcessSpace )
        {
          // The widthMode is either UNSPECIFIED or AT_MOST, and
          // this child is only laid out using excess space. Measure
          // using WRAP_CONTENT so that we can find out the view's
          // optimal width.
          Extents padding = GetPadding();
          const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( widthMeasureSpec, padding.start + padding.end, Toolkit::ChildLayoutData::WRAP_CONTENT );
          const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( heightMeasureSpec, padding.top + padding.bottom, desiredHeight );
          childLayout->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
          childWidth = childLayout->GetMeasuredWidth();
          usedExcessSpace += childWidth;
        }
        else
        {
          MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
          childWidth = childLayout->GetMeasuredWidth();
        }

        DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LinearLayout::OnMeasure childWidth(" << childWidth << ")\n" );
        LayoutLength length = childWidth + childMargin.start + childMargin.end;
        LayoutLength cellPadding = i < GetChildCount() - 1 ? mCellPadding.width : 0;
        if( isExactly )
        {
          mTotalLength += length;
        }
        else
        {
          LayoutLength totalLength = mTotalLength;
          mTotalLength = std::max( totalLength, totalLength + length + cellPadding );
        }
      }

      bool matchHeightLocally = false;
      if( heightMode != MeasureSpec::Mode::EXACTLY && desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchHeight = true;
        matchHeightLocally = true;
      }

      LayoutLength marginHeight = childMargin.top + childMargin.bottom;
      LayoutLength childHeight = childLayout->GetMeasuredHeight() + marginHeight;

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
      if( childWeight > 0 )
      {
        /*
         * Heights of weighted Views are bogus if we end up
         * remeasuring, so keep them separate.
         */
        weightedMaxHeight = std::max( weightedMaxHeight, matchHeightLocally ? marginHeight : childHeight );
      }
      else
      {
        alternativeMaxHeight = std::max( alternativeMaxHeight, matchHeightLocally ? marginHeight : childHeight );
      }
    }
  }

  Extents padding = GetPadding();
  mTotalLength += padding.start + padding.end;
  LayoutLength widthSize = mTotalLength;
  widthSize = std::max( widthSize, GetSuggestedMinimumWidth() );
  MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  widthSize = widthSizeAndState.GetSize();

  // Expand children with weight to take up available space
  // 2nd phase:
  // We cycle through weighted children now (children with weight > 0).
  // The children are measured with exact size equal to their share of the available space based on their weights.
  // mTotalLength is updated to include weighted children measured sizes.
  LayoutLength remainingExcess = widthSize - mTotalLength + usedExcessSpace;
  if( remainingExcess != 0 && totalWeight > 0 )
  {
    float remainingWeightSum = totalWeight;
    maxHeight = 0;
    mTotalLength = 0;

    for( unsigned int i = 0; i < GetChildCount(); ++i )
    {
      auto childLayout = GetChildAt( i );
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
      float childWeight = childOwner.GetProperty<float>( Toolkit::LinearLayout::ChildProperty::WEIGHT );
      Extents childMargin = childLayout->GetMargin();

      LayoutLength childWidth = 0;
      if( childWeight > 0 )
      {
        LayoutLength share = ( childWeight * remainingExcess ) / remainingWeightSum;
        remainingExcess -= share;
        remainingWeightSum -= childWeight;

        // Always lay out weighted elements with intrinsic size regardless of the parent spec.
        // for consistency between specs.
        if( desiredWidth == 0 )
        {
          // This child needs to be laid out from scratch using
          // only its share of excess space.
          childWidth = share;
        }
        else
        {
          // This child had some intrinsic width to which we
          // need to add its share of excess space.
          childWidth = childLayout->GetMeasuredWidth() + share;
        }

        const MeasureSpec childWidthMeasureSpec = MeasureSpec( childWidth, MeasureSpec::Mode::EXACTLY );
        const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( heightMeasureSpec, padding.top + padding.bottom, desiredHeight );
        childLayout->Measure( childWidthMeasureSpec, childHeightMeasureSpec );

        // Child may now not fit in horizontal dimension.
        if( childLayout->GetMeasuredWidthAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
        {
          childState.widthState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
        }
      }

      LayoutLength length = childLayout->GetMeasuredWidth() + LayoutLength( childMargin.start + childMargin.end );
      LayoutLength cellPadding = i < GetChildCount() - 1 ? mCellPadding.width : 0;
      if( isExactly )
      {
        mTotalLength += length;
      }
      else
      {
        LayoutLength totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + length + cellPadding );
      }

      bool matchHeightLocally = heightMode != MeasureSpec::Mode::EXACTLY && desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT;
      LayoutLength marginHeight = childMargin.top + childMargin.bottom;
      LayoutLength childHeight = childLayout->GetMeasuredHeight() + marginHeight;

      maxHeight = std::max( maxHeight, childHeight );
      alternativeMaxHeight = std::max( alternativeMaxHeight, matchHeightLocally ? marginHeight : childHeight );
      allFillParent = (allFillParent && desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT);

      mTotalLength += padding.start + padding.end;
    }
  }
  else
  {
    alternativeMaxHeight = std::max( alternativeMaxHeight, weightedMaxHeight );
  }

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
  MeasureSpec uniformMeasureSpec( GetMeasuredHeight(), MeasureSpec::Mode::EXACTLY );
  for (int i = 0; i < count; ++i)
  {
    LayoutItemPtr childLayout = GetChildAt(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured width
        LayoutLength oldWidth = desiredWidth;
        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, childLayout->GetMeasuredWidth().AsInteger() );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, uniformMeasureSpec, 0 );

        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, oldWidth.AsInteger() );
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

  LayoutLength childTop( padding.top );
  LayoutLength childLeft( padding.start );

  // Where bottom of child should go
  LayoutLength height = bottom - top;

  // Space available for child
  LayoutLength childSpace = height - padding.top - padding.bottom;

  auto count = GetChildCount();

  switch ( mAlignment & HORIZONTAL_ALIGNMENT_MASK )
  {
    case Dali::Toolkit::LinearLayout::Alignment::BEGIN:
    default:
    {
      // mTotalLength contains the padding already
      // In case of RTL map BEGIN alignment to the right edge
      if ( isLayoutRtl )
      {
        childLeft = LayoutLength( padding.start ) + right - left - mTotalLength;
      }
      else
      {
        childLeft = LayoutLength( padding.start );
      }
      break;
    }
    case Dali::Toolkit::LinearLayout::Alignment::END:
    {
      // mTotalLength contains the padding already
      // In case of RTL map END alignment to the left edge
      if ( isLayoutRtl )
      {
        childLeft = LayoutLength( padding.start );
      }
      else
      {
        childLeft = LayoutLength( padding.start ) + right - left - mTotalLength;
      }
      break;
    }
    case Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL:
    {
      // mTotalLength contains the padding already
      childLeft = padding.start + ( right - left - mTotalLength ) / 2.0f;
      break;
    }
  }

  int start = 0;
  int dir = 1;

  // In case of RTL, start drawing from the last child.
  if( isLayoutRtl )
  {
    start = count - 1;
    dir = -1;
  }

  for( unsigned int i = 0; i < count; i++)
  {
    int childIndex = start + dir * i;
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      LayoutLength childWidth = childLayout->GetMeasuredWidth();
      LayoutLength childHeight = childLayout->GetMeasuredHeight();
      Extents childMargin = childLayout->GetMargin();

      switch ( mAlignment & VERTICAL_ALIGNMENT_MASK )
      {
        case Dali::Toolkit::LinearLayout::Alignment::TOP:
        {
          childTop = LayoutLength( padding.top ) + childMargin.top;
          break;
        }
        case Dali::Toolkit::LinearLayout::Alignment::BOTTOM:
        {
          childTop = height - padding.bottom - childHeight - childMargin.bottom;
          break;
        }
        case Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL: // FALLTHROUGH
        default:
        {
          childTop = padding.top + ( ( childSpace - childHeight ) / 2.0f ) + childMargin.top - childMargin.bottom;
          break;
        }
      }
      childLeft += childMargin.start;
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childLeft += childWidth + childMargin.end + mCellPadding.width;
    }
  }
}

void LinearLayout::MeasureVertical( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
  bool isExactly = ( heightMode == MeasureSpec::Mode::EXACTLY );

  bool matchWidth = false;
  bool allFillParent = true;
  LayoutLength maxWidth = 0;
  LayoutLength alternativeMaxWidth = 0;
  LayoutLength weightedMaxWidth = 0;
  float totalWeight = 0;
  LayoutLength usedExcessSpace = 0;
  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // Reset total length
  mTotalLength = 0;

  // measure children, and determine if further resolution is required

  // 1st phase:
  // We cycle through all children and measure children with weight 0 (non weighted children) according to their specs
  // to accumulate total used space in mTotalLength based on measured sizes and margins.
  // Weighted children are not measured at this phase.
  // Available space for weighted children will be calculated in the phase 2 based on mTotalLength value.
  for( unsigned int i = 0; i < GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
      float childWeight = childOwner.GetProperty<float>( Toolkit::LinearLayout::ChildProperty::WEIGHT );
      Extents childMargin = childLayout->GetMargin();

      totalWeight += childWeight;

      bool useExcessSpace = desiredHeight == 0 && childWeight > 0;

      if( isExactly && useExcessSpace )
      {
        LayoutLength totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + childMargin.top + childMargin.bottom );
      }
      else
      {
        LayoutLength childHeight = 0;
        if( useExcessSpace )
        {
          // The heightMode is either UNSPECIFIED or AT_MOST, and
          // this child is only laid out using excess space. Measure
          // using WRAP_CONTENT so that we can find out the view's
          // optimal height. We'll restore the original height of 0
          // after measurement.
          Extents padding = GetPadding();
          const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( widthMeasureSpec, padding.start + padding.end, desiredWidth );
          const MeasureSpec childHeightMeasureSpec = GetChildMeasureSpec( heightMeasureSpec, padding.top + padding.bottom, Toolkit::ChildLayoutData::WRAP_CONTENT );
          childLayout->Measure( childWidthMeasureSpec, childHeightMeasureSpec );
          childHeight = childLayout->GetMeasuredHeight();
          usedExcessSpace += childHeight;
        }
        else
        {
          MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
          childHeight = childLayout->GetMeasuredHeight();
        }

        DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "LinearLayout::MeasureVertical childHeight(" << childHeight << ")\n" );

        LayoutLength length = childHeight + childMargin.top + childMargin.bottom;
        LayoutLength cellPadding = i < GetChildCount() - 1 ? mCellPadding.height : 0;
        LayoutLength totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + length + cellPadding );
      }

      bool matchWidthLocally = false;
      if( widthMode != MeasureSpec::Mode::EXACTLY && desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchWidth = true;
        matchWidthLocally = true;
      }

      LayoutLength marginWidth = childMargin.start + childMargin.end;
      LayoutLength childWidth = childLayout->GetMeasuredWidth() + marginWidth;

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
      if( childWeight > 0 )
      {
        /*
         * Widths of weighted Views are bogus if we end up
         * remeasuring, so keep them separate.
         */
        weightedMaxWidth = std::max( weightedMaxWidth, matchWidthLocally ? marginWidth : childWidth );
      }
      else
      {
        alternativeMaxWidth = std::max( alternativeMaxWidth, matchWidthLocally ? marginWidth : childWidth );
      }
    }
  }

  Extents padding = GetPadding();
  mTotalLength += padding.top + padding.bottom;
  LayoutLength heightSize = mTotalLength;
  heightSize = std::max( heightSize, GetSuggestedMinimumHeight() );
  MeasuredSize heightSizeAndState = ResolveSizeAndState( heightSize, heightMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  heightSize = heightSizeAndState.GetSize();

  // Either expand children with weight to take up available space or
  // shrink them if they extend beyond our current bounds. If we skipped
  // measurement on any children, we need to measure them now.

  // 2nd phase:
  // We cycle through weighted children now (children with weight > 0).
  // The children are measured with exact size equal to their share of the available space based on their weights.
  // mTotalLength is updated to include weighted children measured sizes.
  LayoutLength remainingExcess = heightSize - mTotalLength + usedExcessSpace;
  if( remainingExcess != 0 && totalWeight > 0.0f )
  {
    float remainingWeightSum = totalWeight;

    mTotalLength = 0;

    for( unsigned int i = 0; i < GetChildCount(); ++i )
    {
      auto childLayout = GetChildAt( i );
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
      float childWeight = childOwner.GetProperty<float>( Toolkit::LinearLayout::ChildProperty::WEIGHT );
      Extents childMargin = childLayout->GetMargin();

      LayoutLength childHeight = 0;
      if( childWeight > 0 )
      {
        LayoutLength share = ( childWeight * remainingExcess ) / remainingWeightSum;
        remainingExcess -= share;
        remainingWeightSum -= childWeight;

        // Always lay out weighted elements with intrinsic size regardless of the parent spec
        // for consistency between specs.
        if( desiredHeight == 0 )
        {
          // This child needs to be laid out from scratch using
          // only its share of excess space.
          childHeight = share;
        }
        else
        {
          // This child had some intrinsic width to which we
          // need to add its share of excess space.
          childHeight = childLayout->GetMeasuredHeight() + share;
        }

        const MeasureSpec childWidthMeasureSpec = GetChildMeasureSpec( widthMeasureSpec, padding.start + padding.end, desiredWidth );
        const MeasureSpec childHeightMeasureSpec = MeasureSpec( childHeight, MeasureSpec::Mode::EXACTLY );
        childLayout->Measure( childWidthMeasureSpec, childHeightMeasureSpec );

        // Child may now not fit in vertical dimension.
        if( childLayout->GetMeasuredHeightAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
        {
          childState.heightState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
        }
      }

      bool matchWidthLocally = false;
      if( widthMode != MeasureSpec::Mode::EXACTLY && desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchWidth = true;
        matchWidthLocally = true;
      }

      LayoutLength marginWidth = childMargin.start + childMargin.end;
      LayoutLength childWidth = childLayout->GetMeasuredWidth() + marginWidth;
      maxWidth = std::max( maxWidth, childWidth );
      allFillParent = allFillParent && desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT;
      alternativeMaxWidth = std::max( alternativeMaxWidth, matchWidthLocally ? marginWidth : childWidth );

      childHeight = childLayout->GetMeasuredHeight();
      LayoutLength length = childHeight + childMargin.top + childMargin.bottom;
      LayoutLength cellPadding = i < GetChildCount() - 1 ? mCellPadding.height : 0;
      LayoutLength totalLength = mTotalLength;
      mTotalLength = std::max( totalLength, totalLength + length + cellPadding );
    }

    // Add in our padding
    mTotalLength += padding.top + padding.bottom;
  }
  else
  {
    alternativeMaxWidth = std::max( alternativeMaxWidth, weightedMaxWidth );
  }

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
  MeasureSpec uniformMeasureSpec( GetMeasuredWidth(), MeasureSpec::Mode::EXACTLY );
  for (int i = 0; i < count; ++i)
  {
    LayoutItemPtr childLayout = GetChildAt(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      LayoutLength desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      LayoutLength desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured height
        LayoutLength oldHeight = desiredHeight;
        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, childLayout->GetMeasuredHeight().AsInteger() );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, uniformMeasureSpec, 0, heightMeasureSpec, 0 );

        childOwner.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, oldHeight.AsInteger() );
      }
    }
  }
}

void LinearLayout::LayoutVertical( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  Extents padding = GetPadding();

  LayoutLength childTop( padding.top );
  LayoutLength childLeft( padding.start );

  // Where end of child should go
  LayoutLength width = right - left;

  // Space available for child
  LayoutLength childSpace = width - padding.start - padding.end;
  auto count = GetChildCount();

  switch ( mAlignment & VERTICAL_ALIGNMENT_MASK )
  {
    case Dali::Toolkit::LinearLayout::Alignment::TOP:
    {
      // mTotalLength contains the padding already
      childTop = LayoutLength( padding.top );
      break;
    }
    case Dali::Toolkit::LinearLayout::Alignment::BOTTOM:
    {
      // mTotalLength contains the padding already
      childTop = LayoutLength( padding.top ) + bottom - top - mTotalLength;
      break;
    }
    case Dali::Toolkit::LinearLayout::Alignment::CENTER_VERTICAL:
    default:
    {
      // mTotalLength contains the padding already
      childTop = padding.top + ( bottom - top - mTotalLength ) / 2.0f;
      break;
    }
  }

  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      LayoutLength childWidth = childLayout->GetMeasuredWidth();
      LayoutLength childHeight = childLayout->GetMeasuredHeight();
      Extents childMargin = childLayout->GetMargin();

      childTop += childMargin.top;
      switch ( mAlignment & HORIZONTAL_ALIGNMENT_MASK )
      {
        case Dali::Toolkit::LinearLayout::Alignment::BEGIN:
        default:
        {
          childLeft = LayoutLength( padding.start ) + childMargin.start;
          break;
        }
        case Dali::Toolkit::LinearLayout::Alignment::END:
        {
          childLeft = width - padding.end - childWidth - childMargin.end;
          break;
        }
        case Dali::Toolkit::LinearLayout::Alignment::CENTER_HORIZONTAL:
        {
          childLeft = padding.start + ( childSpace - childWidth ) / 2.0f + childMargin.start - childMargin.end;
          break;
        }
      }
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childTop += childHeight + childMargin.bottom + mCellPadding.height;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
