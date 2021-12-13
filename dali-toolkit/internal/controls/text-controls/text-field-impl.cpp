/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-property-handler.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

using namespace Dali::Toolkit::Text;

#if defined(DEBUG_ENABLED)
Debug::Filter* gTextFieldLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
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
} // unnamed namespace

namespace
{
// Type registration
BaseHandle Create()
{
  return Toolkit::TextField::New();
}

// clang-format off
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextField, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "text",                             STRING,    TEXT                                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "placeholderText",                  STRING,    PLACEHOLDER_TEXT                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "placeholderTextFocused",           STRING,    PLACEHOLDER_TEXT_FOCUSED            )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "fontFamily",                       STRING,    FONT_FAMILY                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "fontStyle",                        MAP,       FONT_STYLE                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "pointSize",                        FLOAT,     POINT_SIZE                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "maxLength",                        INTEGER,   MAX_LENGTH                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "exceedPolicy",                     INTEGER,   EXCEED_POLICY                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "horizontalAlignment",              STRING,    HORIZONTAL_ALIGNMENT                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "verticalAlignment",                STRING,    VERTICAL_ALIGNMENT                  )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "textColor",                        VECTOR4,   TEXT_COLOR                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "placeholderTextColor",             VECTOR4,   PLACEHOLDER_TEXT_COLOR              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "primaryCursorColor",               VECTOR4,   PRIMARY_CURSOR_COLOR                )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "secondaryCursorColor",             VECTOR4,   SECONDARY_CURSOR_COLOR              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "enableCursorBlink",                BOOLEAN,   ENABLE_CURSOR_BLINK                 )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "cursorBlinkInterval",              FLOAT,     CURSOR_BLINK_INTERVAL               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "cursorBlinkDuration",              FLOAT,     CURSOR_BLINK_DURATION               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "cursorWidth",                      INTEGER,   CURSOR_WIDTH                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "grabHandleImage",                  STRING,    GRAB_HANDLE_IMAGE                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "grabHandlePressedImage",           STRING,    GRAB_HANDLE_PRESSED_IMAGE           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "scrollThreshold",                  FLOAT,     SCROLL_THRESHOLD                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "scrollSpeed",                      FLOAT,     SCROLL_SPEED                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandleImageLeft",         MAP,       SELECTION_HANDLE_IMAGE_LEFT         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandleImageRight",        MAP,       SELECTION_HANDLE_IMAGE_RIGHT        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandlePressedImageLeft",  MAP,       SELECTION_HANDLE_PRESSED_IMAGE_LEFT )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandlePressedImageRight", MAP,       SELECTION_HANDLE_PRESSED_IMAGE_RIGHT)
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandleMarkerImageLeft",   MAP,       SELECTION_HANDLE_MARKER_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHandleMarkerImageRight",  MAP,       SELECTION_HANDLE_MARKER_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "selectionHighlightColor",          VECTOR4,   SELECTION_HIGHLIGHT_COLOR           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "decorationBoundingBox",            RECTANGLE, DECORATION_BOUNDING_BOX             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputMethodSettings",              MAP,       INPUT_METHOD_SETTINGS               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputColor",                       VECTOR4,   INPUT_COLOR                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "enableMarkup",                     BOOLEAN,   ENABLE_MARKUP                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputFontFamily",                  STRING,    INPUT_FONT_FAMILY                   )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputFontStyle",                   MAP,       INPUT_FONT_STYLE                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputPointSize",                   FLOAT,     INPUT_POINT_SIZE                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "underline",                        MAP,       UNDERLINE                           )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputUnderline",                   MAP,       INPUT_UNDERLINE                     )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "shadow",                           MAP,       SHADOW                              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputShadow",                      MAP,       INPUT_SHADOW                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "emboss",                           MAP,       EMBOSS                              )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputEmboss",                      MAP,       INPUT_EMBOSS                        )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "outline",                          MAP,       OUTLINE                             )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "inputOutline",                     MAP,       INPUT_OUTLINE                       )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "hiddenInputSettings",              MAP,       HIDDEN_INPUT_SETTINGS               )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "pixelSize",                        FLOAT,     PIXEL_SIZE                          )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "enableSelection",                  BOOLEAN,   ENABLE_SELECTION                    )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "placeholder",                      MAP,       PLACEHOLDER                         )
DALI_PROPERTY_REGISTRATION(Toolkit,                 TextField, "ellipsis",                         BOOLEAN,   ELLIPSIS                            )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "enableShiftSelection",             BOOLEAN,   ENABLE_SHIFT_SELECTION              )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "enableGrabHandle",                 BOOLEAN,   ENABLE_GRAB_HANDLE                  )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "matchSystemLanguageDirection",     BOOLEAN,   MATCH_SYSTEM_LANGUAGE_DIRECTION     )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "enableGrabHandlePopup",            BOOLEAN,   ENABLE_GRAB_HANDLE_POPUP            )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "textBackground",                   VECTOR4,   BACKGROUND                          )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY(Toolkit, TextField, "selectedText",                     STRING,    SELECTED_TEXT                       )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "renderingBackend",                 INTEGER,   RENDERING_BACKEND                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "selectedTextStart",                INTEGER,   SELECTED_TEXT_START                 )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "selectedTextEnd",                  INTEGER,   SELECTED_TEXT_END                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "enableEditing",                    BOOLEAN,   ENABLE_EDITING                      )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "fontSizeScale",                    FLOAT,     FONT_SIZE_SCALE                     )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "primaryCursorPosition",            INTEGER,   PRIMARY_CURSOR_POSITION             )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "grabHandleColor",                  VECTOR4,   GRAB_HANDLE_COLOR                   )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "inputFilter",                      MAP,       INPUT_FILTER                        )
DALI_DEVEL_PROPERTY_REGISTRATION(Toolkit,           TextField, "ellipsisPosition",                 INTEGER,   ELLIPSIS_POSITION                   )

DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "textChanged",           SIGNAL_TEXT_CHANGED           )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "maxLengthReached",      SIGNAL_MAX_LENGTH_REACHED     )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "inputStyleChanged",     SIGNAL_INPUT_STYLE_CHANGED    )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "anchorClicked",         SIGNAL_ANCHOR_CLICKED         )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "inputFiltered",         SIGNAL_INPUT_FILTERED         )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "cursorPositionChanged", SIGNAL_CURSOR_POSITION_CHANGED)
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "selectionChanged",      SIGNAL_SELECTION_CHANGED      )
DALI_SIGNAL_REGISTRATION(Toolkit, TextField, "selectionCleared",      SIGNAL_SELECTION_CLEARED      )

DALI_TYPE_REGISTRATION_END()
// clang-format on

Toolkit::TextField::InputStyle::Mask ConvertInputStyle(Text::InputStyle::Mask inputStyleMask)
{
  Toolkit::TextField::InputStyle::Mask fieldInputStyleMask = Toolkit::TextField::InputStyle::NONE;

  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_COLOR))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::COLOR);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_FAMILY))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_FAMILY);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_POINT_SIZE))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::POINT_SIZE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_WEIGHT))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_WIDTH))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_FONT_SLANT))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_UNDERLINE))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::UNDERLINE);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_SHADOW))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::SHADOW);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_EMBOSS))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::EMBOSS);
  }
  if(InputStyle::NONE != static_cast<InputStyle::Mask>(inputStyleMask & InputStyle::INPUT_OUTLINE))
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>(fieldInputStyleMask | Toolkit::TextField::InputStyle::OUTLINE);
  }
  return fieldInputStyleMask;
}

bool IsHiddenInput(Toolkit::TextField textField)
{
  Property::Map hiddenInputSettings = textField.GetProperty<Property::Map>(Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS);
  auto          mode                = hiddenInputSettings.Find(Toolkit::HiddenInput::Property::MODE);
  if(mode && (mode->Get<int>() != Toolkit::HiddenInput::Mode::HIDE_NONE))
  {
    return true;
  }
  return false;
}

char GetSubstituteCharacter(Toolkit::TextField textField)
{
  Property::Map hiddenInputSettings = textField.GetProperty<Property::Map>(Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS);
  auto          substChar           = hiddenInputSettings.Find(Toolkit::HiddenInput::Property::SUBSTITUTE_CHARACTER);
  if(substChar)
  {
    return static_cast<char>(substChar->Get<int>());
  }
  return STAR;
}

} // namespace

Toolkit::TextField TextField::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr<TextField> impl = new TextField();

  // Pass ownership to CustomActor handle
  Toolkit::TextField handle(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextField::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Toolkit::TextField textField = Toolkit::TextField::DownCast(Dali::BaseHandle(object));

  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField SetProperty\n");

  if(textField)
  {
    PropertyHandler::SetProperty(textField, index, value);
  }
}

Property::Value TextField::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Toolkit::TextField textField = Toolkit::TextField::DownCast(Dali::BaseHandle(object));

  if(textField)
  {
    value = PropertyHandler::GetProperty(textField, index);
  }

  return value;
}

