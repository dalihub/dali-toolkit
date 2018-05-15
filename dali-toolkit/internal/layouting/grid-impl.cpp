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

GridPtr Grid::New()
{
  GridPtr layout( new Grid() );
  return layout;
}

Grid::Grid()
: LayoutGroup(),
  mCellPadding( 0, 0 ),
  mTotalLength( 0 )
{
}

Grid::~Grid()
{
}

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


  auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
  auto widthSize = widthMeasureSpec.GetSize();
  auto heightSize = heightMeasureSpec.GetSize();

  auto padding = Extents(); // todo , should be the padding on this control or just the column padding??

  if( widthMode == MeasureSpec::Mode::UNSPECIFIED )
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

  int childWidth = widthSize - padding.start - padding.end;

//  bool isExactly = (widthMode == MeasureSpec::Mode::EXACTLY);
//  bool matchHeight = false;
//  bool allFillParent = true;
//  LayoutLength maxHeight = 0;
//  LayoutLength alternativeMaxHeight = 0;

  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  // measure children, and determine if further resolution is required
  auto count = GetChildCount();
  for( unsigned int i=0; i<count; ++i )
  {
    auto childLayoutItem = GetChildAt( i );
    if( childLayoutItem )
    {
      auto childOwner = childLayoutItem->GetOwner();
      auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChild( childLayoutItem, widthMeasureSpec, heightMeasureSpec );
      auto childHeight = childLayoutItem->GetMeasuredHeight();
      auto childMargin = Extents(0,0,0,0);//childLayoutItem->GetMargin();

     // childState = combineMeasuredStates(childState, child.getMeasuredState());

      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Grid::OnMeasure childWidth(%d)\n", MeasureSpec::IntType( childWidth ) );

      if ( heightMode == MeasureSpec::Mode::UNSPECIFIED)
      {
         heightSize = padding.top + padding.bottom + childHeight /*+ getVerticalFadingEdgeLength() * 2*/;
      }

      if (heightMode == MeasureSpec::Mode::AT_MOST)
      {
          int ourSize =  padding.top + padding.bottom;

          const int numColumns = mNumColumns;
          for( int i = 0; i < count; i += numColumns)
          {
            ourSize += childHeight;
            if(i + numColumns < count )
            {
              ourSize += mVerticalSpacing;
            }
            if (ourSize >= heightSize)
            {
              ourSize = heightSize;
              break;
            }
          }
          heightSize = ourSize;
      }
///

      if(widthMode == MeasureSpec::Mode::AT_MOST && mRequestedNumColumns != AUTO_FIT)
      {
          int ourSize = (mRequestedNumColumns*mColumnWidth)
                        + ((mRequestedNumColumns-1)*mHorizontalSpacing)
                        + padding.start + padding.end;

          if( ourSize > widthSize /*|| didNotInitiallyFit */)
          {
            widthSize |= MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
          }
      }

      SetMeasuredDimensions( ResolveSizeAndState( widthSize, widthMeasureSpec, childState.widthState ),
                             ResolveSizeAndState( heightSize, heightMeasureSpec, childState.heightState ) );

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

      auto childMargin = Extents(0,0,0,0); //childLayout->GetMargin();

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
