/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <string>
#include <iostream>
#include <cstring>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/devel-api/adaptor-framework/virtual-keyboard.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
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

const Scripting::StringEnum VERTICAL_ALIGNMENT_STRING_TABLE[] =
{
  { "TOP",    Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_TOP    },
  { "CENTER", Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_CENTER },
  { "BOTTOM", Toolkit::Text::LayoutEngine::VERTICAL_ALIGN_BOTTOM },
};
const unsigned int VERTICAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( VERTICAL_ALIGNMENT_STRING_TABLE ) / sizeof( VERTICAL_ALIGNMENT_STRING_TABLE[0] );

// Type registration
BaseHandle Create()
{
  return Toolkit::TextField::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextField, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "rendering-backend",                    INTEGER,   RENDERING_BACKEND                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "text",                                 STRING,    TEXT                                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholder-text",                     STRING,    PLACEHOLDER_TEXT                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholder-text-focused",             STRING,    PLACEHOLDER_TEXT_FOCUSED             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "font-family",                          STRING,    FONT_FAMILY                          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "font-style",                           STRING,    FONT_STYLE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "point-size",                           FLOAT,     POINT_SIZE                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "max-length",                           INTEGER,   MAX_LENGTH                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "exceed-policy",                        INTEGER,   EXCEED_POLICY                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "horizontal-alignment",                 STRING,    HORIZONTAL_ALIGNMENT                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "vertical-alignment",                   STRING,    VERTICAL_ALIGNMENT                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "text-color",                           VECTOR4,   TEXT_COLOR                           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "placeholder-text-color",               VECTOR4,   PLACEHOLDER_TEXT_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "shadow-offset",                        VECTOR2,   SHADOW_OFFSET                        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "shadow-color",                         VECTOR4,   SHADOW_COLOR                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "primary-cursor-color",                 VECTOR4,   PRIMARY_CURSOR_COLOR                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "secondary-cursor-color",               VECTOR4,   SECONDARY_CURSOR_COLOR               )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "enable-cursor-blink",                  BOOLEAN,   ENABLE_CURSOR_BLINK                  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursor-blink-interval",                FLOAT,     CURSOR_BLINK_INTERVAL                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursor-blink-duration",                FLOAT,     CURSOR_BLINK_DURATION                )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "cursor-width",                         INTEGER,   CURSOR_WIDTH                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grab-handle-image",                    STRING,    GRAB_HANDLE_IMAGE                    )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "grab-handle-pressed-image",            STRING,    GRAB_HANDLE_PRESSED_IMAGE            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scroll-threshold",                     FLOAT,     SCROLL_THRESHOLD                     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "scroll-speed",                         FLOAT,     SCROLL_SPEED                         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-image-left",          MAP,       SELECTION_HANDLE_IMAGE_LEFT          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-image-right",         MAP,       SELECTION_HANDLE_IMAGE_RIGHT         )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-pressed-image-left",  MAP,       SELECTION_HANDLE_PRESSED_IMAGE_LEFT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-pressed-image-right", MAP,       SELECTION_HANDLE_PRESSED_IMAGE_RIGHT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-marker-image-left",   MAP,       SELECTION_HANDLE_MARKER_IMAGE_LEFT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-handle-marker-image-right",  MAP,       SELECTION_HANDLE_MARKER_IMAGE_RIGHT  )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "selection-highlight-color",            VECTOR4,   SELECTION_HIGHLIGHT_COLOR            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "decoration-bounding-box",              RECTANGLE, DECORATION_BOUNDING_BOX              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextField, "input-method-settings",                MAP,       INPUT_METHOD_SETTINGS                )

DALI_SIGNAL_REGISTRATION( Toolkit, TextField, "text-changed",       SIGNAL_TEXT_CHANGED )
DALI_SIGNAL_REGISTRATION( Toolkit, TextField, "max-length-reached", SIGNAL_MAX_LENGTH_REACHED )

