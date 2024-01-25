/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-property-handler.h>
#include <dali-toolkit/internal/focus-manager/keyboard-focus-manager-impl.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

using namespace Dali::Toolkit::Text;

#if defined(DEBUG_ENABLED)
Debug::Filter* gTextEditorLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace // unnamed namespace
{
const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::DevelText::DEFAULT_RENDERING_BACKEND;
const float        DEFAULT_SCROLL_SPEED      = 1200.f; ///< The default scroll speed for the text editor in pixels/second.
} // unnamed namespace

namespace
{
const char* const SCROLL_BAR_POSITION("sourcePosition");
const char* const SCROLL_BAR_POSITION_MIN("sourcePositionMin");
const char* const SCROLL_BAR_POSITION_MAX("sourcePositionMax");
const char* const SCROLL_BAR_CONTENT_SIZE("sourceContentSize");

// Type registration
BaseHandle Create()
{
  return Toolkit::TextEditor::New();
}

// clang-format off
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::TextEditor, Toolkit::Control, Create);

DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "text",                                 STRING,    TEXT                                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "textColor",                            VECTOR4,   TEXT_COLOR                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "fontFamily",                           STRING,    FONT_FAMILY                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "fontStyle",                            MAP,       FONT_STYLE                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "pointSize",                            FLOAT,     POINT_SIZE                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "horizontalAlignment",                  STRING,    HORIZONTAL_ALIGNMENT                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "scrollThreshold",                      FLOAT,     SCROLL_THRESHOLD                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "scrollSpeed",                          FLOAT,     SCROLL_SPEED                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "primaryCursorColor",                   VECTOR4,   PRIMARY_CURSOR_COLOR                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "secondaryCursorColor",                 VECTOR4,   SECONDARY_CURSOR_COLOR              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "enableCursorBlink",                    BOOLEAN,   ENABLE_CURSOR_BLINK                 )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "cursorBlinkInterval",                  FLOAT,     CURSOR_BLINK_INTERVAL               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "cursorBlinkDuration",                  FLOAT,     CURSOR_BLINK_DURATION               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "cursorWidth",                          INTEGER,   CURSOR_WIDTH                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "grabHandleImage",                      STRING,    GRAB_HANDLE_IMAGE                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "grabHandlePressedImage",               STRING,    GRAB_HANDLE_PRESSED_IMAGE           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandleImageLeft",             MAP,       SELECTION_HANDLE_IMAGE_LEFT         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandleImageRight",            MAP,       SELECTION_HANDLE_IMAGE_RIGHT        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandlePressedImageLeft",      MAP,       SELECTION_HANDLE_PRESSED_IMAGE_LEFT )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandlePressedImageRight",     MAP,       SELECTION_HANDLE_PRESSED_IMAGE_RIGHT)
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandleMarkerImageLeft",       MAP,       SELECTION_HANDLE_MARKER_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHandleMarkerImageRight",      MAP,       SELECTION_HANDLE_MARKER_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "selectionHighlightColor",              VECTOR4,   SELECTION_HIGHLIGHT_COLOR           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "decorationBoundingBox",                RECTANGLE, DECORATION_BOUNDING_BOX             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "enableMarkup",                         BOOLEAN,   ENABLE_MARKUP                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputColor",                           VECTOR4,   INPUT_COLOR                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputFontFamily",                      STRING,    INPUT_FONT_FAMILY                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputFontStyle",                       MAP,       INPUT_FONT_STYLE                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputPointSize",                       FLOAT,     INPUT_POINT_SIZE                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "lineSpacing",                          FLOAT,     LINE_SPACING                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputLineSpacing",                     FLOAT,     INPUT_LINE_SPACING                  )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "underline",                            MAP,       UNDERLINE                           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputUnderline",                       MAP,       INPUT_UNDERLINE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "shadow",                               MAP,       SHADOW                              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputShadow",                          MAP,       INPUT_SHADOW                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "emboss",                               MAP,       EMBOSS                              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputEmboss",                          MAP,       INPUT_EMBOSS                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "outline",                              MAP,       OUTLINE                             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "inputOutline",                         MAP,       INPUT_OUTLINE                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "smoothScroll",                         BOOLEAN,   SMOOTH_SCROLL                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "smoothScrollDuration",                 FLOAT,     SMOOTH_SCROLL_DURATION              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "enableScrollBar",                      BOOLEAN,   ENABLE_SCROLL_BAR                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "scrollBarShowDuration",                FLOAT,     SCROLL_BAR_SHOW_DURATION            )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "scrollBarFadeDuration",                FLOAT,     SCROLL_BAR_FADE_DURATION            )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "pixelSize",                            FLOAT,     PIXEL_SIZE                          )
DALI_PROPERTY_REGISTRATION_READ_ONLY(Toolkit,       TextEditor, "lineCount",                            INTEGER,   LINE_COUNT                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "enableSelection",                      BOOLEAN,   ENABLE_SELECTION                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "placeholder",                          MAP,       PLACEHOLDER                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextEditor, "lineWrapMode",                         INTEGER,   LINE_WRAP_MODE                      )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "placeholderText",                      STRING,    PLACEHOLDER_TEXT                    )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "placeholderTextColor",                 VECTOR4,   PLACEHOLDER_TEXT_COLOR              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "enableShiftSelection",                 BOOLEAN,   ENABLE_SHIFT_SELECTION              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "enableGrabHandle",                     BOOLEAN,   ENABLE_GRAB_HANDLE                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "matchSystemLanguageDirection",         BOOLEAN,   MATCH_SYSTEM_LANGUAGE_DIRECTION     )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "renderingBackend",                     INTEGER,   RENDERING_BACKEND                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "maxLength",                            INTEGER,   MAX_LENGTH                          )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "selectedTextStart",                    INTEGER,   SELECTED_TEXT_START                 )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "selectedTextEnd",                      INTEGER,   SELECTED_TEXT_END                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "horizontalScrollPosition",             FLOAT,     HORIZONTAL_SCROLL_POSITION          )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "verticalScrollPosition",               INTEGER,   VERTICAL_SCROLL_POSITION            )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "enableEditing",                        BOOLEAN,   ENABLE_EDITING                      )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextEditor, "selectedText",                         STRING,    SELECTED_TEXT                       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "fontSizeScale",                        FLOAT,     FONT_SIZE_SCALE                     )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "enableFontSizeScale",                  BOOLEAN,   ENABLE_FONT_SIZE_SCALE              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "primaryCursorPosition",                INTEGER,   PRIMARY_CURSOR_POSITION             )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "grabHandleColor",                      VECTOR4,   GRAB_HANDLE_COLOR                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "enableGrabHandlePopup",                BOOLEAN,   ENABLE_GRAB_HANDLE_POPUP            )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "inputMethodSettings",                  MAP,       INPUT_METHOD_SETTINGS               )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "inputFilter",                          MAP,       INPUT_FILTER                        )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "ellipsis",                             BOOLEAN,   ELLIPSIS                            )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "ellipsisPosition",                     INTEGER,   ELLIPSIS_POSITION                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "minLineSize",                          FLOAT,     MIN_LINE_SIZE                       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "strikethrough",                        MAP,       STRIKETHROUGH                       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "inputStrikethrough",                   MAP,       INPUT_STRIKETHROUGH                 )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "characterSpacing",                     FLOAT,     CHARACTER_SPACING                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "relativeLineSize",                     FLOAT,     RELATIVE_LINE_SIZE                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "verticalAlignment",                    STRING,    VERTICAL_ALIGNMENT                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextEditor, "selectionPopupStyle",                  MAP,       SELECTION_POPUP_STYLE               )

DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "textChanged",           SIGNAL_TEXT_CHANGED           )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "inputStyleChanged",     SIGNAL_INPUT_STYLE_CHANGED    )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "maxLengthReached",      SIGNAL_MAX_LENGTH_REACHED     )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "anchorClicked",         SIGNAL_ANCHOR_CLICKED         )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "inputFiltered",         SIGNAL_INPUT_FILTERED         )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "cursorPositionChanged", SIGNAL_CURSOR_POSITION_CHANGED)
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "selectionChanged",      SIGNAL_SELECTION_CHANGED      )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "selectionCleared",      SIGNAL_SELECTION_CLEARED      )
DALI_SIGNAL_REGISTRATION(Toolkit, TextEditor, "selectionStarted",      SIGNAL_SELECTION_STARTED      )

DALI_TYPE_REGISTRATION_END()
// clang-format on

Toolkit::TextEditor::InputStyle::Mask ConvertInputStyle(Text::InputStyle::Mask inputStyleMask)
{
  Toolkit::TextEditor::InputStyle::Mask editorInputStyleMask = Toolkit::TextEditor::InputStyle::NONE;

  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_COLOR))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::COLOR);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_FAMILY))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_FAMILY);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_POINT_SIZE))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::POINT_SIZE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_WEIGHT))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_WIDTH))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_SLANT))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_LINE_SPACING))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::LINE_SPACING);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_UNDERLINE))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::UNDERLINE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_SHADOW))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::SHADOW);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_EMBOSS))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::EMBOSS);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_OUTLINE))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::OUTLINE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_STRIKETHROUGH))
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>(editorInputStyleMask | Toolkit::TextEditor::InputStyle::STRIKETHROUGH);
  }

  return editorInputStyleMask;
}

} // namespace

Toolkit::TextEditor TextEditor::New(ControlBehaviour additionalBehaviour)
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextEditor> impl = new TextEditor(additionalBehaviour);

  // Pass ownership to CustomActor handle
  Toolkit::TextEditor handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextEditor::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::TextEditor textEditor = Toolkit::TextEditor::DownCast(Dali::BaseHandle(object));

  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor SetProperty\n");

  if(textEditor)
  {
    PropertyHandler::SetProperty(textEditor, index, value);
  }
}

