/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <float.h>       // for FLT_MAX

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_item_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_item_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const unsigned int TOTAL_ITEM_NUMBER = 100;
const char* TEST_IMAGE_FILE_NAME = "gallery_image_01.jpg";

const int RENDER_FRAME_INTERVAL = 16;                     ///< Duration of each frame in ms. (at approx 60FPS)

static bool gObjectCreatedCallBackCalled;
static bool gOnLayoutActivatedCalled;                     ///< Whether the LayoutActivated signal was invoked.

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

static void OnLayoutActivated()
{
  gOnLayoutActivatedCalled = true;
}

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

// Implementation of ItemFactory for providing actors to ItemView
class TestItemFactory : public ItemFactory
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  TestItemFactory()
  {
  }

public: // From ItemFactory

  /**
   * Query the number of items available from the factory.
   * The maximum available item has an ID of GetNumberOfItems() - 1.
   */
  virtual unsigned int GetNumberOfItems()
  {
    return TOTAL_ITEM_NUMBER;
  }

  /**
   * Create an Actor to represent a visible item.
   * @param itemId
   * @return the created actor.
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    // Create an image actor for this item
    Image image = ResourceImage::New( TEST_IMAGE_FILE_NAME );
    Actor actor = ImageActor::New(image);

    return actor;
  }
};

} // namespace


int UtcDaliItemViewNew(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  DALI_TEST_CHECK(view);

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    TestItemFactory factory;
    ItemView view = ItemView::New(factory);
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliItemViewDownCast(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  const ItemView itemViewConst = ItemView::New(factory);
  ItemView itemView(itemViewConst);

  BaseHandle handle(itemView);

  ItemView newItemView = ItemView::DownCast( handle );
  DALI_TEST_CHECK( itemView );
  DALI_TEST_CHECK( newItemView == itemView );
  END_TEST;
}

int UtcDaliItemViewAddAndGetLayout(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // As we have added one layout, check the number of layout is now 1
  DALI_TEST_CHECK(view.GetLayoutCount() == 1);

  // Create a depth layout and add it to ItemView
  ItemLayoutPtr depthLayout = DefaultItemLayout::New( DefaultItemLayout::DEPTH );
  view.AddLayout(*depthLayout);

  // As we have added another layout, check the number of layout is now 2
  DALI_TEST_CHECK(view.GetLayoutCount() == 2);

  // Create a spiral layout and add it to ItemView
  ItemLayoutPtr spiralLayout = DefaultItemLayout::New( DefaultItemLayout::SPIRAL );
  view.AddLayout(*spiralLayout);

  // As we have added another layout, check the number of layout is now 3
  DALI_TEST_CHECK(view.GetLayoutCount() == 3);

  // Check we are getting the correct layout from ItemView
  DALI_TEST_CHECK(view.GetLayout(0) == gridLayout);
  DALI_TEST_CHECK(view.GetLayout(1) == depthLayout);
  DALI_TEST_CHECK(view.GetLayout(2) == spiralLayout);
  END_TEST;
}

int UtcDaliItemViewAddAndRemoveLayout(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // As we have added one layout, check the number of layout is now 1
  DALI_TEST_CHECK(view.GetLayoutCount() == 1);

  // Create a depth layout and add it to ItemView
  ItemLayoutPtr depthLayout = DefaultItemLayout::New( DefaultItemLayout::DEPTH );
  view.AddLayout(*depthLayout);

  // As we have added another layout, check the number of layout is now 2
  DALI_TEST_CHECK(view.GetLayoutCount() == 2);

  // Check we are getting the correct layout from ItemView
  DALI_TEST_CHECK(view.GetLayout(0) == gridLayout);
  DALI_TEST_CHECK(view.GetLayout(1) == depthLayout);

  // Remove the grid layout
  view.RemoveLayout(0);

  // As we have removed the grid layout, check the number of layout is now 1
  DALI_TEST_CHECK(view.GetLayoutCount() == 1);

  // Check we are getting the correct layout from ItemView
  DALI_TEST_CHECK(view.GetLayout(0) == depthLayout);

  // Remove the depth layout
  view.RemoveLayout(0);

  // As we also removed the depth layout, check the number of layout is now 0
  DALI_TEST_CHECK(view.GetLayoutCount() == 0);
  END_TEST;
}

int UtcDaliItemViewActivateLayoutAndGetActiveLayout(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Create a depth layout and add it to ItemView
  ItemLayoutPtr depthLayout = DefaultItemLayout::New( DefaultItemLayout::DEPTH );
  view.AddLayout(*depthLayout);

  // Create a spiral layout and add it to ItemView
  ItemLayoutPtr spiralLayout = DefaultItemLayout::New( DefaultItemLayout::SPIRAL );
  view.AddLayout(*spiralLayout);

  // As we have added three layouts, check the number of layout is now 3
  DALI_TEST_CHECK(view.GetLayoutCount() == 3);

  // Check there is no active layout at the moment
  DALI_TEST_CHECK(view.GetActiveLayout() == NULL);

  // Activate the depth layout
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(1, stageSize, 0.5f);

  // Check the current active layout is the depth layout
  DALI_TEST_CHECK(view.GetActiveLayout() == depthLayout);

  // Activate the grid layout
  view.ActivateLayout(0, stageSize, 0.5f);

  // Check the current active layout is the grid layout
  DALI_TEST_CHECK(view.GetActiveLayout() == gridLayout);

  // Activate the spiral layout
  view.ActivateLayout(2, stageSize, 0.5f);

  // Check the current active layout is the spiral layout
  DALI_TEST_CHECK(view.GetActiveLayout() == spiralLayout);
  END_TEST;
}

int UtcDaliItemViewDeactivateCurrentLayout(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Check there is no active layout at the moment
  DALI_TEST_CHECK(view.GetActiveLayout() == NULL);

  // Activate the grid layout
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  // Check the current active layout is the grid layout
  DALI_TEST_CHECK(view.GetActiveLayout() == gridLayout);

  // Deactivate the current layout
  view.DeactivateCurrentLayout();

  // Check there is no active layout at the moment
  DALI_TEST_CHECK(view.GetActiveLayout() == NULL);
  END_TEST;
}

int UtcDaliItemViewGetItemAndGetItemId(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  // Get the item given the item ID
  Actor itemActor = view.GetItem(2);

  // Check we are getting the correct Item ID given the specified actor
  DALI_TEST_CHECK(view.GetItemId(itemActor) == 2);
  END_TEST;
}

int UtcDaliItemViewRemoveItem(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  // Get the item given the item ID 2 and 3
  Actor oldItemActorID2 = view.GetItem(2);
  Actor oldItemActorID3 = view.GetItem(3);

  // Remove the item with ID 2
  view.RemoveItem(2, 0.0f);

  // Get the new item given the item ID 2
  Actor newItemActorID2 = view.GetItem(2);

  // Check the original item with item ID 2 was deleted and now item ID 2 represents the original item with ID 3
  DALI_TEST_CHECK(view.GetItemId(newItemActorID2) == 2);
  DALI_TEST_CHECK(oldItemActorID2 != newItemActorID2);
  DALI_TEST_CHECK(newItemActorID2 = oldItemActorID3);
  END_TEST;
}

int UtcDaliItemViewGetCurrentLayoutPosition(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.0f);

  // Check the current layout position for the 10th items is 9.0f
  DALI_TEST_EQUALS(view.GetCurrentLayoutPosition(9), 9.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliItemViewSetAndGetMinimumSwipeSpeed(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the minimum swipe speed to be 1.5f
  view.SetMinimumSwipeSpeed(1.5f);

  // Check the minimum swipe speed is 1.5f
  DALI_TEST_EQUALS(view.GetMinimumSwipeSpeed(), 1.5f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliItemViewSetAndGetMinimumSwipeDistance(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the minimum swipe distance to be 2.5f
  view.SetMinimumSwipeDistance(2.5f);

  // Check the minimum swipe distance is 2.5f
  DALI_TEST_EQUALS(view.GetMinimumSwipeDistance(), 2.5f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliItemViewSetAndGetAnchoring(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Disable the anchor animation
  view.SetAnchoring(false);

  // Check the anchor animation is disabled
  DALI_TEST_CHECK(view.GetAnchoring() == false);
  END_TEST;
}

int UtcDaliItemViewSetAndGetAnchoringDuration(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the duration of anchor animation to be 1.5f
  view.SetAnchoringDuration(1.5f);

  // Check the duration of anchor animation is 1.5f
  DALI_TEST_EQUALS(view.GetAnchoringDuration(), 1.5f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliItemViewSetAndGetRefreshInterval(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the interval between refreshes to be 20
  view.SetRefreshInterval(20);

  view.Refresh();

  // Check the interval between refreshes is 20
  DALI_TEST_CHECK(view.GetRefreshInterval() == 20);
  END_TEST;
}

int UtcDaliItemViewScrollToItem(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  ItemLayoutPtr layout = DefaultItemLayout::New( DefaultItemLayout::GRID );

  view.SetName("view actor");
  view.AddLayout(*layout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  layout->SetOrientation(ControlOrientation::Up);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  // Confirm: we have actors in the view.
  std::vector<unsigned int> indices;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      indices.push_back(i);
    }
  }

  try
  {
    if (!indices.empty())
    {
      const unsigned int firstTargetIndex = indices[indices.size()-1];
      // scroll to last item
      view.ScrollToItem(firstTargetIndex, 0.00f);
      for(int i = 0; i < 10; ++i)
      {
        application.Render(16); // 60hz frames
      }

      std::size_t moveCount = 0;
      for(std::size_t i = 0; i < indices.size(); i++)
      {
        float layoutPosBefore = view.GetCurrentLayoutPosition(i);
        view.ScrollToItem(indices[i], 0.0f);
        float layoutPosAfter = view.GetCurrentLayoutPosition(i);

        if (fabs(layoutPosBefore-layoutPosAfter) <= FLT_EPSILON)
        {
          ++moveCount;
        }
      }

      DALI_TEST_CHECK((moveCount == indices.size()));
    }
  }
  catch(...)
  {
    tet_result(TET_FAIL);
  }

  Stage::GetCurrent().Remove(view);
  END_TEST;
}

int UtcDaliItemViewSetAndGetWheelScrollDistanceStep(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the scroll distance step for the wheel event to be 100.0f
  view.SetWheelScrollDistanceStep(100.0f);

  // Check the scroll distance step is 100.0f
  DALI_TEST_EQUALS(view.GetWheelScrollDistanceStep(), 100.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliItemViewInsertItemP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  // Get the item given the item ID
  Actor itemActor = view.GetItem(2);

  ItemId id = view.GetItemId( itemActor );

  // Check we are getting the correct Item ID given the specified actor
  DALI_TEST_CHECK(view.GetItemId(itemActor) == 2);

  Actor newActor = Actor::New();

  view.InsertItem(Item(id, newActor), 0.5f);

  DALI_TEST_CHECK(view.GetItem(2) == newActor);
  END_TEST;
}

int UtcDaliItemViewInsertItemsP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  unsigned int itemCount = view.GetChildCount();

  ItemContainer insertList;

  for( unsigned int i = 0u; i < 10; ++i )
  {
    Actor child = view.GetChildAt( i );
    Actor newActor = Actor::New();
    newActor.SetName("Inserted");
    insertList.push_back( Item( view.GetItemId(child), newActor ) );
  }

  if( !insertList.empty() )
  {
    view.InsertItems( insertList, 0.5f );
  }

  DALI_TEST_CHECK(view.GetChildCount() == itemCount + 10);

  ItemIdContainer removeList;

  for( unsigned int i = 0u; i < view.GetChildCount(); ++i )
  {
    Actor child = view.GetChildAt( i );

    if( child.GetName() == "Inserted" )
    {
      removeList.push_back( view.GetItemId(child) );
    }
  }

  if( ! removeList.empty() )
  {
    view.RemoveItems( removeList, 0.5f );
  }

  DALI_TEST_CHECK(view.GetChildCount() == itemCount);
  END_TEST;
}

int UtcDaliItemViewReplaceItemP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  Actor newActor = Actor::New();

  view.ReplaceItem( Item( 0, newActor ), 0.5f );

  DALI_TEST_CHECK(view.GetItem(0) == newActor);
  END_TEST;
}

int UtcDaliItemViewReplaceItemsP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  ItemContainer replaceList;

  for( unsigned int i = 0u; i < 10; ++i )
  {
    Actor child = view.GetChildAt( i );
    Actor newActor = Actor::New();
    newActor.SetName("Replaced");

    replaceList.push_back( Item( view.GetItemId(child), newActor ) );
  }

  if( !replaceList.empty() )
  {
    view.ReplaceItems( replaceList, 0.5f );
  }

  DALI_TEST_CHECK(view.GetItem(0).GetName() == "Replaced");
  DALI_TEST_CHECK(view.GetItem(8).GetName() == "Replaced");
  END_TEST;
}

int UtcDaliItemViewGetItemsRangeP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  ItemRange itemRange(0, 0);

  view.GetItemsRange(itemRange);

  DALI_TEST_CHECK(itemRange.Within(0));
  END_TEST;
}

int UtcDaliItemViewSetItemsAnchorPointP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  Vector3 anchorPoint(10.0f, 10.0f, 0.0f);

  view.SetItemsAnchorPoint(anchorPoint);

  DALI_TEST_CHECK(view.GetItemsAnchorPoint() == anchorPoint);
  DALI_TEST_CHECK(view.GetItem(0).GetCurrentAnchorPoint() == anchorPoint);
  END_TEST;
}

int UtcDaliItemViewSetItemsParentOriginP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  Vector3 parentOrigin(10.0f, 10.0f, 0.0f);

  view.SetItemsParentOrigin(parentOrigin);

  DALI_TEST_CHECK(view.GetItemsParentOrigin() == parentOrigin);
  DALI_TEST_CHECK(view.GetItem(0).GetCurrentParentOrigin() == parentOrigin);
  END_TEST;
}

int UtcDaliItemFactoryGetExtention(void)
{
  ToolkitTestApplication application;
  TestItemFactory factory;
  DALI_TEST_CHECK( factory.GetExtension() == NULL );
  END_TEST;
}

int UtcDaliItemViewLayoutActivatedSignalP(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  Stage::GetCurrent().Add( view );

  // Connect the layout activated signal
  view.LayoutActivatedSignal().Connect( &OnLayoutActivated );

  gOnLayoutActivatedCalled = false;

  // Render and notify
  application.SendNotification();
  application.Render();

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.1f);

  // Wait for 0.1 second
  Wait(application, 100);

  DALI_TEST_EQUALS( gOnLayoutActivatedCalled, true, TEST_LOCATION );

  END_TEST;
}
