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
#include <dali-toolkit/internal/layouting/grid-axis.h>


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
  mCellPadding( 0, 0 ),
  mTotalLength( 0 ),
  mNumColumns( AUTO_FIT ),
  mNumRows( AUTO_FIT )
{
}

Grid::~Grid()
{
}

void Grid::SetRowCount( unsigned int rows )
{
  // Store value and invalidate structure.
  mNumRows = rows;
}

void Grid::SetColumnCount( unsigned int columns )
{
  // Store value and invalidate structure.
  mNumColumns = columns;
}

// void Grid::ValidateLayoutParams()
// {
//   const GridAxis axis = mHorizontal ? mHorizontalAxis : mVerticalAxis;
//   const int count = ( axis.definedCount != GridAxis::UNDEFINED ) ? axis.definedCount : 0;

//   int major = 0;
//   int minor = 0;
//   Dali::Vector<unsigned int>  maxSizes;

//   auto childCount = GetChildCount();

//   for( int i = 0; i < childCount; i++ )
//   {
//     LayoutParams lp = (LayoutParams) getChildAt(i).getLayoutParams();

//     const Spec majorSpec = horizontal ? lp.rowSpec : lp.columnSpec;
//     const Interval majorRange = majorSpec.span;
//     const boolean majorWasDefined = majorSpec.startDefined;
//     const int majorSpan = majorRange.size();
//     if (majorWasDefined)
//     {
//       major = majorRange.min;
//     }

//     const Spec minorSpec = horizontal ? lp.columnSpec : lp.rowSpec;
//     const Interval minorRange = minorSpec.span;
//     const boolean minorWasDefined = minorSpec.startDefined;
//     const int minorSpan = clip(minorRange, minorWasDefined, count);

//     if( minorWasDefined )
//     {
//       minor = minorRange.min;
//     }

//     if (count != 0)
//     {
//         // Find suitable row/col values when at least one is undefined.
//         if (!majorWasDefined || !minorWasDefined) {
//             while (!fits(maxSizes, major, minor, minor + minorSpan))
//             {
//               if (minorWasDefined)
//               {
//                 major++;
//               }
//               else
//               {
//                 if (minor + minorSpan <= count)
//                 {
//                   minor++;
//                 }
//                 else
//                 {
//                   minor = 0;
//                   major++;
//                 }
//               }
//             }
//         }
//         procrusteanFill(maxSizes, minor, minor + minorSpan, major + majorSpan);
//     }

//     if (horizontal)
//     {
//         setCellGroup(lp, major, majorSpan, minor, minorSpan);
//     } else {
//         setCellGroup(lp, minor, minorSpan, major, majorSpan);
//     }

//     minor = minor + minorSpan;
//   }
// }

void Grid::DoInitialize()
{
}

void Grid::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::Grid::ChildProperty::WEIGHT ) == indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight",
                                 Toolkit::Grid::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void Grid::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  owner.SetProperty( Toolkit::Grid::ChildProperty::WEIGHT, 1.0f );
}

void Grid::SetCellPadding( LayoutSize size )
{
  mCellPadding = size;
}

LayoutSize Grid::GetCellPadding()
{
  return mCellPadding;
}

// todo remove the availableSpace if not needed due to explict column numbers.
void Grid::DetermineNumberOfColumns( int availableSpace, int numberOfColumns )
{
  if ( mNumColumns == AUTO_FIT )
  {
    // not supported, would determine numbers of colums by dividing available space
    // by the requested column width.
    mNumColumns = 1;
  }
  else
  {
    // Columns numbers can be automatically determined or set.
    if ( mNumColumns <= 0 )
    {
      mNumColumns = 1; // Columns not set so force value
    }
  }
}

void Grid::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "Grid::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Grid::OnMeasure widthSize(%d) \n", widthMeasureSpec.GetSize());

  auto gridWidthMode = widthMeasureSpec.GetMode();
  auto gridHeightMode = heightMeasureSpec.GetMode();
  LayoutLength widthSize = widthMeasureSpec.GetSize();
  LayoutLength heightSize = heightMeasureSpec.GetSize();

  // todo , Column padding not Grid layout padding
  // Will column padding be suported?  Extra data will need to be stored and properties provided.
  auto padding = Extents();

  if( gridWidthMode == MeasureSpec::Mode::UNSPECIFIED )
  {
    if( mColumnWidth > 0 ) //explict value assigned
    {
      widthSize = mColumnWidth + padding.start + padding.end;
    }
    else
    {
      widthSize = padding.start + padding.end;
    }
  }

  int availableChildWidth = widthSize - padding.start - padding.end;
  DetermineNumberOfColumns( availableChildWidth, mNumColumns );

