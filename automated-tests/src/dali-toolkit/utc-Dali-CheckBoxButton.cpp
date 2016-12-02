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

#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>


using namespace Dali;
using namespace Toolkit;

namespace
{

static bool gCheckBoxButtonState = false;
bool CheckBoxButtonClicked( Button button )
{
  gCheckBoxButtonState = button.IsSelected();
  return true;
}

static const char* TEST_IMAGE_ONE = TEST_RESOURCE_DIR "/gallery-small-1.jpg";
const Vector2 TEST_IMAGE_SIZE = Vector2( 66.0f, 66.0f );


} // namespace

void checkbox_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void checkbox_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCheckBoxButtonConstructorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox;

  DALI_TEST_CHECK( !checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonAssignmentOperatorP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  CheckBoxButton copy( checkBox );
  DALI_TEST_CHECK( copy );

  DALI_TEST_CHECK( checkBox == copy );
  END_TEST;
}

int UtcDaliCheckBoxButtonNewP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  DALI_TEST_CHECK( checkBox );
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  BaseHandle object(checkBox);

  CheckBoxButton checkBox2 = CheckBoxButton::DownCast( object );
  DALI_TEST_CHECK(checkBox2);

  CheckBoxButton checkBox3 = DownCast< CheckBoxButton >(object);
  DALI_TEST_CHECK(checkBox3);
  END_TEST;
}

int UtcDaliCheckBoxButtonDownCastN(void)
{
  TestApplication application;

  BaseHandle unInitializedObject;

  CheckBoxButton checkBox1 = CheckBoxButton::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !checkBox1 );

  CheckBoxButton checkBox2 = DownCast< CheckBoxButton >( unInitializedObject );
  DALI_TEST_CHECK( !checkBox2 );
  END_TEST;
}

int UtcDaliCheckBoxButtonSetGetSelected(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCheckBoxButtonSetGetSelected");

  CheckBoxButton checkBoxButton = CheckBoxButton::New();
  checkBoxButton.StateChangedSignal().Connect( &CheckBoxButtonClicked );

  // global var used to check if CheckBoxButtonClicked is called;
  gCheckBoxButtonState = false;

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );

  checkBoxButton.SetSelected( false );

  DALI_TEST_CHECK( !checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( !gCheckBoxButtonState );

  checkBoxButton.SetSelected( true );

  DALI_TEST_CHECK( checkBoxButton.IsSelected() );
  DALI_TEST_CHECK( gCheckBoxButtonState );
  END_TEST;
}

int UtcDaliCheckBoxSetLabelP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  Property::Map propertyMap;

  propertyMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
             .Add( Toolkit::TextVisual::Property::TEXT, "activate" )
             .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f );

  checkBox.SetProperty( checkBox.GetPropertyIndex("label"), propertyMap );

  DALI_TEST_EQUALS( checkBox.GetLabelText(), "activate", TEST_LOCATION ); // Change to use GerProperty once that code is implemented
  END_TEST;
}

int UtcDaliCheckBoxSetLabelDisabledP(void)
{
  TestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();
  Stage::GetCurrent().Add( checkBox );

  checkBox.SetSize( Vector2( 20.0f, 20.0f ) );
  checkBox.SetDisabledBackgroundImage( "Image.jpg" );

  application.SendNotification();
  application.Render();

  Property::Map propertyMap;

  propertyMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
             .Add( Toolkit::TextVisual::Property::TEXT, "activate" )
             .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f );

  checkBox.SetProperty(checkBox.GetPropertyIndex("disabled"), true);
  checkBox.SetProperty( checkBox.GetPropertyIndex("label"), propertyMap );

  DALI_TEST_CHECK(  checkBox.GetProperty<bool>(checkBox.GetPropertyIndex("disabled")) );
  DALI_TEST_EQUALS( checkBox.GetLabelText(), "activate", TEST_LOCATION ); // Change to use GetProperty once that code is implemented

  END_TEST;
}

int UtcDaliCheckBoxSettingDisabled(void)
{
  ToolkitTestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  checkBox.SetProperty(checkBox.GetPropertyIndex("disabled"), true);
  DALI_TEST_CHECK(  checkBox.GetProperty<bool>(checkBox.GetPropertyIndex("disabled")) );

  checkBox.SetProperty(checkBox.GetPropertyIndex("disabled"), false);

  DALI_TEST_CHECK(  !checkBox.GetProperty<bool>(checkBox.GetPropertyIndex("disabled")) );

  END_TEST;
}

