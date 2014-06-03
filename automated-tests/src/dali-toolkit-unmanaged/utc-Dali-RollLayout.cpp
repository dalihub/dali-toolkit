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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;

Vector3 RollLayoutItemSizeFunction(float layoutWidth, float layoutHeight, float rowSpacing)
{
  float height = (layoutHeight - rowSpacing) * 0.5f;
  return Vector3(layoutWidth, height, height);
}

} // namespace


// Implementation of ItemFactory for providing actors to ItemView
class TestItemFactory : public ItemFactory
{
public:

  /**
   * Constructor
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

void dali_roll_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_roll_layout_cleanup(void)
{
  test_return_value = TET_PASS;
}



int UtcDaliRollLayoutNew(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  DALI_TEST_CHECK(rollLayout);
  END_TEST;
}

int UtcDaliRollLayoutSetAndGetRowSpacing(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  // Set the row spacing
  rollLayout->SetRowSpacing(10.0f);

  // Check whether we get the correct row spacing
  DALI_TEST_EQUALS(rollLayout->GetRowSpacing(), 10.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRollLayoutSetAndGetItemSizeFunction(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  // Set the item size function
  rollLayout->SetItemSizeFunction(RollLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(rollLayout->GetItemSizeFunction() == RollLayoutItemSizeFunction);
  END_TEST;
}

int UtcDaliRollLayoutSetAndGetScrollSpeedFactor(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  // Set the scroll speed factor
  rollLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS(rollLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRollLayoutSetAndGetMaximumSwipeSpeed(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  // Set the maximum swipe speed
  rollLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS(rollLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRollLayoutSetAndGetItemFlickAnimationDuration(void)
{
  ToolkitTestApplication application;

  // Create a roll layout
  RollLayoutPtr rollLayout = RollLayout::New();

  // Set the flick animaiton duration
  rollLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( rollLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRollLayoutConstraintLeft(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  RollLayoutPtr rollLayout = RollLayout::New();

  view.SetName("view actor");
  view.AddLayout(*rollLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  rollLayout->SetOrientation(ControlOrientation::Left);
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

int UtcDaliRollLayoutConstraintRight(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  RollLayoutPtr rollLayout = RollLayout::New();

  view.SetName("view actor");
  view.AddLayout(*rollLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  rollLayout->SetOrientation(ControlOrientation::Right);
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

int UtcDaliRollLayoutConstraintUp(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  RollLayoutPtr rollLayout = RollLayout::New();

  view.SetName("view actor");
  view.AddLayout(*rollLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  rollLayout->SetOrientation(ControlOrientation::Up);
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

int UtcDaliRollLayoutConstraintDown(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  RollLayoutPtr rollLayout = RollLayout::New();

  view.SetName("view actor");
  view.AddLayout(*rollLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  rollLayout->SetOrientation(ControlOrientation::Down);
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

int UtcDaliRollLayoutScrollDirection(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);
  Vector3 vec(480.0f, 800.0f, 0.0f);
  RollLayoutPtr rollLayout = RollLayout::New();

  view.SetName("view actor");
  view.AddLayout(*rollLayout);
  view.SetSize(vec);

  Stage::GetCurrent().Add(view);
  rollLayout->SetOrientation(ControlOrientation::Left);
  view.ActivateLayout(0, vec, 0.0f);

  application.SendNotification();
  application.Render(0);

  ItemLayoutPtr layout = rollLayout;

  // render 10 frames
  for(int i = 0; i < 10; ++i)
  {
    application.Render(16); // 60hz frames
  }

  rollLayout->SetOrientation(ControlOrientation::Up);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  Degree deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 0.0f);

  rollLayout->SetOrientation(ControlOrientation::Down);
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

  rollLayout->SetOrientation(ControlOrientation::Right);
  view.ActivateLayout(0, vec, 0.0f);
  application.SendNotification();
  application.Render();

  deg = layout->GetScrollDirection();
  DALI_TEST_CHECK(deg == 270.0f);

  Stage::GetCurrent().Remove(view);
  END_TEST;
}
