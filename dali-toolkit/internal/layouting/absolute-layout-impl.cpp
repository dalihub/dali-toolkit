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

//EXTERNAL HEADERS
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
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
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
: LayoutGroup(),
  mTotalHeight( 0 ),
  mTotalWidth( 0 )
{
}

AbsoluteLayout::~AbsoluteLayout()
{
}

void AbsoluteLayout::DoInitialize()
{
}

void AbsoluteLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::AbsoluteLayout::ChildProperty::WEIGHT ) == indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "weight",
                                 Toolkit::AbsoluteLayout::ChildProperty::WEIGHT, Property::FLOAT );
    }
  }
}

void AbsoluteLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  owner.SetProperty( Toolkit::AbsoluteLayout::ChildProperty::WEIGHT, 1.0f );
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

  //auto widthMode = widthMeasureSpec.GetMode();
  auto heightMode = heightMeasureSpec.GetMode();
//  bool isExactly = (widthMode == MeasureSpec::Mode::EXACTLY);
  bool matchHeight = false;
  bool allFillParent = true;
  LayoutLength maxHeight = 0;
  LayoutLength maxWidth = 0;
  LayoutLength alternativeMaxHeight = 0;
  struct
  {
    MeasuredSize::State widthState;
    MeasuredSize::State heightState;
  } childState = { MeasuredSize::State::MEASURED_SIZE_OK, MeasuredSize::State::MEASURED_SIZE_OK };


  auto minPosition = Vector3( Vector3::ZERO );
  auto maxPosition = Vector3( Vector3::ZERO );
  // measure children, and determine if further resolution is required
  for( unsigned int i=0; i<GetChildCount(); ++i )
  {
    auto childLayout = GetChildAt( i );
    if( childLayout )
    {
      auto childOwner = childLayout->GetOwner();
      auto desiredChildHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
      //auto desiredChildWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );

      MeasureChild( childLayout, widthMeasureSpec, heightMeasureSpec );
      auto childWidth = childLayout->GetMeasuredWidth();
      auto childHeigth = childLayout->GetMeasuredHeight();

      auto childPosition = childOwner.GetProperty< Vector3 >( Actor::Property::POSITION );
      minPosition.x = std::min( minPosition.x, childPosition.x );
      maxPosition.x = std::max( maxPosition.x, childPosition.x + childWidth );

      minPosition.y = std::min( minPosition.y, childPosition.y );
      maxPosition.y = std::max( maxPosition.y, childPosition.y + childHeigth );

      mTotalWidth = maxPosition.x - minPosition.x;
      mTotalHeight = maxPosition.y - minPosition.y;
      DALI_LOG_INFO( gLogFilter, Debug::Concise, "AbsoluteLayout::OnMeasure child width(%f) height(%f) \n", mTotalWidth, mTotalHeight  );


      //bool matchHeightLocally = false;
      if( heightMode != MeasureSpec::Mode::EXACTLY && desiredChildHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
      {
        // Will have to re-measure at least this child when we know exact height.
        matchHeight = true;
        //matchHeightLocally = true;
      }

      auto childHeight = childLayout->GetMeasuredHeight();

      if( childLayout->GetMeasuredWidthAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.widthState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }
      if( childLayout->GetMeasuredHeightAndState().GetState() == MeasuredSize::State::MEASURED_SIZE_TOO_SMALL )
      {
        childState.heightState = MeasuredSize::State::MEASURED_SIZE_TOO_SMALL;
      }

      maxHeight = std::max( maxHeight, childHeight );
      allFillParent = ( allFillParent && desiredChildHeight == Toolkit::ChildLayoutData::MATCH_PARENT );
      alternativeMaxHeight = std::max( alternativeMaxHeight, childHeight );
    }
  }

  auto widthSize = mTotalWidth;
  auto heightSize = mTotalHeight;
  widthSize = std::max( widthSize, GetSuggestedMinimumWidth() );
  MeasuredSize widthSizeAndState = ResolveSizeAndState( widthSize, widthMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  MeasuredSize heightSizeAndState = ResolveSizeAndState( heightSize, heightMeasureSpec, MeasuredSize::State::MEASURED_SIZE_OK);
  widthSize = widthSizeAndState.GetSize();
  heightSize = heightSizeAndState.GetSize();

  if( !allFillParent && heightMode != MeasureSpec::Mode::EXACTLY )
  {
    maxHeight = alternativeMaxHeight;
  }

  maxHeight = std::max( maxHeight, GetSuggestedMinimumHeight() );
  maxWidth = std::max( maxWidth, GetSuggestedMinimumWidth() );

  widthSizeAndState.SetState( childState.widthState );
  heightSizeAndState.SetState( childState.heightState );

  SetMeasuredDimensions( ResolveSizeAndState( maxWidth, widthMeasureSpec, childState.widthState ),
                         ResolveSizeAndState( maxHeight, heightMeasureSpec, childState.heightState ) );

  if( matchHeight )
  {
    ForceUniformHeight( GetChildCount(), widthMeasureSpec );
  }
}

void AbsoluteLayout::ForceUniformHeight( int count, MeasureSpec widthMeasureSpec )
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
      auto desiredChildHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( desiredChildHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
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

void AbsoluteLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  // Absolute layout positions it's children at their Actor positions.

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