Property::Value TextEditor::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::TextEditor textEditor = Toolkit::TextEditor::DownCast(Dali::BaseHandle(object));

  if(textEditor)
  {
    value = PropertyHandler::GetProperty(textEditor, index);
  }
  return value;
}

void TextEditor::SelectWholeText()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectWholeText();
    SetKeyInputFocus();
  }
}

void TextEditor::SelectNone()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectNone();
  }
}

void TextEditor::SelectText(const uint32_t start, const uint32_t end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectText(start, end);
    SetKeyInputFocus();
  }
}

string TextEditor::CopyText()
{
  string copiedText = "";
  if(mController && mController->IsShowingRealText())
  {
    copiedText = mController->CopyText();
  }
  return copiedText;
}

string TextEditor::CutText()
{
  string cutText = "";
  if(mController && mController->IsShowingRealText())
  {
    cutText = mController->CutText();
  }
  return cutText;
}

void TextEditor::PasteText()
{
  if(mController)
  {
    SetKeyInputFocus(); //Giving focus to the editor that was passed to the PasteText in case the passed editor (current editor) doesn't have focus.
    mController->PasteText();
  }
}

void TextEditor::ScrollBy(Vector2 scroll)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->ScrollBy(scroll);
  }
}

float TextEditor::GetHorizontalScrollPosition()
{
  if(mController && mController->IsShowingRealText())
  {
    return mController->GetHorizontalScrollPosition();
  }
  return 0;
}

float TextEditor::GetVerticalScrollPosition()
{
  if(mController && mController->IsShowingRealText())
  {
    return mController->GetVerticalScrollPosition();
  }
  return 0;
}

Vector<Vector2> TextEditor::GetTextSize(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextSize(startIndex, endIndex);
}

Vector<Vector2> TextEditor::GetTextPosition(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextPosition(startIndex, endIndex);
}

Rect<float> TextEditor::GetLineBoundingRectangle(const uint32_t lineIndex) const
{
  return mController->GetLineBoundingRectangle(lineIndex);
}

Rect<float> TextEditor::GetCharacterBoundingRectangle(const uint32_t charIndex) const
{
  return mController->GetCharacterBoundingRectangle(charIndex);
}

int TextEditor::GetCharacterIndexAtPosition(float visualX, float visualY) const
{
  return mController->GetCharacterIndexAtPosition(visualX, visualY);
}

Rect<> TextEditor::GetTextBoundingRectangle(uint32_t startIndex, uint32_t endIndex) const
{
  return mController->GetTextBoundingRectangle(startIndex, endIndex);
}

void TextEditor::SetSpannedText(const Text::Spanned& spannedText)
{
  mController->SetSpannedText(spannedText);
}

string TextEditor::GetSelectedText() const
{
  string selectedText = "";
  if(mController && mController->IsShowingRealText())
  {
    selectedText = mController->GetSelectedText();
  }
  return selectedText;
}

InputMethodContext TextEditor::GetInputMethodContext()
{
  return mInputMethodContext;
}

DevelTextEditor::MaxLengthReachedSignalType& TextEditor::MaxLengthReachedSignal()
{
  return mMaxLengthReachedSignal;
}

DevelTextEditor::AnchorClickedSignalType& TextEditor::AnchorClickedSignal()
{
  return mAnchorClickedSignal;
}

DevelTextEditor::CursorPositionChangedSignalType& TextEditor::CursorPositionChangedSignal()
{
  return mCursorPositionChangedSignal;
}

DevelTextEditor::InputFilteredSignalType& TextEditor::InputFilteredSignal()
{
  return mInputFilteredSignal;
}

DevelTextEditor::SelectionChangedSignalType& TextEditor::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

DevelTextEditor::SelectionClearedSignalType& TextEditor::SelectionClearedSignal()
{
  return mSelectionClearedSignal;
}

DevelTextEditor::SelectionStartedSignalType& TextEditor::SelectionStartedSignal()
{
  return mSelectionStartedSignal;
}

Text::ControllerPtr TextEditor::GetTextController()
{
  return mController;
}

