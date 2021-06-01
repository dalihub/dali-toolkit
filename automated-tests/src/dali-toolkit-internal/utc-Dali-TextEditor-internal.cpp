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

#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-controller-impl.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextEditorSelectText(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliTextEditorSelectText" );

  // Create a text editor
  TextEditor textEditor = TextEditor::New();
  textEditor.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 60.f ) );
  textEditor.SetProperty( TextEditor::Property::TEXT, "Hello World" );

  // Add the text editor to the stage
  application.GetScene().Add( textEditor );

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& textEditorImpl = GetImpl( textEditor );

  application.SendNotification();
  application.Render();

  // Highlight the whole text
  textEditorImpl.SelectWholeText();

  application.SendNotification();
  application.Render();

  std::string selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK( selectedText == "Hello World" );

  // Select None
  textEditorImpl.SelectNone();

  application.SendNotification();
  application.Render();

  selectedText = textEditorImpl.GetSelectedText();
  DALI_TEST_CHECK( selectedText == "" );

  END_TEST;
}