DALI_TYPE_REGISTRATION_END()

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
      case Toolkit::TextField::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p RENDERING_BACKEND %d\n", impl.mController.Get(), backend );

        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mRenderer.Reset();
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT:
      {
        if( impl.mController )
        {
          const std::string text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p TEXT %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetText( text );
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT:
      {
        if( impl.mController )
        {
          const std::string text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetPlaceholderText( PLACEHOLDER_TYPE_INACTIVE, text );
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
      {
        if( impl.mController )
        {
          const std::string text = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_FOCUSED %s\n", impl.mController.Get(), text.c_str() );

          impl.mController->SetPlaceholderText( PLACEHOLDER_TYPE_ACTIVE, text );
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string fontFamily = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p FONT_FAMILY %s\n", impl.mController.Get(), fontFamily.c_str() );

          if( impl.mController->GetDefaultFontFamily() != fontFamily )
          {
            impl.mController->SetDefaultFontFamily( fontFamily, true ); // "true" as SetProperty means user defined font so don't change when system font changes.
          }
        }
        break;
      }
      case Toolkit::TextField::Property::FONT_STYLE:
      {
        if( impl.mController )
        {
          const std::string fontStyle = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p FONT_STYLE %s\n", impl.mController.Get(), fontStyle.c_str() );

          if( impl.mController->GetDefaultFontStyle() != fontStyle )
          {
            impl.mController->SetDefaultFontStyle( fontStyle );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p POINT_SIZE %f\n", impl.mController.Get(), pointSize );

          if( !Equals( impl.mController->GetDefaultPointSize(), pointSize ) )
          {
            impl.mController->SetDefaultPointSize( pointSize );
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
        // TODO
        break;
      }
      case Toolkit::TextField::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const std::string alignStr = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p HORIZONTAL_ALIGNMENT %s\n", impl.mController.Get(), alignStr.c_str() );

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
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const std::string alignStr = value.Get< std::string >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p VERTICAL_ALIGNMENT %s\n", impl.mController.Get(), alignStr.c_str() );

          LayoutEngine::VerticalAlignment alignment( LayoutEngine::VERTICAL_ALIGN_BOTTOM );
          if( Scripting::GetEnumeration< LayoutEngine::VerticalAlignment >( alignStr.c_str(),
                                                                            VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                            VERTICAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                            alignment ) )
          {
            impl.mController->SetVerticalAlignment( alignment );
          }
        }
        break;
      }
      case Toolkit::TextField::Property::TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 textColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

          if( impl.mController->GetTextColor() != textColor )
          {
            impl.mController->SetTextColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 textColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p PLACEHOLDER_TEXT_COLOR %f,%f,%f,%f\n", impl.mController.Get(), textColor.r, textColor.g, textColor.b, textColor.a );

          if( impl.mController->GetPlaceholderTextColor() != textColor )
          {
            impl.mController->SetPlaceholderTextColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW_OFFSET:
      {
        if( impl.mController )
        {
          const Vector2 shadowOffset = value.Get< Vector2 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p SHADOW_OFFSET %f,%f\n", impl.mController.Get(), shadowOffset.x, shadowOffset.y );

          if ( impl.mController->GetShadowOffset() != shadowOffset )
          {
            impl.mController->SetShadowOffset( shadowOffset );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 shadowColor = value.Get< Vector4 >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextField %p SHADOW_COLOR %f,%f,%f,%f\n", impl.mController.Get(), shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a );

          if ( impl.mController->GetShadowColor() != shadowColor )
          {
            impl.mController->SetShadowColor( shadowColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::PRIMARY_CURSOR_COLOR:
      {
        if( impl.mDecorator )
        {
          const Vector4 color = value.Get< Vector4 >();
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
          const Vector4 color = value.Get< Vector4 >();
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
        const ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_IMAGE %s\n", impl.mController.Get(), image.GetUrl().c_str() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
      {
        const ResourceImage image = ResourceImage::New( value.Get< std::string >() );
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField %p GRAB_HANDLE_PRESSED_IMAGE %s\n", impl.mController.Get(), image.GetUrl().c_str() );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_PRESSED, image );
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
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_LEFT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );

        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE, HANDLE_IMAGE_PRESSED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_LEFT:
      {
        const Image image = Scripting::NewImage( value );
        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( LEFT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, image );
          impl.RequestTextRelayout();
        }
        break;
      }
      case Toolkit::TextField::Property::SELECTION_HANDLE_MARKER_IMAGE_RIGHT:
      {
        const Image image = Scripting::NewImage( value );
        if( impl.mDecorator )
        {
          impl.mDecorator->SetHandleImage( RIGHT_SELECTION_HANDLE_MARKER, HANDLE_IMAGE_RELEASED, image );
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
        const Property::Map map = value.Get<Property::Map>();
        VirtualKeyboard::ApplySettings( map );
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
      case Toolkit::TextField::Property::RENDERING_BACKEND:
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
          impl.mController->GetPlaceholderText( PLACEHOLDER_TYPE_INACTIVE, text );
          value = text;
        }
        break;
      }
      case Toolkit::TextField::Property::PLACEHOLDER_TEXT_FOCUSED:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetPlaceholderText( PLACEHOLDER_TYPE_ACTIVE, text );
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
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontStyle();
        }
        break;
      }
      case Toolkit::TextField::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultPointSize();
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
      case Toolkit::TextField::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( impl.mController->GetLayoutEngine().GetVerticalAlignment(),
                                                                                                              VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                              VERTICAL_ALIGNMENT_STRING_TABLE_COUNT );
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
          value = impl.mController->GetTextColor();
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
      case Toolkit::TextField::Property::SHADOW_OFFSET:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowOffset();
        }
        break;
      }
      case Toolkit::TextField::Property::SHADOW_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowColor();
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
          ResourceImage image = ResourceImage::DownCast( impl.mDecorator->GetHandleImage( GRAB_HANDLE, HANDLE_IMAGE_RELEASED ) );
          if( image )
          {
            value = image.GetUrl();
          }
        }
        break;
      }
      case Toolkit::TextField::Property::GRAB_HANDLE_PRESSED_IMAGE:
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
          value = impl.mDecorator->GetBoundingBox();
        }
        break;
      }
      case Toolkit::TextField::Property::INPUT_METHOD_SETTINGS:
      {
        break;
      }
    } //switch
  }

  return value;
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

void TextField::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( *this );

  mDecorator = Text::Decorator::New( *mController,
                                     *mController );

  mController->GetLayoutEngine().SetLayout( LayoutEngine::SINGLE_LINE_BOX );

  mController->EnableTextInput( mDecorator );

  // Forward input events to controller
  EnableGestureDetection( static_cast<Gesture::Type>( Gesture::Tap | Gesture::Pan |Gesture::LongPress ) );
  GetTapGestureDetector().SetMaximumTapsRequired( 2 );

  self.TouchedSignal().Connect( this, &TextField::OnTouched );

  // Set BoundingBox to stage size if not already set.
  if ( mDecorator->GetBoundingBox().IsEmpty() )
  {
    Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();
    mDecorator->SetBoundingBox( Rect<int>( 0.0f, 0.0f, stageSize.width, stageSize.height ) );
  }

  // Fill-parent area by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
  self.OnStageSignal().Connect( this, &TextField::OnStageConnect );
}

void TextField::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnStyleChange\n");

   switch ( change )
   {
     case StyleChange::DEFAULT_FONT_CHANGE:
     {
       DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnStyleChange DEFAULT_FONT_CHANGE\n");
       std::string newFont = styleManager.GetDefaultFontFamily();
       // Property system did not set the font so should update it.
       mController->UpdateAfterFontChange( newFont );
       break;
     }

     case StyleChange::DEFAULT_FONT_SIZE_CHANGE:
     {
       DALI_LOG_INFO( gLogFilter, Debug::General, "TextField::OnStyleChange StyleChange::DEFAULT_FONT_SIZE_CHANGE (%f)\n", mController->GetDefaultPointSize() );

       if ( (mController->GetDefaultPointSize() <= 0.0f) ) // If DefaultPointSize not set by Property system it will be 0.0f
       {
         // Property system did not set the PointSize so should update it.
         // todo instruct text-controller to update model
       }
       break;
     }
     case StyleChange::THEME_CHANGE:
     {
       GetImpl( styleManager ).ApplyThemeStyle( Toolkit::Control( GetOwner() ) );
       break;
     }
   }
}

Vector3 TextField::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextField::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextField::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField OnRelayout\n");

  if( mController->Relayout( size ) ||
      !mRenderer )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnRelayout %p Displaying new contents\n", mController.Get() );

    if( mDecorator )
    {
      mDecorator->Relayout( size );
    }

    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }

    EnableClipping( (Dali::Toolkit::TextField::EXCEED_POLICY_CLIP == mExceedPolicy), size );
    RenderText();
  }
}