bool TextEditor::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool                connected(true);
  Toolkit::TextEditor editor = Toolkit::TextEditor::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_TEXT_CHANGED))
  {
    editor.TextChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_INPUT_STYLE_CHANGED))
  {
    editor.InputStyleChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_MAX_LENGTH_REACHED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.MaxLengthReachedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_ANCHOR_CLICKED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.AnchorClickedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_CURSOR_POSITION_CHANGED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.CursorPositionChangedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_INPUT_FILTERED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.InputFilteredSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_SELECTION_CHANGED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.SelectionChangedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_SELECTION_CLEARED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.SelectionClearedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_SELECTION_STARTED))
  {
    if(editor)
    {
      Internal::TextEditor& editorImpl(GetImpl(editor));
      editorImpl.SelectionStartedSignal().Connect(tracker, functor);
    }
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

Toolkit::TextEditor::TextChangedSignalType& TextEditor::TextChangedSignal()
{
  return mTextChangedSignal;
}

Toolkit::TextEditor::InputStyleChangedSignalType& TextEditor::InputStyleChangedSignal()
{
  return mInputStyleChangedSignal;
}

Toolkit::TextEditor::ScrollStateChangedSignalType& TextEditor::ScrollStateChangedSignal()
{
  return mScrollStateChangedSignal;
}

void TextEditor::OnAccessibilityStatusChanged()
{
  CommonTextUtils::SynchronizeTextAnchorsInParent(Self(), mController, mAnchorActors);
}

void TextEditor::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New(this, this, this, this);

  mDecorator = Text::Decorator::New(*mController,
                                    *mController);

  mInputMethodContext = InputMethodContext::New(self);

  mController->GetLayoutEngine().SetLayout(Layout::Engine::MULTI_LINE_BOX);

  // Enables the text input.
  mController->EnableTextInput(mDecorator, mInputMethodContext);

  // Enables the vertical scrolling after the text input has been enabled.
  mController->SetVerticalScrollEnabled(true);

  // Disables the horizontal scrolling.
  mController->SetHorizontalScrollEnabled(false);

  // Sets the maximum number of characters.
  mController->SetMaximumNumberOfCharacters(std::numeric_limits<Length>::max());

  // Enable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled(true);

  mController->SetNoTextDoubleTapAction(Controller::NoTextTap::HIGHLIGHT);
  mController->SetNoTextLongPressAction(Controller::NoTextTap::HIGHLIGHT);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &TextEditor::OnLayoutDirectionChanged);

  // Forward input events to controller
  EnableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP | GestureType::PAN | GestureType::LONG_PRESS));
  GetTapGestureDetector().SetMaximumTapsRequired(2);
  GetTapGestureDetector().ReceiveAllTapEvents(true);

  self.TouchedSignal().Connect(this, &TextEditor::OnTouched);

  // Set BoundingBox to stage size if not already set.
  Rect<int> boundingBox;
  mDecorator->GetBoundingBox(boundingBox);

  if(boundingBox.IsEmpty())
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox(Rect<int>(0.0f, 0.0f, stageSize.width, stageSize.height));
  }

  // Whether to flip the selection handles as soon as they cross.
  mDecorator->FlipSelectionHandlesOnCrossEnabled(true);

  // Set the default scroll speed.
  mDecorator->SetScrollSpeed(DEFAULT_SCROLL_SPEED);

  // Fill-parent area by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
  self.OnSceneSignal().Connect(this, &TextEditor::OnSceneConnect);

  DevelControl::SetInputMethodContext(*this, mInputMethodContext);

  // Creates an extra control to be used as stencil buffer.
  mStencil = Control::New();
  mStencil.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  mStencil.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  mStencil.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIDDEN, true);

  // Creates a background visual. Even if the color is transparent it updates the stencil.
  mStencil.SetProperty(Toolkit::Control::Property::BACKGROUND,
                       Property::Map().Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR).Add(ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT));

  // Enable the clipping property.
  mStencil.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  mStencil.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

  self.Add(mStencil);

  // Accessibility
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::ENTRY);
  self.SetProperty(DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  Accessibility::Bridge::EnabledSignal().Connect(this, &TextEditor::OnAccessibilityStatusChanged);
  Accessibility::Bridge::DisabledSignal().Connect(this, &TextEditor::OnAccessibilityStatusChanged);
}

DevelControl::ControlAccessible* TextEditor::CreateAccessibleObject()
{
  return new TextEditorAccessible(Self());
}

void TextEditor::OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnStyleChange\n");

  switch(change)
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnStyleChange DEFAULT_FONT_CHANGE\n");
      const std::string& newFont = GetImpl(styleManager).GetDefaultFontFamily();
      // Property system did not set the font so should update it.
      mController->UpdateAfterFontChange(newFont);
      RelayoutRequest();
      break;
    }

    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      GetImpl(styleManager).ApplyThemeStyle(Toolkit::Control(GetOwner()));
      RelayoutRequest();
      break;
    }
    case StyleChange::THEME_CHANGE:
    {
      // Nothing to do, let control base class handle this
      break;
    }
  }

  // Up call to Control
  Control::OnStyleChange(styleManager, change);
}

Vector3 TextEditor::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float TextEditor::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return mController->GetHeightForWidth(width) + padding.top + padding.bottom;
}

void TextEditor::ResizeActor(Actor& actor, const Vector2& size)
{
  if(actor.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
  {
    actor.SetProperty(Actor::Property::SIZE, size);
  }
}

void TextEditor::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnPropertySet index[%d]\n", index);

  switch(index)
  {
    case DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      mController->SetUserInteractionEnabled(enabled);
      if(mStencil)
      {
        float opacity = enabled ? 1.0f : mController->GetDisabledColorOpacity();
        mStencil.SetProperty(Actor::Property::OPACITY, opacity);
      }
      break;
    }
    default:
    {
      Control::OnPropertySet(index, propertyValue); // up call to control for non-handled properties
      break;
    }
  }
}

