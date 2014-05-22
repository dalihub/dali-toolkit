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
#include <tet_api.h>

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 100;
const char* TEST_IMAGE_FILE_NAME = DALI_IMAGE_DIR "gallery_image_01.jpg";

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

} // namespace

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliItemViewNew();
static void UtcDaliItemViewDownCast();
static void UtcDaliItemViewAddAndGetLayout();
static void UtcDaliItemViewAddAndRemoveLayout();
static void UtcDaliItemViewActivateLayoutAndGetActiveLayout();
static void UtcDaliItemViewDeactivateCurrentLayout();
static void UtcDaliItemViewGetItemAndGetItemId();
static void UtcDaliItemViewRemoveItem();
static void UtcDaliItemViewGetCurrentLayoutPosition();
static void UtcDaliItemViewSetAndGetMinimumSwipeSpeed();
static void UtcDaliItemViewSetAndGetMinimumSwipeDistance();
static void UtcDaliItemViewSetAndGetAnchoring();
static void UtcDaliItemViewSetAndGetAnchoringDuration();
static void UtcDaliItemViewSetAndGetRefreshInterval();
static void UtcDaliItemViewScrollToItem();
static void UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliItemViewNew, POSITIVE_TC_IDX },
    { UtcDaliItemViewDownCast, POSITIVE_TC_IDX },
    { UtcDaliItemViewAddAndGetLayout, POSITIVE_TC_IDX },
    { UtcDaliItemViewAddAndRemoveLayout, POSITIVE_TC_IDX },
    { UtcDaliItemViewActivateLayoutAndGetActiveLayout, POSITIVE_TC_IDX },
    { UtcDaliItemViewDeactivateCurrentLayout, POSITIVE_TC_IDX },
    { UtcDaliItemViewGetItemAndGetItemId, POSITIVE_TC_IDX },
    { UtcDaliItemViewRemoveItem, POSITIVE_TC_IDX },
    { UtcDaliItemViewGetCurrentLayoutPosition, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetMinimumSwipeSpeed, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetMinimumSwipeDistance, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetAnchoring, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetAnchoringDuration, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetRefreshInterval, POSITIVE_TC_IDX },
    { UtcDaliItemViewScrollToItem, POSITIVE_TC_IDX },
    { UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
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
    Image image = Image::New( TEST_IMAGE_FILE_NAME );
    Actor actor = ImageActor::New(image);

    return actor;
  }
};

static void UtcDaliItemViewNew()
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
}

static void UtcDaliItemViewDownCast()
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
}

static void UtcDaliItemViewAddAndGetLayout()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // As we have added one layout, check the number of layout is now 1
  DALI_TEST_CHECK(view.GetLayoutCount() == 1);

  // Create a depth layout and add it to ItemView
  DepthLayoutPtr depthLayout = DepthLayout::New();
  view.AddLayout(*depthLayout);

  // As we have added another layout, check the number of layout is now 2
  DALI_TEST_CHECK(view.GetLayoutCount() == 2);

  // Create a spiral layout and add it to ItemView
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();
  view.AddLayout(*spiralLayout);

  // As we have added another layout, check the number of layout is now 3
  DALI_TEST_CHECK(view.GetLayoutCount() == 3);

  // Check we are getting the correct layout from ItemView
  DALI_TEST_CHECK(view.GetLayout(0) == gridLayout);
  DALI_TEST_CHECK(view.GetLayout(1) == depthLayout);
  DALI_TEST_CHECK(view.GetLayout(2) == spiralLayout);
}

static void UtcDaliItemViewAddAndRemoveLayout()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // As we have added one layout, check the number of layout is now 1
  DALI_TEST_CHECK(view.GetLayoutCount() == 1);

  // Create a depth layout and add it to ItemView
  DepthLayoutPtr depthLayout = DepthLayout::New();
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
}

static void UtcDaliItemViewActivateLayoutAndGetActiveLayout()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // Create a depth layout and add it to ItemView
  DepthLayoutPtr depthLayout = DepthLayout::New();
  view.AddLayout(*depthLayout);

  // Create a spiral layout and add it to ItemView
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();
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
}

static void UtcDaliItemViewDeactivateCurrentLayout()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
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
}

static void UtcDaliItemViewGetItemAndGetItemId()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.5f);

  // Get the item given the item ID
  Actor itemActor = view.GetItem(2);

  // Check we are getting the correct Item ID given the specified actor
  DALI_TEST_CHECK(view.GetItemId(itemActor) == 2);
}

static void UtcDaliItemViewRemoveItem()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
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
}

static void UtcDaliItemViewGetCurrentLayoutPosition()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // Activate the grid layout so that the items will be created and added to ItemView
  Vector3 stageSize(Dali::Stage::GetCurrent().GetSize());
  view.ActivateLayout(0, stageSize, 0.0f);

  // Check the current layout position for the 10th items is 9.0f
  DALI_TEST_EQUALS(view.GetCurrentLayoutPosition(9), 9.0f, TEST_LOCATION );
}

static void UtcDaliItemViewSetAndGetMinimumSwipeSpeed()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the minimum swipe speed to be 1.5f
  view.SetMinimumSwipeSpeed(1.5f);

  // Check the minimum swipe speed is 1.5f
  DALI_TEST_EQUALS(view.GetMinimumSwipeSpeed(), 1.5f, TEST_LOCATION );
}

static void UtcDaliItemViewSetAndGetMinimumSwipeDistance()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the minimum swipe distance to be 2.5f
  view.SetMinimumSwipeDistance(2.5f);

  // Check the minimum swipe distance is 2.5f
  DALI_TEST_EQUALS(view.GetMinimumSwipeDistance(), 2.5f, TEST_LOCATION );
}

static void UtcDaliItemViewSetAndGetAnchoring()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Disable the anchor animation
  view.SetAnchoring(false);

  // Check the anchor animation is disabled
  DALI_TEST_CHECK(view.GetAnchoring() == false);
}

static void UtcDaliItemViewSetAndGetAnchoringDuration()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the duration of anchor animation to be 1.5f
  view.SetAnchoringDuration(1.5f);

  // Check the duration of anchor animation is 1.5f
  DALI_TEST_EQUALS(view.GetAnchoringDuration(), 1.5f, TEST_LOCATION );
}

static void UtcDaliItemViewSetAndGetRefreshInterval()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the interval between refreshes to be 20
  view.SetRefreshInterval(20);

  // Check the interval between refreshes is 20
  DALI_TEST_CHECK(view.GetRefreshInterval() == 20);
}

static void UtcDaliItemViewScrollToItem()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr layout = GridLayout::New();

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
}

static void UtcDaliItemViewSetAndGetMouseWheelScrollDistanceStep()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Set the scroll distance step for the mouse wheel event to be 100.0f
  view.SetMouseWheelScrollDistanceStep(100.0f);

  // Check the scroll distance step is 100.0f
  DALI_TEST_EQUALS(view.GetMouseWheelScrollDistanceStep(), 100.0f, TEST_LOCATION );
}
