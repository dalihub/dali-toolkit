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
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/devel-api/adaptor-framework/key-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/public-api/visuals/color-visual-properties.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/controls/text-controls/autofill-container-impl.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>

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
} // unnamed namespace

namespace
{
// Type registration
BaseHandle Create()
{
  return Toolkit::TextField::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextField, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "text",                                 STRING,    TEXT                                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholderText",                      STRING,    PLACEHOLDER_TEXT                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholderTextFocused",               STRING,    PLACEHOLDER_TEXT_FOCUSED             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "fontFamily",                           STRING,    FONT_FAMILY                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "fontStyle",                            MAP,       FONT_STYLE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "pointSize",                            FLOAT,     POINT_SIZE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "maxLength",                            INTEGER,   MAX_LENGTH                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "exceedPolicy",                         INTEGER,   EXCEED_POLICY                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "horizontalAlignment",                  STRING,    HORIZONTAL_ALIGNMENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "verticalAlignment",                    STRING,    VERTICAL_ALIGNMENT                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "textColor",                            VECTOR4,   TEXT_COLOR                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholderTextColor",                 VECTOR4,   PLACEHOLDER_TEXT_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "primaryCursorColor",                   VECTOR4,   PRIMARY_CURSOR_COLOR                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "secondaryCursorColor",                 VECTOR4,   SECONDARY_CURSOR_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "enableCursorBlink",                    BOOLEAN,   ENABLE_CURSOR_BLINK                  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursorBlinkInterval",                  FLOAT,     CURSOR_BLINK_INTERVAL                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursorBlinkDuration",                  FLOAT,     CURSOR_BLINK_DURATION                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursorWidth",                          INTEGER,   CURSOR_WIDTH                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grabHandleImage",                      STRING,    GRAB_HANDLE_IMAGE                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grabHandlePressedImage",               STRING,    GRAB_HANDLE_PRESSED_IMAGE            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scrollThreshold",                      FLOAT,     SCROLL_THRESHOLD                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scrollSpeed",                          FLOAT,     SCROLL_SPEED                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandleImageLeft",             MAP,       SELECTION_HANDLE_IMAGE_LEFT          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandleImageRight",            MAP,       SELECTION_HANDLE_IMAGE_RIGHT         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandlePressedImageLeft",      MAP,       SELECTION_HANDLE_PRESSED_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandlePressedImageRight",     MAP,       SELECTION_HANDLE_PRESSED_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandleMarkerImageLeft",       MAP,       SELECTION_HANDLE_MARKER_IMAGE_LEFT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHandleMarkerImageRight",      MAP,       SELECTION_HANDLE_MARKER_IMAGE_RIGHT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selectionHighlightColor",              VECTOR4,   SELECTION_HIGHLIGHT_COLOR            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "decorationBoundingBox",                RECTANGLE, DECORATION_BOUNDING_BOX              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputMethodSettings",                  MAP,       INPUT_METHOD_SETTINGS                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputColor",                           VECTOR4,   INPUT_COLOR                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "enableMarkup",                         BOOLEAN,   ENABLE_MARKUP                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputFontFamily",                      STRING,    INPUT_FONT_FAMILY                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputFontStyle",                       MAP,       INPUT_FONT_STYLE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputPointSize",                       FLOAT,     INPUT_POINT_SIZE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "underline",                            MAP,       UNDERLINE                            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputUnderline",                       MAP,       INPUT_UNDERLINE                      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "shadow",                               MAP,       SHADOW                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputShadow",                          MAP,       INPUT_SHADOW                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "emboss",                               MAP,       EMBOSS                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputEmboss",                          MAP,       INPUT_EMBOSS                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "outline",                              MAP,       OUTLINE                              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "inputOutline",                         MAP,       INPUT_OUTLINE                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "hiddenInputSettings",                  MAP,       HIDDEN_INPUT_SETTINGS                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "pixelSize",                            FLOAT,     PIXEL_SIZE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "enableSelection",                      BOOLEAN,   ENABLE_SELECTION                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholder",                          MAP,       PLACEHOLDER                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "ellipsis",                             BOOLEAN,   ELLIPSIS                             )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "enableShiftSelection",           BOOLEAN,   ENABLE_SHIFT_SELECTION               )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "enableGrabHandle",               BOOLEAN,   ENABLE_GRAB_HANDLE                   )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "matchSystemLanguageDirection",   BOOLEAN,   MATCH_SYSTEM_LANGUAGE_DIRECTION      )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "enableGrabHandlePopup",          BOOLEAN,   ENABLE_GRAB_HANDLE_POPUP             )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "textBackground",                 VECTOR4,   BACKGROUND                           )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY( Toolkit, TextField, "selectedText",         STRING,    SELECTED_TEXT                        )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit, TextField, "renderingBackend",               INTEGER,   RENDERING_BACKEND                    )

