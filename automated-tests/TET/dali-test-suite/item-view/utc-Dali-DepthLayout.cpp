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
#include <float.h>       // for FLT_MAX
#include <stdlib.h>
#include <tet_api.h>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;

Vector3 DepthLayoutItemSizeFunction(unsigned int numberOfColumns, float layoutWidth)
{
  float width = (layoutWidth / static_cast<float>(numberOfColumns + 1)) * 0.8f;
  return Vector3(width, width, width);
}

float DepthLayoutBottomMarginFunction(float layoutHeight)
{
  return layoutHeight * 0.25f;
}

float DepthLayoutColumnPositionFunction(unsigned int numberOfColumns, unsigned int columnNumber, const Vector3& itemSize, float layoutWidth)
{
  float availableSpace = layoutWidth - itemSize.width * numberOfColumns;
  float leftMargin = availableSpace / numberOfColumns * 0.5f;
  float columnPosition = leftMargin + itemSize.width * 0.5f + columnNumber * (itemSize.width + availableSpace / numberOfColumns);
  return columnPosition - layoutWidth * 0.5f;
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

static void UtcDaliDepthLayoutNew();
static void UtcDaliDepthLayoutSetAndGetNumberOfColumns();
static void UtcDaliDepthLayoutSetAndGetNumberOfRows();
static void UtcDaliDepthLayoutSetAndGetRowSpacing();
static void UtcDaliDepthLayoutSetAndGetTiltAngle();
static void UtcDaliDepthLayoutSetAndGetItemSizeFunction();
static void UtcDaliDepthLayoutSetAndGetBottomMarginFunction();
static void UtcDaliDepthLayoutSetAndGetItemTiltAngle();
static void UtcDaliDepthLayoutSetAndGetColumnPositionFunction();
static void UtcDaliDepthLayoutSetAndGetScrollSpeedFactor();
static void UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed();
static void UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration();
static void UtcDaliDepthLayoutConstraintLeft();
static void UtcDaliDepthLayoutConstraintRight();
static void UtcDaliDepthLayoutConstraintUp();
static void UtcDaliDepthLayoutConstraintDown();
static void UtcDaliDepthLayoutGetScrollToPosition();
static void UtcDaliDepthLayoutScrollDirection();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliDepthLayoutNew, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutScrollDirection, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetNumberOfColumns, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetNumberOfRows, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetRowSpacing, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetTiltAngle, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetItemSizeFunction, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetBottomMarginFunction, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetItemTiltAngle, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetColumnPositionFunction, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetScrollSpeedFactor, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutConstraintLeft, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutConstraintRight, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutConstraintUp, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutConstraintDown, POSITIVE_TC_IDX },
    { UtcDaliDepthLayoutGetScrollToPosition, POSITIVE_TC_IDX },
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

static void UtcDaliDepthLayoutNew()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  DALI_TEST_CHECK(depthLayout);
}

static void UtcDaliDepthLayoutSetAndGetNumberOfColumns()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the number of columns
  depthLayout->SetNumberOfColumns(5);

  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(depthLayout->GetNumberOfColumns() == 5);
}

static void UtcDaliDepthLayoutSetAndGetNumberOfRows()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the number of rows
  depthLayout->SetNumberOfRows(15);

  // Check whether we get the correct number of rows
  DALI_TEST_CHECK(depthLayout->GetNumberOfRows() == 15);
}

static void UtcDaliDepthLayoutSetAndGetRowSpacing()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the row spacing
  depthLayout->SetRowSpacing(30.0f);

  // Check whether we get the correct row spacing
  DALI_TEST_EQUALS(depthLayout->GetRowSpacing(), 30.0f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutSetAndGetTiltAngle()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the tilt angle
  depthLayout->SetTiltAngle(Degree(25.0f));

  // Check whether we get the correct tilt angle
  DALI_TEST_EQUALS(float(depthLayout->GetTiltAngle()), 25.0f, 0.001f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutSetAndGetItemSizeFunction()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the item size function
  depthLayout->SetItemSizeFunction(DepthLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(depthLayout->GetItemSizeFunction() == DepthLayoutItemSizeFunction);
}

static void UtcDaliDepthLayoutSetAndGetBottomMarginFunction()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the bottom margin function
  depthLayout->SetBottomMarginFunction(DepthLayoutBottomMarginFunction);

  // Check whether we get the correct bottom margin function
  DALI_TEST_CHECK(depthLayout->GetBottomMarginFunction() == DepthLayoutBottomMarginFunction);
}

static void UtcDaliDepthLayoutSetAndGetItemTiltAngle()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the item's tilt angle
  depthLayout->SetItemTiltAngle(Degree(5.0f));

  // Check whether we get the correct item's tilt angle
  DALI_TEST_EQUALS(float(depthLayout->GetItemTiltAngle()), 5.0f, 0.001f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutSetAndGetColumnPositionFunction()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the column position function
  depthLayout->SetColumnPositionFunction(DepthLayoutColumnPositionFunction);

  // Check whether we get the correct column position function
  DALI_TEST_CHECK(depthLayout->GetColumnPositionFunction() == DepthLayoutColumnPositionFunction);
}

static void UtcDaliDepthLayoutSetAndGetScrollSpeedFactor()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the scroll speed factor
  depthLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(depthLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the maximum swipe speed
  depthLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(depthLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration()
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the flick animaiton duration
  depthLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( depthLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
}

static void UtcDaliDepthLayoutConstraintLeft()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr navigationLayout = DepthLayout::New();
  navigationLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*navigationLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  navigationLayout->SetOrientation(ControlOrientation::Left);
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

static void UtcDaliDepthLayoutConstraintRight()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr navigationLayout = DepthLayout::New();
  navigationLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*navigationLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  navigationLayout->SetOrientation(ControlOrientation::Right);
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

static void UtcDaliDepthLayoutConstraintUp()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr navigationLayout = DepthLayout::New();
  navigationLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*navigationLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  navigationLayout->SetOrientation(ControlOrientation::Up);
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

static void UtcDaliDepthLayoutConstraintDown()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr navigationLayout = DepthLayout::New();
  navigationLayout->SetNumberOfColumns(6);

  view.SetName("view actor");
  view.AddLayout(*navigationLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  navigationLayout->SetOrientation(ControlOrientation::Down);
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

static void UtcDaliDepthLayoutGetScrollToPosition()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr layout = DepthLayout::New();

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
      application.Render(16); // 60hz frames

      std::size_t moveCount = 0;
      for(std::size_t i = 0; i < indices.size(); i++)
      {
        float layoutPosBefore = view.GetCurrentLayoutPosition(i);
        view.ScrollToItem(indices[i], 0.0f);

        application.Render(16); // 60hz frame

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

static void UtcDaliDepthLayoutScrollDirection()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  DepthLayoutPtr navigationLayout = DepthLayout::New();

  view.SetName("view actor");
  view.AddLayout(*navigationLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  navigationLayout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  ItemLayoutPtr layout = navigationLayout;

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  navigationLayout->SetOrientation(ControlOrientation::Up);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  Degree deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 180.0f);

  navigationLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg == 0.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 270.0f);

  navigationLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 90.0f);

  Stage::GetCurrent().Remove(view);
}
