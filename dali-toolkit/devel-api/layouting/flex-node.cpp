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
#include "flex-node.h"

//EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/weak-handle.h>

//INTERNAL INCLUDES
#include <dali-toolkit/third-party/yoga/Yoga.h>

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_FLEX" );
#endif

namespace Dali
{
namespace Toolkit
{
namespace Flex
{

namespace
{
// Common callback function that is registered when AddChild is called.
// Calls MeasureNode which in turns calls the actual callback passed in AddChild not the common callback.
YGSize MeasureChild(YGNodeRef child, float width, YGMeasureMode measureModeWidth, float height, YGMeasureMode measureModeHeight)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "MeasureChild\n" );
  // Get the Node from the YGNodeRef
  Toolkit::Flex::Node* childNode =  static_cast<Toolkit::Flex::Node*>(YGNodeGetContext(child));

  YGSize childSize{ 1, 1 }; // Initialise variable.

  DALI_ASSERT_DEBUG( childNode );

  // Call measure function assigned to this Node
  Toolkit::Flex::SizeTuple nodeSize = childNode->MeasureNode( width, measureModeWidth, height, measureModeHeight );
  childSize.width = nodeSize.width;
  childSize.height = nodeSize.height;
  DALI_LOG_INFO( gLogFilter, Debug::General, "MeasureChild, childNode valid %f,%f\n", childSize.width, childSize.height );

  return childSize;
}

} // unamed namespace

struct Node;

using NodePtr = std::unique_ptr<Node>;

using  FlexNodeVector = std::vector< NodePtr>;

struct Node::Impl
{
  YGNodeRef mYogaNode;
  MeasureCallback mMeasureCallback;
  WeakHandle< Dali::Actor > mActor;
  FlexNodeVector mChildNodes;
};

Node::Node() : mImpl( new Impl )
{
  mImpl->mYogaNode = YGNodeNew();
  YGNodeSetContext( mImpl->mYogaNode, this );
  mImpl->mMeasureCallback = NULL;
  DALI_LOG_INFO( gLogFilter, Debug::General, "Node()  Context [%p] set to mYogaNode[%p]\n", this, mImpl->mYogaNode );

  // Set default style
  YGNodeStyleSetFlexDirection( mImpl->mYogaNode, YGFlexDirectionColumn );
  YGNodeStyleSetFlexWrap( mImpl->mYogaNode, YGWrapNoWrap );
  YGNodeStyleSetJustifyContent( mImpl->mYogaNode, YGJustifyFlexStart );
  YGNodeStyleSetAlignContent( mImpl->mYogaNode, YGAlignFlexStart );
  YGNodeStyleSetAlignItems( mImpl->mYogaNode, YGAlignFlexStart );
}

Node::~Node()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Destructor() >> \n");
  if( mImpl->mYogaNode )
  {
    YGNodeFreeRecursive( mImpl->mYogaNode );
    mImpl->mYogaNode = nullptr;
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Destructor() <<\n");
}

Node* Node::AddChild( Actor child, Extents margin, MeasureCallback measureFunction, int index )
{
  if( child )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "AddChild[%s] to node[%p] at index:%d\n", child.GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str(), mImpl->mYogaNode, index );

    NodePtr childNode( new Node() );

    // Store measure function passed in so can call it when the MeasureChild function is called.
    childNode->mImpl->mMeasureCallback = measureFunction;

    childNode->mImpl->mActor = child;
    Vector2 minumumSize = child.GetProperty< Vector2 >( Actor::Property::MINIMUM_SIZE );
    Vector2 maximumSize = child.GetProperty< Vector2 >( Actor::Property::MAXIMUM_SIZE );

    YGNodeStyleSetMaxWidth( childNode->mImpl->mYogaNode, maximumSize.width );
    YGNodeStyleSetMaxHeight( childNode->mImpl->mYogaNode, maximumSize.height );
    YGNodeStyleSetMinWidth( childNode->mImpl->mYogaNode, minumumSize.width );
    YGNodeStyleSetMinHeight( childNode->mImpl->mYogaNode, minumumSize.height );