DALI_SIGNAL_REGISTRATION( Toolkit, TextField, "textChanged",        SIGNAL_TEXT_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextField, "maxLengthReached",   SIGNAL_MAX_LENGTH_REACHED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextField, "inputStyleChanged",  SIGNAL_INPUT_STYLE_CHANGED )

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

Toolkit::TextField TextField::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextField > impl = new TextField();

  // Pass ownership to CustomActor handle
  Toolkit::TextField handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextField::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextField textField = Toolkit::TextField::DownCast( Dali::BaseHandle( object ) );

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField SetProperty\n");


  if( textField )
  {
    TextField& impl( GetImpl( textField ) );

    switch( index )
    {
      case Toolkit::DevelTextField::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p RENDERING_BACKEND %d\n", impl.mController.Get(), backend );

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
        if( DevelText::RENDERING_VECTOR_BASED == backend )
        {
          backend = TextAbstraction::BITMAP_GLYPH; // Fallback to bitmap-based rendering
        }
#endif
        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();

          if( impl.mController )
          {
            // When using the vector-based rendering, the size of the GLyphs are different
            TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == impl.mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
            impl.mController->SetGlyphType( glyphType );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT:
      {
        if( impl.mController )
        {
          const std::string& text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p TEXT %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetText( text );
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          const std::string& text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetPlaceholderText( Controller::PLACEHOLDER_TYPE_INACTIVE, text );
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
      {
        if( impl.mController )
        {
          const std::string& text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_FOCUSED %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetPlaceholderText( Controller::PLACEHOLDER_TYPE_ACTIVE, text );
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string& fontFamily = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
          impl.mController->SetDefaultFontFamily( fontFamily );
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p POINT_SIZE %f\n", impl.mController.Get(), pointSize );

          if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE ), pointSize ) )
          {
            impl.mController->SetDefaultFontSize( pointSize, Text::Controller::POINT_SIZE );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::MAX_LENGTH:
      {
        if( impl.mController )
        {
          const int max = value.Get< int >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p MAX_LENGTH %d\n", impl.mController.Get(), max );

          impl.mController->SetMaximumNumberOfCharacters( max );
        }
        break;
      }
      case Toolkit::TextField::Property::EXCEED_POLICY:
      {
        impl.mExceedPolicy = value.Get<int>();

        if( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == impl.mExceedPolicy )
        {
          impl.EnableClipping();
        }
        else
        {
          UnparentAndReset( impl.mStencil );
        }
        impl.RequestTextRelayout();
        break;
      }
      case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          Text::HorizontalAlignment::Type alignment( static_cast< Text::HorizontalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
          if( GetHorizontalAlignmentEnumeration( value, alignment ) )
          {
            DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p HORIZONTAL_ALIGNMENT %d\n", impl.mController.Get(), alignment );
            impl.mController->SetHorizontalAlignment( alignment );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          Toolkit::Text::VerticalAlignment::Type alignment( static_cast< Text::VerticalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
          if( GetVerticalAlignmentEnumeration( value, alignment ) )
          {
            impl.mController->SetVerticalAlignment( alignment );
            DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p VERTICAL_ALIGNMENT %d\n", impl.mController.Get(), alignment );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4& textColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

          if( impl.mController->GetDefaultColor() != textColor )
          {
            impl.mController->SetDefaultColor( textColor );
            impl.mController->SetInputColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4& textColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

          if( impl.mController->GetPlaceholderTextColor() != textColor )
          {
            impl.mController->SetPlaceholderTextColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          const Vector4& color = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PRIMARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

          impl.mDecorator->SetCursorColor( PRIMARY_CURSOR, color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          const Vector4& color = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p SECONDARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

          impl.mDecorator->SetCursorColor( SECONDARY_CURSOR, color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
      {
        if( impl.mController )
        {
          const bool enable = value.Get< bool >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p ENABLE_CURSOR_BLINK %d\n", impl.mController.Get(), enable );

          impl.mController->SetEnableCursorBlink( enable );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          const float interval = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p CURSOR_BLINK_INTERVAL %f\n", impl.mController.Get(), interval );

          impl.mDecorator->SetCursorBlinkInterval( interval );
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          const float duration = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p CURSOR_BLINK_DURATION %f\n", impl.mController.Get(), duration );

          impl.mDecorator->SetCursorBlinkDuration( duration );
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_WIDTH:
      {
        if( impl.mDecorator )
        {
          const int width = value.Get< int >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p CURSOR_WIDTH %d\n", impl.mController.Get(), width );

          impl.mDecorator->SetCursorWidth( width );
          impl.mController->GetLayoutEngine().SetCursorWidth( width );
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
      {
        const std::string imageFileName = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str() );

        if( impl.mDecorator && imageFileName.size() )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED, imageFileName );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        const std::string imageFileName = value.Get< std::string >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), imageFileName.c_str() );

        if( impl.mDecorator && imageFileName.size() )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED, imageFileName );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_THRESHOLD:
      {
        const float threshold = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p SCROLL_THRESHOLD %f\n", impl.mController.Get(), threshold );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollThreshold( threshold );
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_SPEED:
      {
        const float speed = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p SCROLL_SPEED %f\n", impl.mController.Get(), speed );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollSpeed( speed );
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        const std::string filename = GetImageFileNameFromPropertyValue( value );

        if( impl.mDecorator && filename.size() )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, filename );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        const Vector4 color = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p SELECTION_HIGHLIGHT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHighlightColor( color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          const Rect<int> box = value.Get< Rect<int> >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p DECORATION_BOUNDING_BOX %d,%d %dx%d\n", impl.mController.Get(), box.x, box.y, box.width, box.height );

          impl.mDecorator->SetBoundingBox( box );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_METHOD_SETTINGS:
      {
        const Property::Map* map = value.GetMap();
        if (map)
        {
          impl.mInputMethodOptions.ApplyProperty( *map );
        }
        impl.mController->SetInputModePassword( impl.mInputMethodOptions.IsPassword() );

        Toolkit::Control control = Toolkit::KeyInputFocusManager::Get().GetCurrentFocusControl();
        if (control == textField)
        {
          impl.mInputMethodContext.ApplyOptions( impl.mInputMethodOptions );
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 inputColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p INPUT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), inputColor.r, inputColor.g, inputColor.b, inputColor.a );

          impl.mController->SetInputColor( inputColor );
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          const bool enableMarkup = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p ENABLE_MARKUP %d\n", impl.mController.Get(), enableMarkup );

          impl.mController->SetMarkupProcessorEnabled( enableMarkup );
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string& fontFamily = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p INPUT_FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
          impl.mController->SetInputFontFamily( fontFamily );
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p INPUT_POINT_SIZE %f\n", impl.mController.Get(), pointSize );
          impl.mController->SetInputFontPointSize( pointSize );
        }
        break;
      }
      case Toolkit::TextField::Property::UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS:
      {
        const Property::Map* map = value.GetMap();
        if (map)
        {
          impl.mController->SetHiddenInputOption(*map);
        }
        break;
      }
      case Toolkit::TextField::Property::PIXEL_SIZE:
      {
        if( impl.mController )
        {
          const float pixelSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize );

          if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE ), pixelSize ) )
          {
            impl.mController->SetDefaultFontSize( pixelSize, Text::Controller::PIXEL_SIZE );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_SELECTION:
      {
        if( impl.mController )
        {
          const bool enableSelection = value.Get< bool >();
           DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p ENABLE_SELECTION %d\n", impl.mController.Get(), enableSelection );
          impl.mController->SetSelectionEnabled( enableSelection );
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER:
      {
        const Property::Map* map = value.GetMap();
        if( map )
        {
          impl.mController->SetPlaceholderProperty( *map );
        }
        break;
      }
      case Toolkit::TextField::Property::ELLIPSIS:
      {
        if( impl.mController )
        {
          const bool ellipsis = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p ELLIPSIS %d\n", impl.mController.Get(), ellipsis );

          impl.mController->SetTextElideEnabled( ellipsis );
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_SHIFT_SELECTION:
      {
        if( impl.mController )
        {
          const bool shiftSelection = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p ENABLE_SHIFT_SELECTION %d\n", impl.mController.Get(), shiftSelection );

          impl.mController->SetShiftSelectionEnabled( shiftSelection );
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE:
      {
        if( impl.mController )
        {
          const bool grabHandleEnabled = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p ENABLE_GRAB_HANDLE %d\n", impl.mController.Get(), grabHandleEnabled );

          impl.mController->SetGrabHandleEnabled( grabHandleEnabled );
        }
        break;
      }
      case Toolkit::DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        if( impl.mController )
        {
          impl.mController->SetMatchSystemLanguageDirection(value.Get< bool >());
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP:
      {
        if( impl.mController )
        {
          const bool grabHandlePopupEnabled = value.Get<bool>();
          DALI_LOG_INFO(gLogFilter, Debug::General, "TextField %p ENABLE_GRAB_HANDLE_POPUP %d\n", impl.mController.Get(), grabHandlePopupEnabled);

          impl.mController->SetGrabHandlePopupEnabled(grabHandlePopupEnabled);
          break;
        }
      }
      case Toolkit::DevelTextField::Property::BACKGROUND:
      {
        if( impl.mController )
        {
          const Vector4 backgroundColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p BACKGROUND %f,%f,%f,%f\n", impl.mController.Get(), backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a );

          impl.mController->SetBackgroundEnabled( true );
          impl.mController->SetBackgroundColor( backgroundColor );
        }
        break;
      }
    } // switch
  } // textfield
}

Property::Value TextField::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextField textField = Toolkit::TextField::DownCast( Dali::BaseHandle( object ) );

  if( textField )
  {
    TextField& impl( GetImpl( textField ) );

    switch( index )
    {
      case Toolkit::DevelTextField::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextField::Property::TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p returning text: %s\n", impl.mController.Get(), text.c_str() );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetPlaceholderText( Controller::PLACEHOLDER_TYPE_INACTIVE, text );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetPlaceholderText( Controller::PLACEHOLDER_TYPE_ACTIVE, text );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontFamily();
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE );
        }
        break;
      }
      case Toolkit::TextField::Property::MAX_LENGTH:
      {
        if( impl.mController )
        {
          value = impl.mController->GetMaximumNumberOfCharacters();
        }
        break;
      }
      case Toolkit::TextField::Property::EXCEED_POLICY:
      {
        value = impl.mExceedPolicy;
        break;
      }
      case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const char* name = Text::GetHorizontalAlignmentString( impl.mController->GetHorizontalAlignment() );

          if ( name )
          {
            value = std::string( name );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const char* name = Text::GetVerticalAlignmentString( impl.mController->GetVerticalAlignment() );

          if( name )
          {
            value = std::string( name );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetDefaultColor();
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetPlaceholderTextColor();
        }
        break;
      }
      case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( PRIMARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( SECONDARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_CURSOR_BLINK:
      {
        value = impl.mController->GetEnableCursorBlink();
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkInterval();
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkDuration();
        }
        break;
      }
      case Toolkit::TextField::Property::CURSOR_WIDTH:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorWidth();
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_IMAGE:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED );
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED );
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_THRESHOLD:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollThreshold();
        }
        break;
      }
      case Toolkit::TextField::Property::SCROLL_SPEED:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollSpeed();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED ) ;
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED );
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED );
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        impl.GetHandleImagePropertyValue( value, LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        impl.GetHandleImagePropertyValue( value, RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED );
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetHighlightColor();
        }
        break;
      }
      case Toolkit::TextField::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          Rect<int> boundingBox;
          impl.mDecorator->GetBoundingBox( boundingBox );
          value = boundingBox;
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_METHOD_SETTINGS:
      {
        Property::Map map;
        impl.mInputMethodOptions.RetrieveProperty( map );
        value = map;
        break;
      }
      case Toolkit::TextField::Property::INPUT_COLOR:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputColor();
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          value = impl.mController->IsMarkupProcessorEnabled();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_FONT_FAMILY:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputFontFamily();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputFontPointSize();
        }
        break;
      }
      case Toolkit::TextField::Property::UNDERLINE:
      {
        GetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_UNDERLINE:
      {
        GetUnderlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::SHADOW:
      {
        GetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_SHADOW:
      {
        GetShadowProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::EMBOSS:
      {
        GetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_EMBOSS:
      {
        GetEmbossProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::OUTLINE:
      {
        GetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextField::Property::INPUT_OUTLINE:
      {
        GetOutlineProperties( impl.mController, value, Text::EffectStyle::INPUT );
        break;
      }
      case Toolkit::TextField::Property::HIDDEN_INPUT_SETTINGS:
      {
        Property::Map map;
        impl.mController->GetHiddenInputOption(map);
        value = map;
        break;
      }
      case Toolkit::TextField::Property::PIXEL_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE );
        }
        break;
      }
      case Toolkit::TextField::Property::ENABLE_SELECTION:
      {
        if( impl.mController )
        {
          value = impl.mController->IsSelectionEnabled();
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER:
      {
        Property::Map map;
        impl.mController->GetPlaceholderProperty( map );
        value = map;
        break;
      }
      case Toolkit::TextField::Property::ELLIPSIS:
      {
        if( impl.mController )
        {
          value = impl.mController->IsTextElideEnabled();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_SHIFT_SELECTION:
      {
        if( impl.mController )
        {
          value = impl.mController->IsShiftSelectionEnabled();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE:
      {
        if( impl.mController )
        {
          value = impl.mController->IsGrabHandleEnabled();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        if( impl.mController )
        {
          value = impl.mController->IsMatchSystemLanguageDirection();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::ENABLE_GRAB_HANDLE_POPUP:
      {
        if( impl.mController )
        {
          value = impl.mController->IsGrabHandlePopupEnabled();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::BACKGROUND:
      {
        if( impl.mController )
        {
          value = impl.mController->GetBackgroundColor();
        }
        break;
      }
      case Toolkit::DevelTextField::Property::SELECTED_TEXT:
      {
        if( impl.mController )
        {
          value = impl.mController->GetSelectedText( );
        }
        break;
      }
    } //switch
  }

  return value;
}

void TextField::SelectWholeText()
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->SelectEvent( 0.f, 0.f, SelectionType::ALL );
    SetKeyInputFocus();
  }
}

void TextField::SelectNone()
{
  if( mController && mController->IsShowingRealText() )
  {
    mController->SelectEvent( 0.f, 0.f, SelectionType::NONE );
    SetKeyInputFocus();
  }
}

InputMethodContext TextField::GetInputMethodContext()
{
  return mInputMethodContext;
}

bool TextField::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::BaseHandle handle( object );

  bool connected( true );
  Toolkit::TextField field = Toolkit::TextField::DownCast( handle );

  if( 0 == strcmp( signalName.c_str(), SIGNAL_TEXT_CHANGED ) )
  {
    field.TextChangedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MAX_LENGTH_REACHED ) )
  {
    field.MaxLengthReachedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_INPUT_STYLE_CHANGED ) )
  {
    field.InputStyleChangedSignal().Connect( tracker, functor );
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

void TextField::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( this, this );

  // When using the vector-based rendering, the size of the GLyphs are different
  TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
  mController->SetGlyphType( glyphType );

  mDecorator = Text::Decorator::New( *mController,
                                     *mController );

  mInputMethodContext = InputMethodContext::New( self );

  mController->GetLayoutEngine().SetLayout( Layout::Engine::SINGLE_LINE_BOX );

  // Enables the text input.
  mController->EnableTextInput( mDecorator, mInputMethodContext );

  // Enables the horizontal scrolling after the text input has been enabled.
  mController->SetHorizontalScrollEnabled( true );

  // Disables the vertical scrolling.
  mController->SetVerticalScrollEnabled( false );

  // Disable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled( false );

  mController->SetNoTextDoubleTapAction( Controller::NoTextTap::HIGHLIGHT );
  mController->SetNoTextLongPressAction( Controller::NoTextTap::HIGHLIGHT );

  // Sets layoutDirection value
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>( stage.GetRootLayer().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  mController->SetLayoutDirection( layoutDirection );

  // Forward input events to controller
  EnableGestureDetection( static_cast<Gesture::Type>( Gesture::Tap | Gesture::Pan | Gesture::LongPress ) );
  GetTapGestureDetector().SetMaximumTapsRequired( 2 );

  self.TouchSignal().Connect( this, &TextField::OnTouched );

  // Set BoundingBox to stage size if not already set.
  Rect<int> boundingBox;
  mDecorator->GetBoundingBox( boundingBox );

  if( boundingBox.IsEmpty() )
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox( Rect<int>( 0.0f, 0.0f, stageSize.width, stageSize.height ) );
  }

  // Flip vertically the 'left' selection handle
  mDecorator->FlipHandleVertically( LEFT_SELECTION_HANDLE, true );

  // Fill-parent area by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
  self.OnSceneSignal().Connect( this, &TextField::OnSceneConnect );

  DevelControl::SetInputMethodContext( *this, mInputMethodContext );

  if( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == mExceedPolicy )
  {
    EnableClipping();
  }
}

void TextField::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnStyleChange\n");

  switch ( change )
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnStyleChange DEFAULT_FONT_CHANGE\n");
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

Vector3 TextField::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += ( padding.start + padding.end );
  naturalSize.height += ( padding.top + padding.bottom );

  return naturalSize;
}

float TextField::GetHeightForWidth( float width )
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  return mController->GetHeightForWidth( width ) + padding.top + padding.bottom;
}

void TextField::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField OnRelayout\n");

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
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnRelayout %p Displaying new contents\n", mController.Get() );

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

  // The text-field emits signals when the input style changes. These changes of style are
  // detected during the relayout process (size negotiation), i.e after the cursor has been moved. Signals
  // can't be emitted during the size negotiation as the callbacks may update the UI.
  // The text-field adds an idle callback to the adaptor to emit the signals after the size negotiation.
  if( !mController->IsInputStyleChangedSignalsQueueEmpty() )
  {
    if( Adaptor::IsAvailable() )
    {
      Adaptor& adaptor = Adaptor::Get();

      if( NULL == mIdleCallback )
      {
        // @note: The callback manager takes the ownership of the callback object.
        mIdleCallback = MakeCallback( this, &TextField::OnIdleSignal );
        adaptor.AddIdle( mIdleCallback, false );
      }
    }
  }
}

void TextField::RenderText( Text::Controller::UpdateTextType updateTextType )
{
  Actor renderableActor;

  if( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) )
  {
    if( mRenderer )
    {
      Dali::Toolkit::TextField handle = Dali::Toolkit::TextField( GetOwner() );

      renderableActor = mRenderer->Render( mController->GetView(),
                                           handle,
                                           Property::INVALID_INDEX, // Animatable property not supported
                                           mAlignmentOffset,
                                           DepthIndex::CONTENT );
    }

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mBackgroundActor );
      UnparentAndReset( mRenderableActor );
      mRenderableActor = renderableActor;

      if ( mRenderableActor )
      {
        mBackgroundActor = mController->CreateBackgroundActor();
      }
    }
  }

  if( mRenderableActor )
  {
    const Vector2& scrollOffset = mController->GetTextModel()->GetScrollPosition();

    float renderableActorPositionX, renderableActorPositionY;

    if( mStencil )
    {
      renderableActorPositionX = scrollOffset.x + mAlignmentOffset;
      renderableActorPositionY = scrollOffset.y;
    }
    else
    {
      Extents padding;
      padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

      // Support Right-To-Left of padding
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>( Self().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
      if( Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection )
      {
        std::swap( padding.start, padding.end );
      }

      renderableActorPositionX = scrollOffset.x + mAlignmentOffset + padding.start;
      renderableActorPositionY = scrollOffset.y + padding.top;
    }

    mRenderableActor.SetProperty( Actor::Property::POSITION, Vector2( renderableActorPositionX, renderableActorPositionY ));

    // Make sure the actors are parented correctly with/without clipping
    Actor self = mStencil ? mStencil : Self();

    Actor highlightActor;

    for( std::vector<Actor>::iterator it = mClippingDecorationActors.begin(),
           endIt = mClippingDecorationActors.end();
         it != endIt;
         ++it )
    {
      self.Add( *it );
      it->LowerToBottom();

      if ( it->GetProperty< std::string >( Dali::Actor::Property::NAME ) == "HighlightActor" )
      {
        highlightActor = *it;
      }
    }
    mClippingDecorationActors.clear();

    self.Add( mRenderableActor );

    if ( mBackgroundActor )
    {
      if ( mDecorator && mDecorator->IsHighlightVisible() )
      {
        self.Add( mBackgroundActor );
        mBackgroundActor.SetProperty( Actor::Property::POSITION, Vector2( renderableActorPositionX, renderableActorPositionY) ); // In text field's coords.
        mBackgroundActor.LowerBelow( highlightActor );
      }
      else
      {
        mRenderableActor.Add( mBackgroundActor );
        mBackgroundActor.SetProperty( Actor::Property::POSITION, Vector2( 0.0f, 0.0f ) ); // In renderable actor's coords.
        mBackgroundActor.LowerToBottom();
      }
    }
  }
}

void TextField::OnKeyInputFocusGained()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnKeyInputFocusGained %p\n", mController.Get() );
  if( mInputMethodContext )
  {
    mInputMethodContext.ApplyOptions( mInputMethodOptions );

    mInputMethodContext.StatusChangedSignal().Connect( this, &TextField::KeyboardStatusChanged );

    mInputMethodContext.EventReceivedSignal().Connect( this, &TextField::OnInputMethodContextEvent );

    // Notify that the text editing start.
    mInputMethodContext.Activate();

    // When window gain lost focus, the inputMethodContext is deactivated. Thus when window gain focus again, the inputMethodContext must be activated.
    mInputMethodContext.SetRestoreAfterFocusLost( true );
  }

  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );
  if( notifier )
  {
    notifier.ContentSelectedSignal().Connect( this, &TextField::OnClipboardTextSelected );
  }

  Toolkit::Control control = Toolkit::Control::DownCast( Self() );
  Internal::Control& controlImpl = GetImplementation( control );
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
  bool enableAutofill = controlDataImpl.IsAutofillEnabled();
  if( enableAutofill )
  {
    Toolkit::AutofillContainer container = controlDataImpl.GetAutofillContainer();
    container.SetFocusedControl( control );

    Internal::AutofillContainer& containerImpl = GetImpl( container );
    Dali::AutofillGroup containerGroup = containerImpl.GetAutofillGroup();
    if( containerGroup != nullptr )
    {
      containerGroup.RequestAuthentication();
    }

  }
  mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event

  EmitKeyInputFocusSignal( true ); // Calls back into the Control hence done last.
}

void TextField::OnKeyInputFocusLost()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField:OnKeyInputFocusLost %p\n", mController.Get() );
  if ( mInputMethodContext )
  {
    mInputMethodContext.StatusChangedSignal().Disconnect( this, &TextField::KeyboardStatusChanged );
    // The text editing is finished. Therefore the inputMethodContext don't have restore activation.
    mInputMethodContext.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    mInputMethodContext.Deactivate();

    mInputMethodContext.EventReceivedSignal().Disconnect( this, &TextField::OnInputMethodContextEvent );
  }
  ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );

  if ( notifier )
  {
    notifier.ContentSelectedSignal().Disconnect( this, &TextField::OnClipboardTextSelected );
  }

  mController->KeyboardFocusLostEvent();

  EmitKeyInputFocusSignal( false ); // Calls back into the Control hence done last.
}

void TextField::OnTap( const TapGesture& gesture )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnTap %p\n", mController.Get() );
  if ( mInputMethodContext )
  {
    mInputMethodContext.Activate();
  }
  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  mController->TapEvent( gesture.numberOfTaps, gesture.localPoint.x - padding.start, gesture.localPoint.y - padding.top );

  SetKeyInputFocus();
}

void TextField::OnPan( const PanGesture& gesture )
{
  mController->PanEvent( gesture.state, gesture.displacement );
}

void TextField::OnLongPress( const LongPressGesture& gesture )
{
  if ( mInputMethodContext )
  {
    mInputMethodContext.Activate();
  }
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );
  mController->LongPressEvent( gesture.state, gesture.localPoint.x - padding.start, gesture.localPoint.y - padding.top );

  SetKeyInputFocus();
}

bool TextField::OnKeyEvent( const KeyEvent& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnKeyEvent %p keyCode %d\n", mController.Get(), event.GetKeyCode() );

  if( Dali::DALI_KEY_ESCAPE == event.GetKeyCode() && mController->ShouldClearFocusOnEscape() )
  {
    // Make sure ClearKeyInputFocus when only key is up
    if( event.GetState() == KeyEvent::UP )
    {
      ClearKeyInputFocus();
    }

    return true;
  }
  else if( Dali::DevelKey::DALI_KEY_RETURN == event.GetKeyCode() )
  {
    // Do nothing when enter is comming.
    return false;
  }

  return mController->KeyEvent( event );
}

void TextField::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextField::TextChanged()
{
  Dali::Toolkit::TextField handle( GetOwner() );
  mTextChangedSignal.Emit( handle );
}

void TextField::MaxLengthReached()
{
  Dali::Toolkit::TextField handle( GetOwner() );
  mMaxLengthReachedSignal.Emit( handle );
}

void TextField::InputStyleChanged( Text::InputStyle::Mask inputStyleMask )
{
  Dali::Toolkit::TextField handle( GetOwner() );

  Toolkit::TextField::InputStyle::Mask fieldInputStyleMask = Toolkit::TextField::InputStyle::NONE;

  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_COLOR ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::COLOR );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_FAMILY ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_FAMILY );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_POINT_SIZE ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::POINT_SIZE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_WEIGHT ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_WIDTH ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_FONT_SLANT ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::FONT_STYLE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_UNDERLINE ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::UNDERLINE );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_SHADOW ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::SHADOW );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_EMBOSS ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::EMBOSS );
  }
  if( InputStyle::NONE != static_cast<InputStyle::Mask>( inputStyleMask & InputStyle::INPUT_OUTLINE ) )
  {
    fieldInputStyleMask = static_cast<Toolkit::TextField::InputStyle::Mask>( fieldInputStyleMask | Toolkit::TextField::InputStyle::OUTLINE );
  }

  mInputStyleChangedSignal.Emit( handle, fieldInputStyleMask );
}

