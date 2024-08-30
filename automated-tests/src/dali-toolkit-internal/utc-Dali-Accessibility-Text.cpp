/**
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/text.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>

void utc_dali_toolkit_accessibility_text_startup(void)
{
  test_return_value = TET_UNDEF;
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
}

void utc_dali_toolkit_accessibility_text_cleanup(void)
{
  test_return_value = TET_PASS;
  DBusWrapper::Install({}); // Clean up TestDBusWrapper
}

int utcDaliAccessibilityTextEditorGetName(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  DALI_TEST_EQUALS( editor.GetProperty<std::string>(Actor::Property::NAME), "", TEST_LOCATION );
  editor.SetProperty(Actor::Property::NAME, "editor");
  DALI_TEST_EQUALS( editor.GetProperty<std::string>(Actor::Property::NAME), "editor", TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityTextEditorGetText(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetText( 0, 0 ), "", TEST_LOCATION );
    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "exemplary_text" );
    DALI_TEST_EQUALS( x->GetText( 0, 9 ), "exemplary", TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextEditorGetCharacterCount(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetCharacterCount(), 0, TEST_LOCATION );
    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "editor" );
    DALI_TEST_EQUALS( x->GetCharacterCount(), 6, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextEditorGetTextAtOffset(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text editor test sentence" );
    range = x->GetTextAtOffset( 5, Dali::Accessibility::TextBoundary::CHARACTER );
    DALI_TEST_EQUALS( range.content, "e", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 5, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 6, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 3, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 28, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 36, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 4, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "text    \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 6, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " editor  \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 14, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 24, TEST_LOCATION );

    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 8, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " test sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 25, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 39, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextEditorGetSetRangeOfSelection(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );

    x->SetRangeOfSelection( 0, 4, 9 );
    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "exemplary_text" );
    range = x->GetRangeOfSelection( 0 );

    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "plary", TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextEditorRemoveSelection(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetRangeOfSelection( 0, 4, 9 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextEditorGetRangeExtents(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q     = Dali::Accessibility::Accessible::Get( editor );
  auto x     = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );

  if( x )
  {
    auto rangeExtents = x->GetRangeExtents( 0, 0, Dali::Accessibility::CoordinateType::WINDOW );
    DALI_TEST_EQUALS( (int)rangeExtents.x, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.y, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.width, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.height, 0, TEST_LOCATION );

    application.GetScene().Add( editor );

    editor.SetProperty( Toolkit::TextEditor::Property::POINT_SIZE, 7.f );
    editor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
    editor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    editor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "text editor test sentence" );

    // Avoid a crash when core load gl resources.
    application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

    // Render and notify
    application.SendNotification();
    application.Render();

    auto characterCount = x->GetCharacterCount();
    rangeExtents        = x->GetRangeExtents( 0, characterCount, Dali::Accessibility::CoordinateType::WINDOW );

    DALI_TEST_EQUALS((int)rangeExtents.x >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.y >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.width <= 200, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.height <= 200, true, TEST_LOCATION);
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetName(void)
{
  ToolkitTestApplication application;

  auto field = Toolkit::TextField::New();
  DALI_TEST_EQUALS( field.GetProperty<std::string>(Actor::Property::NAME), "", TEST_LOCATION );
  field.SetProperty(Actor::Property::NAME, "field");
  DALI_TEST_EQUALS( field.GetProperty<std::string>(Actor::Property::NAME), "field", TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetText(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetText( 0, 0 ), "", TEST_LOCATION );
    field.SetProperty( Toolkit::TextField::Property::TEXT, "exemplary_text" );
    DALI_TEST_EQUALS( x->GetText( 0, 9 ), "exemplary", TEST_LOCATION );

    Dali::Property::Map hiddenInputSettings;
    hiddenInputSettings[ Toolkit::HiddenInput::Property::MODE ] = Toolkit::HiddenInput::Mode::HIDE_ALL;

    field.SetProperty( Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS, hiddenInputSettings );

    DALI_TEST_EQUALS( x->GetName(), "", TEST_LOCATION );
    DALI_TEST_EQUALS( x->GetText( 0, 9 ), "*********", TEST_LOCATION );

    hiddenInputSettings[ Toolkit::HiddenInput::Property::SUBSTITUTE_CHARACTER ] = 0x23;
    field.SetProperty( Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS, hiddenInputSettings );

    DALI_TEST_EQUALS( x->GetName(), "", TEST_LOCATION );
    DALI_TEST_EQUALS( x->GetText( 0, 9 ), "#########", TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetCharacterCount(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetCharacterCount(), 0, TEST_LOCATION );
    field.SetProperty( Toolkit::TextField::Property::TEXT, "field" );
    DALI_TEST_EQUALS( x->GetCharacterCount(), 5, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetTextAtOffset(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text editor test sentence" );
    range = x->GetTextAtOffset( 5, Dali::Accessibility::TextBoundary::CHARACTER );
    DALI_TEST_EQUALS( range.content, "e", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 5, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 6, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 3, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 28, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 36, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 4, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "text    \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 6, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " editor  \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 14, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 24, TEST_LOCATION );

    field.SetProperty( Toolkit::TextField::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 8, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " test sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 25, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 39, TEST_LOCATION );

    Dali::Property::Map hiddenInputSettings;
    hiddenInputSettings[ Toolkit::HiddenInput::Property::MODE ] = Toolkit::HiddenInput::Mode::HIDE_ALL;
    hiddenInputSettings[ Toolkit::HiddenInput::Property::SUBSTITUTE_CHARACTER ] = 0x23;
    field.SetProperty( Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS, hiddenInputSettings );
    range = x->GetTextAtOffset( 8, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetSetRangeOfSelection(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );

    x->SetRangeOfSelection( 0, 4, 9 );
    field.SetProperty( Toolkit::TextEditor::Property::TEXT, "exemplary_text" );
    range = x->GetRangeOfSelection( 0 );

    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "plary", TEST_LOCATION );

    Dali::Property::Map hiddenInputSettings;
    hiddenInputSettings[ Toolkit::HiddenInput::Property::MODE ] = Toolkit::HiddenInput::Mode::HIDE_ALL;
    field.SetProperty( Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS, hiddenInputSettings );

    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "*****", TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldRemoveSelection(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetRangeOfSelection( 0, 4, 9 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetRangeExtents(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q     = Dali::Accessibility::Accessible::Get( field );
  auto x     = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );

  if( x )
  {
    auto rangeExtents = x->GetRangeExtents( 0, 0, Dali::Accessibility::CoordinateType::WINDOW );
    DALI_TEST_EQUALS( (int)rangeExtents.x, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.y, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.width, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.height, 0, TEST_LOCATION );

    application.GetScene().Add( field );

    field.SetProperty( Toolkit::TextField::Property::POINT_SIZE, 7.f );
    field.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
    field.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    field.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    field.SetProperty( Toolkit::TextField::Property::TEXT, "text field test sentence" );

    // Avoid a crash when core load gl resources.
    application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

    // Render and notify
    application.SendNotification();
    application.Render();

    auto characterCount = x->GetCharacterCount();
    rangeExtents        = x->GetRangeExtents( 0, characterCount, Dali::Accessibility::CoordinateType::WINDOW );

    DALI_TEST_EQUALS((int)rangeExtents.x >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.y >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.width <= 200, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.height <= 200, true, TEST_LOCATION);
  }

  END_TEST;
}

int utcDaliAccessibilityTextLabelGetName(void)
{
  ToolkitTestApplication application;

  auto label = Toolkit::TextLabel::New();
  DALI_TEST_EQUALS( label.GetProperty<std::string>(Actor::Property::NAME), "", TEST_LOCATION );
  label.SetProperty(Actor::Property::NAME, "label");
  DALI_TEST_EQUALS( label.GetProperty<std::string>(Actor::Property::NAME), "label", TEST_LOCATION );

  END_TEST;
}

int utcDaliAccessibilityTextLabelGetText(void)
{
  ToolkitTestApplication application;

  auto label = Dali::Toolkit::TextLabel::New();
  auto q = Dali::Accessibility::Accessible::Get( label );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetText( 0, 0 ), "", TEST_LOCATION );
    label.SetProperty( Toolkit::TextField::Property::TEXT, "exemplary_text" );
    DALI_TEST_EQUALS( x->GetText( 0, 9 ), "exemplary", TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextLabelGetCharacterCount(void)
{
  ToolkitTestApplication application;

  auto label = Dali::Toolkit::TextLabel::New();
  auto q = Dali::Accessibility::Accessible::Get( label );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    DALI_TEST_EQUALS( x->GetCharacterCount(), 0, TEST_LOCATION );
    label.SetProperty( Toolkit::TextField::Property::TEXT, "field" );
    DALI_TEST_EQUALS( x->GetCharacterCount(), 5, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextLabelGetTextAtOffset(void)
{
  ToolkitTestApplication application;

  auto label = Dali::Toolkit::TextLabel::New();
  auto q = Dali::Accessibility::Accessible::Get( label );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text editor test sentence" );
    range = x->GetTextAtOffset( 5, Dali::Accessibility::TextBoundary::CHARACTER );
    DALI_TEST_EQUALS( range.content, "e", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 5, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 6, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 3, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 28, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 36, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 4, Dali::Accessibility::TextBoundary::WORD );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 0, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, "text    \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 6, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " editor  \n", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 14, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 24, TEST_LOCATION );

    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text    \n\n\n\n\n\n editor  \n\n test sentence" );
    range = x->GetTextAtOffset( 8, Dali::Accessibility::TextBoundary::LINE );
    DALI_TEST_EQUALS( range.content, " test sentence", TEST_LOCATION );
    DALI_TEST_EQUALS( range.startOffset, 25, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 39, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextLabelRemoveSelection( void )
{
  ToolkitTestApplication application;

  auto label = Dali::Toolkit::TextLabel::New();
  auto q = Dali::Accessibility::Accessible::Get( label );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetRangeOfSelection( 0, 4, 9 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetRangeOfSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  }

  END_TEST;
}

int utcDaliAccessibilityTextLabelGetRangeExtents(void)
{
  ToolkitTestApplication application;

  auto label = Dali::Toolkit::TextLabel::New();
  auto q     = Dali::Accessibility::Accessible::Get( label );
  auto x     = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );

  if( x )
  {
    auto rangeExtents = x->GetRangeExtents( 0, 0, Dali::Accessibility::CoordinateType::WINDOW );
    DALI_TEST_EQUALS( (int)rangeExtents.x, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.y, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.width, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( (int)rangeExtents.height, 0, TEST_LOCATION );

    application.GetScene().Add( label );

    label.SetProperty( Toolkit::TextLabel::Property::POINT_SIZE, 7.f );
    label.SetProperty( Toolkit::TextLabel::Property::MULTI_LINE, true );
    label.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
    label.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    label.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    label.SetProperty( Toolkit::TextLabel::Property::TEXT, "text label\n test sentence" );

    // Avoid a crash when core load gl resources.
    application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

    // Render and notify
    application.SendNotification();
    application.Render();

    auto characterCount = x->GetCharacterCount();
    rangeExtents        = x->GetRangeExtents( 0, characterCount, Dali::Accessibility::CoordinateType::WINDOW );

    DALI_TEST_EQUALS((int)rangeExtents.x >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.y >= 0, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.width <= 200, true, TEST_LOCATION);
    DALI_TEST_EQUALS((int)rangeExtents.height <= 200, true, TEST_LOCATION);
  }

  END_TEST;
}