int UtcDaliCheckBoxSetLabelPadding(void)
{
  tet_infoline("UtcDaliCheckBoxSetLabelPadding\n");

  ToolkitTestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  Property::Map propertyMap;

  propertyMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
             .Add( Toolkit::TextVisual::Property::TEXT, "activate" )
             .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f );

  checkBox.SetProperty( Toolkit::DevelButton::Property::LABEL, propertyMap );

  application.SendNotification();
  application.Render();

  Vector3 orginalSize = checkBox.GetNaturalSize();

  checkBox.SetProperty( Toolkit::DevelButton::Property::LABEL_PADDING, Vector4( 10.0f, 10.0f, 10.0f, 10.0f ) );

  application.SendNotification();
  application.Render();

  Vector3 paddingAddedSize = checkBox.GetNaturalSize();

  DALI_TEST_EQUALS( checkBox.GetProperty<Vector4>( Toolkit::DevelButton::Property::LABEL_PADDING ), Vector4( 10.0f, 10.0f, 10.0f, 10.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  tet_infoline("Comparing original size of button with just text and button size with text and padding\n");

  DALI_TEST_EQUALS( orginalSize.width +10.0f + 10.0f , paddingAddedSize.width, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  DALI_TEST_EQUALS( orginalSize.height +10.0f + 10.0f , paddingAddedSize.height, Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCheckBoxSetForegroundPadding(void)
{
  tet_infoline("UtcDaliCheckBoxSetForegroundPadding\n");

  ToolkitTestApplication application;

  CheckBoxButton checkBox = CheckBoxButton::New();

  Property::Map propertyMap;

  propertyMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT )
             .Add( Toolkit::TextVisual::Property::TEXT, "activate" )
             .Add( Toolkit::TextVisual::Property::POINT_SIZE, 15.0f );

  checkBox.SetProperty( Toolkit::DevelButton::Property::LABEL, propertyMap );
  checkBox.SetProperty( Toolkit::DevelButton::Property::LABEL_PADDING, Vector4( 5.0f, 5.0f, 5.0f, 5.0f ) );

  application.SendNotification();
  application.Render();

  tet_printf( "Button RelayoutSize with text(%f,%f)\n", checkBox.GetNaturalSize().width, checkBox.GetNaturalSize().height );

  TestPlatformAbstraction& platform = application.GetPlatform();
  platform.SetClosestImageSize( TEST_IMAGE_SIZE );

  checkBox.SetProperty( Toolkit::DevelButton::Property::UNSELECTED_VISUAL, TEST_IMAGE_ONE );
  checkBox.SetProperty( Toolkit::DevelButton::Property::SELECTED_VISUAL, TEST_IMAGE_ONE );


  application.SendNotification();
  application.Render();

  Vector3 preVisualPaddingSize = checkBox.GetNaturalSize();

  tet_printf( "Button RelayoutSize with text and icon (%f,%f)\n", checkBox.GetNaturalSize().width, checkBox.GetNaturalSize().height );

  checkBox.SetProperty( Toolkit::DevelButton::Property::VISUAL_PADDING, Vector4( 25.0f, 25.0f, 25.0f, 25.0f ) );

  application.SendNotification();
  application.Render();

  Vector3 paddingAddedSize = checkBox.GetNaturalSize();

  tet_printf( "Button RelayoutSize with text, icon and padding (%f,%f)\n", checkBox.GetNaturalSize().width, checkBox.GetNaturalSize().height );

  DALI_TEST_EQUALS( checkBox.GetProperty<Vector4>( Toolkit::DevelButton::Property::VISUAL_PADDING ), Vector4( 25.0f, 25.0f, 25.0f, 25.0f ), Math::MACHINE_EPSILON_1000, TEST_LOCATION );

  tet_infoline("Comparing original size of button before adding padding to visual foreground\n");

  DALI_TEST_GREATER( paddingAddedSize.width, preVisualPaddingSize.width , TEST_LOCATION );

  tet_infoline("Text and Visual are side by side, visual height and padding must be greater than text height and padding for this test\n");

  DALI_TEST_GREATER( paddingAddedSize.height, preVisualPaddingSize.height , TEST_LOCATION );

  END_TEST;
}
