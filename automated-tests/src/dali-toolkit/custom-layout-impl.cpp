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

CustomLayoutPtr CustomLayout::New()
{
  CustomLayoutPtr layout( new CustomLayout() );
  return layout;
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
      MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
      accumulatedWidth += childLayout->GetMeasuredWidth();
      maxHeight = std::max( childLayout->GetMeasuredHeight().mValue, maxHeight );
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
  auto height = bottom - top;
  auto middle = height / 2;

  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);

  // Horizontally align the children to the left
  int count = actor.GetChildCount();
  int currentLeft = 0;

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

      auto childWidth = childLayoutImpl.GetMeasuredWidth();
      auto childHeight = childLayoutImpl.GetMeasuredHeight();

      childTop = middle - (childHeight / 2);
      childLayoutImpl.Layout( currentLeft, childTop, currentLeft + childWidth, childTop + childHeight );
      currentLeft += childWidth;
    }
  }
}

} // namespace Internal

} // namespace Test