void TextField::SelectWholeText()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectWholeText();
    SetKeyInputFocus();
  }
}

void TextField::SelectNone()
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectNone();
  }
}

void TextField::SelectText(const uint32_t start, const uint32_t end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SelectText(start, end);
    SetKeyInputFocus();
  }
}

string TextField::GetSelectedText() const
{
  string selectedText = "";
  if(mController && mController->IsShowingRealText())
  {
    selectedText = mController->GetSelectedText();
  }
  return selectedText;
}

void TextField::SetTextSelectionRange(const uint32_t* start, const uint32_t* end)
{
  if(mController && mController->IsShowingRealText())
  {
    mController->SetTextSelectionRange(start, end);
    SetKeyInputFocus();
  }
}

Uint32Pair TextField::GetTextSelectionRange() const
{
  Uint32Pair range;
  if(mController && mController->IsShowingRealText())
  {
    range = mController->GetTextSelectionRange();
  }
  return range;
}

string TextField::CopyText()
{
  string copiedText = "";
  if(mController && mController->IsShowingRealText())
  {
    copiedText = mController->CopyText();
  }
  return copiedText;
}

string TextField::CutText()
{
  string cutText = "";
  if(mController && mController->IsShowingRealText())
  {
    cutText = mController->CutText();
  }
  return cutText;
}

void TextField::PasteText()
{
  if(mController)
  {
    SetKeyInputFocus(); //Giving focus to the field that was passed to the PasteText in case the passed field (current field) doesn't have focus.
    mController->PasteText();
  }
}

InputMethodContext TextField::GetInputMethodContext()
{
  return mInputMethodContext;
}

bool TextField::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool               connected(true);
  Toolkit::TextField field = Toolkit::TextField::DownCast(handle);

  if(0 == strcmp(signalName.c_str(), SIGNAL_TEXT_CHANGED))
  {
    field.TextChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_MAX_LENGTH_REACHED))
  {
    field.MaxLengthReachedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_INPUT_STYLE_CHANGED))
  {
    field.InputStyleChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_ANCHOR_CLICKED))
  {
    if(field)
    {
      Internal::TextField& fieldImpl(GetImpl(field));
      fieldImpl.AnchorClickedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_CURSOR_POSITION_CHANGED))
  {
    if(field)
    {
      Internal::TextField& fieldImpl(GetImpl(field));
      fieldImpl.CursorPositionChangedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_INPUT_FILTERED))
  {
    if(field)
    {
      Internal::TextField& fieldImpl(GetImpl(field));
      fieldImpl.InputFilteredSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_SELECTION_CHANGED))
  {
    if(field)
    {
      Internal::TextField& fieldImpl(GetImpl(field));
      fieldImpl.SelectionChangedSignal().Connect(tracker, functor);
    }
  }
  else if(0 == strcmp(signalName.c_str(), SIGNAL_SELECTION_CLEARED))
  {
    if(field)
    {
      Internal::TextField& fieldImpl(GetImpl(field));
      fieldImpl.SelectionClearedSignal().Connect(tracker, functor);
    }
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

Toolkit::TextField::TextChangedSignalType& TextField::TextChangedSignal()
{
  return mTextChangedSignal;
}

Toolkit::TextField::MaxLengthReachedSignalType& TextField::MaxLengthReachedSignal()
{
  return mMaxLengthReachedSignal;
}

Toolkit::TextField::InputStyleChangedSignalType& TextField::InputStyleChangedSignal()
{
  return mInputStyleChangedSignal;
}

DevelTextField::AnchorClickedSignalType& TextField::AnchorClickedSignal()
{
  return mAnchorClickedSignal;
}

DevelTextField::CursorPositionChangedSignalType& TextField::CursorPositionChangedSignal()
{
  return mCursorPositionChangedSignal;
}

DevelTextField::InputFilteredSignalType& TextField::InputFilteredSignal()
{
  return mInputFilteredSignal;
}

DevelTextField::SelectionChangedSignalType& TextField::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

DevelTextField::SelectionClearedSignalType& TextField::SelectionClearedSignal()
{
  return mSelectionClearedSignal;
}

void TextField::OnAccessibilityStatusChanged()
{
  CommonTextUtils::SynchronizeTextAnchorsInParent(Self(), mController, mAnchorActors);
}

void TextField::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New(this, this, this, this);

  // When using the vector-based rendering, the size of the GLyphs are different
  TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
  mController->SetGlyphType(glyphType);

  mDecorator = Text::Decorator::New(*mController,
                                    *mController);

  mInputMethodContext = InputMethodContext::New(self);

  mController->GetLayoutEngine().SetLayout(Layout::Engine::SINGLE_LINE_BOX);

  // Enables the text input.
  mController->EnableTextInput(mDecorator, mInputMethodContext);

  // Enables the horizontal scrolling after the text input has been enabled.
  mController->SetHorizontalScrollEnabled(true);

  // Disables the vertical scrolling.
  mController->SetVerticalScrollEnabled(false);

  // Disable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled(false);

  mController->SetNoTextDoubleTapAction(Controller::NoTextTap::HIGHLIGHT);
  mController->SetNoTextLongPressAction(Controller::NoTextTap::HIGHLIGHT);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &TextField::OnLayoutDirectionChanged);

  // Forward input events to controller
  EnableGestureDetection(static_cast<GestureType::Value>(GestureType::TAP | GestureType::PAN | GestureType::LONG_PRESS));
  GetTapGestureDetector().SetMaximumTapsRequired(2);
  GetTapGestureDetector().ReceiveAllTapEvents(true);

  self.TouchedSignal().Connect(this, &TextField::OnTouched);

  // Set BoundingBox to stage size if not already set.
  Rect<int> boundingBox;
  mDecorator->GetBoundingBox(boundingBox);

  if(boundingBox.IsEmpty())
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox(Rect<int>(0.0f, 0.0f, stageSize.width, stageSize.height));
  }

  // Flip vertically the 'left' selection handle
  mDecorator->FlipHandleVertically(LEFT_SELECTION_HANDLE, true);

  // Fill-parent area by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT);
  self.OnSceneSignal().Connect(this, &TextField::OnSceneConnect);

  //Enable highightability
  self.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_HIGHLIGHTABLE, true);

  DevelControl::SetInputMethodContext(*this, mInputMethodContext);

  if(Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == mExceedPolicy)
  {
    EnableClipping();
  }

  DevelControl::SetAccessibilityConstructor(self, [](Dali::Actor actor) {
    return std::unique_ptr<Dali::Accessibility::Accessible>(
      new AccessibleImpl(actor, Dali::Accessibility::Role::ENTRY));
  });

  Accessibility::Bridge::EnabledSignal().Connect(this, &TextField::OnAccessibilityStatusChanged);
  Accessibility::Bridge::DisabledSignal().Connect(this, &TextField::OnAccessibilityStatusChanged);
}