void TextEditor::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor OnRelayout\n");

  Actor self = Self();

  Extents padding;
  padding = self.GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  Vector2 contentSize(size.x - (padding.start + padding.end), size.y - (padding.top + padding.bottom));

  // Support Right-To-Left of padding
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  if(mStencil)
  {
    mStencil.SetProperty(Actor::Property::POSITION, Vector2(padding.start, padding.top));
    ResizeActor(mStencil, contentSize);
  }
  if(mActiveLayer)
  {
    mActiveLayer.SetProperty(Actor::Property::POSITION, Vector2(padding.start, padding.top));
    ResizeActor(mActiveLayer, contentSize);
  }
  if(mCursorLayer)
  {
    // The cursor layer is added to the stencil in RenderText.
    // Do not calculate the position because the stencil has already been resized excluding the padding size.
    // There is no case where the text editor does not have a stencil.
    ResizeActor(mCursorLayer, contentSize);
  }

  // If there is text changed, callback is called.
  if(mTextChanged)
  {
    EmitTextChangedSignal();
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != updateTextType) ||
     !mRenderer)
  {
    DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnRelayout %p Displaying new contents\n", mController.Get());

    if(mDecorator &&
       (Text::Controller::NONE_UPDATED != (Text::Controller::DECORATOR_UPDATED & updateTextType)))
    {
      mDecorator->Relayout(contentSize);
    }

    if(!mRenderer)
    {
      mRenderer = Backend::Get().NewRenderer(mRenderingBackend);
    }

    RenderText(updateTextType);
  }

  if(mCursorPositionChanged)
  {
    EmitCursorPositionChangedSignal();
  }

  if(mSelectionStarted)
  {
    EmitSelectionStartedSignal();
  }

  if(mSelectionChanged)
  {
    EmitSelectionChangedSignal();
  }

  if(mSelectionCleared)
  {
    EmitSelectionClearedSignal();
  }

  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  if(!mController->IsInputStyleChangedSignalsQueueEmpty())
  {
    if(Adaptor::IsAvailable())
    {
      Adaptor& adaptor = Adaptor::Get();

      if(NULL == mIdleCallback)
      {
        // @note: The callback manager takes the ownership of the callback object.
        mIdleCallback = MakeCallback(this, &TextEditor::OnIdleSignal);
        if(DALI_UNLIKELY(!adaptor.AddIdle(mIdleCallback, false)))
        {
          DALI_LOG_ERROR("Fail to add idle callback for text editor queue. Skip these callbacks\n");

          // Set the pointer to null as the callback manager deletes the callback even AddIdle failed.
          mIdleCallback = NULL;
        }
      }
    }
  }
}

void TextEditor::RenderText(Text::Controller::UpdateTextType updateTextType)
{
  CommonTextUtils::RenderText(Self(), mRenderer, mController, mDecorator, mAlignmentOffset, mRenderableActor, mBackgroundActor, mCursorLayer, mStencil, mClippingDecorationActors, mAnchorActors, updateTextType);
  if(mRenderableActor)
  {
    ApplyScrollPosition();
  }
  UpdateScrollBar();
}

void TextEditor::OnKeyInputFocusGained()
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnKeyInputFocusGained %p\n", mController.Get());
  if(mInputMethodContext && IsEditable())
  {
    // All input panel properties, such as layout, return key type, and input hint, should be set before input panel activates (or shows).
    mInputMethodContext.ApplyOptions(mInputMethodOptions);
    mInputMethodContext.NotifyTextInputMultiLine(true);

    mInputMethodContext.StatusChangedSignal().Connect(this, &TextEditor::KeyboardStatusChanged);

    mInputMethodContext.EventReceivedSignal().Connect(this, &TextEditor::OnInputMethodContextEvent);

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the InputMethodContext is deactivated. Thus when window gain focus again, the InputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost(true);
  }

  if(IsEditable() && mController->IsUserInteractionEnabled())
  {
    mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event
  }

  EmitKeyInputFocusSignal(true); // Calls back into the Control hence done last.
}

void TextEditor::OnKeyInputFocusLost()
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor:OnKeyInputFocusLost %p\n", mController.Get());
  if(mInputMethodContext)
  {
    mInputMethodContext.StatusChangedSignal().Disconnect(this, &TextEditor::KeyboardStatusChanged);

    // The text editing is finished. Therefore the InputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost(false);

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.EventReceivedSignal().Disconnect(this, &TextEditor::OnInputMethodContextEvent);
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal(false); // Calls back into the Control hence done last.
}

bool TextEditor::OnAccessibilityActivated()
{
  SetKeyInputFocus();
  return true;
}

void TextEditor::OnTap(const TapGesture& gesture)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnTap %p\n", mController.Get());
  if(mInputMethodContext && IsEditable())
  {
    mInputMethodContext.Activate();
  }
  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents padding;
  padding                   = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->TapEvent(gesture.GetNumberOfTaps(), localPoint.x - padding.start, localPoint.y - padding.top);
  mController->AnchorEvent(localPoint.x - padding.start, localPoint.y - padding.top);

  Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
  if(keyboardFocusManager)
  {
    keyboardFocusManager.SetCurrentFocusActor(Self());
  }
  SetKeyInputFocus();
}

