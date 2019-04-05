/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include "flex-layout-impl.h"

//EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>

//INTERNAL INCLUDES
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/third-party/yoga/YGNode.h>

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

FlexLayoutPtr FlexLayout::New()
{
  FlexLayoutPtr layout( new FlexLayout() );
  return layout;
}

void FlexLayout::SetFlexDirection( Dali::Toolkit::FlexLayout::FlexDirection flexDirection )
{
  YGNodeStyleSetFlexDirection( mRoot, static_cast<YGFlexDirection>(flexDirection) );
}

Dali::Toolkit::FlexLayout::FlexDirection FlexLayout::GetFlexDirection() const
{
  return static_cast<Dali::Toolkit::FlexLayout::FlexDirection>(YGNodeStyleGetFlexDirection( mRoot ));
}

void FlexLayout::SetFlexJustification( Dali::Toolkit::FlexLayout::Justification flexJustification )
{
  YGNodeStyleSetJustifyContent( mRoot, static_cast<YGJustify>(flexJustification) );
}

Dali::Toolkit::FlexLayout::Justification FlexLayout::GetFlexJustification() const
{
  return static_cast<Dali::Toolkit::FlexLayout::Justification>(YGNodeStyleGetJustifyContent( mRoot ));
}

void FlexLayout::SetFlexWrap( Dali::Toolkit::FlexLayout::WrapType wrapType )
{
  YGNodeStyleSetFlexWrap( mRoot, static_cast<YGWrap>(wrapType) );
}

Dali::Toolkit::FlexLayout::WrapType FlexLayout::GetFlexWrap() const
{
  return static_cast<Dali::Toolkit::FlexLayout::WrapType>(YGNodeStyleGetFlexWrap( mRoot ));
}

void FlexLayout::SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment )
{
  YGNodeStyleSetAlignContent( mRoot, static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::FlexLayout::Alignment::Type FlexLayout::GetFlexAlignment() const
{
  return static_cast<Dali::Toolkit::FlexLayout::Alignment::Type>(YGNodeStyleGetAlignContent( mRoot ));
}

void FlexLayout::SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment::Type flexAlignment )
{
  YGNodeStyleSetAlignItems( mRoot, static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::FlexLayout::Alignment::Type FlexLayout::GetFlexItemsAlignment() const
{
  return static_cast<Dali::Toolkit::FlexLayout::Alignment::Type>(YGNodeStyleGetAlignItems( mRoot ));
}

FlexLayout::FlexLayout()
: LayoutGroup(),
  mRoot( nullptr )
{
  mRoot = YGNodeNew();
  YGNodeSetContext( mRoot, this );

  // Set default style
  YGNodeStyleSetFlexDirection( mRoot, YGFlexDirectionColumn );
  YGNodeStyleSetFlexWrap( mRoot, YGWrapNoWrap );
  YGNodeStyleSetJustifyContent( mRoot, YGJustifyFlexStart );
  YGNodeStyleSetAlignContent( mRoot, YGAlignFlexStart );
  YGNodeStyleSetAlignItems( mRoot, YGAlignFlexStart );
}

FlexLayout::~FlexLayout()
{
  if( mRoot )
  {
    YGNodeFreeRecursive( mRoot );
  }
}

void FlexLayout::DoInitialize()
{
}

void FlexLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::FlexLayout::ChildProperty::FLEX ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "flex", Toolkit::FlexLayout::ChildProperty::FLEX, Property::FLOAT );
    }

    if( std::find( indices.Begin(), indices.End(), Toolkit::FlexLayout::ChildProperty::ALIGN_SELF ) ==
        indices.End() )
    {
      ChildPropertyRegistration( typeInfo.GetName(), "alignSelf", Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Property::INTEGER );
    }
  }
}

void FlexLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  if(!DevelHandle::DoesCustomPropertyExist(owner, Toolkit::FlexLayout::ChildProperty::FLEX ))
  {
    owner.SetProperty( Toolkit::FlexLayout::ChildProperty::FLEX, 0 );
  }
  if(!DevelHandle::DoesCustomPropertyExist(owner, Toolkit::FlexLayout::ChildProperty::ALIGN_SELF ))
  {
    owner.SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, YGAlignAuto );
  }

  YGNodeRef node = YGNodeNew();
  YGNodeSetContext( node, &child );
  YGNodeSetMeasureFunc( node, OnChildMeasure );
  YGNodeMarkDirty( node );
  YGNodeInsertChild( mRoot, node, GetChildCount()-1 );
}

void FlexLayout::OnChildRemove( LayoutItem& child )
{
  auto count = GetChildCount();
  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( &child == childLayout.Get() )
    {
      YGNodeRef node = YGNodeGetChild( mRoot, childIndex );
      YGNodeRemoveChild( mRoot, node );
      break;
    }
  }
}

void FlexLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
{
  auto actor = Actor::DownCast(GetOwner());
  bool isLayoutRtl = actor ? actor.GetProperty<int>( Actor::Property::LAYOUT_DIRECTION ) == LayoutDirection::RIGHT_TO_LEFT: false;
  Extents padding = GetPadding();
  Extents margin = GetMargin();

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << "FlexLayout::OnMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << " Layout direction:" << actor.GetProperty( Actor::Property::LAYOUT_DIRECTION ).Get<int>() << " ";
  }
  oss << "widthMeasureSpec:" << widthMeasureSpec << " heightMeasureSpec:" << heightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  YGNodeStyleSetMargin( mRoot, YGEdgeLeft, margin.start );
  YGNodeStyleSetMargin( mRoot, YGEdgeTop, margin.top );
  YGNodeStyleSetMargin( mRoot, YGEdgeRight, margin.end );
  YGNodeStyleSetMargin( mRoot, YGEdgeBottom, margin.bottom );
  YGNodeStyleSetPadding( mRoot, YGEdgeLeft, padding.start );
  YGNodeStyleSetPadding( mRoot, YGEdgeTop, padding.top );
  YGNodeStyleSetPadding( mRoot, YGEdgeRight, padding.end );
  YGNodeStyleSetPadding( mRoot, YGEdgeBottom, padding.bottom );

  float width = YGUndefined;
  float height = YGUndefined;

  YGNodeStyleSetWidth( mRoot, YGUndefined );
  YGNodeStyleSetHeight( mRoot, YGUndefined );
  YGNodeStyleSetMinWidth( mRoot, YGUndefined );
  YGNodeStyleSetMinHeight( mRoot, YGUndefined );
  YGNodeStyleSetMaxWidth( mRoot, YGUndefined );
  YGNodeStyleSetMaxHeight( mRoot, YGUndefined );

  if( widthMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY )
  {
    width = widthMeasureSpec.GetSize().AsDecimal();
    YGNodeStyleSetWidth( mRoot, width );
  }
  else if( widthMeasureSpec.GetMode() == MeasureSpec::Mode::AT_MOST )
  {
    width = widthMeasureSpec.GetSize().AsDecimal();
    YGNodeStyleSetMaxWidth( mRoot, width );
  }

  if ( heightMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY )
  {
    height = heightMeasureSpec.GetSize().AsDecimal();
    YGNodeStyleSetHeight( mRoot, height );
  }
  else if ( heightMeasureSpec.GetMode() == MeasureSpec::Mode::AT_MOST )
  {
    height = heightMeasureSpec.GetSize().AsDecimal();
    YGNodeStyleSetMaxHeight( mRoot, height );
  }

  SetChildrenStyle();
  YGNodeCalculateLayout( mRoot, width, height, isLayoutRtl ? YGDirectionRTL : YGDirectionLTR );
  SetMeasuredDimensions( GetDefaultSize( YGNodeLayoutGetWidth(mRoot), widthMeasureSpec ),
                         GetDefaultSize( YGNodeLayoutGetHeight(mRoot), heightMeasureSpec ) );
}

void FlexLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
{
  auto owner = GetOwner();
  auto actor = Actor::DownCast(owner);
  bool isLayoutRtl = actor ? actor.GetProperty( Actor::Property::LAYOUT_DIRECTION ).Get<int>() == LayoutDirection::RIGHT_TO_LEFT: false;
  LayoutLength width = right - left;
  LayoutLength height = bottom - top;

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << "FlexLayout::OnLayout  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << " Layout direction:" << actor.GetProperty( Actor::Property::LAYOUT_DIRECTION ).Get<int>() << " ";
  }
  oss << "changed:" << (int)changed << " left:" << left << " top:" << top << " right:" << right << " bottom:" << bottom << " isLayoutRtl:" << (int)isLayoutRtl << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  YGNodeCalculateLayout( mRoot, width.AsDecimal(), height.AsDecimal(), isLayoutRtl ? YGDirectionRTL : YGDirectionLTR );

  auto count = GetChildCount();
  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      YGNodeRef node = YGNodeGetChild(mRoot, childIndex);
      LayoutLength childLeft = LayoutLength( YGNodeLayoutGetLeft( node ) )+ left;
      LayoutLength childTop = LayoutLength( YGNodeLayoutGetTop( node ) ) + top;
      LayoutLength childWidth = LayoutLength( YGNodeLayoutGetWidth( node ) );
      LayoutLength childHeight = LayoutLength( YGNodeLayoutGetHeight( node ) );
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}

