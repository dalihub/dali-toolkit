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
#include <dali-toolkit/internal/layouting/vbox-layout-impl.h>

//INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
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

VboxLayoutPtr VboxLayout::New()
{
  VboxLayoutPtr layout( new VboxLayout() );
  return layout;
}

VboxLayout::VboxLayout()
: LayoutGroup(),
  mCellPadding( 0, 0 ),
  mTotalLength( 0 )
{
}

VboxLayout::~VboxLayout()
{
}

void VboxLayout::DoInitialize()
{
}

void VboxLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::VboxLayout::ChildProperty::WEIGHT ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight", Toolkit::VboxLayout::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void VboxLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  owner.SetProperty( Toolkit::VboxLayout::ChildProperty::WEIGHT, 1.0f );
}

void VboxLayout::SetCellPadding( LayoutSize size )
{
  mCellPadding = size;
}

LayoutSize VboxLayout::GetCellPadding()
{
  return mCellPadding;
}


void VboxLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "VboxLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

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

void VboxLayout::ForceUniformWidth( int count, MeasureSpec heightMeasureSpec )
{
  // Pretend that the linear layout has an exact size. This is the measured height of
  // ourselves. The measured height should be the max height of the children, changed
  // to accommodate the heightMeasureSpec from the parent
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

void VboxLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  Extents padding = GetPadding();

  LayoutLength childTop( 0 );
  LayoutLength childLeft( padding.start );

  // Where bottom of child should go
  auto width = right - left;

  // Space available for child
  auto childSpace = width - padding.start - padding.end;
  auto count = GetChildCount();

  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      auto childOwner = childLayout->GetOwner();
      auto childMargin = childLayout->GetMargin();

      childTop += childMargin.top;
      childLeft = ( childSpace - childWidth ) / 2 + childMargin.start - childMargin.end;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
      childTop += childHeight + childMargin.bottom + mCellPadding.height;
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
