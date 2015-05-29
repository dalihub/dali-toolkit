/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
    // Create an image actor for this item
    Image image = ResourceImage::New( TEST_IMAGE_FILE_NAME );
    Actor actor = ImageActor::New(image);

    return actor;
  }
};

class TestItemLayout;

typedef IntrusivePtr<TestItemLayout> TestItemLayoutPtr;

// Implementation of ItemLayout
class TestItemLayout : public ItemLayout
{
public:

  /**
   * Constructor
   */
  TestItemLayout()
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~TestItemLayout()
  {
  }

  /**
   * Create a new grid layout.
   */
  static TestItemLayoutPtr New()
  {
    return TestItemLayoutPtr(new TestItemLayout());
  }

public: // From ItemLayout

  /**
   * Query the minimum valid layout position; this is a negative value.
   *
   * When scrolling, the first item will move within the range 0 to GetMinimumLayoutPosition().
   * @param[in] numberOfItems The current number of items in the layout.
   * @param[in] layoutSize The size of the layout area.
   * @return The minimum layout position.
   */
  virtual float GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
  {
    return 0.0f;
  }

  /**
   * Query the closest anchor position for the given layout position.
   *
   * This anchor position is the position where all the items in the layout are aligned to
   * their rounded layout positions in integer.
   * @param[in] layoutPosition The layout position.
   * @return The closest anchor position for the given layout position.
   */
  virtual float GetClosestAnchorPosition(float layoutPosition) const
  {
    return 0.0f;
  }

  /**
   * Query the layout position for the first item in the layout to move to when the layout
   * needs to scroll to a particular item.
   *
   * @param[in] itemId The ID of an item in the layout.
   * @return The layout position for the first item in the layout to move to.
   */
  virtual float GetItemScrollToPosition(unsigned int itemId) const
  {
    return 0.0f;
  }

  /**
   * Query the items within a given layout-area.
   *
   * @param[in] firstItemPosition The layout-position of the first item in the layout.
   * @param[in] layoutSize The size of the layout area.
   * @return The ID of the first & last visible item.
   */
  virtual ItemRange GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
  {
    return ItemRange(0, 10);
  }

  /**
   * Query the number of items that should be reserved, for scrolling purposes.
   *
   * @param[in] layoutSize The size of the layout area.
   * @return The number of extra items.
   */
  virtual unsigned int GetReserveItemCount(Vector3 layoutSize) const
  {
    return 0;
  }

  /**
   * Retrieve the default size of an item in the layout.
   *
   * @param[in] itemId The ID of an item in the layout.
   * @param[in] layoutSize The layout size
   * @param[out] itemSize The target size of an item.
   */
  virtual void GetDefaultItemSize( unsigned int itemId, const Vector3& layoutSize, Vector3& itemSize ) const
  {
  }

  /**
   * Retrieve the resize animation in the layout.
   *
   * @param[in] animation The resize animation, not owned by the layout
   * @param[in] actor The actor to animate
   * @param [in] size The target size.
   * @param [in] durationSeconds The duration of the resizing.
   */
  virtual void GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
  {
  }

  /**
   * @brief Query the scroll direction of the layout.
   * @return The scroll direction in degrees.
   */
  virtual Degree GetScrollDirection() const
  {
    return Degree( 0.0f );
  }

  /**
   * @brief Query the scroll speed factor of the layout while dragging.
   * @return The scroll speed factor of the layout.
   */
  virtual float GetScrollSpeedFactor() const
  {
    return 0;
  }

  /**
   * @brief Query the maximum swipe speed in pixels per second.
   * @return speed The maximum swipe speed.
   */
  virtual float GetMaximumSwipeSpeed() const
  {
    return 0;
  }

  /**
   * @brief Get the duration of the flick animation in second.
   * @return The duration of the flick animation.
   */
  virtual float GetItemFlickAnimationDuration() const
  {
    return 0;
  }

  /*
   * @brief Applies constraints defined by the layout to an actor.
   *
   * @param[in] actor The actor to constrain.
   * @param[in] itemId The ID of the item represented by the actor.
   * @param[in] layoutSize the current size of the item view instance.
   * @param[in] itemViewActor The item view instance which requests the application of constraints.
   */
  virtual void ApplyConstraints( Actor& actor, const int itemId, const Vector3& layoutSize, const Actor& itemViewActor )
  {
  }

