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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 200;
const char* TEST_IMAGE_FILE_NAME = "gallery_image_01.jpg";


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
} // namespace

int UtcDaliItemLayoutSetAndGetOrientation(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // Set the orientation of the layout to be horizontal from left to right
  ItemLayoutPtr layout = view.GetLayout(0);
  layout->SetOrientation(ControlOrientation::Left);

  // Check the orientation of the layout is horizontal from left to right
  DALI_TEST_CHECK(layout->GetOrientation() == ControlOrientation::Left);
  END_TEST;
}

int UtcDaliItemLayoutGetScrollHints(void)
{
  ToolkitTestApplication application;

  // Create the ItemView actor
  TestItemFactory factory;
  ItemView view = ItemView::New(factory);

  // Create a grid layout and add it to ItemView
  GridLayoutPtr gridLayout = GridLayout::New();
  view.AddLayout(*gridLayout);

  // Set the orientation of the layout to be horizontal from left to right
  ItemLayoutPtr layout = view.GetLayout(0);

  Vector2 axisScrollHint;

  layout->SetOrientation(ControlOrientation::Up);
  layout->GetXAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::ZERO, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  layout->GetYAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::YAXIS, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  layout->SetOrientation(ControlOrientation::Down);
  layout->GetXAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::ZERO, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  layout->GetYAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::YAXIS, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  layout->SetOrientation(ControlOrientation::Left);
  layout->GetXAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::XAXIS, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  layout->GetYAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::ZERO, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  layout->SetOrientation(ControlOrientation::Right);
  layout->GetXAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::XAXIS, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  layout->GetYAxisScrollHint(axisScrollHint);
  DALI_TEST_EQUALS(axisScrollHint, Vector2::ZERO, Math::MACHINE_EPSILON_1, TEST_LOCATION);
  END_TEST;
}
