/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextFieldMultipleBackgroundText(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliTextFieldMultipleBackgroundText" );

  // Create a text field
  TextField textField = TextField::New();
  textField.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 60.f ) );
  textField.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  textField.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );

  // Add the text field to the stage
  application.GetScene().Add( textField );

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& textFieldImpl = GetImpl( textField );
  ControllerPtr controller = textFieldImpl.GetTextController();
  Controller::Impl& controllerImpl = Controller::Impl::GetImplementation( *controller.Get() );

  // Add multiple background colors for the text.
  ColorRun backgroundColorRun1;
  backgroundColorRun1.characterRun.characterIndex = 0u;
  backgroundColorRun1.characterRun.numberOfCharacters = 1u;
  backgroundColorRun1.color = Color::RED;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun1 );

  ColorRun backgroundColorRun2;
  backgroundColorRun2.characterRun.characterIndex = 5u;
  backgroundColorRun2.characterRun.numberOfCharacters = 8u;
  backgroundColorRun2.color = Color::CYAN;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun2 );

  ColorRun backgroundColorRun3;
  backgroundColorRun3.characterRun.characterIndex = 23u;
  backgroundColorRun3.characterRun.numberOfCharacters = 6u;
  backgroundColorRun3.color = Color::GREEN;
  controllerImpl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack( backgroundColorRun3 );

  // Check the case where there is only one character in the text
  controller->SetText( "S" );

  application.SendNotification();
  application.Render();

  // The offscreen root actor should have one child: the renderable.
  Actor stencil = textField.GetChildAt( 0u );
  DALI_TEST_CHECK( stencil.GetChildCount() == 1u );

  // The renderable actor should have two children: the text and the background.
  Actor renderableActor = stencil.GetChildAt( 0u );
  DALI_TEST_CHECK( renderableActor.GetChildCount() == 2u );

  // Check that the background is created
  Actor backgroundActor = renderableActor.GetChildAt( 0u );
  DALI_TEST_CHECK( backgroundActor );
  DALI_TEST_CHECK( backgroundActor.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "TextBackgroundColorActor" );

  // Change the text to contain more characters
  controller->SetText( "Text Multiple Background Test" );

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textFieldImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  // Now the offscreen root actor should have three children: the renderable, the highlight, and the background.
  DALI_TEST_CHECK( stencil.GetChildCount() == 3u );
  // The renderable actor should have one child only: the text
  DALI_TEST_CHECK( renderableActor.GetChildCount() == 1u );

  // The background should now be lowered below the highlight
  backgroundActor = stencil.GetChildAt( 0u );
  DALI_TEST_CHECK( backgroundActor );
  DALI_TEST_CHECK( backgroundActor.GetProperty< std::string >( Dali::Actor::Property::NAME ) == "TextBackgroundColorActor" );

  END_TEST;
}

int UtcDaliTextFieldSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliTextFieldSelectText" );

  // Create a text field
  TextField textField = TextField::New();
  textField.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 60.f ) );
  textField.SetProperty( TextField::Property::TEXT, "Hello World" );

  // Add the text field to the stage
  application.GetScene().Add( textField );

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& textFieldImpl = GetImpl( textField );

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textFieldImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( textFieldImpl.GetSelectedText() == "Hello World" );

  // Select None
  textFieldImpl.SelectNone();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( textFieldImpl.GetSelectedText() == "" );

  END_TEST;
}

int UtcDaliTextFieldMarkupUnderline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldMarkupUnderline ");

  TextField textField = TextField::New();

  application.GetScene().Add( textField );

  textField.SetProperty( TextField::Property::TEXT, "<u>ABC</u>EF<u>GH</u>" );
  textField.SetProperty( TextField ::Property::ENABLE_MARKUP,  true );

  application.SendNotification();
  application.Render();

  uint32_t expectedNumberOfUnderlinedGlyphs = 5u;

  Toolkit::Internal::TextField& textFieldImpl = GetImpl( textField );
  const Text::Length numberOfUnderlineRuns = textFieldImpl.getController()->GetTextModel()->GetNumberOfUnderlineRuns();

  DALI_TEST_EQUALS( numberOfUnderlineRuns, expectedNumberOfUnderlinedGlyphs, TEST_LOCATION );

  Vector<GlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  textFieldImpl.getController()->GetTextModel()->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  //ABC are underlined
  DALI_TEST_EQUALS( underlineRuns[0u].glyphIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[1u].glyphIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[2u].glyphIndex, 2u, TEST_LOCATION);

  //GH are underlined
  DALI_TEST_EQUALS( underlineRuns[3u].glyphIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS( underlineRuns[4u].glyphIndex, 6u, TEST_LOCATION);

  END_TEST;

int UtcDaliTextFieldFontPointSizeLargerThanAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldFontPointSizeLargerThanAtlas ");

  // Create a Text field
  TextField textField = TextField::New();
  //Set size to avoid automatic eliding
  textField.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set very large font-size using point-size
  textField.SetProperty( TextField::Property::POINT_SIZE, 1000) ;
  //Specify font-family
  textField.SetProperty( TextField::Property::FONT_FAMILY, "DejaVu Sans");
  //Set text to check if appear or not
  textField.SetProperty( TextField::Property::TEXT, "A");

  application.GetScene().Add( textField );

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS( countAtlas, 1, TEST_LOCATION );


  END_TEST;
}

int UtcDaliTextFieldFontPointSizeLargerThanAtlasPlaceholderCase(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextFieldFontPointSizeLargerThanAtlasPlaceholderCase ");

  //Set Map of placeholder: text, font-family and point-size
  Property::Map placeholderMapSet;
  placeholderMapSet["text"] = "A";
  placeholderMapSet["fontFamily"] = "DejaVu Sans";
  placeholderMapSet["pixelSize"] = 1000.0f;

  // Create a text editor
  TextField textField = TextField::New();
  //Set size to avoid automatic eliding
  textField.SetProperty( Actor::Property::SIZE, Vector2(1025, 1025));
  //Set placeholder
  textField.SetProperty( TextField::Property::PLACEHOLDER, placeholderMapSet) ;

  application.GetScene().Add( textField );

  application.SendNotification();
  application.Render();

  //Check if Glyph is added to AtlasGlyphManger or not
  int countAtlas = AtlasGlyphManager::Get().GetMetrics().mAtlasMetrics.mAtlasCount;
  DALI_TEST_EQUALS( countAtlas, 1, TEST_LOCATION );


  END_TEST;
}