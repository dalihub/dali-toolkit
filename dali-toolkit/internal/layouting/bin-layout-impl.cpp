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
#include <dali-toolkit/internal/layouting/bin-layout-impl.h>

//INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/actors/actor.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/size-negotiation-mapper.h>

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

BinLayoutPtr BinLayout::New()
{
  BinLayoutPtr layout( new BinLayout() );
  return layout;
}

BinLayout::BinLayout()
: LayoutGroup()
{
}

BinLayout::~BinLayout()
{
}

void BinLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(GetOwner());

  std::ostringstream oss;
  oss << "BinLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << "  ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  auto childCount = GetChildCount();

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose,
                  "BinLayout::OnMeasure Actor Id:" <<  Actor::DownCast(GetOwner()).GetId() <<
                  " Owner:" <<  Actor::DownCast(GetOwner()).GetName() <<
                  " Child Count:" << childCount <<
                  " MeasureSpecs( width:"<<widthMeasureSpec<<", height:"<<heightMeasureSpec );

  auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
  LayoutLength widthSpecSize = widthMeasureSpec.GetSize();
  LayoutLength heightSpecSize = heightMeasureSpec.GetSize();

  bool exactWidth ( false );
  bool exactHeight ( false );

  // Layouting behaviour
  // EXACT, width and height as provided.
  // MATCH_PARENT, width and height that of parent
  // WRAP_CONTENT, take width of widest child and height size of longest child (within given limit)
  // UNSPECIFIED, take width of widest child and height size of longest child.

  LayoutLength layoutWidth( 0 );
  LayoutLength layoutHeight( 0 );

  // If BinLayout has children then measure children to get max dimensions
  if ( childCount > 0 )
  {
    for( unsigned int i=0; i<childCount; ++i )
    {
      auto childLayout = GetChildAt( i );
      if( childLayout )
      {
        auto childControl = Toolkit::Control::DownCast(childLayout->GetOwner());

        // If child control has children check if a ResizePolicy is set on it.  A LayoutItem could be a legacy container.
        // A legacy container would need it's ResizePolicy to be applied as a MeasureSpec.

        // Check below will be true for legacy containers and for controls with the layout flag required set.
        // Other layouts will have their own OnMeasure (a requirement) hence not execute BinLayout::OnMeasure.
        // Controls which have set the layout required flag will not be legacy controls hence should not have a ResizePolicy set.
        if( childControl.GetChildCount() > 0 )
        {
          // First pass, Static mappings that are not dependant on parent
          SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( childControl, childLayout, Dimension::WIDTH );
          SizeNegotiationMapper::SetLayoutParametersUsingResizePolicy( childControl, childLayout, Dimension::HEIGHT );
        }

        // Second pass, if any mappings were not possible due to parent size dependencies then calculate an exact desired size for child
        if( true == childLayout->IsResizePolicyRequired() ) // No need to test child count as this flag would only be set if control had children.
        {
          // Get last stored width and height specifications for the child
          LayoutLength desiredWidth = childControl.GetProperty<float>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
          LayoutLength desiredHeight = childControl.GetProperty<float>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

          DALI_LOG_INFO( gLogFilter, Debug::General, "BinLayout::MeasureChild Initial desired size pre ResizePolicy(%f,%f)\n", desiredWidth.AsInteger(), desiredHeight.AsInteger() );

          childLayout->SetResizePolicyRequired( false ); // clear flag incase in case of changes before next Measure
          SizeNegotiationMapper::GetSizeofChildForParentDependentResizePolicy( childControl, widthMeasureSpec, heightMeasureSpec, desiredWidth, desiredHeight );

          // Parent dependant ResizePolicies become exact sizes so are now set on the child before it's measured.
          childControl.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, desiredWidth.AsInteger() );
          childControl.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, desiredHeight.AsInteger()  );

          DALI_LOG_INFO( gLogFilter, Debug::General, " BinLayout::OnMeasure ResizePolicy Required resulting size(%f,%f)\n",  desiredWidth.AsInteger(), desiredHeight.AsInteger() );
        }

        // Get size of child
        MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
        LayoutLength childWidth = childLayout->GetMeasuredWidth();
        LayoutLength childHeight = childLayout->GetMeasuredHeight();

        Extents childMargin = childLayout->GetMargin();

        // BinLayout width is that of it's widest child and it's height of the tallest child.
        // MeasureSpec::Mode::UNSPECIFIED will use these sizes
        layoutWidth = std::max( layoutWidth, childWidth + childMargin.start + childMargin.end );
        layoutHeight = std::max( layoutHeight, childHeight + childMargin.top + childMargin.bottom );
        DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "BinLayout::OnMeasure calculated current BinLayout width[" << layoutWidth << "] height[" << layoutHeight << "]\n" );
      }
      else
      {
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "BinLayout::OnMeasure Not a layout\n" );
      }
    }

    Extents padding = GetPadding();
    layoutWidth += padding.start + padding.end;
    layoutHeight += padding.top + padding.bottom;
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "BinLayout::OnMeasure Getting default size as a leaf\n" );
    // BinLayout does not contain any children so must be a leaf
    layoutWidth = GetDefaultSize( GetSuggestedMinimumWidth(), widthMeasureSpec );
    layoutHeight = GetDefaultSize( GetSuggestedMinimumHeight(), heightMeasureSpec );
  }

  // Can't exceed specified width
  if( widthMode == MeasureSpec::Mode::EXACTLY )
  {
    exactWidth = true;
  }
  else if ( widthMode == MeasureSpec::Mode::AT_MOST )
  {
    layoutWidth = std::min( layoutWidth, widthSpecSize );
  }

  // Can't exceed specified height
  if( heightMode == MeasureSpec::Mode::EXACTLY )
  {
    exactHeight = true;
  }
  else if ( heightMode == MeasureSpec::Mode::AT_MOST )
  {
    layoutHeight = std::min( layoutHeight, heightSpecSize );
  }

  layoutWidth = std::max( layoutWidth, GetSuggestedMinimumWidth() );
  layoutHeight = std::max( layoutHeight, GetSuggestedMinimumHeight() );

  if( exactWidth )
  {
    layoutWidth = widthSpecSize;
  }

  if( exactHeight )
  {
    layoutHeight = heightSpecSize;
  }

  DALI_LOG_STREAM( gLogFilter, Debug::General, "BinLayout::OnMeasure Measured size(" << layoutWidth << "," << layoutHeight << ") for : " << Actor::DownCast(GetOwner()).GetName() << " \n" );
  SetMeasuredDimensions( MeasuredSize( layoutWidth ), MeasuredSize( layoutHeight ) );

}

void BinLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
    auto count = GetChildCount();

  DALI_LOG_STREAM( gLogFilter, Debug::Verbose, "BinLayout OnLayout owner:" << ( ( Toolkit::Control::DownCast(GetOwner())) ? Toolkit::Control::DownCast(GetOwner()).GetName() : "invalid" )  << " childCount:" << count );

  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {

      auto childOwner = childLayout->GetOwner();
      LayoutLength childWidth = childLayout->GetMeasuredWidth();
      LayoutLength childHeight = childLayout->GetMeasuredHeight();
      Extents childMargin = childLayout->GetMargin();
      auto control = Toolkit::Control::DownCast( childOwner );
      Extents padding = GetPadding();

      auto childPosition = control.GetProperty< Vector3 >( Actor::Property::POSITION );
      auto anchorPoint = control.GetProperty< Vector3 >( Actor::Property::ANCHOR_POINT );

      DALI_LOG_STREAM( gLogFilter, Debug::General, "BinLayout::OnLayout child[" << control.GetName() <<
                       "] position(" << childPosition << ") child width[" << childWidth << "] height[" << childHeight << "]\n" );

      // Margin and Padding only supported when child anchor point is TOP_LEFT.
      int paddingAndMarginOffsetX = ( AnchorPoint::TOP_LEFT == anchorPoint ) ? ( padding.top + childMargin.top ) : 0;
      int paddingAndMarginOffsetY = ( AnchorPoint::TOP_LEFT == anchorPoint ) ? ( padding.start + childMargin.start ) : 0;
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "BinLayout::OnLayout paddingMargin offset(%d,%d)\n", paddingAndMarginOffsetX, paddingAndMarginOffsetY );

      LayoutLength childLeft = childPosition.x + paddingAndMarginOffsetX;
      LayoutLength childTop = childPosition.y + paddingAndMarginOffsetY;

      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
