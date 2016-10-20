/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/devel-api/adaptor-framework/virtual-keyboard.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/adaptors/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

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

const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;
const float DEFAULT_SCROLL_SPEED = 1200.f; ///< The default scroll speed for the text editor in pixels/second.
} // unnamed namespace

namespace
{

const Scripting::StringEnum HORIZONTAL_ALIGNMENT_STRING_TABLE[] =
{
  { "BEGIN",  Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_BEGIN  },
  { "CENTER", Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_CENTER },
  { "END",    Toolkit::Text::LayoutEngine::HORIZONTAL_ALIGN_END    },
};
const unsigned int HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE ) / sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE[0] );

// Type registration
BaseHandle Create()
{
  return Toolkit::TextEditor::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextEditor, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "renderingBackend",                     INTEGER,   RENDERING_BACKEND                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "text",                                 STRING,    TEXT                                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "textColor",                            VECTOR4,   TEXT_COLOR                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "fontFamily",                           STRING,    FONT_FAMILY                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "fontStyle",                            STRING,    FONT_STYLE                           )
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
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputFontStyle",                       STRING,    INPUT_FONT_STYLE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputPointSize",                       FLOAT,     INPUT_POINT_SIZE                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "lineSpacing",                          FLOAT,     LINE_SPACING                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputLineSpacing",                     FLOAT,     INPUT_LINE_SPACING                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "underline",                            STRING,    UNDERLINE                            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputUnderline",                       STRING,    INPUT_UNDERLINE                      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "shadow",                               STRING,    SHADOW                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputShadow",                          STRING,    INPUT_SHADOW                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "emboss",                               STRING,    EMBOSS                               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputEmboss",                          STRING,    INPUT_EMBOSS                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "outline",                              STRING,    OUTLINE                              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextEditor, "inputOutline",                         STRING,    INPUT_OUTLINE                        )

DALI_SIGNAL_REGISTRATION( Toolkit, TextEditor, "textChanged",        SIGNAL_TEXT_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextEditor, "inputStyleChanged",  SIGNAL_INPUT_STYLE_CHANGED )

