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
#include <float.h>       // for FLT_MAX
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

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


void depth_layout_startup(void)
{
}

void depth_layout_cleanup(void)
{
}

int UtcDaliDepthLayoutNew(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  DALI_TEST_CHECK(depthLayout);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetNumberOfColumns(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the number of columns
  depthLayout->SetNumberOfColumns(5);

  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(depthLayout->GetNumberOfColumns() == 5);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetNumberOfRows(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the number of rows
  depthLayout->SetNumberOfRows(15);

  // Check whether we get the correct number of rows
  DALI_TEST_CHECK(depthLayout->GetNumberOfRows() == 15);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetRowSpacing(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the row spacing
  depthLayout->SetRowSpacing(30.0f);

  // Check whether we get the correct row spacing
  DALI_TEST_EQUALS(depthLayout->GetRowSpacing(), 30.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetTiltAngle(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the tilt angle
  depthLayout->SetTiltAngle(Degree(25.0f));

  // Check whether we get the correct tilt angle
  DALI_TEST_EQUALS(depthLayout->GetTiltAngle(), Degree(25.0f), 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetItemSizeFunction(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the item size function
  depthLayout->SetItemSizeFunction(DepthLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(depthLayout->GetItemSizeFunction() == DepthLayoutItemSizeFunction);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetBottomMarginFunction(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the bottom margin function
  depthLayout->SetBottomMarginFunction(DepthLayoutBottomMarginFunction);

  // Check whether we get the correct bottom margin function
  DALI_TEST_CHECK(depthLayout->GetBottomMarginFunction() == DepthLayoutBottomMarginFunction);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetItemTiltAngle(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the item's tilt angle
  depthLayout->SetItemTiltAngle(Degree(5.0f));

  // Check whether we get the correct item's tilt angle
  DALI_TEST_EQUALS(depthLayout->GetItemTiltAngle(), Degree(5.0f), 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetColumnPositionFunction(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the column position function
  depthLayout->SetColumnPositionFunction(DepthLayoutColumnPositionFunction);

  // Check whether we get the correct column position function
  DALI_TEST_CHECK(depthLayout->GetColumnPositionFunction() == DepthLayoutColumnPositionFunction);
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetScrollSpeedFactor(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the scroll speed factor
  depthLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(depthLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetMaximumSwipeSpeed(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the maximum swipe speed
  depthLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(depthLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutSetAndGetItemFlickAnimationDuration(void)
{
  ToolkitTestApplication application;

  // Create a depth layout
  DepthLayoutPtr depthLayout = DepthLayout::New();

  // Set the flick animaiton duration
  depthLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( depthLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliDepthLayoutConstraintLeft(void)
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
  END_TEST;
}

int UtcDaliDepthLayoutConstraintRight(void)
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
  END_TEST;
}

int UtcDaliDepthLayoutConstraintUp(void)
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
  END_TEST;
}

int UtcDaliDepthLayoutConstraintDown(void)
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
  END_TEST;
}

int UtcDaliDepthLayoutGetScrollToPosition(void)
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
  END_TEST;
}

int UtcDaliDepthLayoutScrollDirection(void)
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
  DALI_TEST_CHECK(deg.degree == 180.0f);

  navigationLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg.degree == 0.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg.degree == 270.0f);

  navigationLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg.degree == 90.0f);

  Stage::GetCurrent().Remove(view);
  END_TEST;
}
