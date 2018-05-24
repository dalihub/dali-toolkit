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
#include <dali-toolkit/internal/layouting/fbox-layout-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/extents.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/handle-devel.h>
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

FboxLayoutPtr FboxLayout::New()
{
  FboxLayoutPtr layout( new FboxLayout() );
  return layout;
}

void FboxLayout::SetFlexDirection( Dali::Toolkit::FboxLayout::FlexDirection flexDirection )
{
  YGNodeStyleSetFlexDirection( mRoot, static_cast<YGFlexDirection>(flexDirection) );
}

Dali::Toolkit::FboxLayout::FlexDirection FboxLayout::GetFlexDirection()
{
  return static_cast<Dali::Toolkit::FboxLayout::FlexDirection>(YGNodeStyleGetFlexDirection( mRoot ));
}

void FboxLayout::SetFlexJustification( Dali::Toolkit::FboxLayout::Justification flexJustification )
{
  YGNodeStyleSetJustifyContent( mRoot, static_cast<YGJustify>(flexJustification) );
}

Dali::Toolkit::FboxLayout::Justification FboxLayout::GetFlexJustification()
{
  return static_cast<Dali::Toolkit::FboxLayout::Justification>(YGNodeStyleGetJustifyContent( mRoot ));
}

void FboxLayout::SetFlexWrap( Dali::Toolkit::FboxLayout::WrapType wrapType )
{
  YGNodeStyleSetFlexWrap( mRoot, static_cast<YGWrap>(wrapType) );
}

Dali::Toolkit::FboxLayout::WrapType FboxLayout::GetFlexWrap()
{
  return static_cast<Dali::Toolkit::FboxLayout::WrapType>(YGNodeStyleGetFlexWrap( mRoot ));
}

FboxLayout::FboxLayout()
: LayoutGroup(),
  mRoot( nullptr )
{
  mRoot = YGNodeNew();
  YGNodeSetContext( mRoot, this );

  // Set default style
  YGNodeStyleSetFlexDirection( mRoot, YGFlexDirectionRow );
  YGNodeStyleSetAlignItems( mRoot, YGAlignFlexStart );
  YGNodeStyleSetFlexWrap( mRoot, YGWrapNoWrap );
  YGNodeStyleSetJustifyContent( mRoot, YGJustifyFlexStart );
  // TODO:
  //YGNodeStyleSetAlignContent( mRoot, static_cast<YGAlign>( mAlignContent ) );
}

FboxLayout::~FboxLayout()
{
  if ( mRoot )
  {
	YGNodeFreeRecursive( mRoot );
  }
}

void FboxLayout::DoInitialize()
{
  // GetOwner().SetProperty( Toolkit::FboxLayout::ChildProperty::FLEX, 0 );
}

void FboxLayout::DoRegisterChildProperties( const std::string& containerType )
{
  auto typeInfo = Dali::TypeRegistry::Get().GetTypeInfo( containerType );
  if( typeInfo )
  {
    Property::IndexContainer indices;
    typeInfo.GetChildPropertyIndices( indices );

    if( std::find( indices.Begin(), indices.End(), Toolkit::FboxLayout::ChildProperty::FLEX ) ==
        indices.End() )
    {
      // TODO children properties
      ChildPropertyRegistration( typeInfo.GetName(), "flex", Toolkit::FboxLayout::ChildProperty::FLEX, Property::INTEGER );
//      GetOwner().GetProperty<int>( Toolkit::FboxLayout::ChildProperty::FLEX )) )
    }
  }
}

YGSize FboxLayout::OnMeasure( YGNodeRef node,
                              float innerWidth,
                              YGMeasureMode widthMode,
                              float innerHeight,
                              YGMeasureMode heightMode ) {
  LayoutItem* childLayout = static_cast<LayoutItem*>(node->getContext());

  auto childOwner = childLayout->GetOwner();
  auto desiredWidth = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
  auto desiredHeight = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );
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

void FboxLayout::OnChildAdd( LayoutItem& child )
{
  auto owner = child.GetOwner();
  // Not needed when LayoutGroup is fixed to call DoInitialize.
  DoInitialize();

  YGNodeRef node = YGNodeNew();
  YGNodeSetContext( node, &child );
  YGNodeSetMeasureFunc( node, OnMeasure );
  YGNodeMarkDirty( node );
  YGNodeInsertChild( mRoot, node, GetChildCount()-1 );
}

void FboxLayout::SetChildrenStyle()
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

        // Have to do manually for nodes with custom measure function
        YGNodeMarkDirty( childNode );
      }
    }
  }
}

void FboxLayout::OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec )
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

  SetChildrenStyle();
  YGNodeCalculateLayout( mRoot, width, height, isLayoutRtl ? YGDirectionRTL : YGDirectionLTR );
  SetMeasuredDimensions( GetDefaultSize( YGNodeLayoutGetWidth(mRoot), widthMeasureSpec ),
		                 GetDefaultSize( YGNodeLayoutGetHeight(mRoot), heightMeasureSpec ) );
}

void FboxLayout::OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom )
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
