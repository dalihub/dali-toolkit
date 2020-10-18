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

// CLASS HEADER
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>

// EXTERNAL INCLUDES
#include <cstring>
#include <limits>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

using namespace Dali::Toolkit::Text;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace // unnamed namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_CONTROLS");
#endif

const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::DevelText::DEFAULT_RENDERING_BACKEND;
const float DEFAULT_SCROLL_SPEED = 1200.f; ///< The default scroll speed for the text editor in pixels/second.
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

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextEditor, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "text",                                 STRING,    TEXT                                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "textColor",                            VECTOR4,   TEXT_COLOR                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "fontFamily",                           STRING,    FONT_FAMILY                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "fontStyle",                            MAP,       FONT_STYLE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "pointSize",                            FLOAT,     POINT_SIZE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "horizontalAlignment",                  STRING,    HORIZONTAL_ALIGNMENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "scrollThreshold",                      FLOAT,     SCROLL_THRESHOLD                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "scrollSpeed",                          FLOAT,     SCROLL_SPEED                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "primaryCursorColor",                   VECTOR4,   PRIMARY_CURSOR_COLOR                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "secondaryCursorColor",                 VECTOR4,   SECONDARY_CURSOR_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableCursorBlink",                    BOOLEAN,   ENABLE_CURSOR_BLINK                  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "cursorBlinkInterval",                  FLOAT,     CURSOR_BLINK_INTERVAL                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "cursorBlinkDuration",                  FLOAT,     CURSOR_BLINK_DURATION                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "cursorWidth",                          INTEGER,   CURSOR_WIDTH                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "grabHandleImage",                      STRING,    GRAB_HANDLE_IMAGE                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "grabHandlePressedImage",               STRING,    GRAB_HANDLE_PRESSED_IMAGE            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandleImageLeft",             MAP,       SELECTION_HANDLE_IMAGE_LEFT          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandleImageRight",            MAP,       SELECTION_HANDLE_IMAGE_RIGHT         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandlePressedImageLeft",      MAP,       SELECTION_HANDLE_PRESSED_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandlePressedImageRight",     MAP,       SELECTION_HANDLE_PRESSED_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandleMarkerImageLeft",       MAP,       SELECTION_HANDLE_MARKER_IMAGE_LEFT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHandleMarkerImageRight",      MAP,       SELECTION_HANDLE_MARKER_IMAGE_RIGHT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectionHighlightColor",              VECTOR4,   SELECTION_HIGHLIGHT_COLOR            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "decorationBoundingBox",                RECTANGLE, DECORATION_BOUNDING_BOX              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableMarkup",                         BOOLEAN,   ENABLE_MARKUP                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputColor",                           VECTOR4,   INPUT_COLOR                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputFontFamily",                      STRING,    INPUT_FONT_FAMILY                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputFontStyle",                       MAP,       INPUT_FONT_STYLE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputPointSize",                       FLOAT,     INPUT_POINT_SIZE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "lineSpacing",                          FLOAT,     LINE_SPACING                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputLineSpacing",                     FLOAT,     INPUT_LINE_SPACING                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "underline",                            MAP,       UNDERLINE                            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputUnderline",                       MAP,       INPUT_UNDERLINE                      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "shadow",                               MAP,       SHADOW                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputShadow",                          MAP,       INPUT_SHADOW                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "emboss",                               MAP,       EMBOSS                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputEmboss",                          MAP,       INPUT_EMBOSS                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "outline",                              MAP,       OUTLINE                              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputOutline",                         MAP,       INPUT_OUTLINE                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "smoothScroll",                         BOOLEAN,   SMOOTH_SCROLL                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "smoothScrollDuration",                 FLOAT,     SMOOTH_SCROLL_DURATION               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableScrollBar",                      BOOLEAN,   ENABLE_SCROLL_BAR                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "scrollBarShowDuration",                FLOAT,     SCROLL_BAR_SHOW_DURATION             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "scrollBarFadeDuration",                FLOAT,     SCROLL_BAR_FADE_DURATION             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "pixelSize",                            FLOAT,     PIXEL_SIZE                           )
DALI_PROPERTY_REGISTRATION_READ_ONLY( Toolkit, TextEditor, "lineCount",                  INTEGER,   LINE_COUNT                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableSelection",                      BOOLEAN,   ENABLE_SELECTION                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "placeholder",                          MAP,       PLACEHOLDER                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "lineWrapMode",                         INTEGER,   LINE_WRAP_MODE                       )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "placeholderText",                STRING,    PLACEHOLDER_TEXT                     )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "placeholderTextColor",           VECTOR4,   PLACEHOLDER_TEXT_COLOR               )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableShiftSelection",           BOOLEAN,   ENABLE_SHIFT_SELECTION               )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableGrabHandle",               BOOLEAN,   ENABLE_GRAB_HANDLE                   )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "matchSystemLanguageDirection",   BOOLEAN,   MATCH_SYSTEM_LANGUAGE_DIRECTION      )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "renderingBackend",               INTEGER,   RENDERING_BACKEND                    )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "maxLength",                      INTEGER,   MAX_LENGTH                           )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectedTextStart",              INTEGER,   SELECTED_TEXT_START                  )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "selectedTextEnd",                INTEGER,   SELECTED_TEXT_END                    )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "horizontalScrollPosition",       FLOAT,     HORIZONTAL_SCROLL_POSITION           )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "verticalScrollPosition",         INTEGER,   VERTICAL_SCROLL_POSITION             )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextEditor, "enableEditing",                  BOOLEAN,   ENABLE_EDITING                       )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY( Toolkit, TextEditor, "selectedText",         STRING,    SELECTED_TEXT                        )

