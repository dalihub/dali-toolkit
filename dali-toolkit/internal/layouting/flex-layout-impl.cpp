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
#include "flex-layout-impl.h"

//EXTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>

//INTERNAL HEADERS
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/third-party/yoga/YGNode.h>

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_LAYOUT" );
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

Dali::Toolkit::FlexLayout::FlexDirection FlexLayout::GetFlexDirection()
{
  return static_cast<Dali::Toolkit::FlexLayout::FlexDirection>(YGNodeStyleGetFlexDirection( mRoot ));
}

void FlexLayout::SetFlexJustification( Dali::Toolkit::FlexLayout::Justification flexJustification )
{
  YGNodeStyleSetJustifyContent( mRoot, static_cast<YGJustify>(flexJustification) );
}

Dali::Toolkit::FlexLayout::Justification FlexLayout::GetFlexJustification()
{
  return static_cast<Dali::Toolkit::FlexLayout::Justification>(YGNodeStyleGetJustifyContent( mRoot ));
}

void FlexLayout::SetFlexWrap( Dali::Toolkit::FlexLayout::WrapType wrapType )
{
  YGNodeStyleSetFlexWrap( mRoot, static_cast<YGWrap>(wrapType) );
}

Dali::Toolkit::FlexLayout::WrapType FlexLayout::GetFlexWrap()
{
  return static_cast<Dali::Toolkit::FlexLayout::WrapType>(YGNodeStyleGetFlexWrap( mRoot ));
}

void FlexLayout::SetFlexAlignment( Dali::Toolkit::FlexLayout::Alignment flexAlignment )
{
  YGNodeStyleSetAlignContent( mRoot, static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::FlexLayout::Alignment FlexLayout::GetFlexAlignment()
{
  return static_cast<Dali::Toolkit::FlexLayout::Alignment>(YGNodeStyleGetAlignContent( mRoot ));
}

void FlexLayout::SetFlexItemsAlignment( Dali::Toolkit::FlexLayout::Alignment flexAlignment )
{
  YGNodeStyleSetAlignItems( mRoot, static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::FlexLayout::Alignment FlexLayout::GetFlexItemsAlignment()
{
  return static_cast<Dali::Toolkit::FlexLayout::Alignment>(YGNodeStyleGetAlignItems( mRoot ));
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
  if ( mRoot )
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
      ChildPropertyRegistration( typeInfo.GetName(), "alignSelf", Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, Property::INTEGER );
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

  // TODO: check
  // if ( desiredHeight == Toolkit::ChildLayoutData::MATCH_PARENT )
  // if ( desiredWidth == Toolkit::ChildLayoutData::MATCH_PARENT )

  if (innerWidth < desiredWidth)
    desiredWidth = innerWidth;
  if (innerHeight < desiredHeight)
    desiredHeight = innerHeight;

  MeasureSpec widthMeasureSpec = MeasureSpec( desiredWidth, static_cast<MeasureSpec::Mode>(widthMode) );
  MeasureSpec heightMeasureSpec = MeasureSpec( desiredHeight, static_cast<MeasureSpec::Mode>(heightMode) );
  childLayout->Measure( widthMeasureSpec, heightMeasureSpec );
  return YGSize{
      .width = childLayout->GetMeasuredWidth(), .height = childLayout->GetMeasuredHeight(),
  };
}

void FlexLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  if (!DevelHandle::DoesCustomPropertyExist(owner, Toolkit::FlexLayout::ChildProperty::FLEX ))
    owner.SetProperty( Toolkit::FlexLayout::ChildProperty::FLEX, 0 );
  if (!DevelHandle::DoesCustomPropertyExist(owner, Toolkit::FlexLayout::ChildProperty::ALIGN_SELF ))
    owner.SetProperty( Toolkit::FlexLayout::ChildProperty::ALIGN_SELF, YGAlignAuto );

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
    if ( &child == childLayout.Get() )
    {
      YGNodeRef node = YGNodeGetChild( mRoot, childIndex );
      YGNodeRemoveChild( mRoot, node );
      break;
    }
  }
}

void FlexLayout::SetChildrenStyle()
{
  if ( mRoot )
  {
    auto count = GetChildCount();
    for( unsigned int childIndex = 0; childIndex < count; childIndex++)
    {
      LayoutItemPtr childLayout = GetChildAt( childIndex );
      if ( childLayout != nullptr )
      {
        Extents padding = childLayout->GetPadding();
        Extents margin = childLayout->GetMargin();
        auto childOwner = childLayout->GetOwner();
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

        YGNodeStyleSetFlex( childNode, flex );
        YGNodeStyleSetAlignSelf( childNode, alignSelf );

        // Have to do manually for nodes with custom measure function
        // TODO: check the style is changed before marking the node
        YGNodeMarkDirty( childNode );
      }
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
  oss << "FboxLayout::OnMeasure  ";
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

  auto width = widthMeasureSpec.GetSize();
  auto height = heightMeasureSpec.GetSize();

  YGNodeStyleSetWidth( mRoot, YGUndefined );
  YGNodeStyleSetHeight( mRoot, YGUndefined );
  YGNodeStyleSetMinWidth( mRoot, YGUndefined );
  YGNodeStyleSetMinHeight( mRoot, YGUndefined );
  YGNodeStyleSetMaxWidth( mRoot, YGUndefined );
  YGNodeStyleSetMaxHeight( mRoot, YGUndefined );

  if (widthMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY)
    YGNodeStyleSetWidth( mRoot, width );
  else if (widthMeasureSpec.GetMode() == MeasureSpec::Mode::AT_MOST)
    YGNodeStyleSetMaxWidth( mRoot, width );

  if (heightMeasureSpec.GetMode() == MeasureSpec::Mode::EXACTLY)
    YGNodeStyleSetHeight( mRoot, height );
  else if (widthMeasureSpec.GetMode() == MeasureSpec::Mode::AT_MOST)
    YGNodeStyleSetMaxHeight( mRoot, width );

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
  auto width = right - left;
  auto height = bottom - top;

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << "FboxLayout::OnLayout  ";
  if( actor )
  {
    oss << "Actor Id:" << actor.GetId() << " Name:" << actor.GetName() << " Layout direction:" << actor.GetProperty( Actor::Property::LAYOUT_DIRECTION ).Get<int>() << " ";
  }
  oss << "changed:" << (int)changed << " width:" << width << " height:" << height << " isLayoutRtl:" << (int)isLayoutRtl << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Concise, oss.str().c_str() );
#endif

  YGNodeCalculateLayout( mRoot, width, height, isLayoutRtl ? YGDirectionRTL : YGDirectionLTR );

  auto count = GetChildCount();
  for( unsigned int childIndex = 0; childIndex < count; childIndex++)
  {
    LayoutItemPtr childLayout = GetChildAt( childIndex );
    if( childLayout != nullptr )
    {
      YGNodeRef node = YGNodeGetChild(mRoot, childIndex);
      auto childLeft = YGNodeLayoutGetLeft( node ) + left;
      auto childTop = YGNodeLayoutGetTop( node ) + top;
      auto childWidth = YGNodeLayoutGetWidth( node );
      auto childHeight = YGNodeLayoutGetHeight( node );
      childLayout->Layout( childLeft, childTop, childLeft + childWidth, childTop + childHeight );
    }
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