void TextField::RenderText()
{
  Actor self = Self();
  Actor renderableActor;
  if( mRenderer )
  {
    renderableActor = mRenderer->Render( mController->GetView(), self.GetHierarchyDepth() );
  }

  if( renderableActor != mRenderableActor )
  {
    UnparentAndReset( mRenderableActor );
    mRenderableActor = renderableActor;
  }

  if( mRenderableActor )
  {
    const Vector2 offset = mController->GetScrollPosition() + mController->GetAlignmentOffset();

    mRenderableActor.SetPosition( offset.x, offset.y );

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

void TextField::OnKeyInputFocusGained()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnKeyInputFocusGained %p\n", mController.Get() );

  VirtualKeyboard::StatusChangedSignal().Connect( this, &TextField::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();

  if ( imfManager )
  {
    imfManager.EventReceivedSignal().Connect( this, &TextField::OnImfEvent );

    // Notify that the text editing start.
    imfManager.Activate();

    // When window gain lost focus, the imf manager is deactivated. Thus when window gain focus again, the imf manager must be activated.
    imfManager.SetRestoreAfterFocusLost( true );
  }

   ClipboardEventNotifier notifier( ClipboardEventNotifier::Get() );

   if ( notifier )
   {
      notifier.ContentSelectedSignal().Connect( this, &TextField::OnClipboardTextSelected );
   }

  mController->KeyboardFocusGainEvent(); // Called in the case of no virtual keyboard to trigger this event

  EmitKeyInputFocusSignal( true ); // Calls back into the Control hence done last.
}

void TextField::OnKeyInputFocusLost()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField:OnKeyInputFocusLost %p\n", mController.Get() );

  VirtualKeyboard::StatusChangedSignal().Disconnect( this, &TextField::KeyboardStatusChanged );

  ImfManager imfManager = ImfManager::Get();
  if ( imfManager )
  {
    // The text editing is finished. Therefore the imf manager don't have restore activation.
    imfManager.SetRestoreAfterFocusLost( false );

    // Notify that the text editing finish.
    imfManager.Deactivate();

    imfManager.EventReceivedSignal().Disconnect( this, &TextField::OnImfEvent );
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

  // Show the keyboard if it was hidden.
  if (!VirtualKeyboard::IsVisible())
  {
    VirtualKeyboard::Show();
  }

  // Deliver the tap before the focus event to controller; this allows us to detect when focus is gained due to tap-gestures
  mController->TapEvent( gesture.numberOfTaps, gesture.localPoint.x, gesture.localPoint.y );

  SetKeyInputFocus();
}

void TextField::OnPan( const PanGesture& gesture )
{
  mController->PanEvent( gesture.state, gesture.displacement );
}

void TextField::OnLongPress( const LongPressGesture& gesture )
{
  mController->LongPressEvent( gesture.state, gesture.localPoint.x, gesture.localPoint.y );
}

bool TextField::OnKeyEvent( const KeyEvent& event )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnKeyEvent %p keyCode %d\n", mController.Get(), event.keyCode );

  if( Dali::DALI_KEY_ESCAPE == event.keyCode ||
      "Return" == event.keyPressedName ) // Make a Dali key code for this
  {
    ClearKeyInputFocus();
    return true;
  }

  return mController->KeyEvent( event );
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
      Self().Add( actor );
    }
  }
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