void TextEditor::OnPan(const PanGesture& gesture)
{
  mController->PanEvent(gesture.GetState(), gesture.GetDisplacement());
  if(gesture.GetState() == GestureState::STARTED && !mController->IsScrollable(gesture.GetDisplacement()))
  {
    Dali::DevelActor::SetNeedGesturePropagation(Self(), true);
  }
}

void TextEditor::OnLongPress(const LongPressGesture& gesture)
{
  if(mInputMethodContext && IsEditable())
  {
    mInputMethodContext.Activate();
  }
  Extents padding;
  padding                   = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->LongPressEvent(gesture.GetState(), localPoint.x - padding.start, localPoint.y - padding.top);

  SetKeyInputFocus();
}

bool TextEditor::OnKeyEvent(const KeyEvent& event)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnKeyEvent %p keyCode %d\n", mController.Get(), event.GetKeyCode());

  if(Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape())
  {
    // Make sure ClearKeyInputFocus when only key is up
    if(event.GetState() == KeyEvent::UP)
    {
      Dali::Toolkit::KeyboardFocusManager keyboardFocusManager = Dali::Toolkit::KeyboardFocusManager::Get();
      if(keyboardFocusManager)
      {
        keyboardFocusManager.ClearFocus();
      }
      ClearKeyInputFocus();
    }

    return true;
  }

  return mController->KeyEvent(event);
}

void TextEditor::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextEditor::TextInserted(unsigned int position, unsigned int length, const std::string& content)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    accessible->EmitTextInserted(position, length, content);
  }
}

void TextEditor::TextDeleted(unsigned int position, unsigned int length, const std::string& content)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    accessible->EmitTextDeleted(position, length, content);
  }
}

void TextEditor::CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition)
{
  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    accessible->EmitTextCursorMoved(newPosition);
  }

  if((oldPosition != newPosition) && !mCursorPositionChanged)
  {
    mCursorPositionChanged = true;
    mOldPosition           = oldPosition;
  }
}

void TextEditor::TextChanged(bool immediate)
{
  if(immediate) // Emits TextChangedSignal immediately
  {
    EmitTextChangedSignal();
  }
  else
  {
    mTextChanged = true;
  }
}

void TextEditor::EmitTextChangedSignal()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mTextChangedSignal.Emit(handle);
  mTextChanged = false;
}

void TextEditor::MaxLengthReached()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mMaxLengthReachedSignal.Emit(handle);
}

void TextEditor::InputStyleChanged(Text::InputStyle::Mask inputStyleMask)
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mInputStyleChangedSignal.Emit(handle, ConvertInputStyle(inputStyleMask));
}

void TextEditor::AnchorClicked(const std::string& href)
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mAnchorClickedSignal.Emit(handle, href.c_str(), href.length());
}

void TextEditor::EmitCursorPositionChangedSignal()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mCursorPositionChanged = false;
  mCursorPositionChangedSignal.Emit(handle, mOldPosition);
}

void TextEditor::InputFiltered(Toolkit::InputFilter::Property::Type type)
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mInputFilteredSignal.Emit(handle, type);
}

void TextEditor::EmitSelectionChangedSignal()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mSelectionChangedSignal.Emit(handle, mOldSelectionStart, mOldSelectionEnd);
  mSelectionChanged = false;
}

void TextEditor::EmitSelectionClearedSignal()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mSelectionClearedSignal.Emit(handle);
  mSelectionCleared = false;
}

void TextEditor::EmitSelectionStartedSignal()
{
  Dali::Toolkit::TextEditor handle(GetOwner());
  mSelectionStartedSignal.Emit(handle);
  mSelectionStarted = false;
}

void TextEditor::SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd)
{
  if(((oldStart != newStart) || (oldEnd != newEnd)) && !mSelectionChanged)
  {
    if(newStart == newEnd)
    {
      mSelectionCleared = true;
    }
    else
    {
      if(oldStart == oldEnd)
      {
        mSelectionStarted = true;
      }
    }

    mSelectionChanged  = true;
    mOldSelectionStart = oldStart;
    mOldSelectionEnd   = oldEnd;

    if(mOldSelectionStart > mOldSelectionEnd)
    {
      //swap
      uint32_t temp      = mOldSelectionStart;
      mOldSelectionStart = mOldSelectionEnd;
      mOldSelectionEnd   = temp;
    }
  }
}

void TextEditor::AddDecoration(Actor& actor, DecorationType type, bool needsClipping)
{
  if(actor)
  {
    if(needsClipping)
    {
      mClippingDecorationActors.push_back(actor);
    }

    // If the actor is a layer type, add it.
    if(type == DecorationType::ACTIVE_LAYER)
    {
      AddLayer(mActiveLayer, actor);
    }
    else if(type == DecorationType::CURSOR_LAYER)
    {
      AddLayer(mCursorLayer, actor);
    }
  }
}

