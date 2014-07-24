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
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

using namespace Toolkit;

using namespace std;


namespace
{

Vector3 AlbumLayoutItemSizeFunction(const Vector3& layoutSize)
{
  float width = layoutSize.width * 0.2f;
  return Vector3(width, width, width);
}

float AlbumLayoutAlbumRadiusFunction(const Vector3& layoutSize)
{
  return layoutSize.width * 0.5f;
}
} // namespace


void album_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void album_layout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAlbumLayoutNew(void)
{
  ToolkitTestApplication application;

  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  DALI_TEST_CHECK(albumLayout);
  END_TEST;
}

int UtcDaliAlbumSetAndGetItemSizeFunction(void)
{
  ToolkitTestApplication application;

  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  // Set the item size function
  albumLayout->SetItemSizeFunction(AlbumLayoutItemSizeFunction);

  // Check whether we get the correct item size function
  DALI_TEST_CHECK(albumLayout->GetItemSizeFunction() == AlbumLayoutItemSizeFunction);
  END_TEST;
}

int UtcDaliAlbumSetAndGetScrollSpeedFactor(void)
{
  ToolkitTestApplication application;

  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  // Set the scroll speed factor
  albumLayout->SetScrollSpeedFactor(0.05f);

  // Check whether we get the correct scroll speed factor
  DALI_TEST_EQUALS( albumLayout->GetScrollSpeedFactor(), 0.05f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAlbumSetAndGetMaximumSwipeSpeed(void)
{
  ToolkitTestApplication application;

  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  // Set the maximum swipe speed
  albumLayout->SetMaximumSwipeSpeed(50.0f);

  // Check whether we get the correct maximum swipe speed
  DALI_TEST_EQUALS( albumLayout->GetMaximumSwipeSpeed(), 50.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAlbumLayoutSetAndGetItemFlickAnimationDuration(void)
{
  ToolkitTestApplication application;

  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  // Set the flick animaiton duration
  albumLayout->SetItemFlickAnimationDuration(0.35f);

  // Check whether we get the correct flick animaiton duration
  DALI_TEST_EQUALS( albumLayout->GetItemFlickAnimationDuration(), 0.35f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliAlbumSetNumOfItems(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetNumOfItems(15);

  DALI_TEST_CHECK(albumLayout->GetNumOfItems() == 15);
  END_TEST;
}

int UtcDaliAlbumSetStackNum(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetStackNum(30);

  DALI_TEST_CHECK(albumLayout->GetStackNum() == 30);
  END_TEST;
}

int UtcDaliAlbumSetPosition(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  std::vector<Vector3> position_list;

  /*(0.0f)*/
  Vector3 pos = Vector3(850.0f,-250.0f,0.0);
  position_list.push_back(pos);

  /*(1.0f)*/
  pos = Vector3(700.0f,50.0f,0.0);
  position_list.push_back(pos);

  /*(2.0f)*/
  pos = Vector3(440.0f,227.0f,0.0);
  position_list.push_back(pos);

  /*(4.0f)*/
  pos = Vector3(-440.0f,227.0f,0.0);
  position_list.push_back(pos);

  /*(5.0f)*/
  pos = Vector3(-700.0f,50.0f,0.0);
  position_list.push_back(pos);

  /*(6.0f)*/
  pos = Vector3(-850.0f,-250.0f,0.0);
  position_list.push_back(pos);

  albumLayout->SetPosition(position_list);

  DALI_TEST_CHECK(albumLayout->GetPosition() == position_list);
  END_TEST;
}

int UtcDaliAlbumSetRotationX(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetRotationX(Math::PI/4.0f);

  DALI_TEST_CHECK(albumLayout->GetRotationX() == Math::PI/4.0f);
  END_TEST;
}

int UtcDaliAlbumSetRotationZ(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  std::vector<float> rotation_list;

  /*(0.0f)*/
  float rotate = Math::PI/6.0f;
  rotation_list.push_back(rotate);

  /*(1.0f)*/
  rotate = 0.0f;
  rotation_list.push_back(rotate);

  /*(2.0f)*/
  rotate = Math::PI/6.0f;
  rotation_list.push_back(rotate);

  /*(4.0f)*/
  rotate = -Math::PI/6.0f;
  rotation_list.push_back(rotate);

  /*(5.0f)*/
  rotate = 0.0f;
  rotation_list.push_back(rotate);

  /*(6.0f)*/
  rotate = -Math::PI/6.0f;
  rotation_list.push_back(rotate);

  albumLayout->SetRotationZ(rotation_list);

  DALI_TEST_CHECK(albumLayout->GetRotationZ() == rotation_list);
  END_TEST;
}

int UtcDaliAlbumSetScale(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  std::vector<float> scale_list;

  /*(0.0f)*/
  float scale = 1.0f;
  scale_list.push_back(scale);

  /*(1.0f)*/
  scale = 0.0f;
  scale_list.push_back(scale);

  /*(2.0f)*/
  scale = Math::PI/6.0f;
  scale_list.push_back(scale);

  /*(4.0f)*/
  scale = -Math::PI/6.0f;
  scale_list.push_back(scale);

  /*(5.0f)*/
  scale = 0.0f;
  scale_list.push_back(scale);

  /*(6.0f)*/
  scale = -Math::PI/6.0f;
  scale_list.push_back(scale);

  albumLayout->SetScale(scale_list);

  DALI_TEST_CHECK(albumLayout->GetScale() == scale_list);
  END_TEST;
}

int UtcDaliAlbumSetColor(void)
{
  // Create a album layout
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  std::vector<Vector2> color_list;

  /*(0.0f)*/
  Vector2 color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  /*(1.0f)*/
  color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  /*(2.0f)*/
  color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  /*(4.0f)*/
  color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  /*(5.0f)*/
  color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  /*(6.0f)*/
  color = Vector2(1.0f,1.0f);
  color_list.push_back(color);

  albumLayout->SetColor(color_list);

  DALI_TEST_CHECK(albumLayout->GetColor() == color_list);
  END_TEST;
}

int UtcDaliAlbumSetCenterPosition(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetCenterPosition(Vector3( 0.0f,-80.0f,100.0f));

  DALI_TEST_CHECK(albumLayout->GetCenterPosition() == Vector3( 0.0f,-80.0f,100.0f));
  END_TEST;
}

int UtcDaliAlbumSetSetCenterScale(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetCenterScale(1.75f);

  DALI_TEST_CHECK(albumLayout->GetCenterScale() == 1.75f);
  END_TEST;
}

int UtcDaliAlbumSetSetCenterColor(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetCenterColor(Vector2(1.0f,1.0f));

  DALI_TEST_CHECK(albumLayout->GetCenterColor() == Vector2(1.0f,1.0f));
  END_TEST;
}

int UtcDaliAlbumSetStackPosition(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetStackPosition(Vector3(750.0f,-500.0f,0.0f),Vector3(-750.0f,-500.0f,0.0f));

  DALI_TEST_CHECK(albumLayout->GetRightStackPosition() == Vector3(750.0f,-500.0f,0.0f) && albumLayout->GetLeftStackPosition() == Vector3(-750.0f,-500.0f,0.0f));
  END_TEST;
}

int UtcDaliAlbumSetSetStackScale(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetStackScale(1.0f,1.0f);

  DALI_TEST_CHECK(albumLayout->GetRightStackScale() == 1.0f && albumLayout->GetLeftStackScale() == 1.0f);
  END_TEST;
}

int UtcDaliAlbumSetStackColor(void)
{
  AlbumLayoutPtr albumLayout = AlbumLayout::New();

  albumLayout->SetStackColor(Vector2(1.0f,1.0f),Vector2(1.0f,1.0f));

  DALI_TEST_CHECK(albumLayout->GetRightStackColor() == Vector2(1.0f,1.0f) && albumLayout->GetLeftStackColor() == Vector2(1.0f,1.0f));
  END_TEST;
}
