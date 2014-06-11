//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <float.h>       // for FLT_MAX
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;

Vector3 GridLayoutItemSizeFunction(unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)
{
  float width = (layoutWidth - sideMargin * 2.0f - columnSpacing * static_cast<float>(numberOfColumns - 1)) / static_cast<float>(numberOfColumns);

  return Vector3(width, width, width);
}

} // namespace

static void Startup();
static void Cleanup();

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
    // Create an test actor for this item
    ImageActor actor = CreateSolidColorActor(Color::RED);
    actor.SetSize(64.0f, 64.0f);
    return actor;
  }
};

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliGridLayoutNew();
static void UtcDaliGridLayoutSetAndGetNumberOfColumns();
static void UtcDaliGridLayoutSetAndGetRowSpacing();
static void UtcDaliGridLayoutSetAndGetColumnSpacing();
static void UtcDaliGridLayoutSetAndGetTopMargin();
static void UtcDaliGridLayoutSetAndGetBottomMargin();
static void UtcDaliGridLayoutSetAndGetSideMargin();
static void UtcDaliGridLayoutSetAndGetZGap();
static void UtcDaliGridLayoutSetAndGetItemSizeFunction();
static void UtcDaliGridLayoutSetAndGetScrollSpeedFactor();
static void UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed();
static void UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration();
static void UtcDaliGridLayoutConstraintLeft();
static void UtcDaliGridLayoutConstraintRight();
static void UtcDaliGridLayoutConstraintUp();
static void UtcDaliGridLayoutConstraintDown();
static void UtcDaliGridLayoutScrollDirection();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliGridLayoutNew, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetNumberOfColumns, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetRowSpacing, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetColumnSpacing, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetTopMargin, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetBottomMargin, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetSideMargin, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetZGap, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetItemSizeFunction, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetScrollSpeedFactor, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutConstraintLeft, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutConstraintRight, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutConstraintUp, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutConstraintDown, POSITIVE_TC_IDX },
    { UtcDaliGridLayoutScrollDirection, POSITIVE_TC_IDX },
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

static void UtcDaliGridLayoutNew()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  DALI_TEST_CHECK(gridLayout);
}

static void UtcDaliGridLayoutSetAndGetNumberOfColumns()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the number of columns
  gridLayout->SetNumberOfColumns(6);

  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(gridLayout->GetNumberOfColumns() == 6);
}

static void UtcDaliGridLayoutSetAndGetRowSpacing()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the row spacing
  gridLayout->SetRowSpacing(10.0f);

  // Check whether we get the correct row spacing
  DALI_TEST_EQUALS(gridLayout->GetRowSpacing(), 10.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetColumnSpacing()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the column spacing
  gridLayout->SetColumnSpacing(10.0f);

  // Check whether we get the correct column spacing
  DALI_TEST_EQUALS(gridLayout->GetColumnSpacing(), 10.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetTopMargin()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the top margin
  gridLayout->SetTopMargin(30.0f);

  // Check whether we get the correct top margin
  DALI_TEST_EQUALS(gridLayout->GetTopMargin(), 30.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetBottomMargin()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the bottom margin
  gridLayout->SetBottomMargin(30.0f);

  // Check whether we get the correct bottom margin
  DALI_TEST_EQUALS(gridLayout->GetBottomMargin(), 30.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetSideMargin()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the side margin
  gridLayout->SetSideMargin(10.0f);

  // Check whether we get the correct side margin
  DALI_TEST_EQUALS(gridLayout->GetSideMargin(), 10.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetZGap()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the gap of items in the Z axis in different columns
  gridLayout->SetZGap(5.0f);

  // Check whether we get the correct Z gap
  DALI_TEST_EQUALS(gridLayout->GetZGap(), 5.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetItemSizeFunction()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the item size function
  gridLayout->SetItemSizeFunction(GridLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(gridLayout->GetItemSizeFunction() == GridLayoutItemSizeFunction);
}

static void UtcDaliGridLayoutSetAndGetScrollSpeedFactor()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the scroll speed factor
  gridLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(gridLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the maximum swipe speed
  gridLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(gridLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
}

static void UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration()
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the flick animaiton duration
  gridLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( gridLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
}

static void UtcDaliGridLayoutConstraintLeft()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*gridLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  gridLayout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  // Confirm: we have actors in the view and they are positioned some distance from the origin.
  int nonZeroCount = 0;
  int elementsFound = 0;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.LengthSquared() > 0.0f)
      {
        nonZeroCount++;
      }
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroCount == elementsFound));
  Stage::GetCurrent().Remove(view);
}

static void UtcDaliGridLayoutConstraintRight()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*gridLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  gridLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  // Confirm: we have actors in the view and they are positioned some distance from the origin.
  int nonZeroCount = 0;
  int elementsFound = 0;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.LengthSquared() > 0.0f)
      {
        nonZeroCount++;
      }
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroCount == elementsFound));
  Stage::GetCurrent().Remove(view);
}

static void UtcDaliGridLayoutConstraintUp()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*gridLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  gridLayout->SetOrientation(ControlOrientation::Up);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  // Confirm: we have actors in the view and they are positioned some distance from the origin.
  int nonZeroCount = 0;
  int elementsFound = 0;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.LengthSquared() > 0.0f)
      {
        nonZeroCount++;
      }
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroCount == elementsFound));

  ItemLayoutPtr layout = gridLayout;
  layout->GetClosestOnScreenLayoutPosition(0, 0.0f, vec);
  int nextItem = layout->GetNextFocusItemID(0, 10, Dali::Toolkit::Control::Right, false);
  DALI_TEST_CHECK(nextItem == 1);

  Stage::GetCurrent().Remove(view);
}

static void UtcDaliGridLayoutConstraintDown()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*gridLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  gridLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  // Confirm: we have actors in the view and they are positioned some distance from the origin.
  int nonZeroCount = 0;
  int elementsFound = 0;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.LengthSquared() > 0.0f)
      {
        nonZeroCount++;
      }
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroCount == elementsFound));
  Stage::GetCurrent().Remove(view);
}

static void UtcDaliGridLayoutScrollDirection()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  GridLayoutPtr gridLayout = GridLayout::New();
  gridLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*gridLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  gridLayout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  ItemLayoutPtr layout = gridLayout;

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  gridLayout->SetOrientation(ControlOrientation::Up);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  Degree deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 0.0f);

  gridLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg == 180.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 90.f);

  gridLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 270.0f);

  Stage::GetCurrent().Remove(view);
}