YGSize FlexLayout::OnChildMeasure( YGNodeRef node,
                              float innerWidth,
                              YGMeasureMode widthMode,
                              float innerHeight,
                              YGMeasureMode heightMode ) {
  LayoutItem* childLayout = static_cast<LayoutItem*>(node->getContext());
  auto childOwner = childLayout->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
  MeasureSpec parentWidthMeasureSpec( 0 );
  if ( innerWidth != YGUndefined )
  {
    parentWidthMeasureSpec = MeasureSpec( innerWidth, static_cast<MeasureSpec::Mode>(widthMode) );
  }
  auto parentHeightMeasureSpec = MeasureSpec( 0 );
  if ( innerHeight != YGUndefined )
  {
    parentHeightMeasureSpec = MeasureSpec( innerHeight, static_cast<MeasureSpec::Mode>(heightMode) );
  }
  auto childWidthMeasureSpec = LayoutGroup::GetChildMeasureSpec( parentWidthMeasureSpec, 0, desiredWidth);
  auto childHeightMeasureSpec = LayoutGroup::GetChildMeasureSpec( parentHeightMeasureSpec, 0, desiredHeight);

  // Force to fill parent if a child wants to match parent even if GetChildMeasureSpec sets otherwise
  if( desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT && innerWidth != YGUndefined )
  {
    childWidthMeasureSpec = MeasureSpec( innerWidth, MeasureSpec::Mode::EXACTLY );
  }
  if( desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT && innerHeight != YGUndefined )
  {
    childHeightMeasureSpec = MeasureSpec( innerHeight, MeasureSpec::Mode::EXACTLY );
  }

#if defined(DEBUG_ENABLED)
  auto actor = Actor::DownCast(childOwner);
  std::ostringstream oss;
  oss << "FlexLayout::OnChildMeasure  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << " ";
  }
  oss << "innerWidth:" << ( ( innerWidth == YGUndefined ) ? "YGUndefined " : "" ) << innerWidth <<
         " innerHeight:" << ( ( innerHeight == YGUndefined ) ? "YGUndefined " : "" ) << innerHeight <<
         " desiredWidth:" << desiredWidth << " desiredHeight:" << desiredHeight <<
         " childWidthMeasureSpec:" << childWidthMeasureSpec << " childHeightMeasureSpec:" << childHeightMeasureSpec << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  childLayout->Measure( childWidthMeasureSpec, childHeightMeasureSpec );

  // Remove padding here since Yoga doesn't consider it as a part of the node size
  Extents padding = childLayout->GetPadding();
  LayoutLength measuredWidth = childLayout->GetMeasuredWidth() - padding.end - padding.start;
  LayoutLength measuredHeight = childLayout->GetMeasuredHeight() - padding.bottom - padding.top;

  // Create struct instance first and manually set params to support all compiler versions
  YGSize ygSize;
  ygSize.width = measuredWidth.AsDecimal();
  ygSize.height = measuredHeight.AsDecimal();
  return ygSize;
}

void FlexLayout::SetChildrenStyle()
{
  if( mRoot )
  {
    auto count = GetChildCount();
    for( unsigned int childIndex = 0; childIndex < count; childIndex++)
    {
      LayoutItemPtr childLayout = GetChildAt( childIndex );
      if( childLayout != nullptr )
      {
        Extents padding = childLayout->GetPadding();
        Extents margin = childLayout->GetMargin();
        auto childOwner = childLayout->GetOwner();
        auto childActor = Actor::DownCast(childOwner);
        auto flex = childOwner.GetProperty<float>( Toolkit::FlexLayout::ChildProperty::FLEX );
        auto alignSelf = static_cast<YGAlign>( childOwner.GetProperty<int>( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF ));

        YGNodeRef childNode = YGNodeGetChild( mRoot, childIndex );
        // Initialise the style of the child.
        YGNodeStyleSetMargin( childNode, YGEdgeLeft, margin.start );
        YGNodeStyleSetMargin( childNode, YGEdgeTop, margin.top );
        YGNodeStyleSetMargin( childNode, YGEdgeRight, margin.end );
        YGNodeStyleSetMargin( childNode, YGEdgeBottom, margin.bottom );

        YGNodeStyleSetPadding( childNode, YGEdgeLeft, padding.start );
        YGNodeStyleSetPadding( childNode, YGEdgeTop, padding.top );
        YGNodeStyleSetPadding( childNode, YGEdgeRight, padding.end );
        YGNodeStyleSetPadding( childNode, YGEdgeBottom, padding.bottom );

        YGNodeStyleSetWidth( childNode, YGUndefined );
        YGNodeStyleSetHeight( childNode, YGUndefined );
        // TODO: check if we are supposed to use actor properties here, max/min is needed for stretch
        YGNodeStyleSetMinWidth( childNode, childActor.GetMinimumSize().x );
        YGNodeStyleSetMinHeight( childNode, childActor.GetMinimumSize().y );
        if( childActor.GetMaximumSize().x == FLT_MAX )
        {
          YGNodeStyleSetMaxWidth( childNode, YGUndefined );
        }
        else
        {
          YGNodeStyleSetMaxWidth( childNode, childActor.GetMaximumSize().x );
        }
        if( childActor.GetMaximumSize().y == FLT_MAX )
        {
          YGNodeStyleSetMaxHeight( childNode, YGUndefined );
        }
        else
        {
          YGNodeStyleSetMaxHeight( childNode, childActor.GetMaximumSize().y );
        }

        YGNodeStyleSetFlex( childNode, flex );
        YGNodeStyleSetAlignSelf( childNode, alignSelf );

        // Have to do manually for nodes with custom measure function
        // TODO: check the style is changed before marking the node
        YGNodeMarkDirty( childNode );
      }
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
