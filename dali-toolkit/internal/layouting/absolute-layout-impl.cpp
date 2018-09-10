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
#include <dali-toolkit/internal/layouting/absolute-layout-impl.h>

//INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/actors/actor.h>
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

AbsoluteLayoutPtr AbsoluteLayout::New()
{
  AbsoluteLayoutPtr layout( new AbsoluteLayout() );
  return layout;
}

AbsoluteLayout::AbsoluteLayout()
: LayoutGroup()
{
}

AbsoluteLayout::~AbsoluteLayout()
{
}

void AbsoluteLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "AbsoluteLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  LayoutLength totalHeight( 0 );
  LayoutLength totalWidth( 0 );

  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };

  auto minPosition = Vector3( Vector3::ZERO );
  auto maxPosition = Vector3( Vector3::ZERO );

  // measure children
  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();

      // Get size of child
      MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      // Determine the width and height needed by the children using their given position and size.
      // Children could overlap so find the left most and right most child.
      auto childPosition = childOwner.GetProperty< Vector3 >( Actor::Property::POSITION );
      minPosition.x = std::min( minPosition.x, childPosition.x );
      maxPosition.x = std::max( maxPosition.x, childPosition.x + childWidth );
      // Children could overlap so find the highest and lowest child.
      minPosition.y = std::min( minPosition.y, childPosition.y );
      maxPosition.y = std::max( maxPosition.y, childPosition.y + childHeight );

      // Store current width and height needed to contain all children.
      totalWidth = maxPosition.x - minPosition.x;
      totalHeight = maxPosition.y - minPosition.y;
      DALI_LOG_INFO( gLogFilter, Debug::Concise, "AbsoluteLayout::OnMeasure child width(%f) height(%f) \n", (float)totalWidth, (float)totalHeight  );

      if( childLayout->GetMeasuredWidthAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.widthState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }
      if( childLayout->GetMeasuredHeightAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.heightState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }
    }
  }

  MeasuredSize widthSizeAndState = ResolveSizeAndState( totalWidth, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  MeasuredSize heightSizeAndState = ResolveSizeAndState( totalHeight, heightMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  totalWidth = widthSizeAndState.GetSize();
  totalHeight = heightSizeAndState.GetSize();

  // Ensure layout respects it's given minimum size
  totalWidth = std::max( totalWidth, GetSuggestedMinimumWidth() );
  totalHeight = std::max( totalHeight, GetSuggestedMinimumHeight() );

  widthSizeAndState.SetState( childState.widthState );
  heightSizeAndState.SetState( childState.heightState );

  SetMeasuredDimensions( ResolveSizeAndState( totalWidth, widthMeasureSpec, childState.widthState ),
                         ResolveSizeAndState( totalHeight, heightMeasureSpec, childState.heightState ) );

}

void AbsoluteLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  // Absolute layout positions it's children at their Actor positions.
  // Children could overlap or spill outside the parent, as is the nature of absolute positions.
  auto count = GetChildCount();

  for( unsigned int i = 0; i < count; i++)
  {
    LayoutItemPtr childLayout = GetChildAt( i );
    if( childLayout != nullptr )
    {
      auto childOwner = childLayout->GetOwner();
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeight = childLayout->GetMeasuredHeight();

      auto childPosition = childOwner.GetProperty< Vector3 >( Actor::Property::POSITION );

      auto childTop = childPosition.y;
      auto childLeft = childPosition.x;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
