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
#include <dali-toolkit/internal/layouting/grid-impl.h>

//EXTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/actors/actor.h>

//INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-length.h>
#include <dali-toolkit/internal/layouting/grid-locations.h>

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

GridPtr Grid::New()
{
  GridPtr layout( new Grid() );
  return layout;
}

Grid::Grid()
: LayoutGroup(),
  mTotalHeight( 0 ),
  mTotalWidth( 0 ),
  mNumColumns( 1 ),
  mNumRows( 1 ),
  mRequestedColumnWidth( 0 ),
  mRequestedNumColumns( AUTO_FIT )
{
  mLocations = GridLocations::New();
}

Grid::~Grid(){}

void Grid::SetNumberOfColumns( int columns )
{
  mRequestedNumColumns = columns;
  // Store value and Relayout if changed.
  if( columns != mNumColumns )
  {
    mNumColumns = std::max( 1, columns );
    RequestLayout();
  }
}

int Grid::GetNumberOfColumns() const
{
  return mNumColumns;
}

void Grid::DetermineNumberOfColumns( LayoutLength availableSpace )
{
  if( mRequestedNumColumns == AUTO_FIT )
  {
    if( availableSpace > 0 )
    {
      // Can only calculate number of columns if a column width has been set
      mNumColumns = ( mRequestedColumnWidth > 0 ) ? ( availableSpace.AsInteger() / mRequestedColumnWidth ) : 1;
    }
  }
}

void Grid::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  DALI_LOG_STREAM( gLogFilter, Debug::Verbose,
                   "Grid::OnMeasure Actor Id:" <<  Actor::DownCast(GetOwner()).GetId() <<
                   " Owner:" <<  Actor::DownCast(GetOwner()).GetName() <<
                   " MeasureSpecs( width:"<<widthMeasureSpec<<", height:"<<heightMeasureSpec );

  auto gridWidthMode = widthMeasureSpec.GetMode();
  auto gridHeightMode = heightMeasureSpec.GetMode();
  LayoutLength widthSize = widthMeasureSpec.GetSize();
  LayoutLength heightSize = heightMeasureSpec.GetSize();

  LayoutLength availableContentWidth( 0 );
  LayoutLength availableContentHeight( 0 );

  LayoutLength desiredChildHeight( 0 );
  LayoutLength desiredChildWidth( 0 );

  Extents gridLayoutPadding = GetPadding();

  auto childCount = GetChildCount();

  // WIDTH SPECIFICATIONS

  // measure first child and use it's dimensions for layout measurement
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Grid::OnMeasure MeasureSpec::Mode::EXACTLY or MeasureSpec::Mode::AT_MOST \n" );

  if( childCount > 0 )
  {
    auto childLayoutItem = GetChildAt( 0 );
    if( childLayoutItem )
    {
      auto childOwner = childLayoutItem->GetOwner();

      MeasureChild( childLayoutItem, widthMeasureSpec, heightMeasureSpec );
      desiredChildHeight = childLayoutItem->GetMeasuredHeight();
      desiredChildWidth = childLayoutItem->GetMeasuredWidth();

      // If child has a margin then add it to desired size
      Extents childMargin = childOwner.GetProperty<Extents>( Toolkit::Control::Property::MARGIN );
      desiredChildHeight += childMargin.top + childMargin.bottom;
      desiredChildWidth += childMargin.start + childMargin.end;

      mTotalWidth = desiredChildWidth * mNumColumns;
      DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "Grid::OnMeasure TotalDesiredWidth(" << mTotalWidth << ") \n" );
    } // Child is LayoutItem
  } // Child exists

  // Include padding for max and min checks
  mTotalWidth += gridLayoutPadding.start + gridLayoutPadding.end;

  // Ensure width does not exceed specified atmost width or less than mininum width
  mTotalWidth = std::max( mTotalWidth, GetSuggestedMinimumWidth() );

  // widthMode EXACTLY so grid must be the given width
  if( gridWidthMode == MeasureSpec::Mode::EXACTLY || gridWidthMode == MeasureSpec::Mode::AT_MOST )
  {
    // In the case of AT_MOST, widthSize is the max limit.
      mTotalWidth = std::min( mTotalWidth, widthSize );
  }

  availableContentWidth = mTotalWidth - gridLayoutPadding.start - gridLayoutPadding.end;
  widthSize = mTotalWidth;

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "Grid::OnMeasure availableContentWidth" << availableContentWidth << " mTotalWidth(" << mTotalWidth << ") \n" );
  // HEIGHT SPECIFICATIONS

  // heightMode EXACTLY so grid must be the given height
  if( gridHeightMode == MeasureSpec::Mode::EXACTLY || gridHeightMode == MeasureSpec::Mode::AT_MOST )
  {
    if( childCount > 0 )
    {
      mTotalHeight = gridLayoutPadding.top + gridLayoutPadding.bottom;

      for( auto i = 0u; i < childCount; i += mNumColumns )
      {
        mTotalHeight += desiredChildHeight;
      }
      DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "Grid::OnMeasure TotalDesiredHeight(" << mTotalHeight << ") \n" );

      // Ensure ourHeight does not exceed specified atmost height
      mTotalHeight = std::min( mTotalHeight, heightSize );
      mTotalHeight = std::max( mTotalHeight, GetSuggestedMinimumHeight() );

      heightSize = mTotalHeight;
    } // Child exists

    // In the case of AT_MOST, availableContentHeigth is the max limit.
    availableContentHeight = heightSize - gridLayoutPadding.top - gridLayoutPadding.bottom;
  }
  else
  {
    // Grid expands to fit content

    // If number of columns AUTO_FIT then set to 1 column.
    mNumColumns = ( mNumColumns > 0 ) ? mNumColumns : 1;
    // Calculate numbers of rows, round down result as later check for remainder.
    mNumRows = childCount / mNumColumns;
    // If number of cells not cleanly dividable by colums, add another row to house remainder cells.
    mNumRows += ( childCount % mNumColumns ) ? 1 : 0;

    availableContentHeight = desiredChildHeight * mNumRows;
  }

  // If number of columns not defined
  DetermineNumberOfColumns( availableContentWidth );

  // Locations define the start, end,top and bottom of each cell.
  mLocations->CalculateLocations( mNumColumns, availableContentWidth.AsInteger(), availableContentHeight.AsInteger(), childCount, 0, 0 );


  SetMeasuredDimensions( ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK ),
                         ResolveSizeAndState( heightSize, heightMeasureSpec,  MeasuredSize::State::MEASURED_SIZE_OK ) );
}

