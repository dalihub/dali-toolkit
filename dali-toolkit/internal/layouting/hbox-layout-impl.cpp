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
#include <dali-toolkit/devel-api/layouting/layout-base.h>


#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

HboxLayoutPtr HboxLayout::New( Handle& owner)
{
  HboxLayoutPtr layout( new HboxLayout() );
  layout->Initialize( owner );

  auto layoutController = Toolkit::LayoutController::Get();
  layoutController.RegisterLayout( Toolkit::HboxLayout(layout.Get()) );

  return layout;
}

HboxLayout::HboxLayout()
: LayoutGroup(),
  mMode( Toolkit::HboxView::FIXED ),
  mCellPadding( 20, 20 ),
  mTotalLength( 0 )
{
}

HboxLayout::~HboxLayout()
{
}

  /**
   * @copydoc LayoutBase::DoRegisterChildProperties()
   */
void HboxLayout::DoRegisterChildProperties( const std::type_info& containerType )
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

void HboxLayout::SetMode( Dali::Toolkit::HboxView::Mode mode )
{
  mMode = mode;
}

void HboxLayout::SetCellPadding( LayoutSize size )
{
  mCellPadding = size;
}

Dali::Toolkit::HboxView::Mode HboxLayout::GetMode()
{
  return mMode;
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

  const uint32_t widthMode = widthMeasureSpec.GetMode();
  const uint32_t heightMode = heightMeasureSpec.GetMode();
  bool isExactly = (widthMode == MeasureSpec::EXACTLY);
  bool matchHeight = false;
  bool allFillParent = true;
  uint16_t maxHeight = 0;
  uint16_t alternativeMaxHeight = 0;
  uint16_t childState = 0;

  // measure children, and determine if further resolution is required
  for( unsigned int i=0; i<GetChildCount(); ++i ) // @todo assumes layoutId is contiguous... probably need an iterator
  {
    auto childLayout = GetChild( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto widthMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
      auto heightMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

      MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, heightMeasureSpec, 0 );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );
      auto length = childWidth + childMargin.start + childMargin.end;

      if( isExactly )
      {
        mTotalLength += length;
      }
      else
      {
        int totalLength = mTotalLength;
        mTotalLength = std::max( totalLength, totalLength + length );
      }

      bool matchHeightLocally = false;
      if( heightMode != MeasureSpec::EXACTLY && heightMeasureSpec == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchHeight = true;
        matchHeightLocally = true;
      }

      uint16_t marginHeight = childMargin.top + childMargin.bottom;
      uint16_t childHeight = childLayout->GetMeasuredHeight() + marginHeight;

      childState |= ( childLayout->GetMeasuredWidthAndState().GetState()
                      |  childLayout->GetMeasuredHeightAndState().GetState() >> 1 );

      maxHeight = std::max( maxHeight, childHeight );
      allFillParent = ( allFillParent && heightMeasureSpec == Toolkit::ChildLayoutData::MATCH_PARENT );
      alternativeMaxHeight = std::max( alternativeMaxHeight, matchHeightLocally ? marginHeight : childHeight );
    }
  }

  Extents padding = GetPadding();
  mTotalLength += padding.start + padding.end;
  uint16_t widthSize = mTotalLength;
  widthSize = std::max( widthSize, GetSuggestedMinimumWidth() );
  MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, 0 );
  widthSize = widthSizeAndState.GetSize();

  if( !allFillParent && heightMode != MeasureSpec::EXACTLY )
  {
    maxHeight = alternativeMaxHeight;
  }
  maxHeight += padding.top + padding.bottom;
  maxHeight = std::max( maxHeight, GetSuggestedMinimumHeight() );

  widthSizeAndState.SetState( childState );

  SetMeasuredDimensions( widthSizeAndState | ( childState & MeasuredSize::MEASURED_STATE_MASK ),
                         ResolveSizeAndState( maxHeight, heightMeasureSpec,
                                              (childState<<1) & MeasuredSize::MEASURED_STATE_MASK ) );

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
  MeasureSpec uniformMeasureSpec = MeasureSpec::MakeMeasureSpec( GetMeasuredHeight(), MeasureSpec::EXACTLY );
  for (int i = 0; i < count; ++i)
  {
    LayoutBasePtr childLayout = GetChild(i);
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      auto widthMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION );
      auto heightMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutBase::ChildProperty::HEIGHT_SPECIFICATION );

      if( heightMeasureSpec == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Temporarily force children to reuse their old measured width
        int oldWidth = widthMeasureSpec;
        childOwner.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, childLayout->GetMeasuredWidth() );

        // Remeasure with new dimensions
        MeasureChildWithMargins( childLayout, widthMeasureSpec, 0, uniformMeasureSpec, 0);

        childOwner.SetProperty( Toolkit::LayoutBase::ChildProperty::WIDTH_SPECIFICATION, oldWidth );
      }
    }
  }
}


void HboxLayout::OnLayout( bool changed, int left, int top, int right, int bottom, bool animate )
{
  bool isLayoutRtl = false;//IsLayoutRtl();
  Extents padding = GetPadding();

  int childTop = 0;
  int childLeft = padding.start;

  // Where bottom of child should go
  int height = bottom - top;

  // Space available for child
  int childSpace = height - padding.top - padding.bottom;

  int count = GetChildCount();


  int start = 0;
  int dir = 1;
  //In case of RTL, start drawing from the last child.
  if (isLayoutRtl) {
    start = count - 1;
    dir = -1;
  }

  for(int i = 0; i < count; i++)
  {
    int childIndex = start + dir * i;
    LayoutBasePtr childLayout = GetChild( childIndex );
    if( childLayout != nullptr )
    {
      uint16_t childWidth = childLayout->GetMeasuredWidth();
      uint16_t childHeight = childLayout->GetMeasuredHeight();

      auto childOwner = childLayout->GetOwner();
      auto childMargin = childOwner.GetProperty<Extents>( Toolkit::LayoutGroup::ChildProperty::MARGIN_SPECIFICATION );

      childTop = padding.top + ((childSpace - childHeight) / 2) + childMargin.top - childMargin.bottom;

      childLeft += childMargin.start;
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight, false );
      childLeft += childWidth + childMargin.end;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
