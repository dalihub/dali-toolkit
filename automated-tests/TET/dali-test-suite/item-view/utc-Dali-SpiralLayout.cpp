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
#include <dali-toolkit/public-api/controls/default-controls/solid-color-actor.h>
#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;

Vector3 SpiralLayoutItemSizeFunction(const Vector3& layoutSize)
{
  float width = layoutSize.width * 0.2f;
  return Vector3(width, width, width);
}

float SpiralLayoutSpiralRadiusFunction(const Vector3& layoutSize)
{
  return layoutSize.width * 0.5f;
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

static void UtcDaliSpiralLayoutNew();
static void UtcDaliSpiralSetAndGetItemSizeFunction();
static void UtcDaliSpiralSetAndGetItemSpacing();
static void UtcDaliSpiralSetAndGetRevolutionDistance();
static void UtcDaliSpiralSetAndGetSpiralRadiusFunction();
static void UtcDaliSpiralSetAndGetTopItemAlignment();
static void UtcDaliSpiralSetAndGetScrollSpeedFactor();
static void UtcDaliSpiralSetAndGetMaximumSwipeSpeed();
static void UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration();
static void UtcDaliSpiralLayoutConstraintLeft();
static void UtcDaliSpiralLayoutConstraintRight();
static void UtcDaliSpiralLayoutConstraintUp();
static void UtcDaliSpiralLayoutConstraintDown();
static void UtcDaliSpiralLayoutGetScrollToPosition();
static void UtcDaliSpiralLayoutScrollDirection();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliSpiralLayoutScrollDirection, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutNew, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetItemSizeFunction, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetItemSpacing, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetRevolutionDistance, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetSpiralRadiusFunction, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetTopItemAlignment, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetScrollSpeedFactor, POSITIVE_TC_IDX },
    { UtcDaliSpiralSetAndGetMaximumSwipeSpeed, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutConstraintLeft, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutConstraintRight, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutConstraintUp, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutConstraintDown, POSITIVE_TC_IDX },
    { UtcDaliSpiralLayoutGetScrollToPosition, POSITIVE_TC_IDX },
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

static void UtcDaliSpiralLayoutNew()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  DALI_TEST_CHECK(spiralLayout);
}

static void UtcDaliSpiralSetAndGetItemSizeFunction()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the item size function
  spiralLayout->SetItemSizeFunction(SpiralLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(spiralLayout->GetItemSizeFunction() == SpiralLayoutItemSizeFunction);
}

static void UtcDaliSpiralSetAndGetItemSpacing()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the item spacing
  spiralLayout->SetItemSpacing(Radian(0.6f));

  // Check whether we get the correct item spacing
  DALI_TEST_EQUALS(spiralLayout->GetItemSpacing(), 0.6f, TEST_LOCATION );
}

static void UtcDaliSpiralSetAndGetRevolutionDistance()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the revolution distance
  spiralLayout->SetRevolutionDistance(150.0f);

  // Check whether we get the correct revolution distance
  DALI_TEST_EQUALS(spiralLayout->GetRevolutionDistance(), 150.0f, TEST_LOCATION );
}

static void UtcDaliSpiralSetAndGetSpiralRadiusFunction()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the spiral radius function
  spiralLayout->SetSpiralRadiusFunction(SpiralLayoutSpiralRadiusFunction);

  // Check whether we get the correct spiral radius function
  DALI_TEST_CHECK(spiralLayout->GetSpiralRadiusFunction() == SpiralLayoutSpiralRadiusFunction);
}

static void UtcDaliSpiralSetAndGetTopItemAlignment()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the alignment of the top item
  spiralLayout->SetTopItemAlignment(-0.25f);

  // Check whether we get the correct alignment of the top item
  DALI_TEST_EQUALS(spiralLayout->GetTopItemAlignment(), -0.25f, TEST_LOCATION );
}

static void UtcDaliSpiralSetAndGetScrollSpeedFactor()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the scroll speed factor
  spiralLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(spiralLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
}

static void UtcDaliSpiralSetAndGetMaximumSwipeSpeed()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the maximum swipe speed
  spiralLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(spiralLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
}

static void UtcDaliSpiralLayoutSetAndGetItemFlickAnimationDuration()
{
  ToolkitTestApplication application;

  // Create a spiral layout
  SpiralLayoutPtr spiralLayout = SpiralLayout::New();

  // Set the flick animaiton duration
  spiralLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( spiralLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
}

static void UtcDaliSpiralLayoutConstraintLeft()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr layout = SpiralLayout::New();

  view.SetName("view actor");
  view.AddLayout(*layout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  layout->SetOrientation(ControlOrientation::Left);
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

static void UtcDaliSpiralLayoutConstraintRight()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr layout = SpiralLayout::New();

  view.SetName("view actor");
  view.AddLayout(*layout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  layout->SetOrientation(ControlOrientation::Right);
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

static void UtcDaliSpiralLayoutConstraintUp()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr layout = SpiralLayout::New();

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

  layout->GetClosestOnScreenLayoutPosition(0, 0.0f, vec);
  int nextItem = layout->GetNextFocusItemID(0, 10, Dali::Toolkit::Control::Right, false);
  DALI_TEST_CHECK(nextItem == 1);

  Stage::GetCurrent().Remove(view);
}

static void UtcDaliSpiralLayoutConstraintDown()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr layout = SpiralLayout::New();

  view.SetName("view actor");
  view.AddLayout(*layout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  layout->SetOrientation(ControlOrientation::Down);
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

static void UtcDaliSpiralLayoutScrollDirection()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr navigationLayout = SpiralLayout::New();

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
  DALI_TEST_CHECK(deg == -45.0f);

  navigationLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg == 180.0f - 45.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 45.0f);

  navigationLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == (270.0f - 45.0f));

  Stage::GetCurrent().Remove(view);
}

static void UtcDaliSpiralLayoutGetScrollToPosition()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  SpiralLayoutPtr layout = SpiralLayout::New();

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