void Grid::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  auto count = GetChildCount();

  GridLocations::LocationVector locations =  mLocations->GetLocations();

  Extents gridLayoutPadding = GetPadding();
  Extents childMargins;

  // Margin for all children dependant on if set on first child
  if( count )
  {
    LayoutItemPtr childLayout = GetChildAt( 0 );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      if( childOwner )
      {
        childMargins = childOwner.GetProperty<Extents>( Toolkit::Control::Property::MARGIN );
      }
    }
  }

  for( unsigned int i = 0; i < count; i++)
  {
    // for each child
    int childIndex = i;
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      // Get start and end position of child x1,x2
      auto x1 = locations[ i ].xStart;
      auto x2 = locations[ i ].xEnd;

      // Get top and bottom position of child y1,y2
      auto y1 = locations[ i ].yTop;
      auto y2 = locations[ i ].yBottom;

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Grid::OnLayout cellSize(%d,%d) \n", x2 - x1,  y2 - y1);

      // Offset children by the grids padding if present
      x1 += gridLayoutPadding.start;
      x2 += gridLayoutPadding.start;
      y1 += gridLayoutPadding.top;
      y2 += gridLayoutPadding.top;

      // Offset children by the margin of the first child ( if required ).
      x1 += childMargins.start;
      x2 -= childMargins.end;
      y1 += childMargins.top;
      y2 -= childMargins.bottom;

      childLayout->Layout( x1, y1, x2, y2 );
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