void TextField::OnStyleChange(Toolkit::StyleManager styleManager, StyleChange::Type change)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnStyleChange\n");

  switch(change)
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnStyleChange DEFAULT_FONT_CHANGE\n");
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

Vector3 TextField::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float TextField::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return mController->GetHeightForWidth(width) + padding.top + padding.bottom;
}

void TextField::ResizeActor(Actor& actor, const Vector2& size)
{
  if(actor.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
  {
    actor.SetProperty(Actor::Property::SIZE, size);
  }
}

void TextField::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField OnRelayout\n");

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

  // If there is text changed, callback is called.
  if(mTextChanged)
  {
    EmitTextChangedSignal();
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != updateTextType) ||
     !mRenderer)
  {
    DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnRelayout %p Displaying new contents\n", mController.Get());

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

  if(mSelectionChanged)
  {
    EmitSelectionChangedSignal();
  }

  if(mSelectionCleared)
  {
    EmitSelectionClearedSignal();
  }

  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
  if(!mController->IsInputStyleChangedSignalsQueueEmpty())
  {
    if(Adaptor::IsAvailable())
    {
      Adaptor& adaptor = Adaptor::Get();

      if(NULL == mIdleCallback)
      {
        // @note: The callback manager takes the ownership of the callback object.
        mIdleCallback = MakeCallback(this, &TextField::OnIdleSignal);
        adaptor.AddIdle(mIdleCallback, false);
      }
    }
  }
}

Text::ControllerPtr TextField::GetTextController()
{
  return mController;
}

void TextField::RenderText(Text::Controller::UpdateTextType updateTextType)
{
  CommonTextUtils::RenderText(Self(), mRenderer, mController, mDecorator, mAlignmentOffset, mRenderableActor, mBackgroundActor, mStencil, mClippingDecorationActors, mAnchorActors, updateTextType);
}