DALI_TYPE_REGISTRATION_END()

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

    switch( index )
    {
      case Toolkit::TextEditor::Property::RENDERING_BACKEND:
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
        if( impl.mController )
        {
          const std::string text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p TEXT %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetText( text );
        }
        break;
      }
      case Toolkit::TextEditor::Property::TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 textColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

          if( impl.mController->GetTextColor() != textColor )
          {
            impl.mController->SetTextColor( textColor );
            impl.mController->SetInputColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextEditor::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string fontFamily = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
          impl.mController->SetDefaultFontFamily( fontFamily );
        }
        break;
      }
      case Toolkit::TextEditor::Property::FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p POINT_SIZE %f\n", impl.mController.Get(), pointSize );

          if( !Equals( impl.mController->GetDefaultPointSize(), pointSize ) )
          {
            impl.mController->SetDefaultPointSize( pointSize );
          }
        }
        break;
      }
      case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const std::string alignStr = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p HORIZONTAL_ALIGNMENT %s\n", impl.mController.Get(), alignStr.c_str() );

          LayoutEngine::HorizontalAlignment alignment( LayoutEngine::HORIZONTAL_ALIGN_BEGIN );
          if( Scripting::GetEnumeration< LayoutEngine::HorizontalAlignment >( alignStr.c_str(),
                                                                              HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                              HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                              alignment ) )
          {
            impl.mController->SetHorizontalAlignment( alignment );
          }
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
      {
        const float threshold = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p SCROLL_THRESHOLD %f\n", impl.mController.Get(), threshold );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollThreshold( threshold );
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_SPEED:
      {
        const float speed = value.Get< float >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p SCROLL_SPEED %f\n", impl.mController.Get(), speed );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetScrollSpeed( speed );
        }
        break;
      }
      case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          const Vector4 color = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p PRIMARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

          impl.mDecorator->SetCursorColor( PRIMARY_CURSOR, color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          const Vector4 color = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SECONDARY_CURSOR_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

          impl.mDecorator->SetCursorColor( SECONDARY_CURSOR, color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
      {
        if( impl.mController )
        {
          const bool enable = value.Get< bool >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p ENABLE_CURSOR_BLINK %d\n", impl.mController.Get(), enable );

          impl.mController->SetEnableCursorBlink( enable );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          const float interval = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_INTERVAL %f\n", impl.mController.Get(), interval );

          impl.mDecorator->SetCursorBlinkInterval( interval );
        }
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          const float duration = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_BLINK_DURATION %f\n", impl.mController.Get(), duration );

          impl.mDecorator->SetCursorBlinkDuration( duration );
        }
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_WIDTH:
      {
        if( impl.mDecorator )
        {
          const int width = value.Get< int >();
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p CURSOR_WIDTH %d\n", impl.mController.Get(), width );

          impl.mDecorator->SetCursorWidth( width );
          impl.mController->GetLayoutEngine().SetCursorWidth( width );
        }
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
      {
        const ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), image.GetUrl().c_str() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        const ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), image.GetUrl().c_str() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_LEFT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator && image )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SELECTION_HIGHLIGHT_COLOR:
      {
        const Vector4 color = value.Get< Vector4 >();
        DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p SELECTION_HIGHLIGHT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), color.r, color.g, color.b, color.a );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHighlightColor( color );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          const Rect<int> box = value.Get< Rect<int> >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p DECORATION_BOUNDING_BOX %d,%d %dx%d\n", impl.mController.Get(), box.x, box.y, box.width, box.height );

          impl.mDecorator->SetBoundingBox( box );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          const bool enableMarkup = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p ENABLE_MARKUP %d\n", impl.mController.Get(), enableMarkup );

          impl.mController->SetMarkupProcessorEnabled( enableMarkup );
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 inputColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), inputColor.r, inputColor.g, inputColor.b, inputColor.a );

          impl.mController->SetInputColor( inputColor );
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string fontFamily = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );
          impl.mController->SetInputFontFamily( fontFamily );
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p INPUT_POINT_SIZE %f\n", impl.mController.Get(), pointSize );
          impl.mController->SetInputFontPointSize( pointSize );
        }
        break;
      }
      case Toolkit::TextEditor::Property::LINE_SPACING:
      {
        if( impl.mController )
        {
          const float lineSpacing = value.Get<float>();
          impl.mController->SetDefaultLineSpacing( lineSpacing );
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
      {
        if( impl.mController )
        {
          const float lineSpacing = value.Get<float>();
          impl.mController->SetInputLineSpacing( lineSpacing );
          impl.mRenderer.Reset();
        }
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

    switch( index )
    {
      case Toolkit::TextEditor::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextEditor::Property::TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextEditor %p returning text: %s\n", impl.mController.Get(), text.c_str() );
          value = text;
        }
        break;
      }
      case Toolkit::TextEditor::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetTextColor();
        }
        break;
      }
      case Toolkit::TextEditor::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontFamily();
        }
        break;
      }
      case Toolkit::TextEditor::Property::FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextEditor::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultPointSize();
        }
        break;
      }
      case Toolkit::TextEditor::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::HorizontalAlignment >( impl.mController->GetLayoutEngine().GetHorizontalAlignment(),
                                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );
          if( name )
          {
            value = std::string( name );
          }
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_THRESHOLD:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollThreshold();
        }
        break;
      }
      case Toolkit::TextEditor::Property::SCROLL_SPEED:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetScrollSpeed();
        }
        break;
      }
      case Toolkit::TextEditor::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( PRIMARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextEditor::Property::SECONDARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetColor( SECONDARY_CURSOR );
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_CURSOR_BLINK:
      {
        value = impl.mController->GetEnableCursorBlink();
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_INTERVAL:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkInterval();
        }
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_BLINK_DURATION:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorBlinkDuration();
        }
        break;
      }
      case Toolkit::TextEditor::Property::CURSOR_WIDTH:
      {
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetCursorWidth();
        }
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextEditor::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        if( impl.mDecorator )
        {
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
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
        if( impl.mDecorator )
        {
          value = impl.mDecorator->GetHighlightColor();
        }
        break;
      }
      case Toolkit::TextEditor::Property::DECORATION_BOUNDING_BOX:
      {
        if( impl.mDecorator )
        {
          Rect<int> boundingBox;
          impl.mDecorator->GetBoundingBox( boundingBox );
          value = boundingBox;
        }
        break;
      }
      case Toolkit::TextEditor::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          value = impl.mController->IsMarkupProcessorEnabled();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_COLOR:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputColor();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_FAMILY:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputFontFamily();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::INPUT );
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputFontPointSize();
        }
        break;
      }
      case Toolkit::TextEditor::Property::LINE_SPACING:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultLineSpacing();
        }
        break;
      }
      case Toolkit::TextEditor::Property::INPUT_LINE_SPACING:
      {
        if( impl.mController )
        {
          value = impl.mController->GetInputLineSpacing();
        }
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
    } //switch
  }

  return value;
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