//  bool isExactly = (gridWidthMode == MeasureSpec::Mode::EXACTLY);
//  bool matchHeight = false;
//  bool allFillParent = true;
//  LayoutLength maxHeight = 0;
//  LayoutLength alternativeMaxHeight = 0;

  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // measure first child and use it's dimensions for layout measurement
  auto childCount = GetChildCount();
  if( childCount > 0 )
  {
    auto childLayoutItem = GetChildAt( 0 );
    if( childLayoutItem )
    {
      auto childOwner = childLayoutItem->GetOwner();
      //auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChild( childLayoutItem, widthMeasureSpec, heightMeasureSpec );
      auto childHeight = childLayoutItem->GetMeasuredHeight();
      //auto childMargin = Extents(0,0,0,0);//childLayoutItem->GetMargin();

     // childState = combineMeasuredStates(childState, child.getMeasuredState());

      if ( gridHeightMode == MeasureSpec::Mode::UNSPECIFIED )
      {
         heightSize = padding.top + padding.bottom + childHeight;
      }

      if ( gridHeightMode == MeasureSpec::Mode::AT_MOST )
      {
          int ourSize =  padding.top + padding.bottom;

          for( auto i = 0u; i < childCount; i += mNumColumns )
          {
            mTotalLength += childHeight;
            if( i + mNumColumns < childCount )
            {
              ourSize += mVerticalSpacing;
            }
            if( ourSize >= heightSize )
            {
              ourSize = heightSize;
              break;
            }
          }
          heightSize = ourSize;
      }

      if( gridWidthMode == MeasureSpec::Mode::AT_MOST && mRequestedNumColumns != AUTO_FIT )
      {
          int ourSize = ( mRequestedNumColumns*mColumnWidth )
                        + ( ( mRequestedNumColumns-1 )*mHorizontalSpacing )
                        + padding.start + padding.end;

          if( ourSize > widthSize )
          {
            // widthSize |= MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;  OR
            //   MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);

          }
      }

      SetMeasuredDimensions( ResolveSizeAndState( widthSize, widthMeasureSpec, childState.widthState ),
                             ResolveSizeAndState( heightSize, heightMeasureSpec, childState.heightState ) );

    } // Child is LayoutItem
  } // Child exists

  Extents gridLayoutPadding = GetPadding();
  mTotalLength += gridLayoutPadding.start + gridLayoutPadding.end;
  widthSize = std::max( mTotalLength, GetSuggestedMinimumWidth() );
  MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK );
  widthSize = widthSizeAndState.GetSize();

  //if( !allFillParent && gridHeightMode != MeasureSpec::Mode::EXACTLY )
  {
    //heightSize = alternativeMaxHeight;
  }

  heightSize += gridLayoutPadding.top + gridLayoutPadding.bottom;
  //heightSize = std::max( maxHeight, GetSuggestedMinimumHeight() );

  widthSizeAndState.SetState( childState.widthState );

  SetMeasuredDimensions( widthSizeAndState,
                         ResolveSizeAndState( heightSize, heightMeasureSpec, childState.heightState ) );

  //if( matchHeight )
  //{
   // ForceUniformHeight( GetChildCount(), widthMeasureSpec );
 // }
}

void Grid::ForceUniformHeight( int count, MeasureSpec widthMeasureSpec )
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

void Grid::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  bool isLayoutRtl = actor ? actor.GetProperty<bool>( Actor::Property::LAYOUT_DIRECTION ) : false;

  Extents padding = GetPadding();

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
    // for each child
    int childIndex = start + dir * i;
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      // Get column and row spec


      // Get start and end position of child x1,x2
      auto x1 = 0;
      auto x2 = 1;
      // Get top and bottom position of child y1,y2
      auto y1 = 0;
      auto y2 = 1;
      // Get cell width and height
      auto cellWidth = x2 - x1;
      auto cellHeight = y2 - y1;

      // Get child measured size
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      auto childMargin = Extents(0,0,0,0); //childLayout->GetMargin();

      int sumMarginsX = childMargin.start + childMargin.end;
      int sumMarginsY = childMargin.top + childMargin.bottom;

      cellWidth -= sumMarginsX;
      cellHeight -= sumMarginsY;

      // Get size  of child in the cell
      //int width = hAlign.getSizeInCell(c, childWidth, cellWidth - sumMarginsX);
      //int height = vAlign.getSizeInCell(c, childHeight, cellHeight - sumMarginsY);

      int childTop = LayoutLength(padding.top) + childMargin.top;
      int childLeft = childMargin.start;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}

class Grid::Locations
{

};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
