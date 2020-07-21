/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
 *
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/devel-api/layouting/flex-node.h>

using namespace Dali;
using namespace Toolkit;

void dali_flexNodeContainer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_flexNodeContainer_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const Flex::SizeTuple ITEM_SIZE = Flex::SizeTuple{ 10.0f, 10.0f };
const Flex::SizeTuple ITEM_SIZE_CALLBACK_TEST = Flex::SizeTuple{ 15.0f, 15.0f };

Flex::SizeTuple MeasureChild( Actor child, float width, int measureModeWidth, float height, int measureModeHeight)
{
  Flex::SizeTuple childSize = ITEM_SIZE;
  if (child.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "callbackTest")
  {
    childSize = ITEM_SIZE_CALLBACK_TEST;
  }
  tet_printf(" MeasureChild test callback executed (%f,%f)\n", childSize.width, childSize.height );
  return childSize;
}

}

int UtcDaliToolkitFlexNodeConstructorP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeNewP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );
  END_TEST;
}

int UtcDaliToolkitFlexNodeAddChildWithMarginP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeAddChildWithMarginP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements as a Row
  flexNode->SetFlexDirection(Flex::FlexDirection::ROW);

  // Create two actors and add them to the parent flex node
  Actor actor = Actor::New();
  DALI_TEST_CHECK( actor );

  Extents margin( 5,5,5,5);
  flexNode->AddChild(actor, margin, &MeasureChild, 0);

  DALI_TEST_EQUALS( (int)flexNode->GetFlexDirection(), (int)Flex::FlexDirection::ROW, TEST_LOCATION );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actorFrame = flexNode->GetNodeFrame(0);

  tet_printf("Actor frame(left:%f,top:%f,right:%f,bottom:%f)\n", actorFrame.x, actorFrame.y, actorFrame.z, actorFrame.w);

  DALI_TEST_EQUALS( actorFrame, Vector4( 5.0f, 5.0f, ITEM_SIZE.width+5, ITEM_SIZE.height+5 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeAddChildrenRowP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeAddChildrenRowP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements as a Row
  flexNode->SetFlexDirection(Flex::FlexDirection::ROW);

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  DALI_TEST_EQUALS( (int)flexNode->GetFlexDirection(), (int)Flex::FlexDirection::ROW, TEST_LOCATION );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actor1Frame = flexNode->GetNodeFrame(0);
  Vector4 actor2Frame = flexNode->GetNodeFrame(1);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( ITEM_SIZE.width, 0.0f, ITEM_SIZE.width * 2, ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeAddChildrenColumnP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeAddChildrenColumnP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements in a Column
  flexNode->SetFlexDirection(Flex::FlexDirection::COLUMN);

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  flexNode->CalculateLayout(480, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, ITEM_SIZE.height, ITEM_SIZE.width, ITEM_SIZE.height *2 ), TEST_LOCATION );

  END_TEST;
}


int UtcDaliToolkitFlexNodeAddChildrenColumnJustify(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeAddChildrenColumnJustify");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements in a Column
  flexNode->SetFlexDirection(Flex::FlexDirection::COLUMN);

  tet_infoline("Justify to the Start, align to start, third item should be displayed at the top and the end");
  flexNode->SetFlexJustification(Flex::Justification::FLEX_START);
  flexNode->SetFlexItemsAlignment( Flex::Alignment::FLEX_START );

  // Create three actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  Actor actor3 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );
  DALI_TEST_CHECK( actor3 );

  DALI_TEST_EQUALS( (int)flexNode->GetFlexJustification(), (int)Flex::Justification::FLEX_START, TEST_LOCATION );
  DALI_TEST_EQUALS( (int)flexNode->GetFlexItemsAlignment(), (int)Flex::Alignment::FLEX_START, TEST_LOCATION );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);
  Flex::Node* actor3node = flexNode->AddChild(actor3, Extents(0,0,0,0), &MeasureChild, 2);
  actor3node->SetFlexAlignmentSelf( Flex::Alignment::FLEX_END );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  Vector4 actor3Frame = flexNode->GetNodeFrame(2); // 2 is third child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);

  /*
    ---------
    |1      |
    |2      |
    |      3|
    |       |
    |       |
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, ITEM_SIZE.height, ITEM_SIZE.width, ITEM_SIZE.height *2 ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( root.z - ITEM_SIZE.width, ITEM_SIZE.height *2, root.z, ITEM_SIZE.height *3 ), TEST_LOCATION );

  tet_infoline(" Justify to the End, items should now be displayed at the bottom, third item should now be displayed at the end");
  flexNode->SetFlexJustification( Flex::Justification::FLEX_END );
  flexNode->SetFlexItemsAlignment( Flex::Alignment::FLEX_START );

  // Recalulate layout
  flexNode->CalculateLayout(480, 800, false);

  root = flexNode->GetNodeFrame(-1); // -1 is the root
  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  actor3Frame = flexNode->GetNodeFrame(2); // 2 is third child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);

  /*
    ---------
    |       |
    |       |
    |1      |
    |2      |
    |      3|
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, root.w - (ITEM_SIZE.height*3), ITEM_SIZE.width,  root.w - (ITEM_SIZE.height*2) ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, root.w - (ITEM_SIZE.height*2), ITEM_SIZE.width, root.w - ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( root.z - ITEM_SIZE.width, root.w - ITEM_SIZE.height, root.z, root.w ), TEST_LOCATION );

  tet_infoline(" Align to End, items should now be displayed at the bottom and the end");
  flexNode->SetFlexJustification( Flex::Justification::FLEX_END );
  flexNode->SetFlexItemsAlignment( Flex::Alignment::FLEX_END );
  // Recalulate layout
  flexNode->CalculateLayout(480, 800, false);

  root = flexNode->GetNodeFrame(-1); // -1 is the root
  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  actor3Frame = flexNode->GetNodeFrame(2); // 2 is third child

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);

  /*
    ---------
    |       |
    |       |
    |      1|
    |      2|
    |      3|
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( root.z - ITEM_SIZE.width, root.w - (ITEM_SIZE.height*3), root.z,  root.w - (ITEM_SIZE.height*2) ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( root.z - ITEM_SIZE.width, root.w - (ITEM_SIZE.height*2), root.z,  root.w - ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( root.z - ITEM_SIZE.width, root.w - ITEM_SIZE.height, root.z, root.w ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeSizingP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeSizingP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  flexNode->CalculateLayout(480, 800, false);

  DALI_TEST_EQUALS( flexNode->GetFlexWidth(), 480.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( flexNode->GetFlexHeight(), 800.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeWrapModeP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeWrapModeP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements in a Column
  flexNode->SetFlexDirection( Flex::FlexDirection::ROW );
  flexNode->SetFlexAlignment( Flex::Alignment::FLEX_START );
  flexNode->SetFlexWrap( Flex::WrapType::NO_WRAP );

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  Actor actor3 = Actor::New();
  Actor actor4 = Actor::New();

  DALI_TEST_EQUALS( (int)flexNode->GetFlexJustification(), (int)Flex::Justification::FLEX_START, TEST_LOCATION );
  DALI_TEST_EQUALS( (int)flexNode->GetFlexItemsAlignment(), (int)Flex::Alignment::FLEX_START, TEST_LOCATION );
  DALI_TEST_EQUALS( (int)flexNode->GetFlexAlignment(), (int)Flex::Alignment::FLEX_START, TEST_LOCATION );
  DALI_TEST_EQUALS( (int)flexNode->GetFlexWrap(), (int)Flex::WrapType::NO_WRAP, TEST_LOCATION );

  flexNode->AddChild( actor1, Extents(0,0,0,0), &MeasureChild, 0 );
  flexNode->AddChild( actor2, Extents(0,0,0,0), &MeasureChild, 1 );
  flexNode->AddChild( actor2, Extents(0,0,0,0), &MeasureChild, 2 );
  flexNode->AddChild( actor2, Extents(0,0,0,0), &MeasureChild, 3 );

  flexNode->CalculateLayout(30, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  Vector4 actor3Frame = flexNode->GetNodeFrame(2); // 2 is first child
  Vector4 actor4Frame = flexNode->GetNodeFrame(3); // 3 is second child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);
  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:cdt%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);
  tet_printf("Actor 4 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor4Frame.x, actor4Frame.y, actor4Frame.z, actor4Frame.w);

  /*
    -------
    |1 2 3 4     |
    |     |
    |     |
    |     |
    |     |
    -------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f,            0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( ITEM_SIZE.width, 0.0f, ITEM_SIZE.width*2, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( ITEM_SIZE.width*2, 0.0f, ITEM_SIZE.width*3, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor4Frame, Vector4( ITEM_SIZE.width*3, 0.0f, ITEM_SIZE.width*4, ITEM_SIZE.height ), TEST_LOCATION );

  flexNode->SetFlexWrap( Flex::WrapType::WRAP );

  flexNode->CalculateLayout( 30, 800, false );
  root = flexNode->GetNodeFrame(-1); // -1 is the root

  DALI_TEST_EQUALS( (int)flexNode->GetFlexWrap(), (int)Flex::WrapType::WRAP, TEST_LOCATION );
  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  actor3Frame = flexNode->GetNodeFrame(2); // 2 is first child
  actor4Frame = flexNode->GetNodeFrame(3); // 3 is second child

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);
  tet_printf("Actor 4 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor4Frame.x, actor4Frame.y, actor4Frame.z, actor4Frame.w);

  /*
    -------
    |1 2 3|     |
    |4    |
    |     |
    |     |
    |     |
    -------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f,            0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( ITEM_SIZE.width, 0.0f, ITEM_SIZE.width*2, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( ITEM_SIZE.width*2, 0.0f, ITEM_SIZE.width*3, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor4Frame, Vector4( 0.0,ITEM_SIZE.height, ITEM_SIZE.width, ITEM_SIZE.height*2 ), TEST_LOCATION );


  END_TEST;
}

int UtcDaliToolkitFlexNodeRemoveChildP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeRemoveChildP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  actor1.SetProperty( Dali::Actor::Property::NAME,"Actor1");
  actor2.SetProperty( Dali::Actor::Property::NAME,"Actor2");

  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actor1Frame = flexNode->GetNodeFrame(0);
  Vector4 actor2Frame = flexNode->GetNodeFrame(1);

  tet_printf("Actor 1 frame(%f,%f,%f,%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(%f,%f,%f,%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, ITEM_SIZE.width, ITEM_SIZE.width, ITEM_SIZE.height*2 ), TEST_LOCATION );

  flexNode->RemoveChild(actor1);

  flexNode->CalculateLayout(480, 800, false);

  actor2Frame = flexNode->GetNodeFrame(0);

  tet_printf("Actor 1 frame(%f,%f,%f,%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(%f,%f,%f,%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeRemoveAllChildrenP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodeRemoveAllChildrenP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  actor1.SetProperty( Dali::Actor::Property::NAME,"Actor1");
  actor2.SetProperty( Dali::Actor::Property::NAME,"Actor2");

  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actor1Frame = flexNode->GetNodeFrame(0);
  Vector4 actor2Frame = flexNode->GetNodeFrame(1);

  tet_printf("Actor 1 frame(%f,%f,%f,%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(%f,%f,%f,%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  flexNode->RemoveChild(actor1);
  flexNode->RemoveChild(actor2);

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actor1FrameRemoved = flexNode->GetNodeFrame(0);
  Vector4 actor2FrameRemoved = flexNode->GetNodeFrame(1);

  tet_printf("Actor 1 frame(%f,%f,%f,%f)\n", actor1FrameRemoved.x, actor1FrameRemoved.y, actor1FrameRemoved.z, actor1FrameRemoved.w);
  tet_printf("Actor 2 frame(%f,%f,%f,%f)\n", actor2FrameRemoved.x, actor2FrameRemoved.y, actor2FrameRemoved.z, actor2FrameRemoved.w);

  DALI_TEST_NOT_EQUALS( actor1Frame, actor1FrameRemoved, 0.1, TEST_LOCATION );
  DALI_TEST_NOT_EQUALS( actor2Frame, actor2FrameRemoved, 0.1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodePaddingMarginP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitFlexNodePaddingMarginP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );
  flexNode->SetFlexDirection( Flex::FlexDirection::ROW );

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  Extents padding( 5,5,5,5);
  Extents margin( 5,5,5,5);

  flexNode->SetPadding( padding );
  flexNode->SetMargin( margin );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 actor1Frame = flexNode->GetNodeFrame(0);
  Vector4 actor2Frame = flexNode->GetNodeFrame(1);

  /*  p = padding
  -----
  |ppppp|
  |p1 2p|
  |p   p|
  |ppppp|
  -------
  */
  DALI_TEST_EQUALS( actor1Frame, Vector4( 5.0f, 5.0f, ITEM_SIZE.width +5 , ITEM_SIZE.height + 5 ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 5+ ITEM_SIZE.width, 5.0f, (ITEM_SIZE.width*2) +5, ITEM_SIZE.height +5 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeCallbackTestP(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeCallbackTestP");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements in a Column
  flexNode->SetFlexDirection(Flex::FlexDirection::COLUMN);

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();

  actor1.SetProperty( Dali::Actor::Property::NAME,"callbackTest");

  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  flexNode->CalculateLayout(480, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE_CALLBACK_TEST.width, ITEM_SIZE_CALLBACK_TEST.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, ITEM_SIZE_CALLBACK_TEST.height, ITEM_SIZE.width, ITEM_SIZE_CALLBACK_TEST.height + ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeFlexPositionType(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeFlexPositionType");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  tet_infoline(" FlexPositionType is RELATIVE by default");

  // Create two actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );

  flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  Flex::Node* actor2node = flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);

  DALI_TEST_EQUALS( (int)actor2node->GetFlexPositionType(), (int)Flex::PositionType::RELATIVE, TEST_LOCATION );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  /*
    ---------
    |1      |
    |2      |
    |       |
    |       |
    |       |
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, ITEM_SIZE.height, ITEM_SIZE.width, ITEM_SIZE.height *2 ), TEST_LOCATION );

  tet_infoline(" ABSOLUTE FlexPositionType, second item should now be ignore any properties");
  actor2node->SetFlexPositionType( Flex::PositionType::ABSOLUTE );

  // Recalulate layout
  flexNode->CalculateLayout(480, 800, false);

  root = flexNode->GetNodeFrame(-1); // -1 is the root
  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);

  /*
    ---------
    |1(2)   |
    |       |
    |       |
    |       |
    |       |
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeFlexAspectRatio(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeFlexAspectRatio");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Create a actor and add them to the parent flex node
  Actor actor1 = Actor::New();
  DALI_TEST_CHECK( actor1 );

  tet_infoline(" 1.0 FlexAspectRatio");
  Flex::Node* actor1node = flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  actor1node->SetFlexAspectRatio(1.0);

  DALI_TEST_EQUALS( actor1node->GetFlexAspectRatio(), 1.0f, TEST_LOCATION );

  flexNode->CalculateLayout(480, 800, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);

  /*
    ---------
    |---    |
    ||1|    |
    |---    |
    |       |
    |       |
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width, ITEM_SIZE.height ), TEST_LOCATION );

  tet_infoline(" 2.0 FlexAspectRatio");
  actor1node->SetFlexAspectRatio(2.0);

  DALI_TEST_EQUALS( actor1node->GetFlexAspectRatio(), 2.0f, TEST_LOCATION );

  // Recalulate layout
  flexNode->CalculateLayout(480, 800, false);

  root = flexNode->GetNodeFrame(-1); // -1 is the root
  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);

  /*
    ---------
    |------ |
    || 1  | |
    |------ |
    |       |
    |       |
    ---------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, ITEM_SIZE.width*2, ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliToolkitFlexNodeFlexBasisShrinkGrow(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliToolkitFlexNodeFlexBasisShrinkGrow");
  Flex::Node* flexNode = new Flex::Node();
  DALI_TEST_CHECK( flexNode );

  // Position elements as a Row
  flexNode->SetFlexDirection(Flex::FlexDirection::ROW);

  // Create three actors and add them to the parent flex node
  Actor actor1 = Actor::New();
  Actor actor2 = Actor::New();
  Actor actor3 = Actor::New();
  DALI_TEST_CHECK( actor1 );
  DALI_TEST_CHECK( actor2 );
  DALI_TEST_CHECK( actor3 );

  Flex::Node* actor1node = flexNode->AddChild(actor1, Extents(0,0,0,0), &MeasureChild, 0);
  Flex::Node* actor2node = flexNode->AddChild(actor2, Extents(0,0,0,0), &MeasureChild, 1);
  Flex::Node* actor3node = flexNode->AddChild(actor3, Extents(0,0,0,0), &MeasureChild, 2);

  float basis = 5;

  actor1node->SetFlexGrow(0.0);
  actor2node->SetFlexGrow(0.0);
  actor3node->SetFlexGrow(0.0);
  actor1node->SetFlexShrink(1.0);
  actor2node->SetFlexShrink(1.0);
  actor3node->SetFlexShrink(1.0);
  actor1node->SetFlexBasis(basis);
  actor2node->SetFlexBasis(basis);
  actor3node->SetFlexBasis(basis);

  DALI_TEST_EQUALS( actor1node->GetFlexGrow(), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor1node->GetFlexShrink(), 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor1node->GetFlexBasis(), basis, TEST_LOCATION );

  flexNode->CalculateLayout(600, 200, false);

  Vector4 root = flexNode->GetNodeFrame(-1); // -1 is the root
  Vector4 actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  Vector4 actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  Vector4 actor3Frame = flexNode->GetNodeFrame(2); // 2 is third child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);

  /*
    -------------------
    ||1||2||3|        |
    |                 |
    -------------------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, basis, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( basis, 0.0f, basis*2, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( basis*2, 0.0f, basis*3, ITEM_SIZE.height ), TEST_LOCATION );


  actor2node->SetFlexGrow(1.0);
  actor3node->SetFlexGrow(1.0);

  // Recalulate layout
  flexNode->CalculateLayout(605, 200, false);

  root = flexNode->GetNodeFrame(-1); // -1 is the root
  actor1Frame = flexNode->GetNodeFrame(0); // 0 is first child
  actor2Frame = flexNode->GetNodeFrame(1); // 1 is second child
  actor3Frame = flexNode->GetNodeFrame(2); // 2 is third child

  tet_printf("Root frame(left:%f,top:%f,right:%f,bottom:%f)\n", root.x, root.y, root.z, root.w);

  tet_printf("Actor 1 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor1Frame.x, actor1Frame.y, actor1Frame.z, actor1Frame.w);
  tet_printf("Actor 2 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor2Frame.x, actor2Frame.y, actor2Frame.z, actor2Frame.w);
  tet_printf("Actor 3 frame(left:%f,top:%f,right:%f,bottom:%f)\n", actor3Frame.x, actor3Frame.y, actor3Frame.z, actor3Frame.w);

  /*
    -------------------
    ||1||  2  ||  3  ||
    |                 |
    -------------------
  */

  DALI_TEST_EQUALS( actor1Frame, Vector4( 0.0f, 0.0f, basis, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor2Frame, Vector4( basis, 0.0f, basis + (root.z-basis)/2, ITEM_SIZE.height ), TEST_LOCATION );
  DALI_TEST_EQUALS( actor3Frame, Vector4( basis + (root.z-basis)/2, 0.0f, root.z, ITEM_SIZE.height ), TEST_LOCATION );

  END_TEST;
}
