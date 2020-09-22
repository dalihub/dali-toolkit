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
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

void utc_dali_toolkit_accessibility_text_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_accessibility_text_cleanup(void)
{
  test_return_value = TET_PASS;
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

int utcDaliAccessibilityTextEditorGetSetSelection(void)
{
  ToolkitTestApplication application;

  auto editor = Dali::Toolkit::TextEditor::New();
  auto q = Dali::Accessibility::Accessible::Get( editor );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );

    x->SetSelection( 0, 4, 9 );
    editor.SetProperty( Toolkit::TextEditor::Property::TEXT, "exemplary_text" );
    range = x->GetSelection( 0 );

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
    auto range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetSelection( 0, 4, 9 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
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
  }

  END_TEST;
}

int utcDaliAccessibilityTextFieldGetSetSelection(void)
{
  ToolkitTestApplication application;

  auto field = Dali::Toolkit::TextField::New();
  auto q = Dali::Accessibility::Accessible::Get( field );
  auto x = dynamic_cast< Dali::Accessibility::Text* >( q );
  DALI_TEST_CHECK( x );
  if( x )
  {
    auto range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "", TEST_LOCATION );

    x->SetSelection( 0, 4, 9 );
    field.SetProperty( Toolkit::TextEditor::Property::TEXT, "exemplary_text" );
    range = x->GetSelection( 0 );

    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );
    DALI_TEST_EQUALS( range.content, "plary", TEST_LOCATION );
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
    auto range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetSelection( 0, 4, 9 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
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
    auto range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );

    x->SetSelection( 0, 4, 9 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 4, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 9, TEST_LOCATION );

    x->RemoveSelection( 0 );
    range = x->GetSelection( 0 );
    DALI_TEST_EQUALS( range.startOffset, 0, TEST_LOCATION );
    DALI_TEST_EQUALS( range.endOffset, 0, TEST_LOCATION );
  }

  END_TEST;
}