void TextField::AddDecoration( Actor& actor, bool needsClipping )
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

void TextField::OnSceneConnect( Dali::Actor actor )
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

InputMethodContext::CallbackData TextField::OnInputMethodContextEvent( Dali::InputMethodContext& inputMethodContext, const InputMethodContext::EventData& inputMethodContextEvent )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnInputMethodContextEvent %p eventName %d\n", mController.Get(), inputMethodContextEvent.eventName );
  return mController->OnInputMethodContextEvent( inputMethodContext, inputMethodContextEvent );
}

void TextField::GetHandleImagePropertyValue(  Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType )
{
  if( mDecorator )
  {
    Property::Map map;
    map[ IMAGE_MAP_FILENAME_STRING ] = mDecorator->GetHandleImage( handleType, handleImageType );
    value = map;
  }
}

void TextField::EnableClipping()
{
  if( !mStencil )
  {
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

    Self().Add( mStencil );
  }
}

void TextField::OnClipboardTextSelected( ClipboardEventNotifier& clipboard )
{
  mController->PasteClipboardItemEvent();
}

void TextField::KeyboardStatusChanged(bool keyboardShown)
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::KeyboardStatusChanged %p keyboardShown %d\n", mController.Get(), keyboardShown );

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

void TextField::OnSceneConnection( int depth )
{
  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth( depth );

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().

  // Call the Control::OnSceneConnection() to set the depth of the background.
  Control::OnSceneConnection( depth );
}

bool TextField::OnTouched( Actor actor, const TouchEvent& touch )
{
  return false;
}

void TextField::OnIdleSignal()
{
  // Emits the change of input style signals.
  mController->ProcessInputStyleChangedSignals();

  // Set the pointer to null as the callback manager deletes the callback after execute it.
  mIdleCallback = NULL;
}

TextField::TextField()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mIdleCallback( NULL ),
  mAlignmentOffset( 0.f ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mExceedPolicy( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP ),
  mHasBeenStaged( false )
{
}

TextField::~TextField()
{
  UnparentAndReset( mStencil );

  if( ( NULL != mIdleCallback ) && Adaptor::IsAvailable() )
  {
    Adaptor::Get().RemoveIdle( mIdleCallback );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