  /**
   * @brief Gets the position of a given item
   *
   * @param[in] itemID id of the item we want to get its position
   * @param[in] currentLayoutPosition the current layout position of the item view instance
   * @param[in] layoutSize the current size of the item view instance
   * @return The item position (x,y,z)
   */
  virtual Vector3 GetItemPosition(int itemID, float currentLayoutPosition, const Vector3& layoutSize) const
  {
    return Vector3::ZERO;
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
  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  view.AddLayout(*gridLayout);

  // Set the orientation of the layout to be horizontal from left to right
  ItemLayoutPtr layout = view.GetLayout(0);

  DALI_TEST_CHECK(gridLayout == layout);

  layout->SetOrientation(ControlOrientation::Left);

  // Check the orientation of the layout is horizontal from left to right
  DALI_TEST_CHECK(layout->GetOrientation() == ControlOrientation::Left);

  Vector3 itemSize(100.0f, 100.0f, 100.0f);
  layout->SetItemSize(itemSize);

  Vector3 itemSize1;
  layout->GetItemSize(0u, Vector3(Stage::GetCurrent().GetSize()), itemSize1);

  DALI_TEST_CHECK(itemSize == itemSize1);

  float position = layout->GetClosestOnScreenLayoutPosition(0, 0.0f, Vector3(Stage::GetCurrent().GetSize()));

  DALI_TEST_EQUALS(position, 0.0f, TEST_LOCATION);

  int focusItem = layout->GetNextFocusItemID(0, TOTAL_ITEM_NUMBER, Control::Left, true);

  DALI_TEST_CHECK(focusItem != 0);

  float flickSpeedFactor = layout->GetFlickSpeedFactor();

  DALI_TEST_CHECK(flickSpeedFactor != 0.0f);

  ItemLayoutPtr depthLayout = DefaultItemLayout::New( DefaultItemLayout::DEPTH );
  view.AddLayout(*depthLayout);

  layout = view.GetLayout(1);
  DALI_TEST_CHECK(depthLayout == layout);

  ItemLayoutPtr listLayout = DefaultItemLayout::New( DefaultItemLayout::LIST );
  view.AddLayout(*listLayout);

  layout = view.GetLayout(2);
  DALI_TEST_CHECK(listLayout == layout);

  ItemLayoutPtr spiralLayout = DefaultItemLayout::New( DefaultItemLayout::SPIRAL );
  view.AddLayout(*spiralLayout);

  layout = view.GetLayout(3);
  DALI_TEST_CHECK(spiralLayout == layout);
  END_TEST;
}

int UtcDaliItemLayoutGetExtension(void)
{
  ToolkitTestApplication application;

  ItemLayoutPtr gridLayout = DefaultItemLayout::New( DefaultItemLayout::GRID );
  DALI_TEST_CHECK( gridLayout );
  DALI_TEST_CHECK( !gridLayout->GetExtension() );

  END_TEST;
}

int UtcDaliItemLayoutGetClosestOnScreenLayoutPosition(void)
{
  ToolkitTestApplication application;

  TestItemLayoutPtr layout = TestItemLayout::New();
  DALI_TEST_CHECK( layout );
  DALI_TEST_EQUALS(layout->GetClosestOnScreenLayoutPosition(0, 0.0f, Vector3::ZERO), 0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS(layout->GetClosestOnScreenLayoutPosition(0, 0.0f, Vector3(-800.0f, -1200.0f, 0.0f)), 0.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliItemLayoutGetNextFocusItemID(void)
{
  ToolkitTestApplication application;

  TestItemLayoutPtr layout = TestItemLayout::New();
  DALI_TEST_CHECK( layout );
  DALI_TEST_EQUALS(layout->GetNextFocusItemID(0, 100, Control::Left, true), 99, TEST_LOCATION );
  DALI_TEST_EQUALS(layout->GetNextFocusItemID(110, 100, Control::Right, true), 0, TEST_LOCATION );

  END_TEST;
}