void TextEditor::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( this, this );

  mDecorator = Text::Decorator::New( *mController,
                                     *mController );

  mController->GetLayoutEngine().SetLayout( LayoutEngine::MULTI_LINE_BOX );

  // Enables the text input.
  mController->EnableTextInput( mDecorator );

  // Enables the vertical scrolling after the text input has been enabled.
  mController->SetVerticalScrollEnabled( true );

  // Disables the horizontal scrolling.
  mController->SetHorizontalScrollEnabled( false );

  // Sets the maximum number of characters.
  mController->SetMaximumNumberOfCharacters( std::numeric_limits<Length>::max() );

  // Enable the smooth handle panning.
  mController->SetSmoothHandlePanEnabled( true );

  // Forward input events to controller
  EnableGestureDetection( static_cast<Gesture::Type>( Gesture::Tap | Gesture::Pan | Gesture::LongPress ) );
  GetTapGestureDetector().SetMaximumTapsRequired( 2 );

  self.TouchSignal().Connect( this, &TextEditor::OnTouched );

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
  self.OnStageSignal().Connect( this, &TextEditor::OnStageConnect );
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
      break;
    }

    case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
      break;
    }
    case StyleChange::THEME_CHANGE:
    {
      GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
      break;
    }
  }
}

Vector3 TextEditor::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextEditor::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextEditor::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor OnRelayout\n");

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( size );

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

    EnableClipping( size );
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
        adaptor.AddIdle( mIdleCallback );
      }
    }
  }
}

void TextEditor::RenderText( Text::Controller::UpdateTextType updateTextType )
{
  Actor self = Self();
  Actor renderableActor;

  if( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) )
  {
    if( mRenderer )
    {
      renderableActor = mRenderer->Render( mController->GetView(), DepthIndex::TEXT );
    }

    if( renderableActor != mRenderableActor )
    {
      UnparentAndReset( mRenderableActor );
      mRenderableActor = renderableActor;
    }
  }

  if( mRenderableActor )
  {
    const Vector2& scrollOffset = mController->GetScrollPosition();

    mRenderableActor.SetPosition( scrollOffset.x, scrollOffset.y );

    Actor clipRootActor;
    if( mClipper )
    {
      clipRootActor = mClipper->GetRootActor();
    }

    for( std::vector<Actor>::const_iterator it = mClippingDecorationActors.begin(),
           endIt = mClippingDecorationActors.end();
         it != endIt;
         ++it )
    {
      Actor actor = *it;

      if( clipRootActor )
      {
        clipRootActor.Add( actor );
      }
      else
      {
        self.Add( actor );
      }
    }
    mClippingDecorationActors.clear();

    // Make sure the actor is parented correctly with/without clipping
    if( clipRootActor )
    {
      clipRootActor.Add( mRenderableActor );
    }
    else
    {
      self.Add( mRenderableActor );
    }
  }
}