void TextField::OnKeyInputFocusGained()
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnKeyInputFocusGained %p\n", mController.Get());
  if(mInputMethodContext && IsEditable())
  {
    // All input panel properties, such as layout, return key type, and input hint, should be set before input panel activates (or shows).
    mInputMethodContext.ApplyOptions(mInputMethodOptions);
    mInputMethodContext.NotifyTextInputMultiLine(false);

    mInputMethodContext.StatusChangedSignal().Connect(this, &TextField::KeyboardStatusChanged);

    mInputMethodContext.EventReceivedSignal().Connect(this, &TextField::OnInputMethodContextEvent);

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the inputMethodContext is deactivated. Thus when window gain focus again, the inputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost(true);
  }
  ClipboardEventNotifier notifier(ClipboardEventNotifier::Get());

  if(notifier)
  {
    notifier.ContentSelectedSignal().Connect(this, &TextField::OnClipboardTextSelected);
  }

  mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event

  EmitKeyInputFocusSignal(true); // Calls back into the Control hence done last.
}

void TextField::OnKeyInputFocusLost()
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField:OnKeyInputFocusLost %p\n", mController.Get());
  if(mInputMethodContext)
  {
    mInputMethodContext.StatusChangedSignal().Disconnect(this, &TextField::KeyboardStatusChanged);
    // The text editing is finished. Therefore the inputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost(false);

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.EventReceivedSignal().Disconnect(this, &TextField::OnInputMethodContextEvent);
  }
  ClipboardEventNotifier notifier(ClipboardEventNotifier::Get());

  if(notifier)
  {
    notifier.ContentSelectedSignal().Disconnect(this, &TextField::OnClipboardTextSelected);
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal(false); // Calls back into the Control hence done last.
}

bool TextField::OnAccessibilityActivated()
{
  SetKeyInputFocus();
  return true;
}

void TextField::OnTap(const TapGesture& gesture)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnTap %p\n", mController.Get());
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

  SetKeyInputFocus();
}

void TextField::OnPan(const PanGesture& gesture)
{
  mController->PanEvent(gesture.GetState(), gesture.GetDisplacement());
}

void TextField::OnLongPress(const LongPressGesture& gesture)
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

bool TextField::OnKeyEvent(const KeyEvent& event)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnKeyEvent %p keyCode %d\n", mController.Get(), event.GetKeyCode());

  if(Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape())
  {
    // Make sure ClearKeyInputFocus when only key is up
    if(event.GetState() == KeyEvent::UP)
    {
      ClearKeyInputFocus();
    }

    return true;
  }
  else if(Dali::DevelKey::DALI_KEY_RETURN == event.GetKeyCode())
  {
    // Do nothing when enter is comming.
    return false;
  }

  return mController->KeyEvent(event);
}

void TextField::RequestTextRelayout()
{
  RelayoutRequest();
}

bool TextField::IsEditable() const
{
  return mController->IsEditable();
}

void TextField::SetEditable(bool editable)
{
  mController->SetEditable(editable);
  if(mInputMethodContext && !editable)
  {
    mInputMethodContext.Deactivate();
  }
}

void TextField::TextInserted(unsigned int position, unsigned int length, const std::string& content)
{
  if(Accessibility::IsUp())
  {
    Control::Impl::GetAccessibilityObject(Self())->EmitTextInserted(position, length, content);
  }
}

void TextField::TextDeleted(unsigned int position, unsigned int length, const std::string& content)
{
  if(Accessibility::IsUp())
  {
    Control::Impl::GetAccessibilityObject(Self())->EmitTextDeleted(position, length, content);
  }
}

void TextField::CursorPositionChanged(unsigned int oldPosition, unsigned int newPosition)
{
  if(Accessibility::IsUp())
  {
    Control::Impl::GetAccessibilityObject(Self())->EmitTextCursorMoved(newPosition);
  }

  if((oldPosition != newPosition) && !mCursorPositionChanged)
  {
    mCursorPositionChanged = true;
    mOldPosition           = oldPosition;
  }
}

void TextField::TextChanged(bool immediate)
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

void TextField::EmitTextChangedSignal()
{
  Dali::Toolkit::TextField handle(GetOwner());
  mTextChangedSignal.Emit(handle);
  mTextChanged = false;
}

void TextField::MaxLengthReached()
{
  Dali::Toolkit::TextField handle(GetOwner());
  mMaxLengthReachedSignal.Emit(handle);
}

void TextField::InputStyleChanged(Text::InputStyle::Mask inputStyleMask)
{
  Dali::Toolkit::TextField handle(GetOwner());
  mInputStyleChangedSignal.Emit(handle, ConvertInputStyle(inputStyleMask));
}

void TextField::AnchorClicked(const std::string& href)
{
  Dali::Toolkit::TextField handle(GetOwner());
  mAnchorClickedSignal.Emit(handle, href.c_str(), href.length());
}