void TextEditor::AddLayer(Actor& layer, Actor& actor)
{
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  Self().Add(actor);
  layer = actor;
}

void TextEditor::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SetTextSelectionRange(start, end);
    SetKeyInputFocus();
  }
}

Uint32Pair TextEditor::GetTextSelectionRange() const
{
  Uint32Pair range(0, 0);
  if(mController && mController->IsShowingRealText())
  {
    range = mController->GetTextSelectionRange();
  }
  return range;
}

void TextEditor::GetControlBackgroundColor(Vector4& color) const
{
  Property::Value propValue = Self().GetProperty(Toolkit::Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();

  Property::Value* colorValue = nullptr;
  if(resultMap && (colorValue = resultMap->Find(ColorVisual::Property::MIX_COLOR)))
  {
    colorValue->Get(color);
  }
}

void TextEditor::UpdateScrollBar()
{
  using namespace Dali;

  float scrollPosition;
  float controlSize;
  float layoutSize;
  bool  latestScrolled;

  if(!mScrollBarEnabled)
  {
    return;
  }
  latestScrolled = mController->GetTextScrollInfo(scrollPosition, controlSize, layoutSize);
  if(!latestScrolled || controlSize > layoutSize)
  {
    return;
  }

  CustomActor self = Self();
  if(!mScrollBar)
  {
    mScrollBar = Toolkit::ScrollBar::New(Toolkit::ScrollBar::VERTICAL);
    mScrollBar.SetIndicatorHeightPolicy(Toolkit::ScrollBar::VARIABLE);
    mScrollBar.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT);
    mScrollBar.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT);
    mScrollBar.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
    mScrollBar.SetResizePolicy(ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH);

    // Register the scroll position property
    Property::Index propertyScrollPosition = self.RegisterProperty(SCROLL_BAR_POSITION, scrollPosition);
    // Register the minimum scroll position property
    Property::Index propertyMinScrollPosition = self.RegisterProperty(SCROLL_BAR_POSITION_MIN, 0.0f);
    // Register the maximum scroll position property
    Property::Index propertyMaxScrollPosition = self.RegisterProperty(SCROLL_BAR_POSITION_MAX, (layoutSize - controlSize));
    // Register the scroll content size property
    Property::Index propertyScrollContentSize = self.RegisterProperty(SCROLL_BAR_CONTENT_SIZE, layoutSize);

    mScrollBar.SetScrollPropertySource(self, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);

    // Set style name of ScrollBar for styling
    mScrollBar.SetStyleName("TextEditorScrollBar");
    Toolkit::Control scrollIndicator = Toolkit::Control::DownCast(mScrollBar.GetScrollIndicator());
    if(scrollIndicator)
    {
      // Set style name of ScrollBarIndicator for styling
      scrollIndicator.SetStyleName("TextEditorScrollBarIndicator");
    }

    self.Add(mScrollBar);
  }
  else
  {
    Property::Index propertyScrollPosition    = self.GetPropertyIndex(SCROLL_BAR_POSITION);
    Property::Index propertyMaxScrollPosition = self.GetPropertyIndex(SCROLL_BAR_POSITION_MAX);
    Property::Index propertyScrollContentSize = self.GetPropertyIndex(SCROLL_BAR_CONTENT_SIZE);

    self.SetProperty(propertyScrollPosition, scrollPosition);
    self.SetProperty(propertyMaxScrollPosition, (layoutSize - controlSize));
    self.SetProperty(propertyScrollContentSize, layoutSize);
  }

  // If scrolling is not started, start scrolling and emit ScrollStateChangedSignal
  if(!mScrollStarted)
  {
    mScrollStarted = true;
    Dali::Toolkit::TextEditor handle(GetOwner());
    mScrollStateChangedSignal.Emit(handle, Toolkit::TextEditor::Scroll::STARTED);
  }

  Actor indicator = mScrollBar.GetScrollIndicator();
  if(mAnimation)
  {
    mAnimation.Stop(); // Cancel any animation
  }
  else
  {
    mAnimation = Animation::New(mAnimationPeriod.durationSeconds);
  }
  indicator.SetProperty(Actor::Property::OPACITY, 1.0f);
  mAnimation.AnimateTo(Property(indicator, Actor::Property::COLOR_ALPHA), 0.0f, AlphaFunction::EASE_IN, mAnimationPeriod);
  mAnimation.Play();
  mAnimation.FinishedSignal().Connect(this, &TextEditor::OnScrollIndicatorAnimationFinished);
}

void TextEditor::OnScrollIndicatorAnimationFinished(Animation& animation)
{
  // If animation is successfully ended, then emit ScrollStateChangedSignal
  if(Dali::EqualsZero(animation.GetCurrentProgress()))
  {
    mScrollStarted = false;
    Dali::Toolkit::TextEditor handle(GetOwner());
    mScrollStateChangedSignal.Emit(handle, Toolkit::TextEditor::Scroll::FINISHED);
  }
}

