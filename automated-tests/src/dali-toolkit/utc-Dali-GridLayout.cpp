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

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_grid_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_grid_layout_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

const unsigned int TOTAL_ITEM_NUMBER = 200;

Vector3 GridLayoutItemSizeFunction(unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)
{
  float width = (layoutWidth - sideMargin * 2.0f - columnSpacing * static_cast<float>(numberOfColumns - 1)) / static_cast<float>(numberOfColumns);

  return Vector3(width, width, width);
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
    // Create an test actor for this item
    ImageActor actor = CreateSolidColorActor(Color::RED);
    actor.SetSize(64.0f, 64.0f);
    return actor;
  }
};

} // namespace



int UtcDaliGridLayoutNew(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  DALI_TEST_CHECK(gridLayout);
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetNumberOfColumns(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the number of columns
  gridLayout->SetNumberOfColumns(6);

  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(gridLayout->GetNumberOfColumns() == 6);
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetRowSpacing(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the row spacing
  gridLayout->SetRowSpacing(10.0f);

  // Check whether we get the correct row spacing
  DALI_TEST_EQUALS(gridLayout->GetRowSpacing(), 10.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetColumnSpacing(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the column spacing
  gridLayout->SetColumnSpacing(10.0f);

  // Check whether we get the correct column spacing
  DALI_TEST_EQUALS(gridLayout->GetColumnSpacing(), 10.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetTopMargin(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the top margin
  gridLayout->SetTopMargin(30.0f);

  // Check whether we get the correct top margin
  DALI_TEST_EQUALS(gridLayout->GetTopMargin(), 30.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetBottomMargin(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the bottom margin
  gridLayout->SetBottomMargin(30.0f);

  // Check whether we get the correct bottom margin
  DALI_TEST_EQUALS(gridLayout->GetBottomMargin(), 30.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetSideMargin(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the side margin
  gridLayout->SetSideMargin(10.0f);

  // Check whether we get the correct side margin
  DALI_TEST_EQUALS(gridLayout->GetSideMargin(), 10.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetZGap(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the gap of items in the Z axis in different columns
  gridLayout->SetZGap(5.0f);

  // Check whether we get the correct Z gap
  DALI_TEST_EQUALS(gridLayout->GetZGap(), 5.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetItemSizeFunction(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the item size function
  gridLayout->SetItemSizeFunction(GridLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(gridLayout->GetItemSizeFunction() == GridLayoutItemSizeFunction);
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetScrollSpeedFactor(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the scroll speed factor
  gridLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(gridLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetMaximumSwipeSpeed(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the maximum swipe speed
  gridLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(gridLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutSetAndGetItemFlickAnimationDuration(void)
{
  ToolkitTestApplication application;

  // Create a grid layout
  GridLayoutPtr gridLayout = GridLayout::New();

  // Set the flick animaiton duration
  gridLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( gridLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGridLayoutConstraintLeft(void)
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
  END_TEST;
}

int UtcDaliGridLayoutConstraintRight(void)
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
  END_TEST;
}

int UtcDaliGridLayoutConstraintUp(void)
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
  END_TEST;
}

int UtcDaliGridLayoutConstraintDown(void)
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
  END_TEST;
}

int UtcDaliGridLayoutScrollDirection(void)
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
  DALI_TEST_CHECK(deg.degree == 0.0f);

  gridLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg.degree == 180.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg.degree == 90.f);

  gridLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg.degree == 270.0f);

  Stage::GetCurrent().Remove(view);
  END_TEST;
}