void TextField::EmitCursorPositionChangedSignal()
{
  Dali::Toolkit::TextField handle(GetOwner());
  mCursorPositionChangedSignal.Emit(handle, mOldPosition);
  mCursorPositionChanged = false;
}

void TextField::InputFiltered(Toolkit::InputFilter::Property::Type type)
{
  Dali::Toolkit::TextField handle(GetOwner());
  mInputFilteredSignal.Emit(handle, type);
}

void TextField::EmitSelectionChangedSignal()
{
  Dali::Toolkit::TextField handle(GetOwner());
  mSelectionChangedSignal.Emit(handle, mOldSelectionStart, mOldSelectionEnd);
  mSelectionChanged = false;
}

void TextField::EmitSelectionClearedSignal()
{
  Dali::Toolkit::TextField handle(GetOwner());
  mSelectionClearedSignal.Emit(handle);
  mSelectionCleared = false;
}

void TextField::SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd)
{
  if(((oldStart != newStart) || (oldEnd != newEnd)) && !mSelectionChanged)
  {
    if(newStart == newEnd)
    {
      mSelectionCleared = true;
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

void TextField::AddDecoration(Actor& actor, bool needsClipping)
{
  if(actor)
  {
    if(needsClipping)
    {
      mClippingDecorationActors.push_back(actor);
    }
    else
    {
      actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      Self().Add(actor);
      mActiveLayer = actor;
    }
  }
}

void TextField::GetControlBackgroundColor(Vector4& color) const
{
  Property::Value propValue = Self().GetProperty(Toolkit::Control::Property::BACKGROUND);
  Property::Map*  resultMap = propValue.GetMap();

  Property::Value* colorValue = nullptr;
  if(resultMap && (colorValue = resultMap->Find(ColorVisual::Property::MIX_COLOR)))
  {
    colorValue->Get(color);
  }
}

void TextField::OnSceneConnect(Dali::Actor actor)
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

InputMethodContext::CallbackData TextField::OnInputMethodContextEvent(Dali::InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::OnInputMethodContextEvent %p eventName %d\n", mController.Get(), inputMethodContextEvent.eventName);
  return mController->OnInputMethodContextEvent(inputMethodContext, inputMethodContextEvent);
}

void TextField::GetHandleImagePropertyValue(Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType)
{
  if(mDecorator)
  {
    Property::Map map;
    map[PropertyHandler::IMAGE_MAP_FILENAME_STRING] = mDecorator->GetHandleImage(handleType, handleImageType);

    value = map;
  }
}

void TextField::EnableClipping()
{
  if(!mStencil)
  {
    // Creates an extra control to be used as stencil buffer.
    mStencil = Control::New();
    mStencil.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    mStencil.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

    // Creates a background visual. Even if the color is transparent it updates the stencil.
    mStencil.SetProperty(Toolkit::Control::Property::BACKGROUND,
                         Property::Map().Add(Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR).Add(ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT));

    // Enable the clipping property.
    mStencil.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    mStencil.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);

    Self().Add(mStencil);
  }
}

void TextField::OnClipboardTextSelected(ClipboardEventNotifier& clipboard)
{
  mController->PasteClipboardItemEvent();
}

void TextField::KeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_INFO(gTextFieldLogFilter, Debug::Verbose, "TextField::KeyboardStatusChanged %p keyboardShown %d\n", mController.Get(), keyboardShown);

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

void TextField::OnSceneConnection(int depth)
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth(depth);

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  Control::OnSceneConnection(depth);
}

bool TextField::OnTouched(Actor actor, const TouchEvent& touch)
{
  return false;
}

void TextField::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

void TextField::OnIdleSignal()
{
  // Emits the change of input style signals.
  mController->ProcessInputStyleChangedSignals();

  // Set the pointer to null as the callback manager deletes the callback after execute it.
  mIdleCallback = NULL;
}

TextField::TextField()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mIdleCallback(NULL),
  mAlignmentOffset(0.f),
  mRenderingBackend(DEFAULT_RENDERING_BACKEND),
  mExceedPolicy(Dali::Toolkit::TextField::EXCEED_POLICY_CLIP),
  mHasBeenStaged(false),
  mTextChanged(false),
  mCursorPositionChanged(false),
  mSelectionChanged(false),
  mSelectionCleared(false),
  mOldPosition(0u),
  mOldSelectionStart(0u),
  mOldSelectionEnd(0u)
{
}

TextField::~TextField()
{
  UnparentAndReset(mStencil);

  if((NULL != mIdleCallback) && Adaptor::IsAvailable())
  {
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

Vector<Vector2> TextField::GetTextSize(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextSize(startIndex, endIndex);
}

Vector<Vector2> TextField::GetTextPosition(const uint32_t startIndex, const uint32_t endIndex) const
{
  return mController->GetTextPosition(startIndex, endIndex);
}

std::string TextField::AccessibleImpl::GetName()
{
  auto self = Toolkit::TextField::DownCast(Self());
  if(IsHiddenInput(self))
  {
    return {};
  }

  return self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();
}

std::string TextField::AccessibleImpl::GetText(size_t startOffset, size_t endOffset)
{
  if(endOffset <= startOffset)
  {
    return {};
  }

  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();

  if(startOffset > text.size() || endOffset > text.size())
  {
    return {};
  }
  if(IsHiddenInput(self))
  {
    return std::string(endOffset - startOffset, GetSubstituteCharacter(self));
  }
  return text.substr(startOffset, endOffset - startOffset);
}

size_t TextField::AccessibleImpl::GetCharacterCount()
{
  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();

  return text.size();
}

size_t TextField::AccessibleImpl::GetCursorOffset()
{
  auto self = Toolkit::TextField::DownCast(Self());
  return Dali::Toolkit::GetImpl(self).GetTextController()->GetCursorPosition();
}

bool TextField::AccessibleImpl::SetCursorOffset(size_t offset)
{
  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();
  if(offset > text.size())
  {
    return false;
  }

  auto& selfImpl = Dali::Toolkit::GetImpl(self);
  selfImpl.GetTextController()->ResetCursorPosition(offset);
  selfImpl.RequestTextRelayout();

  return true;
}

Dali::Accessibility::Range TextField::AccessibleImpl::GetTextAtOffset(
  size_t offset, Dali::Accessibility::TextBoundary boundary)
{
  auto self  = Toolkit::TextField::DownCast(Self());
  auto range = Dali::Accessibility::Range{};

  if(IsHiddenInput(self))
  {
    // Returning empty object, as there is no possibility to parse the textfield
    // when its content is hidden.
    return range;
  }

  auto text     = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();
  auto textSize = text.size();

  switch(boundary)
  {
    case Dali::Accessibility::TextBoundary::CHARACTER:
    {
      if(offset < textSize)
      {
        range.content     = text[offset];
        range.startOffset = offset;
        range.endOffset   = offset + 1;
      }
      break;
    }
    case Dali::Accessibility::TextBoundary::WORD:
    case Dali::Accessibility::TextBoundary::LINE:
    {
      auto textString = text.c_str();
      auto breaks     = std::vector<char>(textSize, 0);

      if(boundary == Dali::Accessibility::TextBoundary::WORD)
      {
        Accessibility::Accessible::FindWordSeparationsUtf8(reinterpret_cast<const utf8_t*>(textString), textSize, "", breaks.data());
      }
      else
      {
        Accessibility::Accessible::FindLineSeparationsUtf8(reinterpret_cast<const utf8_t*>(textString), textSize, "", breaks.data());
      }

      auto index   = 0u;
      auto counter = 0u;
      while(index < textSize && counter <= offset)
      {
        auto start = index;
        if(breaks[index])
        {
          while(breaks[index])
          {
            index++;
          }
          counter++;
        }
        else
        {
          if(boundary == Dali::Accessibility::TextBoundary::WORD)
          {
            index++;
          }
          if(boundary == Dali::Accessibility::TextBoundary::LINE)
          {
            counter++;
          }
        }

        if((counter > 0) && ((counter - 1) == offset))
        {
          range.content     = text.substr(start, index - start + 1);
          range.startOffset = start;
          range.endOffset   = index + 1;
        }

        if(boundary == Dali::Accessibility::TextBoundary::LINE)
        {
          index++;
        }
      }
      break;
    }
    case Dali::Accessibility::TextBoundary::SENTENCE:
    {
      /* not supported by default */
      break;
    }
    case Dali::Accessibility::TextBoundary::PARAGRAPH:
    {
      /* Paragraph is not supported by libunibreak library */
      break;
    }
    default:
      break;
  }

  return range;
}

Dali::Accessibility::Range TextField::AccessibleImpl::GetRangeOfSelection(size_t selectionIndex)
{
  // Since DALi supports only one selection indexes higher than 0 are ignored
  if(selectionIndex > 0)
  {
    return {};
  }

  auto self       = Toolkit::TextField::DownCast(Self());
  auto controller = Dali::Toolkit::GetImpl(self).GetTextController();
  auto indices    = controller->GetSelectionIndexes();

  auto startOffset = static_cast<size_t>(indices.first);
  auto endOffset   = static_cast<size_t>(indices.second);

  if(IsHiddenInput(self))
  {
    return {startOffset, endOffset, std::string(endOffset - startOffset, GetSubstituteCharacter(self))};
  }

  std::string value{};
  controller->RetrieveSelection(value);
  return {startOffset, endOffset, value};
}

bool TextField::AccessibleImpl::RemoveSelection(size_t selectionIndex)
{
  // Since DALi supports only one selection indexes higher than 0 are ignored
  if(selectionIndex > 0)
  {
    return false;
  }

  auto self = Toolkit::TextField::DownCast(Self());
  Dali::Toolkit::GetImpl(self).GetTextController()->SetSelection(0, 0);
  return true;
}

bool TextField::AccessibleImpl::SetRangeOfSelection(size_t selectionIndex, size_t startOffset, size_t endOffset)
{
  // Since DALi supports only one selection indexes higher than 0 are ignored
  if(selectionIndex > 0)
  {
    return false;
  }

  auto self = Toolkit::TextField::DownCast(Self());
  Dali::Toolkit::GetImpl(self).GetTextController()->SetSelection(startOffset, endOffset);
  return true;
}

bool TextField::AccessibleImpl::CopyText(size_t startPosition, size_t endPosition)
{
  if(endPosition <= startPosition)
  {
    return false;
  }

  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();
  Dali::Toolkit::GetImpl(self).GetTextController()->CopyStringToClipboard(text.substr(startPosition, endPosition - startPosition));

  return true;
}

bool TextField::AccessibleImpl::CutText(size_t startPosition, size_t endPosition)
{
  if(endPosition <= startPosition)
  {
    return false;
  }

  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();
  Dali::Toolkit::GetImpl(self).GetTextController()->CopyStringToClipboard(text.substr(startPosition, endPosition - startPosition));

  self.SetProperty(Toolkit::TextField::Property::TEXT, text.substr(0, startPosition) + text.substr(endPosition));

  return true;
}

bool TextField::AccessibleImpl::DeleteText(size_t startPosition, size_t endPosition)
{
  if(endPosition <= startPosition)
  {
    return false;
  }

  auto self = Toolkit::TextField::DownCast(Self());
  auto text = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();

  self.SetProperty(Toolkit::TextField::Property::TEXT, text.substr(0, startPosition) + text.substr(endPosition));

  return true;
}

Dali::Accessibility::States TextField::AccessibleImpl::CalculateStates()
{
  using namespace Dali::Accessibility;

  auto states = DevelControl::AccessibleImpl::CalculateStates();

  states[State::EDITABLE]  = true;
  states[State::FOCUSABLE] = true;

  Toolkit::Control focusControl = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
  if(mSelf == focusControl)
  {
    states[State::FOCUSED] = true;
  }

  return states;
}

bool TextField::AccessibleImpl::InsertText(size_t startPosition, std::string text)
{
  auto self         = Toolkit::TextField::DownCast(Self());
  auto insertedText = self.GetProperty(Toolkit::TextField::Property::TEXT).Get<std::string>();

  insertedText.insert(startPosition, text);

  self.SetProperty(Toolkit::TextField::Property::TEXT, std::move(insertedText));

  return true;
}

bool TextField::AccessibleImpl::SetTextContents(std::string newContents)
{
  auto self = Toolkit::TextField::DownCast(Self());
  self.SetProperty(Toolkit::TextField::Property::TEXT, std::move(newContents));
  return true;
}

int32_t TextField::AccessibleImpl::GetLinkCount() const
{
  auto self = Toolkit::TextField::DownCast(Self());
  return Dali::Toolkit::GetImpl(self).mAnchorActors.size();
}

Accessibility::Hyperlink* TextField::AccessibleImpl::GetLink(int32_t linkIndex) const
{
  if(linkIndex < 0 || linkIndex >= GetLinkCount())
  {
    return nullptr;
  }
  auto self        = Toolkit::TextField::DownCast(Self());
  auto anchorActor = Dali::Toolkit::GetImpl(self).mAnchorActors[linkIndex];
  return dynamic_cast<Accessibility::Hyperlink*>(Dali::Accessibility::Accessible::Get(anchorActor));
}

int32_t TextField::AccessibleImpl::GetLinkIndex(int32_t characterOffset) const
{
  auto self       = Toolkit::TextField::DownCast(Self());
  auto controller = Dali::Toolkit::GetImpl(self).GetTextController();
  return controller->GetAnchorIndex(static_cast<size_t>(characterOffset));
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