void TextField::OnStageConnect( Dali::Actor actor )
{
  if ( mHasBeenStaged )
  {
    RenderText();
  }
  else
  {
    mHasBeenStaged = true;
  }
}

void TextField::MaxLengthReached()
{
  Dali::Toolkit::TextField handle( GetOwner() );
  mMaxLengthReachedSignal.Emit( handle );
}

ImfManager::ImfCallbackData TextField::OnImfEvent( Dali::ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextField::OnImfEvent %p eventName %d\n", mController.Get(), imfEvent.eventName );
  return mController->OnImfEvent( imfManager, imfEvent );
}

void TextField::GetHandleImagePropertyValue(  Property::Value& value, Text::HandleType handleType, Text::HandleImageType handleImageType )
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

void TextField::EnableClipping( bool clipping, const Vector2& size )
{
  if( clipping )
  {
    // Not worth to created clip actor if width or height is equal to zero.
    if( size.width > Math::MACHINE_EPSILON_1000 && size.height > Math::MACHINE_EPSILON_1000 )
    {
      if( !mClipper )
      {
        Actor self = Self();

        mClipper = Clipper::New( size );
        self.Add( mClipper->GetRootActor() );
        self.Add( mClipper->GetImageActor() );
      }
      else if ( mClipper )
      {
        mClipper->Refresh( size );
      }
    }
  }
  else
  {
    // Note - this will automatically remove the root & image actors
    mClipper.Reset();
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

void TextField::OnStageConnection( int depth )
{
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // Sets the depth to the renderers inside the text's decorator.
  mDecorator->SetTextDepth( depth );

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().
}

bool TextField::OnTouched( Actor actor, const TouchEvent& event )
{
  return true;
}

TextField::TextField()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mExceedPolicy( Dali::Toolkit::TextField::EXCEED_POLICY_CLIP ),
  mHasBeenStaged( false )
{
}

TextField::~TextField()
{
  mClipper.Reset();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