void TextEditor::OnSceneConnect(Dali::Actor actor)
{
  if(mHasBeenStaged)
  {
    RenderText(static_cast<Text::Controller::UpdateTextType>(Text::Controller::MODEL_UPDATED | Text::Controller::DECORATOR_UPDATED));
  }
  else
  {
    mHasBeenStaged = true;
  }
}

InputMethodContext::CallbackData TextEditor::OnInputMethodContextEvent(Dali::InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::OnInputMethodContextEvent %p eventName %d\n", mController.Get(), inputMethodContextEvent.eventName);
  return mController->OnInputMethodContextEvent(inputMethodContext, inputMethodContextEvent);
}

void TextEditor::GetHandleImagePropertyValue(Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType)
{
  if(mDecorator)
  {
    Property::Map map;
    map[TextEditor::PropertyHandler::IMAGE_MAP_FILENAME_STRING] = mDecorator->GetHandleImage(handleType, handleImageType);

    value = map;
  }
}

void TextEditor::KeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_INFO(gTextEditorLogFilter, Debug::Verbose, "TextEditor::KeyboardStatusChanged %p keyboardShown %d\n", mController.Get(), keyboardShown);

  // Just hide the grab handle when keyboard is hidden.
  if(!keyboardShown)
  {
    mController->KeyboardFocusLostEvent();
  }
  else
  {
    mController->KeyboardFocusGainEvent(); // Initially called by OnKeyInputFocusGained
  }
}

void TextEditor::OnSceneConnection(int depth)
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth(depth);

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  Control::OnSceneConnection(depth);
}

bool TextEditor::OnTouched(Actor actor, const TouchEvent& touch)
{
  return false;
}

void TextEditor::OnIdleSignal()
{
  // Emits the change of input style signals.
  mController->ProcessInputStyleChangedSignals();

  // Set the pointer to null as the callback manager deletes the callback after execute it.
  mIdleCallback = NULL;
}

void TextEditor::ApplyScrollPosition()
{
  const Vector2& scrollOffset = mController->GetTextModel()->GetScrollPosition();
  float          scrollAmount = 0.0f;

  if(mScrollAnimationEnabled)
  {
    scrollAmount = mController->GetScrollAmountByUserInput();
  }
  if(mTextVerticalScroller)
  {
    mTextVerticalScroller->CheckStartAnimation(mRenderableActor, scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount, scrollAmount);
  }
  else if(Equals(scrollAmount, 0.0f, Math::MACHINE_EPSILON_1))
  {
    mRenderableActor.SetProperty(Actor::Property::POSITION, Vector2(scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount));
  }
  else
  {
    mTextVerticalScroller = Text::TextVerticalScroller::New();
    if(!Equals(mScrollAnimationDuration, 0.0f, Math::MACHINE_EPSILON_1))
    {
      mTextVerticalScroller->SetDuration(mScrollAnimationDuration);
    }
    mTextVerticalScroller->CheckStartAnimation(mRenderableActor, scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount, scrollAmount);
  }
}

bool TextEditor::IsEditable() const
{
  return mController->IsEditable();
}

void TextEditor::SetEditable(bool editable)
{
  mController->SetEditable(editable);
  if(mInputMethodContext && !editable)
  {
    mInputMethodContext.Deactivate();
  }
}

void TextEditor::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

TextEditor::TextEditor(ControlBehaviour additionalBehaviour)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour)),
  mAnimationPeriod(0.0f, 0.0f),
  mIdleCallback(NULL),
  mAlignmentOffset(0.f),
  mScrollAnimationDuration(0.f),
  mLineSpacing(0.f),
  mRenderingBackend(DEFAULT_RENDERING_BACKEND),
  mHasBeenStaged(false),
  mScrollAnimationEnabled(false),
  mScrollBarEnabled(false),
  mScrollStarted(false),
  mTextChanged(false),
  mCursorPositionChanged(false),
  mSelectionChanged(false),
  mSelectionCleared(false),
  mOldPosition(0u),
  mOldSelectionStart(0u),
  mOldSelectionEnd(0u),
  mSelectionStarted(false)
{
}

TextEditor::~TextEditor()
{
  UnparentAndReset(mStencil);

  if((NULL != mIdleCallback) && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the text-editor is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

std::string TextEditor::TextEditorAccessible::GetName() const
{
  return GetWholeText();
}

const std::vector<Toolkit::TextAnchor>& TextEditor::TextEditorAccessible::GetTextAnchors() const
{
  auto self = Toolkit::TextEditor::DownCast(Self());

  return Toolkit::GetImpl(self).mAnchorActors;
}

Toolkit::Text::ControllerPtr TextEditor::TextEditorAccessible::GetTextController() const
{
  auto self = Toolkit::TextEditor::DownCast(Self());

  return Toolkit::GetImpl(self).GetTextController();
}

void TextEditor::TextEditorAccessible::RequestTextRelayout()
{
  auto  self     = Toolkit::TextEditor::DownCast(Self());
  auto& selfImpl = Toolkit::GetImpl(self);

  selfImpl.RequestTextRelayout();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