    YGNodeStyleSetMargin( childNode->mImpl->mYogaNode, YGEdgeLeft, margin.start );
    YGNodeStyleSetMargin( childNode->mImpl->mYogaNode, YGEdgeTop, margin.top );
    YGNodeStyleSetMargin( childNode->mImpl->mYogaNode, YGEdgeRight, margin.end );
    YGNodeStyleSetMargin( childNode->mImpl->mYogaNode, YGEdgeBottom, margin.bottom );

    YGNodeSetMeasureFunc( childNode->mImpl->mYogaNode, &MeasureChild );

    YGNodeInsertChild( mImpl->mYogaNode, childNode->mImpl->mYogaNode, index );

    Node* result = childNode.get();
    mImpl->mChildNodes.emplace_back( std::move(childNode) );

    return result;;
  }
  return nullptr;
}

void Node::RemoveChild( Actor child )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "RemoveChild child:[%s] from internal nodeCount[%d] childCount[%d]\n", child.GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str(), YGNodeGetChildCount( mImpl->mYogaNode ), mImpl->mChildNodes.size()  );

  auto iterator = std::find_if( mImpl->mChildNodes.begin(),mImpl->mChildNodes.end(),
                                [&child]( NodePtr& childNode ){ return childNode->mImpl->mActor.GetHandle() == child;});

  if( iterator != mImpl->mChildNodes.end() )
  {
      YGNodeRemoveChild( mImpl->mYogaNode, (*iterator)->mImpl->mYogaNode );
      mImpl->mChildNodes.erase(iterator);
  }

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "RemoveChild internal nodeCount[%d] childCount[%d]\n", YGNodeGetChildCount( mImpl->mYogaNode ), mImpl->mChildNodes.size()  );
}

SizeTuple Node::MeasureNode( float width, int widthMode, float height, int heightMode)
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "MeasureNode\n" );

  // Execute callback registered with AddChild
  Toolkit::Flex::SizeTuple nodeSize{8,8}; // Default size set to 8,8 to aid bug detection.
  if( mImpl->mMeasureCallback && mImpl->mActor.GetHandle() )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "MeasureNode MeasureCallback executing on %s\n", mImpl->mActor.GetHandle().GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str() );
    mImpl->mMeasureCallback( mImpl->mActor.GetHandle(), width, widthMode, height, heightMode, &nodeSize );
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "MeasureNode nodeSize width:%f height:%f\n", nodeSize.width, nodeSize.height );
  return nodeSize;
}

void Node::CalculateLayout(float availableWidth, float availableHeight, bool isRTL)
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "CalculateLayout availableSize(%f,%f)\n", availableWidth, availableHeight );
  YGNodeCalculateLayout( mImpl->mYogaNode, availableWidth, availableHeight, isRTL ? YGDirectionRTL : YGDirectionLTR );
}

Dali::Vector4 Node::GetNodeFrame( int index ) const
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetNodeFrame[%d]\n", index );
  YGNodeRef childNode = YGNodeGetChild( mImpl->mYogaNode, index );
  Dali::Vector4 frame = Vector4::ZERO;
  if(childNode)
  {
    frame.x = YGNodeLayoutGetLeft( childNode );
    frame.y = YGNodeLayoutGetTop( childNode );
    frame.z = frame.x + YGNodeLayoutGetWidth( childNode );
    frame.w = frame.y + YGNodeLayoutGetHeight( childNode );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetNodeFrame Node index[%d] child ptr[%p] GetYogaNodeFrame left:%f top:%f right:%f bottom:%f\n",
                   index, childNode, frame.x , frame.y, frame.z, frame.w);
  }
  else
  {
    frame.x = YGNodeLayoutGetLeft( mImpl->mYogaNode );
    frame.y = YGNodeLayoutGetTop( mImpl->mYogaNode );
    frame.z = frame.x + YGNodeLayoutGetWidth( mImpl->mYogaNode );
    frame.w = frame.y + YGNodeLayoutGetHeight( mImpl->mYogaNode );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "GetNodeFrame Root index[%d] root ptr[%p] GetYogaNodeFrame left:%f top:%f right:%f bottom:%f\n",
                   index, mImpl->mYogaNode, frame.x , frame.y, frame.z, frame.w);
  }

  return frame;
}
void Node::SetFlexDirection( Dali::Toolkit::Flex::FlexDirection flexDirection )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex direction[%d]\n", flexDirection );

  YGNodeStyleSetFlexDirection( mImpl->mYogaNode, static_cast<YGFlexDirection>(flexDirection) );
}

