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

static void UtcDaliNavigationLayoutNew();
static void UtcDaliNavigationLayoutColumns();
static void UtcDaliNavigationLayoutSetGetOrientation();
static void UtcDaliNavigationLayoutTestConstraintLeft();
static void UtcDaliNavigationLayoutTestConstraintRight();
static void UtcDaliNavigationLayoutTestConstraintUp();
static void UtcDaliNavigationLayoutTestConstraintDown();
static void UtcDaliNavigationLayoutScrollDirection();
static void UtcDaliNavigationLayoutSetGetColumnSpacing();
static void UtcDaliNavigationLayoutSetGetTopMargin();
static void UtcDaliNavigationLayoutSetGetBottomMargin();
static void UtcDaliNavigationLayoutSetGetScrollSpeedFactor();
static void UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed();
static void UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration();
static void UtcDaliNavigationLayoutGetScrollToPosition();


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    {  UtcDaliNavigationLayoutNew, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutColumns, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetOrientation, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutTestConstraintLeft, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutTestConstraintRight, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutTestConstraintUp, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutTestConstraintDown, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutScrollDirection, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetColumnSpacing, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetTopMargin, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetBottomMargin, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetScrollSpeedFactor, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration, POSITIVE_TC_IDX },
    {  UtcDaliNavigationLayoutGetScrollToPosition, POSITIVE_TC_IDX },
    {  NULL, 0 }
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


// Positive test case for a method
static void UtcDaliNavigationLayoutNew()
{
  ToolkitTestApplication application;

  // Create a navigation layout
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  navigationLayout->SetNumberOfColumns(6);
  DALI_TEST_CHECK(navigationLayout);
}

static void UtcDaliNavigationLayoutColumns()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  navigationLayout->SetNumberOfColumns(6);
  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(navigationLayout->GetNumberOfColumns() == 6);
}

static void UtcDaliNavigationLayoutSetGetOrientation()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetOrientation(ControlOrientation::Right);
  DALI_TEST_CHECK(navigationLayout->GetOrientation() == ControlOrientation::Right);
}

static void UtcDaliNavigationLayoutTestConstraintLeft()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
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

  // Confirm: we have actors in the view and all of them is positioned at X = 0
  // and the series is monotonely decreasing.
  int nonZeroXCount = 0;
  int elementsFound = 0;
  int wrongDirectionCount = 0;
  float prevY = FLT_MAX;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.x != 0.0f)
      {
        nonZeroXCount++;
      }

      if (pos.y >= prevY)
      {
        wrongDirectionCount++;
      }

      prevY = pos.y;
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroXCount == 0) && (wrongDirectionCount == 0));
  Stage::GetCurrent().Remove(view);
}

static void UtcDaliNavigationLayoutTestConstraintRight()
{
  ToolkitTestApplication application;

   // Create the ItemView actor
   TestItemFactory factory;
   ItemView view = ItemView::New(factory);
   Vector3 vec(480.0f, 800.0f, 0.0f);
   NavigationLayoutPtr navigationLayout = NavigationLayout::New();
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

   // Confirm: we have actors in the view and all of them is positioned at X = 0
   // and the series is monotonely increasing.
   int nonZeroXCount = 0;
   int elementsFound = 0;
   int wrongDirectionCount = 0;
   float prevY = -FLT_MAX;
   for(unsigned int i = 0; i < 10; i++)
   {
     Actor testActor = view.GetItem(i);
     if (testActor)
     {
       elementsFound++;
       Vector3 pos = testActor.GetCurrentPosition();

       if (pos.x != 0.0f)
       {
         nonZeroXCount++;
       }

       if (pos.y <= prevY)
       {
         wrongDirectionCount++;
       }

       prevY = pos.y;
     }
   }

   DALI_TEST_CHECK((elementsFound > 0) && (nonZeroXCount == 0) && (wrongDirectionCount == 0));
   Stage::GetCurrent().Remove(view);
}

static void UtcDaliNavigationLayoutTestConstraintUp()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
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

  // Confirm: we have actors in the view and all of them is positioned at X = 0
  // and the series is monotonely decreasing.
  int nonZeroYCount = 0;
  int elementsFound = 0;
  int wrongDirectionCount = 0;
  float prevX = -FLT_MAX;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.y != 0.0f)
      {
        nonZeroYCount++;
      }

      if (pos.x <= prevX)
      {
        wrongDirectionCount++;
      }

      prevX = pos.x;
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroYCount == 0) && (wrongDirectionCount == 0));
  Stage::GetCurrent().Remove(view);
}

static void UtcDaliNavigationLayoutTestConstraintDown()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
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

  // Confirm: we have actors in the view and all of them is positioned at X = 0
  // and the series is monotonely decreasing.
  int nonZeroYCount = 0;
  int elementsFound = 0;
  int wrongDirectionCount = 0;
  float prevX = FLT_MAX;
  for(unsigned int i = 0; i < 10; i++)
  {
    Actor testActor = view.GetItem(i);
    if (testActor)
    {
      elementsFound++;
      Vector3 pos = testActor.GetCurrentPosition();

      if (pos.y != 0.0f)
      {
        nonZeroYCount++;
      }

      if (pos.x > prevX)
      {
        wrongDirectionCount++;
      }

      prevX = pos.x;
    }
  }

  DALI_TEST_CHECK((elementsFound > 0) && (nonZeroYCount == 0) && (wrongDirectionCount == 0));
  Stage::GetCurrent().Remove(view);
}


static void UtcDaliNavigationLayoutScrollDirection()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  navigationLayout->SetNumberOfColumns(6);

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
  DALI_TEST_CHECK(deg == (180.0f - 45.0f));

  navigationLayout->SetOrientation(ControlOrientation::Down);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK((deg == -45.0f));

  layout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == (270.0f - 45.0f));

  navigationLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == (90.0f - 45.0f));

  Stage::GetCurrent().Remove(view);
}

static void UtcDaliNavigationLayoutSetGetColumnSpacing()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 11.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetColumnSpacing(testValue);
  DALI_TEST_CHECK(navigationLayout->GetColumnSpacing() == testValue);
}

static void UtcDaliNavigationLayoutSetGetTopMargin()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 11.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetTopMargin(testValue);
  DALI_TEST_CHECK(navigationLayout->GetTopMargin() == testValue);
}

static void UtcDaliNavigationLayoutSetGetBottomMargin()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 12.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetBottomMargin(testValue);
  DALI_TEST_CHECK(navigationLayout->GetBottomMargin() == testValue);
}

static void UtcDaliNavigationLayoutSetGetScrollSpeedFactor()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 15.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetScrollSpeedFactor(testValue);
  DALI_TEST_CHECK(navigationLayout->GetScrollSpeedFactor() == testValue);
}

static void UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed()
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 10.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetMaximumSwipeSpeed(testValue);
  DALI_TEST_CHECK(navigationLayout->GetMaximumSwipeSpeed() == testValue);
}

static void UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration()
{
  ToolkitTestApplication application;

  // Create a navigation layout
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  // Set the flick animaiton duration
  navigationLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( navigationLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
}

static void UtcDaliNavigationLayoutGetScrollToPosition()
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  NavigationLayoutPtr layout = NavigationLayout::New();

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
