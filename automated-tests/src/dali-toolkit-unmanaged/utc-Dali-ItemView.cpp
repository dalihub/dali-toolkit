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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER = 100;
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



int UtcDaliItemViewAddAndGetLayout(void)
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
  END_TEST;
}

int UtcDaliItemViewAddAndRemoveLayout(void)
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
  END_TEST;
}

int UtcDaliItemViewActivateLayoutAndGetActiveLayout(void)
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
  END_TEST;
}
