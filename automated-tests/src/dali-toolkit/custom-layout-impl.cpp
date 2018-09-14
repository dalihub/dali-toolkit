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

// CLASS HEADER
#include "custom-layout-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

namespace Test
{

namespace Internal
{

using Dali::Actor;
using Dali::Toolkit::MeasuredSize;

CustomLayout::CustomLayout()
: mBehaviourFlags( 0 )
{
}

CustomLayoutPtr CustomLayout::New()
{
  CustomLayoutPtr layout( new CustomLayout() );
  return layout;
}

void CustomLayout::MeasureChildren( Dali::Toolkit::Internal::LayoutItemPtr childLayout, MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec, LayoutLength resultingWidth, LayoutLength resultingHeight )
{
  // Initially use the measure spec of the child's parent
  auto childWidthMeasureSpec = widthMeasureSpec;
  auto childHeightMeasureSpec = heightMeasureSpec;

  if ( true == GetCustomBehaviourFlags( Test::CustomLayout::BEHAVIOUR_FLAG_UNCONSTRAINED_CHILD_WIDTH ) )
  {
    // Use unspecified width measure spec, child can be any width it desires
    childWidthMeasureSpec = MeasureSpec( widthMeasureSpec.GetSize(), MeasureSpec::Mode::UNSPECIFIED );
  }

  if ( true == GetCustomBehaviourFlags( Test::CustomLayout::BEHAVIOUR_FLAG_UNCONSTRAINED_CHILD_HEIGHT) )
  {
    // Use unspecified height measure spec, child can be any height it desires
    childHeightMeasureSpec = MeasureSpec( heightMeasureSpec.GetSize(), MeasureSpec::Mode::UNSPECIFIED );
  }

  MeasureChild( childLayout, childWidthMeasureSpec, childHeightMeasureSpec );
  resultingWidth += childLayout->GetMeasuredWidth();
  resultingHeight = std::max( childLayout->GetMeasuredHeight(), resultingHeight );
}

void CustomLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  auto accumulatedWidth = 0;
  auto maxHeight = 0;

  // In this layout we will:
  //  Measuring the layout with the children in a horizontal configuration, one after another
  //  Set the required width to be the accumulated width of our children
  //  Set the required height to be the maximum height of any of our children

  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
        MeasureChildren( childLayout, widthMeasureSpec, heightMeasureSpec, accumulatedWidth, maxHeight );
    }
  }

  // Finally, call this method to set the dimensions we would like
  SetMeasuredDimensions( MeasuredSize( accumulatedWidth ), MeasuredSize( maxHeight ) );
}

void CustomLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  LayoutLength childTop( 0 );
  LayoutLength childLeft( 0 );

  // We want to vertically align the children to the middle
  LayoutLength height = bottom - top;
  LayoutLength middle = height / 2;

  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);

  // Horizontally align the children to the left
  int count = actor.GetChildCount();
  LayoutLength currentLeft = 0;

  for( int i = 0; i < count; i++)
  {
    Dali::Toolkit::Control child = Dali::Toolkit::Control::DownCast( actor.GetChildAt( i ) );
    if( !child )
    {
      continue;
    }

    Dali::Toolkit::LayoutItem childLayout = Dali::Toolkit::DevelControl::GetLayout( child );
    if( childLayout )
    {
      Dali::Toolkit::Internal::LayoutItem& childLayoutImpl = GetImplementation( childLayout );

      LayoutLength childWidth = childLayoutImpl.GetMeasuredWidth();
      LayoutLength childHeight = childLayoutImpl.GetMeasuredHeight();

      childTop = middle - (childHeight / 2);
      childLayoutImpl.Layout( currentLeft, childTop, currentLeft + childWidth, childTop + childHeight );
      currentLeft += childWidth;
    }
  }
}

void CustomLayout::SetCustomBehaviourFlag( int flag )
{
  mBehaviourFlags |= flag;
  RequestLayout();
}

bool CustomLayout::GetCustomBehaviourFlags( int flagToCheck )
{
  return ( mBehaviourFlags & flagToCheck ) != 0;
}

void CustomLayout::ClearPrivateFlag( int flag )
{
    mBehaviourFlags &= ~flag;
    RequestLayout();
}

} // namespace Internal

} // namespace Test