Dali::Toolkit::Flex::FlexDirection Node::GetFlexDirection() const
{
  return static_cast<Dali::Toolkit::Flex::FlexDirection>(YGNodeStyleGetFlexDirection( mImpl->mYogaNode ));
}

void Node::SetFlexJustification( Dali::Toolkit::Flex::Justification flexJustification )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex justification[%d]\n", flexJustification )

  YGNodeStyleSetJustifyContent( mImpl->mYogaNode, static_cast<YGJustify>(flexJustification) );
}

Dali::Toolkit::Flex::Justification Node::GetFlexJustification() const
{
  return static_cast<Dali::Toolkit::Flex::Justification>(YGNodeStyleGetJustifyContent( mImpl->mYogaNode ));
}

Dali::Toolkit::Flex::WrapType Node::GetFlexWrap() const
{
  return static_cast<Dali::Toolkit::Flex::WrapType>(YGNodeStyleGetFlexWrap( mImpl->mYogaNode ));
}

void Node::SetFlexAlignment(Dali::Toolkit::Flex::Alignment flexAlignment )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex alignment[%d]\n", flexAlignment )

  YGNodeStyleSetAlignContent( mImpl->mYogaNode , static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::Flex::Alignment Node::GetFlexAlignment() const
{
  return static_cast<Dali::Toolkit::Flex::Alignment>(YGNodeStyleGetAlignContent( mImpl->mYogaNode ));
}

void Node::SetFlexItemsAlignment(Dali::Toolkit::Flex::Alignment flexAlignment )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex items alignment[%d] on mYogaNode[%p]\n", flexAlignment, mImpl->mYogaNode )

  YGNodeStyleSetAlignItems( mImpl->mYogaNode, static_cast<YGAlign>(flexAlignment) );
}

Dali::Toolkit::Flex::Alignment Node::GetFlexItemsAlignment() const
{
  return static_cast<Dali::Toolkit::Flex::Alignment>( YGNodeStyleGetAlignItems( mImpl->mYogaNode ));
}

void Node::SetFlexAlignmentSelf( Dali::Toolkit::Flex::Alignment flexAlignmentSelf )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex alignment self [%d] on mYogaNode[%p]\n", flexAlignmentSelf, mImpl->mYogaNode )

  YGNodeStyleSetAlignSelf( mImpl->mYogaNode, static_cast<YGAlign>(flexAlignmentSelf) );
}

Dali::Toolkit::Flex::Alignment Node::GetFlexAlignmentSelf() const
{
  return static_cast<Dali::Toolkit::Flex::Alignment>(YGNodeStyleGetAlignSelf( mImpl->mYogaNode ));
}

void Node::SetFlexPositionType( Dali::Toolkit::Flex::PositionType flexPositionType )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex position type [%d] on mYogaNode[%p]\n", flexPositionType, mImpl->mYogaNode )

  YGNodeStyleSetPositionType( mImpl->mYogaNode, static_cast<YGPositionType>(flexPositionType) );
}

