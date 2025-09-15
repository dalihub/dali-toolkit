/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali.h>

#include <dali-toolkit/internal/controls/scrollable/item-view/grid-layout.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const unsigned int TOTAL_ITEM_NUMBER    = 200;
const char*        TEST_IMAGE_FILE_NAME = "gallery_image_01.jpg";

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

  /**
   * Virtual destructor.
   */
  virtual ~TestItemFactory()
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
    // Create a renderable actor for this item
    return ImageView::New(TEST_IMAGE_FILE_NAME);
  }
};

} //namespace

int UtcDaliItemLayoutCheckPropertiesSetBeforeActivateLayout(void)
{
  ToolkitTestApplication application;

  TestItemFactory factory;
  ItemView        view = ItemView::New(factory);

  Property::Map gridLayoutProperty;
  gridLayoutProperty.Insert(DefaultItemLayoutProperty::TYPE, Dali::Property::Value((int)DefaultItemLayout::GRID));

  //Set the column of grid-layout.
  gridLayoutProperty.Insert(DefaultItemLayoutProperty::GRID_COLUMN_NUMBER, Dali::Property::Value(6));

  Property::Array layoutArray;

  layoutArray.PushBack(gridLayoutProperty);

  view.SetProperty(ItemView::Property::LAYOUT, layoutArray);

  Dali::Toolkit::Internal::GridLayout* gridLayout = dynamic_cast<Dali::Toolkit::Internal::GridLayout*>(view.GetLayout(0).Get());

  Dali::Integration::Scene scene = application.GetScene();
  Vector3                  stageSize(scene.GetSize());

  //Check if the number of columns is equals to 6 which is set before.
  DALI_TEST_EQUALS(gridLayout->GetNumberOfColumns(), 6, TEST_LOCATION);
  view.ActivateLayout(0, stageSize, 0.0f);
  END_TEST;
}