void TextEditor::OnKeyInputFocusGained()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnKeyInputFocusGained %p\n", mController.Get() );

  VirtualKeyboard::StatusChangedSignal().Connect( this, &TextEditor::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();

  if ( imfManager )
  {
    imfManager.EventReceivedSignal().Connect( this, &TextEditor::OnImfEvent );

    // Notify that the text editing start.
    imfManager.Activate();

    // When window gain lost focus, the imf manager is deactivated. Thus when window gain focus again, the imf manager must be activated.
    imfManager.SetRestoreAfterFocusLost( true );
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

  VirtualKeyboard::StatusChangedSignal().Disconnect( this, &TextEditor::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    // The text editing is finished. Therefore the imf manager don't have restore activation.
    imfManager.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    imfManager.Deactivate();

    imfManager.EventReceivedSignal().Disconnect( this, &TextEditor::OnImfEvent );
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

  // Show the keyboard if it was hidden.
  if (!VirtualKeyboard::IsVisible())
  {
    VirtualKeyboard::Show();
  }

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  mController->TapEvent( gesture.numberOfTaps, gesture.localPoint.x, gesture.localPoint.y );

  SetKeyInputFocus();
}

void TextEditor::OnPan( const PanGesture& gesture )
{
  mController->PanEvent( gesture.state, gesture.displacement );
}

void TextEditor::OnLongPress( const LongPressGesture& gesture )
{
  // Show the keyboard if it was hidden.
  if (!VirtualKeyboard::IsVisible())
  {
    VirtualKeyboard::Show();
  }

  mController->LongPressEvent( gesture.state, gesture.localPoint.x, gesture.localPoint.y );

  SetKeyInputFocus();
}

bool TextEditor::OnKeyEvent( const KeyEvent& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnKeyEvent %p keyCode %d\n", mController.Get(), event.keyCode );

  if( Dali::DALI_KEY_ESCAPE == event.keyCode ) // Make a Dali key code for this
  {
    ClearKeyInputFocus();
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
  // Nothing to do as TextEditor doesn't emit a max length reached signal.
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
      Self().Add( actor );
    }
  }
}

void TextEditor::OnStageConnect( Dali::Actor actor )
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

ImfManager::ImfCallbackData TextEditor::OnImfEvent( Dali::ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextEditor::OnImfEvent %p eventName %d\n", mController.Get(), imfEvent.eventName );
  return mController->OnImfEvent( imfManager, imfEvent );
}

void TextEditor::GetHandleImagePropertyValue(  Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType )
{
  if( mDecorator )
  {
    ResourceImage image = ResourceImage::DownCast( mDecorator->GetHandleImage( handleType, handleImageType ) );

    if ( image )
    {
      Property::Map map;
      Scripting::CreatePropertyMap( image, map );
      value = map;
    }
  }
}

void TextEditor::EnableClipping( const Vector2& size )
{
  // Not worth to created clip actor if width or height is equal to zero.
  if( size.width > Math::MACHINE_EPSILON_1000 && size.height > Math::MACHINE_EPSILON_1000 )
  {
    if( !mClipper )
    {
      Actor self = Self();

      mClipper = Clipper::New( size );
      self.Add( mClipper->GetRootActor() );
      self.Add( mClipper->GetImageView() );
    }
    else if ( mClipper )
    {
      mClipper->Refresh( size );
    }
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

void TextEditor::OnStageConnection( int depth )
{
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // Sets the depth to the visuals inside the text's decorator.
  mDecorator->SetTextDepth( depth );

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().
}

bool TextEditor::OnTouched( Actor actor, const TouchData& touch )
{
  return true;
}

void TextEditor::OnIdleSignal()
{
  // Emits the change of input style signals.
  mController->ProcessInputStyleChangedSignals();

  // Set the pointer to null as the callback manager deletes the callback after execute it.
  mIdleCallback = NULL;
}

TextEditor::TextEditor()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mIdleCallback( NULL ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mHasBeenStaged( false )
{
}

TextEditor::~TextEditor()
{
  mClipper.Reset();

  if( ( NULL != mIdleCallback ) && Adaptor::IsAvailable() )
  {
    // Removes the callback from the callback manager in case the text-editor is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle( mIdleCallback );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