Dali::Toolkit::Flex::PositionType Node::GetFlexPositionType() const
{
  return static_cast<Dali::Toolkit::Flex::PositionType>(YGNodeStyleGetPositionType( mImpl->mYogaNode ));
}

void Node::SetFlexAspectRatio( float flexAspectRatio )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex aspect ratio [%d] on mYogaNode[%p]\n", flexAspectRatio, mImpl->mYogaNode )

  YGNodeStyleSetAspectRatio( mImpl->mYogaNode, static_cast<float>(flexAspectRatio) );
}

float Node::GetFlexAspectRatio() const
{
  return static_cast<float>(YGNodeStyleGetAspectRatio( mImpl->mYogaNode ));
}

void Node::SetFlexBasis( float flexBasis )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex basis [%d] on mYogaNode[%p]\n", flexBasis, mImpl->mYogaNode )

  YGNodeStyleSetFlexBasis( mImpl->mYogaNode, static_cast<float>(flexBasis) );
}

float Node::GetFlexBasis() const
{
  return static_cast<float>(YGNodeStyleGetFlexBasis( mImpl->mYogaNode ).value);
}

void Node::SetFlexShrink( float flexShrink )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex shrink [%d] on mYogaNode[%p]\n", flexShrink, mImpl->mYogaNode )

  YGNodeStyleSetFlexShrink( mImpl->mYogaNode, static_cast<float>(flexShrink) );
}

float Node::GetFlexShrink() const
{
  return static_cast<float>(YGNodeStyleGetFlexShrink( mImpl->mYogaNode ));
}

void Node::SetFlexGrow( float flexGrow )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex grow [%d] on mYogaNode[%p]\n", flexGrow, mImpl->mYogaNode )

  YGNodeStyleSetFlexGrow( mImpl->mYogaNode, static_cast<float>(flexGrow) );
}

float Node::GetFlexGrow() const
{
  return static_cast<float>(YGNodeStyleGetFlexGrow( mImpl->mYogaNode ));
}

float Node::GetFlexWidth() const
{
  float flexWidth = YGNodeLayoutGetWidth( mImpl->mYogaNode );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Get flex mYogaNode[%p] width[%f]\n", mImpl->mYogaNode, flexWidth)

  return flexWidth;
}

float Node::GetFlexHeight() const
{
  float flexHeight = YGNodeLayoutGetHeight( mImpl->mYogaNode );
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Get flex mYogaNode[%p] height[%f]\n", mImpl->mYogaNode, flexHeight)

  return flexHeight;
}

void Node::SetMargin( Extents margin )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex margin\n")

  YGNodeStyleSetMargin( mImpl->mYogaNode, YGEdgeLeft, margin.start );
  YGNodeStyleSetMargin( mImpl->mYogaNode, YGEdgeTop, margin.top );
  YGNodeStyleSetMargin( mImpl->mYogaNode, YGEdgeRight, margin.end );
  YGNodeStyleSetMargin( mImpl->mYogaNode, YGEdgeBottom, margin.bottom );
}

void Node::SetPadding( Extents padding )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set padding\n")

  YGNodeStyleSetPadding( mImpl->mYogaNode, YGEdgeLeft, padding.start );
  YGNodeStyleSetPadding( mImpl->mYogaNode, YGEdgeTop, padding.top );
  YGNodeStyleSetPadding( mImpl->mYogaNode, YGEdgeRight, padding.end );
  YGNodeStyleSetPadding( mImpl->mYogaNode, YGEdgeBottom, padding.bottom );
}

void Node::SetFlexWrap( Dali::Toolkit::Flex::WrapType wrapType )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Set flex wrap[%d] on mYogaNode[%p]\n", wrapType, mImpl->mYogaNode )

  YGNodeStyleSetFlexWrap( mImpl->mYogaNode, static_cast<YGWrap>(wrapType) );
}

} // Flex
} // namespace Toolkit
} // namespace Dali
