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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;


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


// Positive test case for a method
int UtcDaliNavigationLayoutNew(void)
{
  ToolkitTestApplication application;

  // Create a navigation layout
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  navigationLayout->SetNumberOfColumns(6);
  DALI_TEST_CHECK(navigationLayout);
  END_TEST;
}

int UtcDaliNavigationLayoutColumns(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  navigationLayout->SetNumberOfColumns(6);
  // Check whether we get the correct number of columns
  DALI_TEST_CHECK(navigationLayout->GetNumberOfColumns() == 6);
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetOrientation(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetOrientation(ControlOrientation::Right);
  DALI_TEST_CHECK(navigationLayout->GetOrientation() == ControlOrientation::Right);
  END_TEST;
}

int UtcDaliNavigationLayoutTestConstraintLeft(void)
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
  END_TEST;
}

int UtcDaliNavigationLayoutTestConstraintRight(void)
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
   END_TEST;
}

int UtcDaliNavigationLayoutTestConstraintUp(void)
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
  END_TEST;
}

int UtcDaliNavigationLayoutTestConstraintDown(void)
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
  END_TEST;
}


int UtcDaliNavigationLayoutScrollDirection(void)
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
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetColumnSpacing(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 11.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetColumnSpacing(testValue);
  DALI_TEST_CHECK(navigationLayout->GetColumnSpacing() == testValue);
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetTopMargin(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 11.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetTopMargin(testValue);
  DALI_TEST_CHECK(navigationLayout->GetTopMargin() == testValue);
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetBottomMargin(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 12.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetBottomMargin(testValue);
  DALI_TEST_CHECK(navigationLayout->GetBottomMargin() == testValue);
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetScrollSpeedFactor(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 15.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetScrollSpeedFactor(testValue);
  DALI_TEST_CHECK(navigationLayout->GetScrollSpeedFactor() == testValue);
  END_TEST;
}

int UtcDaliNavigationLayoutSetGetMaximumSwipeSpeed(void)
{
  ToolkitTestApplication application;
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();
  const float testValue = 10.0f;

  navigationLayout->SetNumberOfColumns(6);
  navigationLayout->SetMaximumSwipeSpeed(testValue);
  DALI_TEST_CHECK(navigationLayout->GetMaximumSwipeSpeed() == testValue);
  END_TEST;
}

int UtcDaliNavigationLayoutSetAndGetItemFlickAnimationDuration(void)
{
  ToolkitTestApplication application;

  // Create a navigation layout
  NavigationLayoutPtr navigationLayout = NavigationLayout::New();

  // Set the flick animaiton duration
  navigationLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( navigationLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliNavigationLayoutGetScrollToPosition(void)
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
  END_TEST;
}