DALI_SIGNAL_REGISTRATION( Toolkit, TextEditor, "textChanged",        SIGNAL_TEXT_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextEditor, "inputStyleChanged",  SIGNAL_INPUT_STYLE_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextEditor, "maxLengthReached",   SIGNAL_MAX_LENGTH_REACHED )

DALI_TYPE_REGISTRATION_END()

const char * const IMAGE_MAP_FILENAME_STRING = "filename";

/// Retrieves a filename from a value that is a Property::Map
std::string GetImageFileNameFromPropertyValue( const Property::Value& value )
{
  std::string filename;
  const Property::Map* map = value.GetMap();
  if( map )
  {
    const Property::Value* filenameValue = map->Find( IMAGE_MAP_FILENAME_STRING );
    if( filenameValue )
    {
      filenameValue->Get( filename );
    }
  }
  return filename;
}

} // namespace

Toolkit::TextEditor TextEditor::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextEditor > impl = new TextEditor();

  // Pass ownership to CustomActor handle
  Toolkit::TextEditor handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextEditor::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextEditor textEditor = Toolkit::TextEditor::DownCast( Dali::BaseHandle( object ) );

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor SetProperty\n");


  if( textEditor )
  {
    TextEditor& impl( GetImpl( textEditor ) );
    DALI_ASSERT_DEBUG( impl.mController && "No text contoller" );
    DALI_ASSERT_DEBUG( impl.mDecorator && "No text decorator" );

    switch( index )
    {
      case Toolkit::DevelTextEditor::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p RENDERING_BACKEND %d\n", impl.mController.Get(), backend );

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::TEXT:
      {
        const std::string& text = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p TEXT %s\n", impl.mController.Get(), text.c_str() );

        impl.mController->SetText( text );
        break;
      }
      case Toolkit::TextEditor::Property::TEXT_COLOR:
      {
        const Vector4& textColor = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

        if( impl.mController->GetDefaultColor() != textColor )
        {
          impl.mController->SetDefaultColor( textColor );
          impl.mController->SetInputColor( textColor );
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::FONT_FAMILY:
      {
        const std::string& fontFamily = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
        impl.mController->SetDefaultFontFamily( fontFamily );
        break;
      }
      case Toolkit::TextEditor::Property::FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::POINT_SIZE:
      {
        const float pointSize = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p POINT_SIZE %f\n", impl.mController.Get(), pointSize );

        if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE ), pointSize ) )
        {
          impl.mController->SetDefaultFontSize( pointSize, Text::Controller::POINT_SIZE );
        }
        break;
      }
      case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
      {
        Text::HorizontalAlignment::Type alignment( static_cast< Text::HorizontalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
        if( Text::GetHorizontalAlignmentEnumeration( value, alignment ) )
        {
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p HORIZONTAL_ALIGNMENT %d\n", impl.mController.Get(), alignment );
          impl.mController->SetHorizontalAlignment( alignment );
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
      {
        const float threshold = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p SCROLL_THRESHOLD %f\n", impl.mController.Get(), threshold );

        impl.mDecorator->SetScrollThreshold( threshold );
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_SPEED:
      {
        const float speed = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p SCROLL_SPEED %f\n", impl.mController.Get(), speed );

        impl.mDecorator->SetScrollSpeed( speed );
        break;
      }
      case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
      {
        const Vector4& color = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p PRIMARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

        impl.mDecorator->SetCursorColor( PRIMARY_CURSOR, color );
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
      {
        const Vector4& color = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SECONDARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

        impl.mDecorator->SetCursorColor( SECONDARY_CURSOR, color );
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
      {
        const bool enable = value.Get< bool >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p ENABLE_CURSOR_BLINK %d\n", impl.mController.Get(), enable );

        impl.mController->SetEnableCursorBlink( enable );
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
      {
        const float interval = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_INTERVAL %f\n", impl.mController.Get(), interval );

        impl.mDecorator->SetCursorBlinkInterval( interval );
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
      {
        const float duration = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_DURATION %f\n", impl.mController.Get(), duration );

        impl.mDecorator->SetCursorBlinkDuration( duration );
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_WIDTH:
      {
        const int width = value.Get< int >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_WIDTH %d\n", impl.mController.Get(), width );

        impl.mDecorator->SetCursorWidth( width );
        impl.mController->GetLayoutEngine().SetCursorWidth( width );
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
      {
        const std::string imageFileName = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str() );

        if( imageFileName.size() )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED, imageFileName );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        const std::string imageFileName = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str() );

        if( imageFileName.size() )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED, imageFileName );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        const Vector4 color = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SELECTION_HIGHLIGHT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

        impl.mDecorator->SetHighlightColor( color );
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
      {
        const Rect<int>& box = value.Get< Rect<int> >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p DECORATION_BOUNDING_BOX %d,%d %dx%d\n", impl.mController.Get(), box.x, box.y, box.width, box.height );

        impl.mDecorator->SetBoundingBox( box );
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_MARKUP:
      {
        const bool enableMarkup = value.Get<bool>();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_MARKUP %d\n", impl.mController.Get(), enableMarkup );

        impl.mController->SetMarkupProcessorEnabled( enableMarkup );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_COLOR:
      {
        const Vector4& inputColor = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), inputColor.r, inputColor.g, inputColor.b, inputColor.a );

        impl.mController->SetInputColor( inputColor );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
      {
        const std::string& fontFamily = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
        impl.mController->SetInputFontFamily( fontFamily );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
      {
        const float pointSize = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_POINT_SIZE %f\n", impl.mController.Get(), pointSize );
        impl.mController->SetInputFontPointSize( pointSize );
        break;
      }
      case Toolkit::TextEditor::Property::LINE_SPACING:
      {
        // The line spacing isn't supported by the TextEditor. Since it's supported
        // by the TextLabel for now it must be ignored. The property is being shadowed
        // locally so its value isn't affected.
        const float lineSpacing = value.Get<float>();
        impl.mLineSpacing = lineSpacing;
        // set it to 0.0 due to missing implementation
        impl.mController->SetDefaultLineSpacing( 0.0f );
        impl.mRenderer.Reset();
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
      {
        const float lineSpacing = value.Get<float>();
        impl.mController->SetInputLineSpacing( lineSpacing );
        impl.mRenderer.Reset();
        break;
      }
      case Toolkit::TextEditor::Property::UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SMOOTH_SCROLL:
      {
        const bool enable = value.Get< bool >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor SMOOTH_SCROLL %d\n", enable );

        impl.mScrollAnimationEnabled = enable;
        break;
      }
      case Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION:
      {
        const float duration = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor SMOOTH_SCROLL_DURATION %f\n", duration );

        impl.mScrollAnimationDuration = duration;
        if ( impl.mTextVerticalScroller )
        {
          impl.mTextVerticalScroller->SetDuration( duration );
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR:
      {
        const bool enable = value.Get< bool >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor SHOW_SCROLL_BAR %d\n", enable );

        impl.mScrollBarEnabled = enable;
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION:
      {
        const float duration = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor SCROLL_BAR_SHOW_DURATION %f\n", duration );

        impl.mAnimationPeriod.delaySeconds = duration;
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION:
      {
        const float duration = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor SCROLL_BAR_FADE_DURATION %f\n", duration );

        impl.mAnimationPeriod.durationSeconds = duration;
        break;
      }
      case Toolkit::TextEditor::Property::PIXEL_SIZE:
      {
        const float pixelSize = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize );

        if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE ), pixelSize ) )
        {
          impl.mController->SetDefaultFontSize( pixelSize, Text::Controller::PIXEL_SIZE );
        }
        break;
      }
      case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT:
      {
        const std::string& text = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor::OnPropertySet %p PLACEHOLDER_TEXT %s\n", impl.mController.Get(), text.c_str() );

        impl.mController->SetPlaceholderText( Controller::PLACEHOLDER_TYPE_INACTIVE, text );
        break;
      }
      case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR:
      {
        const Vector4& textColor = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p PLACEHOLDER_TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

        if( impl.mController->GetPlaceholderTextColor() != textColor )
        {
          impl.mController->SetPlaceholderTextColor( textColor );
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_SELECTION:
      {
        const bool enableSelection = value.Get< bool >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_SELECTION %d\n", impl.mController.Get(), enableSelection );
        impl.mController->SetSelectionEnabled( enableSelection );
        break;
      }
      case Toolkit::TextEditor::Property::PLACEHOLDER:
      {
        const Property::Map* map = value.GetMap();
        if( map )
        {
          impl.mController->SetPlaceholderProperty( *map );
        }
        break;
      }
      case Toolkit::TextEditor::Property::LINE_WRAP_MODE:
      {
        Text::LineWrap::Mode lineWrapMode( static_cast< Text::LineWrap::Mode >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
        if( GetLineWrapModeEnumeration( value, lineWrapMode ) )
        {
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p LineWrap::MODE %d\n", impl.mController.Get(), lineWrapMode );
          impl.mController->SetLineWrapMode( lineWrapMode );
        }
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_SHIFT_SELECTION:
      {
        const bool shiftSelection = value.Get<bool>();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_SHIFT_SELECTION %d\n", impl.mController.Get(), shiftSelection );

        impl.mController->SetShiftSelectionEnabled( shiftSelection );
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE:
      {
        const bool grabHandleEnabled = value.Get<bool>();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_GRAB_HANDLE %d\n", impl.mController.Get(), grabHandleEnabled );

        impl.mController->SetGrabHandleEnabled( grabHandleEnabled );
        break;
      }
      case Toolkit::DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        impl.mController->SetMatchSystemLanguageDirection(value.Get< bool >());
        break;
      }
      case Toolkit::DevelTextEditor::Property::MAX_LENGTH:
      {
        const int max = value.Get< int >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p MAX_LENGTH %d\n", impl.mController.Get(), max );

        impl.mController->SetMaximumNumberOfCharacters( max );
        break;
      }
      case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_START:
      {
        uint32_t start = static_cast<uint32_t>(value.Get< int >());
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SELECTED_TEXT_START %d\n", impl.mController.Get(), start );
        impl.SetTextSelectionRange( &start, nullptr );
        break;
      }
      case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_END:
      {
        uint32_t end = static_cast<uint32_t>(value.Get< int >());
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SELECTED_TEXT_END %d\n", impl.mController.Get(), end );
        impl.SetTextSelectionRange( nullptr, &end );
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_EDITING:
      {
        const bool editable = value.Get< bool >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_EDITING %d\n", impl.mController.Get(), editable );
        impl.SetEditable( editable );
        break;
      }
      case Toolkit::DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION:
      {
        float horizontalScroll = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p HORIZONTAL_SCROLL_POSITION %d\n", impl.mController.Get(), horizontalScroll );
        if (horizontalScroll >= 0.0f)
        {
          impl.ScrollBy( Vector2(horizontalScroll - impl.GetHorizontalScrollPosition(), 0 ));
        }
        break;
      }
      case Toolkit::DevelTextEditor::Property::VERTICAL_SCROLL_POSITION:
      {
        float verticalScroll = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p VERTICAL_SCROLL_POSITION %d\n", impl.mController.Get(), verticalScroll );
        if (verticalScroll >= 0.0f)
        {
          impl.ScrollBy( Vector2(0, verticalScroll - impl.GetVerticalScrollPosition() ));
        }
        break;
      }
    } // switch
  } // texteditor
}

Property::Value TextEditor::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextEditor textEditor = Toolkit::TextEditor::DownCast( Dali::BaseHandle( object ) );

  if( textEditor )
  {
    TextEditor& impl( GetImpl( textEditor ) );
    DALI_ASSERT_DEBUG( impl.mController && "No text contoller" );
    DALI_ASSERT_DEBUG( impl.mDecorator && "No text decorator" );

    switch( index )
    {
      case Toolkit::DevelTextEditor::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextEditor::Property::TEXT:
      {
        std::string text;
        impl.mController->GetText( text );
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p returning text: %s\n", impl.mController.Get(), text.c_str() );
        value = text;
        break;
      }
      case Toolkit::TextEditor::Property::TEXT_COLOR:
      {
        value = impl.mController->GetDefaultColor();
        break;
      }
      case Toolkit::TextEditor::Property::FONT_FAMILY:
      {
        value = impl.mController->GetDefaultFontFamily();
        break;
      }
      case Toolkit::TextEditor::Property::FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::POINT_SIZE:
      {
        value = impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE );
        break;
      }
      case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
      {
        const char* name = GetHorizontalAlignmentString( impl.mController->GetHorizontalAlignment() );
        if( name )
        {
          value = std::string( name );
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
      {
        value = impl.mDecorator->GetScrollThreshold();
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_SPEED:
      {
        value = impl.mDecorator->GetScrollSpeed();
        break;
      }
      case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
      {
        value = impl.mDecorator->GetColor( PRIMARY_CURSOR );
        break;
      }
      case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
      {
        value = impl.mDecorator->GetColor( SECONDARY_CURSOR );
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
      {
        value = impl.mController->GetEnableCursorBlink();
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
      {
        value = impl.mDecorator->GetCursorBlinkInterval();
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
      {
        value = impl.mDecorator->GetCursorBlinkDuration();
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_WIDTH:
      {
        value = impl.mDecorator->GetCursorWidth();
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
      {
        value = impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        value = impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED ) ;
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        value = impl.mDecorator->GetHighlightColor();
        break;
      }
      case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
      {
        Rect<int> boundingBox;
        impl.mDecorator->GetBoundingBox( boundingBox );
        value = boundingBox;
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_MARKUP:
      {
        value = impl.mController->IsMarkupProcessorEnabled();
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_COLOR:
      {
        value = impl.mController->GetInputColor();
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
      {
        value = impl.mController->GetInputFontFamily();
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
      {
        value = impl.mController->GetInputFontPointSize();
        break;
      }
      case Toolkit::TextEditor::Property::LINE_SPACING:
      {
        // LINE_SPACING isn't implemented for the TextEditor. Returning
        // only shadowed value, not the real one.
        value = impl.mLineSpacing;
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
      {
        value = impl.mController->GetInputLineSpacing();
        break;
      }
      case Toolkit::TextEditor::Property::UNDERLINE:
      {
        GetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_UNDERLINE:
      {
        GetUnderlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::SHADOW:
      {
        GetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_SHADOW:
      {
        GetShadowProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::EMBOSS:
      {
        GetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_EMBOSS:
      {
        GetEmbossProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::OUTLINE:
      {
        GetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_OUTLINE:
      {
        GetOutlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::SMOOTH_SCROLL:
      {
        value = impl.mScrollAnimationEnabled;
        break;
      }
      case Toolkit::TextEditor::Property::SMOOTH_SCROLL_DURATION:
      {
        value = impl.mScrollAnimationDuration;
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_SCROLL_BAR:
      {
        value = impl.mScrollBarEnabled;
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_BAR_SHOW_DURATION:
      {
        value = impl.mAnimationPeriod.delaySeconds;
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_BAR_FADE_DURATION:
      {
        value = impl.mAnimationPeriod.durationSeconds;
        break;
      }
      case Toolkit::TextEditor::Property::PIXEL_SIZE:
      {
        value = impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE );
        break;
      }
      case Toolkit::TextEditor::Property::LINE_COUNT:
      {
        float width = textEditor.GetProperty( Actor::Property::SIZE_WIDTH ).Get<float>();
        value = impl.mController->GetLineCount( width );
        break;
      }
      case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT:
      {
        std::string text;
        impl.mController->GetPlaceholderText( Controller::PLACEHOLDER_TYPE_INACTIVE, text );
        value = text;
        break;
      }
      case Toolkit::DevelTextEditor::Property::PLACEHOLDER_TEXT_COLOR:
      {
        value = impl.mController->GetPlaceholderTextColor();
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_SELECTION:
      {
        value = impl.mController->IsSelectionEnabled();
        break;
      }
      case Toolkit::TextEditor::Property::PLACEHOLDER:
      {
        Property::Map map;
        impl.mController->GetPlaceholderProperty( map );
        value = map;
        break;
      }
      case Toolkit::TextEditor::Property::LINE_WRAP_MODE:
      {
        value = impl.mController->GetLineWrapMode();
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_SHIFT_SELECTION:
      {
        value = impl.mController->IsShiftSelectionEnabled();
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_GRAB_HANDLE:
      {
        value = impl.mController->IsGrabHandleEnabled();
        break;
      }
      case Toolkit::DevelTextEditor::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        value = impl.mController->IsMatchSystemLanguageDirection();
        break;
      }
      case Toolkit::DevelTextEditor::Property::MAX_LENGTH:
      {
        value = impl.mController->GetMaximumNumberOfCharacters();
        break;
      }
      case Toolkit::DevelTextEditor::Property::SELECTED_TEXT:
      {
        value = impl.mController->GetSelectedText( );
        break;
      }
      case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_START:
      {
        Uint32Pair range = impl.GetTextSelectionRange();
        value = static_cast<int>(range.first);
        break;
      }
      case Toolkit::DevelTextEditor::Property::SELECTED_TEXT_END:
      {
        Uint32Pair range = impl.GetTextSelectionRange();
        value = static_cast<int>(range.second);
        break;
      }
      case Toolkit::DevelTextEditor::Property::ENABLE_EDITING:
      {
        value = impl.IsEditable();
        break;
      }
      case Toolkit::DevelTextEditor::Property::HORIZONTAL_SCROLL_POSITION:
      {
        value = impl.GetHorizontalScrollPosition();
        break;
      }
      case Toolkit::DevelTextEditor::Property::VERTICAL_SCROLL_POSITION:
      {
        value = impl.GetVerticalScrollPosition();
        break;
      }
    } //switch
  }

  return value;
}

void TextEditor::SelectWholeText()
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->SelectWholeText();
    SetKeyInputFocus();
  }
}

void TextEditor::SelectNone()
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->SelectNone();
  }
}

void TextEditor::ScrollBy(Vector2 scroll)
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->ScrollBy(scroll);
  }
}

float TextEditor::GetHorizontalScrollPosition()
{
  if( mController && mController->IsShowingRealText() )
  {
    return mController->GetHorizontalScrollPosition();
  }
  return 0;
}

float TextEditor::GetVerticalScrollPosition()
{
  if( mController && mController->IsShowingRealText() )
  {
    return mController->GetVerticalScrollPosition();
  }
  return 0;
}

string TextEditor::GetSelectedText() const
{
  string selectedText = "";
  if( mController && mController->IsShowingRealText() )
  {
    selectedText = mController->GetSelectedText( );
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

bool TextEditor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::TextEditor editor = Toolkit::TextEditor::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TEXT_CHANGED ) )
  {
    editor.TextChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_INPUT_STYLE_CHANGED ) )
  {
    editor.InputStyleChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MAX_LENGTH_REACHED ) )
  {
    if( editor )
    {
      Internal::TextEditor& editorImpl( GetImpl( editor ) );
      editorImpl.MaxLengthReachedSignal().Connect( tracker, functor );
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

void TextEditor::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( this, this, this);

  mDecorator = Text::Decorator::New( *mController,
                                     *mController );

  mInputMethodContext = InputMethodContext::New( self );

  mController->GetLayoutEngine().SetLayout( Layout::Engine::MULTI_LINE_BOX );

  // Enables the text input.
  mController->EnableTextInput( mDecorator, mInputMethodContext );

  // Enables the vertical scrolling after the text input has been enabled.
  mController->SetVerticalScrollEnabled( true );

  // Disables the horizontal scrolling.
  mController->SetHorizontalScrollEnabled( false );

  // Sets the maximum number of characters.
  mController->SetMaximumNumberOfCharacters( std::numeric_limits<Length>::max() );

  // Enable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled( true );

  mController->SetNoTextDoubleTapAction( Controller::NoTextTap::HIGHLIGHT );
  mController->SetNoTextLongPressAction( Controller::NoTextTap::HIGHLIGHT );

  // Sets layoutDirection value
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>( stage.GetRootLayer().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  mController->SetLayoutDirection( layoutDirection );

  // Forward input events to controller
  EnableGestureDetection( static_cast<GestureType::Value>( GestureType::TAP | GestureType::PAN | GestureType::LONG_PRESS ) );
  GetTapGestureDetector().SetMaximumTapsRequired( 2 );

  self.TouchedSignal().Connect( this, &TextEditor::OnTouched );

  // Set BoundingBox to stage size if not already set.
  Rect<int> boundingBox;
  mDecorator->GetBoundingBox( boundingBox );

  if( boundingBox.IsEmpty() )
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox( Rect<int>( 0.0f, 0.0f, stageSize.width, stageSize.height ) );
  }

  // Whether to flip the selection handles as soon as they cross.
  mDecorator->FlipSelectionHandlesOnCrossEnabled( true );

  // Set the default scroll speed.
  mDecorator->SetScrollSpeed( DEFAULT_SCROLL_SPEED );

  // Fill-parent area by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
  self.OnSceneSignal().Connect( this, &TextEditor::OnSceneConnect );

  DevelControl::SetInputMethodContext( *this, mInputMethodContext );

  // Creates an extra control to be used as stencil buffer.
  mStencil = Control::New();
  mStencil.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  mStencil.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );

  // Creates a background visual. Even if the color is transparent it updates the stencil.
  mStencil.SetProperty( Toolkit::Control::Property::BACKGROUND,
                        Property::Map().Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::COLOR ).
                        Add( ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT ) );

  // Enable the clipping property.
  mStencil.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX );
  mStencil.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  self.Add( mStencil );
}

void TextEditor::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnStyleChange\n");

  switch ( change )
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnStyleChange DEFAULT_FONT_CHANGE\n");
      const std::string& newFont = GetImpl( styleManager ).GetDefaultFontFamily();
      // Property system did not set the font so should update it.
      mController->UpdateAfterFontChange( newFont );
      RelayoutRequest();
      break;
    }

    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
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
  Control::OnStyleChange( styleManager, change );
}

Vector3 TextEditor::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += ( padding.start + padding.end );
  naturalSize.height += ( padding.top + padding.bottom );

  return naturalSize;
}

float TextEditor::GetHeightForWidth( float width )
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  return mController->GetHeightForWidth( width ) + padding.top + padding.bottom;
}

void TextEditor::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor OnRelayout\n");

  Actor self = Self();

  Extents padding;
  padding = self.GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  Vector2 contentSize( size.x - ( padding.start + padding.end ), size.y - ( padding.top + padding.bottom ) );

  // Support Right-To-Left of padding
  Dali::LayoutDirection::Type layoutDirection;
  if( mController->IsMatchSystemLanguageDirection() )
  {
    layoutDirection = static_cast<Dali::LayoutDirection::Type>( DevelWindow::Get( self ).GetRootLayer().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  }
  else
  {
    layoutDirection = static_cast<Dali::LayoutDirection::Type>( self.GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  }
  if( Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection )
  {
    std::swap( padding.start, padding.end );
  }

  if( mStencil )
  {
    mStencil.SetProperty( Actor::Property::POSITION, Vector2( padding.start, padding.top ));
  }
  if( mActiveLayer )
  {
    mActiveLayer.SetProperty( Actor::Property::POSITION, Vector2( padding.start, padding.top ));
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( contentSize, layoutDirection );

  if( ( Text::Controller::NONE_UPDATED != updateTextType ) ||
      !mRenderer )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnRelayout %p Displaying new contents\n", mController.Get() );

    if( mDecorator &&
        ( Text::Controller::NONE_UPDATED != ( Text::Controller::DECORATOR_UPDATED & updateTextType ) ) )
    {
      mDecorator->Relayout( size );
    }

    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

    RenderText( updateTextType );

  }

  // The text-editor emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-editor adds an idle callback to the adaptor to emit the signals after the size negotiation.
  if( !mController->IsInputStyleChangedSignalsQueueEmpty() )
  {
    if( Adaptor::IsAvailable() )
    {
      Adaptor& adaptor = Adaptor::Get();

      if( NULL == mIdleCallback )
      {
        // @note: The callback manager takes the ownership of the callback object.
        mIdleCallback = MakeCallback( this, &TextEditor::OnIdleSignal );
        adaptor.AddIdle( mIdleCallback, false );
      }
    }
  }
}

void TextEditor::RenderText( Text::Controller::UpdateTextType updateTextType )
{
  Actor renderableActor;

  if( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) )
  {
    if( mRenderer )
    {
      Dali::Toolkit::TextEditor handle = Dali::Toolkit::TextEditor( GetOwner() );

      renderableActor = mRenderer->Render( mController->GetView(),
                                           handle,
                                           Property::INVALID_INDEX, // Animatable property not supported
                                           mAlignmentOffset,
                                           DepthIndex::CONTENT );
    }

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mRenderableActor );
      mRenderableActor = renderableActor;
    }
  }

  if( mRenderableActor )
  {
    // Make sure the actors are parented correctly with/without clipping
    Actor self = mStencil ? mStencil : Self();

    for( std::vector<Actor>::iterator it = mClippingDecorationActors.begin(),
           endIt = mClippingDecorationActors.end();
         it != endIt;
         ++it )
    {
      self.Add( *it );
      it->LowerToBottom();
    }
    mClippingDecorationActors.clear();

    self.Add( mRenderableActor );

    ApplyScrollPosition();
  }
  UpdateScrollBar();
}

void TextEditor::OnKeyInputFocusGained()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnKeyInputFocusGained %p\n", mController.Get() );
  if ( mInputMethodContext  && IsEditable() )
  {
    mInputMethodContext.StatusChangedSignal().Connect( this, &TextEditor::KeyboardStatusChanged );

    mInputMethodContext.EventReceivedSignal().Connect( this, &TextEditor::OnInputMethodContextEvent );

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the InputMethodContext is deactivated. Thus when window gain focus again, the InputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost( true );
  }
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );

  if ( notifier )
  {
    notifier.ContentSelectedSignal().Connect( this, &TextEditor::OnClipboardTextSelected );
  }

  mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event

  EmitKeyInputFocusSignal( true ); // Calls back into the Control hence done last.
}

void TextEditor::OnKeyInputFocusLost()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor:OnKeyInputFocusLost %p\n", mController.Get() );
  if ( mInputMethodContext )
  {
    mInputMethodContext.StatusChangedSignal().Disconnect( this, &TextEditor::KeyboardStatusChanged );

    // The text editing is finished. Therefore the InputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.EventReceivedSignal().Disconnect( this, &TextEditor::OnInputMethodContextEvent );
  }
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );

  if ( notifier )
  {
    notifier.ContentSelectedSignal().Disconnect( this, &TextEditor::OnClipboardTextSelected );
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal( false ); // Calls back into the Control hence done last.
}

void TextEditor::OnTap( const TapGesture& gesture )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnTap %p\n", mController.Get() );
  if ( mInputMethodContext && IsEditable() )
  {
    mInputMethodContext.Activate();
  }
  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->TapEvent( gesture.GetNumberOfTaps(), localPoint.x - padding.start, localPoint.y - padding.top );

  SetKeyInputFocus();
}

void TextEditor::OnPan( const PanGesture& gesture )
{
  mController->PanEvent( gesture.GetState(), gesture.GetDisplacement() );
}

void TextEditor::OnLongPress( const LongPressGesture& gesture )
{
  if ( mInputMethodContext && IsEditable() )
  {
    mInputMethodContext.Activate();
  }
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  const Vector2& localPoint = gesture.GetLocalPoint();
  mController->LongPressEvent( gesture.GetState(), localPoint.x - padding.start, localPoint.y - padding.top );

  SetKeyInputFocus();
}

bool TextEditor::OnKeyEvent( const KeyEvent& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnKeyEvent %p keyCode %d\n", mController.Get(), event.GetKeyCode() );

  if( Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape() )
  {
    // Make sure ClearKeyInputFocus when only key is up
    if( event.GetState() == KeyEvent::UP )
    {
      ClearKeyInputFocus();
    }

    return true;
  }

  return mController->KeyEvent( event );
}

void TextEditor::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextEditor::TextChanged()
{
  Dali::Toolkit::TextEditor handle( GetOwner() );
  mTextChangedSignal.Emit( handle );
}

void TextEditor::MaxLengthReached()
{
  Dali::Toolkit::TextEditor handle( GetOwner() );
  mMaxLengthReachedSignal.Emit( handle );
}

void TextEditor::InputStyleChanged( Text::InputStyle::Mask inputStyleMask )
{
  Dali::Toolkit::TextEditor handle( GetOwner() );

  Toolkit::TextEditor::InputStyle::Mask editorInputStyleMask = Toolkit::TextEditor::InputStyle::NONE;

  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_COLOR ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::COLOR );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_FAMILY ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_FAMILY );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_POINT_SIZE ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::POINT_SIZE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_WEIGHT ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_WIDTH ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_SLANT ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_LINE_SPACING ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::LINE_SPACING );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_UNDERLINE ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::UNDERLINE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_SHADOW ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::SHADOW );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_EMBOSS ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::EMBOSS );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_OUTLINE ) )
  {
    editorInputStyleMask = static_cast<Toolkit::TextEditor::InputStyle::Mask>( editorInputStyleMask | Toolkit::TextEditor::InputStyle::OUTLINE );
  }

  mInputStyleChangedSignal.Emit( handle, editorInputStyleMask );
}

void TextEditor::AddDecoration( Actor& actor, bool needsClipping )
{
  if( actor )
  {
    if( needsClipping )
    {
      mClippingDecorationActors.push_back( actor );
    }
    else
    {
      actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      Self().Add( actor );
      mActiveLayer = actor;
    }
  }
}

void TextEditor::SetTextSelectionRange(const uint32_t *start, const uint32_t *end)
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->SetTextSelectionRange( start, end );
    SetKeyInputFocus();
  }
}

Uint32Pair TextEditor::GetTextSelectionRange() const
{
  Uint32Pair range(0, 0);
  if( mController && mController->IsShowingRealText() )
  {
    range = mController->GetTextSelectionRange();
  }
  return range;
}

void TextEditor::UpdateScrollBar()
{
  using namespace Dali;

  float scrollPosition;
  float controlSize;
  float layoutSize;
  bool latestScrolled;

  if ( !mScrollBarEnabled )
  {
    return;
  }
  latestScrolled = mController->GetTextScrollInfo( scrollPosition, controlSize, layoutSize );
  if ( !latestScrolled || controlSize > layoutSize)
  {
    return;
  }

  CustomActor self = Self();
  if( !mScrollBar )
  {
    mScrollBar = Toolkit::ScrollBar::New( Toolkit::ScrollBar::VERTICAL );
    mScrollBar.SetIndicatorHeightPolicy( Toolkit::ScrollBar::VARIABLE );
    mScrollBar.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_RIGHT );
    mScrollBar.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_RIGHT );
    mScrollBar.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
    mScrollBar.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH );

    // Register the scroll position property
    Property::Index propertyScrollPosition = self.RegisterProperty( SCROLL_BAR_POSITION, scrollPosition );
    // Register the minimum scroll position property
    Property::Index propertyMinScrollPosition = self.RegisterProperty( SCROLL_BAR_POSITION_MIN, 0.0f );
    // Register the maximum scroll position property
    Property::Index propertyMaxScrollPosition = self.RegisterProperty( SCROLL_BAR_POSITION_MAX, (layoutSize - controlSize) );
    // Register the scroll content size property
    Property::Index propertyScrollContentSize = self.RegisterProperty( SCROLL_BAR_CONTENT_SIZE, layoutSize );

    mScrollBar.SetScrollPropertySource(self, propertyScrollPosition, propertyMinScrollPosition, propertyMaxScrollPosition, propertyScrollContentSize);

    // Set style name of ScrollBar for styling
    mScrollBar.SetStyleName("TextEditorScrollBar");
    Toolkit::Control scrollIndicator = Toolkit::Control::DownCast( mScrollBar.GetScrollIndicator() );
    if( scrollIndicator )
    {
      // Set style name of ScrollBarIndicator for styling
      scrollIndicator.SetStyleName("TextEditorScrollBarIndicator");
    }

    self.Add( mScrollBar );
  }
  else
  {
    Property::Index propertyScrollPosition = self.GetPropertyIndex( SCROLL_BAR_POSITION );
    Property::Index propertyMaxScrollPosition = self.GetPropertyIndex( SCROLL_BAR_POSITION_MAX );
    Property::Index propertyScrollContentSize = self.GetPropertyIndex( SCROLL_BAR_CONTENT_SIZE );

    self.SetProperty( propertyScrollPosition, scrollPosition );
    self.SetProperty( propertyMaxScrollPosition, (layoutSize - controlSize) );
    self.SetProperty( propertyScrollContentSize, layoutSize );
  }

  // If scrolling is not started, start scrolling and emit ScrollStateChangedSignal
  if( !mScrollStarted )
  {
    mScrollStarted = true;
    Dali::Toolkit::TextEditor handle( GetOwner() );
    mScrollStateChangedSignal.Emit( handle, Toolkit::TextEditor::Scroll::STARTED );
  }

  Actor indicator = mScrollBar.GetScrollIndicator();
  if( mAnimation )
  {
    mAnimation.Stop(); // Cancel any animation
  }
  else
  {
    mAnimation = Animation::New( mAnimationPeriod.durationSeconds );
  }
  indicator.SetProperty( Actor::Property::OPACITY,1.0f);
  mAnimation.AnimateTo( Property( indicator, Actor::Property::COLOR_ALPHA ), 0.0f, AlphaFunction::EASE_IN, mAnimationPeriod );
  mAnimation.Play();
  mAnimation.FinishedSignal().Connect( this, &TextEditor::OnScrollIndicatorAnimationFinished );
}

void TextEditor::OnScrollIndicatorAnimationFinished( Animation& animation )
{
  // If animation is successfully ended, then emit ScrollStateChangedSignal
  if( animation.GetCurrentProgress() == 0.0f )
  {
    mScrollStarted = false;
    Dali::Toolkit::TextEditor handle( GetOwner() );
    mScrollStateChangedSignal.Emit( handle, Toolkit::TextEditor::Scroll::FINISHED );
  }
}

void TextEditor::OnSceneConnect( Dali::Actor actor )
{
  if ( mHasBeenStaged )
  {
    RenderText( static_cast<Text::Controller::UpdateTextType>( Text::Controller::MODEL_UPDATED | Text::Controller::DECORATOR_UPDATED ) );
  }
  else
  {
    mHasBeenStaged = true;
  }
}

InputMethodContext::CallbackData TextEditor::OnInputMethodContextEvent( Dali::InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnInputMethodContextEvent %p eventName %d\n", mController.Get(), inputMethodContextEvent.eventName );
  return mController->OnInputMethodContextEvent( inputMethodContext, inputMethodContextEvent );
}

void TextEditor::GetHandleImagePropertyValue(  Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType )
{
  if( mDecorator )
  {
    Property::Map map;
    map[ IMAGE_MAP_FILENAME_STRING ] = mDecorator->GetHandleImage( handleType, handleImageType );
    value = map;
  }
}

void TextEditor::OnClipboardTextSelected( ClipboardEventNotifier& clipboard )
{
  mController->PasteClipboardItemEvent();
}

void TextEditor::KeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::KeyboardStatusChanged %p keyboardShown %d\n", mController.Get(), keyboardShown );

  // Just hide the grab handle when keyboard is hidden.
  if (!keyboardShown )
  {
    mController->KeyboardFocusLostEvent();
  }
  else
  {
    mController->KeyboardFocusGainEvent(); // Initially called by OnKeyInputFocusGained
  }
}

void TextEditor::OnSceneConnection( int depth )
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth( depth );

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  Control::OnSceneConnection( depth );
}

bool TextEditor::OnTouched( Actor actor, const TouchEvent& touch )
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
  float scrollAmount = 0.0f;

  if ( mScrollAnimationEnabled )
  {
    scrollAmount = mController->GetScrollAmountByUserInput();
  }
  if ( mTextVerticalScroller )
  {
    mTextVerticalScroller->CheckStartAnimation( mRenderableActor, scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount, scrollAmount );
  }
  else if ( Equals( scrollAmount, 0.0f, Math::MACHINE_EPSILON_1 ))
  {
    mRenderableActor.SetProperty( Actor::Property::POSITION, Vector2( scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount ));
  }
  else
  {
    mTextVerticalScroller = Text::TextVerticalScroller::New();
    if ( !Equals( mScrollAnimationDuration, 0.0f, Math::MACHINE_EPSILON_1 ))
    {
      mTextVerticalScroller->SetDuration( mScrollAnimationDuration );
    }
    mTextVerticalScroller->CheckStartAnimation( mRenderableActor, scrollOffset.x + mAlignmentOffset, scrollOffset.y - scrollAmount, scrollAmount );
  }
}

bool TextEditor::IsEditable() const
{
  return mController->IsEditable();
}

void TextEditor::SetEditable( bool editable )
{
  mController->SetEditable(editable);
  if ( mInputMethodContext && !editable )
  {
    mInputMethodContext.Deactivate();
  }
}

TextEditor::TextEditor()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mAnimationPeriod( 0.0f, 0.0f ),
  mIdleCallback( NULL ),
  mAlignmentOffset( 0.f ),
  mScrollAnimationDuration( 0.f ),
  mLineSpacing( 0.f ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mHasBeenStaged( false ),
  mScrollAnimationEnabled( false ),
  mScrollBarEnabled( false ),
  mScrollStarted( false )
{
}

TextEditor::~TextEditor()
{
  UnparentAndReset( mStencil );

  if( ( NULL != mIdleCallback ) && Adaptor::IsAvailable() )
  {
    // Removes the callback from the callback manager in case the text-editor is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle( mIdleCallback );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
