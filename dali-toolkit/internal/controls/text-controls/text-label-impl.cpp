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
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/property-string-parser.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

#include <dali-toolkit/public-api/align-enumerations.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>

using namespace Dali::Toolkit::Text;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::DevelText::DEFAULT_RENDERING_BACKEND;

  /**
   * @brief How the text visual should be aligned vertically inside the control.
   *
   * 0.0f aligns the text to the top, 0.5f aligns the text to the center, 1.0f aligns the text to the bottom.
   * The alignment depends on the alignment value of the text label (Use Text::VerticalAlignment enumerations).
   */
  const float VERTICAL_ALIGNMENT_TABLE[ Text::VerticalAlignment::BOTTOM + 1 ] =
  {
    0.0f,  // VerticalAlignment::TOP
    0.5f,  // VerticalAlignment::CENTER
    1.0f   // VerticalAlignment::BOTTOM
  };

  const std::string TEXT_FIT_ENABLE_KEY( "enable" );
  const std::string TEXT_FIT_MIN_SIZE_KEY( "minSize" );
  const std::string TEXT_FIT_MAX_SIZE_KEY( "maxSize" );
  const std::string TEXT_FIT_STEP_SIZE_KEY( "stepSize" );
  const std::string TEXT_FIT_FONT_SIZE_TYPE_KEY( "fontSizeType" );
}

namespace
{

#if defined ( DEBUG_ENABLED )
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const Scripting::StringEnum AUTO_SCROLL_STOP_MODE_TABLE[] =
{
  { "IMMEDIATE", Toolkit::TextLabel::AutoScrollStopMode::IMMEDIATE },
  { "FINISH_LOOP",  Toolkit::TextLabel::AutoScrollStopMode::FINISH_LOOP  },
};
const unsigned int AUTO_SCROLL_STOP_MODE_TABLE_COUNT = sizeof( AUTO_SCROLL_STOP_MODE_TABLE ) / sizeof( AUTO_SCROLL_STOP_MODE_TABLE[0] );

// Type registration
BaseHandle Create()
{
  return Toolkit::TextLabel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextLabel, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "text",                      STRING,  TEXT                       )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "fontFamily",                STRING,  FONT_FAMILY                )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "fontStyle",                 MAP,     FONT_STYLE                 )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "pointSize",                 FLOAT,   POINT_SIZE                 )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "multiLine",                 BOOLEAN, MULTI_LINE                 )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "horizontalAlignment",       STRING,  HORIZONTAL_ALIGNMENT       )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "verticalAlignment",         STRING,  VERTICAL_ALIGNMENT         )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "enableMarkup",              BOOLEAN, ENABLE_MARKUP              )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "enableAutoScroll",          BOOLEAN, ENABLE_AUTO_SCROLL         )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "autoScrollSpeed",           INTEGER, AUTO_SCROLL_SPEED          )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "autoScrollLoopCount",       INTEGER, AUTO_SCROLL_LOOP_COUNT     )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "autoScrollGap",             FLOAT,   AUTO_SCROLL_GAP            )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "lineSpacing",               FLOAT,   LINE_SPACING               )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "underline",                 MAP,     UNDERLINE                  )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "shadow",                    MAP,     SHADOW                     )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "emboss",                    MAP,     EMBOSS                     )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "outline",                   MAP,     OUTLINE                    )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "pixelSize",                 FLOAT,   PIXEL_SIZE                 )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "ellipsis",                  BOOLEAN, ELLIPSIS                   )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "autoScrollLoopDelay",       FLOAT,   AUTO_SCROLL_LOOP_DELAY     )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "autoScrollStopMode",        STRING,  AUTO_SCROLL_STOP_MODE      )
DALI_PROPERTY_REGISTRATION_READ_ONLY( Toolkit, TextLabel, "lineCount",                 INTEGER, LINE_COUNT                 )
DALI_PROPERTY_REGISTRATION( Toolkit,           TextLabel, "lineWrapMode",              INTEGER, LINE_WRAP_MODE             )
DALI_DEVEL_PROPERTY_REGISTRATION_READ_ONLY( Toolkit, TextLabel, "textDirection",       INTEGER, TEXT_DIRECTION             )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "verticalLineAlignment",     INTEGER, VERTICAL_LINE_ALIGNMENT    )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "textBackground",            MAP,     BACKGROUND                 )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "ignoreSpacesAfterText",     BOOLEAN, IGNORE_SPACES_AFTER_TEXT   )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "matchSystemLanguageDirection", BOOLEAN, MATCH_SYSTEM_LANGUAGE_DIRECTION )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "textFit",                   MAP,     TEXT_FIT                   )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "minLineSize",               FLOAT,   MIN_LINE_SIZE              )
DALI_DEVEL_PROPERTY_REGISTRATION( Toolkit,     TextLabel, "renderingBackend",          INTEGER, RENDERING_BACKEND          )
DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT( Toolkit, TextLabel, "textColor",      Color::BLACK,     TEXT_COLOR     )
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit,    TextLabel, "textColorRed",   TEXT_COLOR_RED,   TEXT_COLOR, 0  )
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit,    TextLabel, "textColorGreen", TEXT_COLOR_GREEN, TEXT_COLOR, 1  )
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit,    TextLabel, "textColorBlue",  TEXT_COLOR_BLUE,  TEXT_COLOR, 2  )
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( Toolkit,    TextLabel, "textColorAlpha", TEXT_COLOR_ALPHA, TEXT_COLOR, 3  )
DALI_TYPE_REGISTRATION_END()

} // namespace

Toolkit::TextLabel TextLabel::New()
{
  // Create the implementation, temporarily owned by this handle on stack
  IntrusivePtr< TextLabel > impl = new TextLabel();

  // Pass ownership to CustomActor handle
  Toolkit::TextLabel handle( *impl );

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void TextLabel::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& impl( GetImpl( label ) );
    switch( index )
    {
      case Toolkit::DevelTextLabel::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
        if( DevelText::RENDERING_VECTOR_BASED == backend )
        {
          backend = TextAbstraction::BITMAP_GLYPH; // Fallback to bitmap-based rendering
        }
#endif
        if( impl.mRenderingBackend != backend )
        {
          impl.mRenderingBackend = backend;
          impl.mTextUpdateNeeded = true;

          if( impl.mController )
          {
            // When using the vector-based rendering, the size of the GLyphs are different
            TextAbstraction::GlyphType glyphType = (DevelText::RENDERING_VECTOR_BASED == impl.mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
            impl.mController->SetGlyphType( glyphType );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        if( impl.mController )
        {
          impl.mController->SetText( value.Get< std::string >() );
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          const std::string& fontFamily = value.Get< std::string >();

          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextLabel::SetProperty Property::FONT_FAMILY newFont(%s)\n", fontFamily.c_str() );
          impl.mController->SetDefaultFontFamily( fontFamily );
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_STYLE:
      {
        SetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          const float pointSize = value.Get< float >();

          if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE ), pointSize ) )
          {
            impl.mController->SetDefaultFontSize( pointSize, Text::Controller::POINT_SIZE );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        if( impl.mController )
        {
          impl.mController->SetMultiLineEnabled( value.Get< bool >() );
        }
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          Text::HorizontalAlignment::Type alignment( static_cast< Text::HorizontalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
          if( Text::GetHorizontalAlignmentEnumeration( value, alignment ) )
          {
            impl.mController->SetHorizontalAlignment( alignment );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
      {
        if( impl.mController )
        {
          Toolkit::Text::VerticalAlignment::Type alignment( static_cast< Text::VerticalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
          if( Text::GetVerticalAlignmentEnumeration( value, alignment ) )
          {
            impl.mController->SetVerticalAlignment( alignment );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          const bool enableMarkup = value.Get<bool>();
          impl.mController->SetMarkupProcessorEnabled( enableMarkup );
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL:
      {
        if( impl.mController )
        {
          const bool enableAutoScroll = value.Get<bool>();
          // If request to auto scroll is the same as current state then do nothing.
          if ( enableAutoScroll != impl.mController->IsAutoScrollEnabled() )
          {
             // If request is disable (false) and auto scrolling is enabled then need to stop it
             if ( enableAutoScroll == false )
             {
               if( impl.mTextScroller )
               {
                 impl.mTextScroller->StopScrolling();
               }
             }
             // If request is enable (true) then start autoscroll as not already running
             else
             {
               impl.mController->SetAutoScrollEnabled( enableAutoScroll );
             }
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE:
      {
        if( !impl.mTextScroller )
        {
          impl.mTextScroller = Text::TextScroller::New( impl );
        }
        Toolkit::TextLabel::AutoScrollStopMode::Type stopMode = impl.mTextScroller->GetStopMode();
        if( Scripting::GetEnumerationProperty< Toolkit::TextLabel::AutoScrollStopMode::Type >( value,
                                                                                                    AUTO_SCROLL_STOP_MODE_TABLE,
                                                                                                    AUTO_SCROLL_STOP_MODE_TABLE_COUNT,
                                                                                                    stopMode ) )
        {
            impl.mTextScroller->SetStopMode( stopMode );
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED:
      {
        if( !impl.mTextScroller )
        {
          impl.mTextScroller = Text::TextScroller::New( impl );
        }
        impl.mTextScroller->SetSpeed( value.Get<int>() );
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT:
      {
        if( !impl.mTextScroller )
        {
          impl.mTextScroller = Text::TextScroller::New( impl );
        }
        impl.mTextScroller->SetLoopCount( value.Get<int>() );
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY:
      {
         if( !impl.mTextScroller )
        {
          impl.mTextScroller = Text::TextScroller::New( impl );
        }
        impl.mTextScroller->SetLoopDelay( value.Get<float>() );
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_GAP:
      {
        if( !impl.mTextScroller )
        {
          impl.mTextScroller = Text::TextScroller::New( impl );
        }
        impl.mTextScroller->SetGap( value.Get<float>() );
        break;
      }
      case Toolkit::TextLabel::Property::LINE_SPACING:
      {
        if( impl.mController )
        {
          const float lineSpacing = value.Get<float>();

          // Don't trigger anything if the line spacing didn't change
          if( impl.mController->SetDefaultLineSpacing( lineSpacing ) )
          {
            impl.mTextUpdateNeeded = true;
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::TextLabel::Property::PIXEL_SIZE:
      {
        if( impl.mController )
        {
          const float pixelSize = value.Get< float >();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel %p PIXEL_SIZE %f\n", impl.mController.Get(), pixelSize );

          if( !Equals( impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE ), pixelSize ) )
          {
            impl.mController->SetDefaultFontSize( pixelSize, Text::Controller::PIXEL_SIZE );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::ELLIPSIS:
      {
        if( impl.mController )
        {
          const bool ellipsis = value.Get<bool>();
          DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel %p ELLIPSIS %d\n", impl.mController.Get(), ellipsis );

          impl.mController->SetTextElideEnabled( ellipsis );
        }
        break;
      }
      case Toolkit::TextLabel::Property::LINE_WRAP_MODE:
      {
        if( impl.mController )
        {
          Text::LineWrap::Mode lineWrapMode( static_cast< Text::LineWrap::Mode >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
          if( GetLineWrapModeEnumeration( value, lineWrapMode ) )
          {
            DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel %p LineWrap::MODE %d\n", impl.mController.Get(), lineWrapMode );
            impl.mController->SetLineWrapMode( lineWrapMode );
          }
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT:
      {
        if( impl.mController && impl.mController->GetTextModel() )
        {
          DevelText::VerticalLineAlignment::Type alignment = static_cast<DevelText::VerticalLineAlignment::Type>( value.Get<int>() );

          impl.mController->SetVerticalLineAlignment( alignment );

          // Property doesn't affect the layout, only Visual must be updated
          TextVisual::EnableRendererUpdate( impl.mVisual );

          // No need to trigger full re-layout. Instead call UpdateRenderer() directly
          TextVisual::UpdateRenderer( impl.mVisual );
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::BACKGROUND:
      {
        const bool update = SetBackgroundProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mTextUpdateNeeded = true;
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::IGNORE_SPACES_AFTER_TEXT:
      {
        impl.mController->SetIgnoreSpacesAfterText(value.Get< bool >());
        break;
      }
      case Toolkit::DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        impl.mController->SetMatchSystemLanguageDirection(value.Get< bool >());
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_FIT:
      {
        const Property::Map& propertiesMap = value.Get<Property::Map>();

        bool enabled = false;
        float minSize = 0.f;
        float maxSize = 0.f;
        float stepSize = 0.f;
        bool isMinSizeSet = false, isMaxSizeSet = false, isStepSizeSet = false;
        Controller::FontSizeType type = Controller::FontSizeType::POINT_SIZE;

        if ( !propertiesMap.Empty() )
        {
          const unsigned int numberOfItems = propertiesMap.Count();

          // Parses and applies
          for( unsigned int index = 0u; index < numberOfItems; ++index )
          {
            const KeyValuePair& valueGet = propertiesMap.GetKeyValue( index );

            if( ( Controller::TextFitInfo::Property::TEXT_FIT_ENABLE == valueGet.first.indexKey ) || ( TEXT_FIT_ENABLE_KEY == valueGet.first.stringKey ) )
            {
              /// Enable key.
              enabled = valueGet.second.Get< bool >();
            }
            else if( ( Controller::TextFitInfo::Property::TEXT_FIT_MIN_SIZE == valueGet.first.indexKey ) || ( TEXT_FIT_MIN_SIZE_KEY == valueGet.first.stringKey ) )
            {
              /// min size.
              minSize = valueGet.second.Get< float >();
              isMinSizeSet = true;
            }
            else if( ( Controller::TextFitInfo::Property::TEXT_FIT_MAX_SIZE == valueGet.first.indexKey ) || ( TEXT_FIT_MAX_SIZE_KEY == valueGet.first.stringKey ) )
            {
              /// max size.
              maxSize = valueGet.second.Get< float >();
              isMaxSizeSet = true;
            }
            else if( ( Controller::TextFitInfo::Property::TEXT_FIT_STEP_SIZE == valueGet.first.indexKey ) || ( TEXT_FIT_STEP_SIZE_KEY == valueGet.first.stringKey ) )
            {
              /// step size.
              stepSize = valueGet.second.Get< float >();
              isStepSizeSet = true;
            }
            else if( ( Controller::TextFitInfo::Property::TEXT_FIT_FONT_SIZE_TYPE == valueGet.first.indexKey ) || ( TEXT_FIT_FONT_SIZE_TYPE_KEY == valueGet.first.stringKey ) )
            {
              if( "pixelSize" == valueGet.second.Get< std::string >() )
              {
                type = Controller::FontSizeType::PIXEL_SIZE;
              }
            }
          }

          impl.mController->SetTextFitEnabled( enabled );
          if( isMinSizeSet )
          {
            impl.mController->SetTextFitMinSize( minSize, type );
          }
          if( isMaxSizeSet )
          {
            impl.mController->SetTextFitMaxSize( maxSize, type );
          }
          if( isStepSizeSet )
          {
            impl.mController->SetTextFitStepSize( stepSize, type );
          }
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::MIN_LINE_SIZE:
      {
        if( impl.mController )
        {
          const float lineSize = value.Get<float>();

          if( impl.mController->SetDefaultLineSize( lineSize ) )
          {
            impl.mTextUpdateNeeded = true;
          }
        }
        break;
      }
    }

    // Request relayout when text update is needed. It's necessary to call it
    // as changing the property not via UI interaction brings no effect if only
    // the mTextUpdateNeeded is changed.
    if( impl.mTextUpdateNeeded )
    {
      // need to request relayout as size of text may have changed
      impl.RequestTextRelayout();
    }
  }
}

Property::Value TextLabel::GetProperty( BaseObject* object, Property::Index index )
{
  Property::Value value;

  Toolkit::TextLabel label = Toolkit::TextLabel::DownCast( Dali::BaseHandle( object ) );

  if( label )
  {
    TextLabel& impl( GetImpl( label ) );
    switch( index )
    {
      case Toolkit::DevelTextLabel::Property::RENDERING_BACKEND:
      {
        value = impl.mRenderingBackend;
        break;
      }
      case Toolkit::TextLabel::Property::TEXT:
      {
        if( impl.mController )
        {
          std::string text;
          impl.mController->GetText( text );
          value = text;
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_FAMILY:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontFamily();
        }
        break;
      }
      case Toolkit::TextLabel::Property::FONT_STYLE:
      {
        GetFontStyleProperty( impl.mController, value, Text::FontStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::POINT_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontSize( Text::Controller::POINT_SIZE );
        }
        break;
      }
      case Toolkit::TextLabel::Property::MULTI_LINE:
      {
        if( impl.mController )
        {
          value = impl.mController->IsMultiLineEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT:
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
      case Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT:
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
      case Toolkit::TextLabel::Property::ENABLE_MARKUP:
      {
        if( impl.mController )
        {
          value = impl.mController->IsMarkupProcessorEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::ENABLE_AUTO_SCROLL:
      {
        if( impl.mController )
        {
          value = impl.mController->IsAutoScrollEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_STOP_MODE:
      {
        if( impl.mTextScroller )
        {
          const char* mode = Scripting::GetEnumerationName< Toolkit::TextLabel::AutoScrollStopMode::Type >( impl.mTextScroller->GetStopMode(),
                                                                                                                 AUTO_SCROLL_STOP_MODE_TABLE,
                                                                                                                 AUTO_SCROLL_STOP_MODE_TABLE_COUNT );
          if( mode )
          {
            value = std::string( mode );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_SPEED:
      {
        TextLabel& impl( GetImpl( label ) );
        if ( impl.mTextScroller )
        {
          value = impl.mTextScroller->GetSpeed();
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_COUNT:
      {
        if( impl.mController )
        {
          TextLabel& impl( GetImpl( label ) );
          if ( impl.mTextScroller )
          {
            value = impl.mTextScroller->GetLoopCount();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_LOOP_DELAY:
      {
        if( impl.mController )
        {
          TextLabel& impl( GetImpl( label ) );
          if ( impl.mTextScroller )
          {
            value = impl.mTextScroller->GetLoopDelay();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::AUTO_SCROLL_GAP:
      {
        TextLabel& impl( GetImpl( label ) );
        if ( impl.mTextScroller )
        {
          value = impl.mTextScroller->GetGap();
        }
        break;
      }
      case Toolkit::TextLabel::Property::LINE_SPACING:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultLineSpacing();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE:
      {
        GetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW:
      {
        GetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::EMBOSS:
      {
        GetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::OUTLINE:
      {
        GetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::TextLabel::Property::PIXEL_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultFontSize( Text::Controller::PIXEL_SIZE );
        }
        break;
      }
      case Toolkit::TextLabel::Property::ELLIPSIS:
      {
        if( impl.mController )
        {
          value = impl.mController->IsTextElideEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::LINE_WRAP_MODE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetLineWrapMode();
        }
        break;
      }
      case Toolkit::TextLabel::Property::LINE_COUNT:
      {
        if( impl.mController )
        {
          float width = label.GetProperty( Actor::Property::SIZE_WIDTH ).Get<float>();
          value = impl.mController->GetLineCount( width );
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_DIRECTION:
      {
        if( impl.mController )
        {
          value = impl.mController->GetTextDirection();
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::VERTICAL_LINE_ALIGNMENT:
      {
        if( impl.mController )
        {
          value = impl.mController->GetVerticalLineAlignment();
        }
        break;
      }
      case Toolkit::DevelTextLabel::Property::BACKGROUND:
      {
        GetBackgroundProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        break;
      }
      case Toolkit::DevelTextLabel::Property::IGNORE_SPACES_AFTER_TEXT:
      {
        value = impl.mController->IsIgnoreSpacesAfterText();
        break;
      }
      case Toolkit::DevelTextLabel::Property::MATCH_SYSTEM_LANGUAGE_DIRECTION:
      {
        value = impl.mController->IsMatchSystemLanguageDirection();
        break;
      }
      case Toolkit::DevelTextLabel::Property::TEXT_FIT:
      {
        const bool enabled = impl.mController->IsTextFitEnabled();
        const float minSize = impl.mController->GetTextFitMinSize();
        const float maxSize = impl.mController->GetTextFitMaxSize();
        const float stepSize = impl.mController->GetTextFitStepSize();

        Property::Map map;
        map.Insert( TEXT_FIT_ENABLE_KEY, enabled );
        map.Insert( TEXT_FIT_MIN_SIZE_KEY, minSize );
        map.Insert( TEXT_FIT_MAX_SIZE_KEY, maxSize );
        map.Insert( TEXT_FIT_STEP_SIZE_KEY, stepSize );
        map.Insert( TEXT_FIT_FONT_SIZE_TYPE_KEY, "pointSize" );

        value = map;
        break;
      }
      case Toolkit::DevelTextLabel::Property::MIN_LINE_SIZE:
      {
        if( impl.mController )
        {
          value = impl.mController->GetDefaultLineSize();
        }
        break;
      }
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  Actor self = Self();

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT );

  mVisual =  Toolkit::VisualFactory::Get().CreateVisual( propertyMap );
  DevelControl::RegisterVisual( *this, Toolkit::TextLabel::Property::TEXT, mVisual  );

  TextVisual::SetAnimatableTextColorProperty( mVisual, Toolkit::TextLabel::Property::TEXT_COLOR );

  mController = TextVisual::GetController(mVisual);
  if( mController )
  {
    mController->SetControlInterface(this);
  }

  // Use height-for-width negotiation by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

  // Enable the text ellipsis.
  mController->SetTextElideEnabled( true );   // If false then text larger than control will overflow

  // Sets layoutDirection value
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>( stage.GetRootLayer().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  mController->SetLayoutDirection( layoutDirection );

  Layout::Engine& engine = mController->GetLayoutEngine();
  engine.SetCursorWidth( 0u ); // Do not layout space for the cursor.
}

void TextLabel::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextLabel::OnStyleChange\n");

  switch ( change )
  {
    case StyleChange::DEFAULT_FONT_CHANGE:
    {
      // Property system did not set the font so should update it.
      const std::string& newFont = GetImpl( styleManager ).GetDefaultFontFamily();
      DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnStyleChange StyleChange::DEFAULT_FONT_CHANGE newFont(%s)\n", newFont.c_str() );
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

Vector3 TextLabel::GetNaturalSize()
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += ( padding.start + padding.end );
  naturalSize.height += ( padding.top + padding.bottom );

  return naturalSize;
}

float TextLabel::GetHeightForWidth( float width )
{
  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  return mController->GetHeightForWidth( width ) + padding.top + padding.bottom;
}

void TextLabel::OnPropertySet( Property::Index index, const Property::Value& propertyValue )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "TextLabel::OnPropertySet index[%d]\n", index );

  switch ( index )
  {
    case Toolkit::TextLabel::Property::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get< Vector4 >();
      if( mController->GetDefaultColor() != textColor )
      {
         mController->SetDefaultColor( textColor );
         mTextUpdateNeeded = true;
      }
      break;
    }
    default:
    {
      Control::OnPropertySet( index, propertyValue ); // up call to control for non-handled properties
      break;
    }
  }
}

void TextLabel::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnRelayout\n" );

  Extents padding;
  padding = Self().GetProperty<Extents>( Toolkit::Control::Property::PADDING );

  Vector2 contentSize( size.x - ( padding.start + padding.end ), size.y - ( padding.top + padding.bottom ) );

  if( mController->IsTextFitEnabled() )
  {
    mController->FitPointSizeforLayout( contentSize );
    mController->SetTextFitContentSize( contentSize );
  }

  // Support Right-To-Left
  Dali::LayoutDirection::Type layoutDirection;
  if( mController->IsMatchSystemLanguageDirection() )
  {
    layoutDirection = static_cast<Dali::LayoutDirection::Type>( DevelWindow::Get( Self() ).GetRootLayer().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  }
  else
  {
    layoutDirection = static_cast<Dali::LayoutDirection::Type>( Self().GetProperty( Dali::Actor::Property::LAYOUT_DIRECTION ).Get<int>() );
  }
  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( contentSize, layoutDirection );

  if( ( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) )
     || mTextUpdateNeeded )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnRelayout IsAutoScrollEnabled[%s] [%p]\n", ( mController->IsAutoScrollEnabled())?"true":"false", this );

    // Update the visual
    TextVisual::EnableRendererUpdate( mVisual );

    // Support Right-To-Left of padding
    if( Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection )
    {
      std::swap( padding.start, padding.end );
    }

    // Calculate the size of the visual that can fit the text
    Size layoutSize = mController->GetTextModel()->GetLayoutSize();
    layoutSize.x = contentSize.x;

    const Vector2& shadowOffset = mController->GetTextModel()->GetShadowOffset();
    if ( shadowOffset.y > Math::MACHINE_EPSILON_1 )
    {
      layoutSize.y += shadowOffset.y;
    }

    float outlineWidth = mController->GetTextModel()->GetOutlineWidth();
    layoutSize.y += outlineWidth * 2.0f;
    layoutSize.y = std::min( layoutSize.y, contentSize.y );

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y = ( contentSize.y - layoutSize.y ) * VERTICAL_ALIGNMENT_TABLE[mController->GetVerticalAlignment()];

    Property::Map visualTransform;
    visualTransform.Add( Toolkit::Visual::Transform::Property::SIZE, layoutSize )
                   .Add( Toolkit::Visual::Transform::Property::SIZE_POLICY, Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ) )
                   .Add( Toolkit::Visual::Transform::Property::OFFSET, Vector2( padding.start, padding.top ) + alignmentOffset )
                   .Add( Toolkit::Visual::Transform::Property::OFFSET_POLICY, Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ) )
                   .Add( Toolkit::Visual::Transform::Property::ORIGIN, Toolkit::Align::TOP_BEGIN )
                   .Add( Toolkit::Visual::Transform::Property::ANCHOR_POINT, Toolkit::Align::TOP_BEGIN );
    mVisual.SetTransformAndSize( visualTransform, size );

    if ( mController->IsAutoScrollEnabled() )
    {
      SetUpAutoScrolling();
    }

    mTextUpdateNeeded = false;
  }
}

void TextLabel::RequestTextRelayout()
{
  RelayoutRequest();
  // Signal that a Relayout may be needed
}

void TextLabel::SetUpAutoScrolling()
{
  const Size& controlSize = mController->GetView().GetControlSize();
  const Size textNaturalSize = GetNaturalSize().GetVectorXY(); // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
  const Text::CharacterDirection direction = mController->GetAutoScrollDirection();

  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling textNaturalSize[%f,%f] controlSize[%f,%f]\n",
                 textNaturalSize.x,textNaturalSize.y , controlSize.x,controlSize.y );

  if ( !mTextScroller )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling Creating default TextScoller\n" );

    // If speed, loopCount or gap not set via property system then will need to create a TextScroller with defaults
    mTextScroller = Text::TextScroller::New( *this );
  }

  // Calculate the actual gap before scrolling wraps.
  int textPadding = std::max( controlSize.x - textNaturalSize.x, 0.0f );
  float wrapGap = std::max( mTextScroller->GetGap(), textPadding );
  Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture

  // Create a texture of the text for scrolling
  Size verifiedSize = textureSize;
  const int maxTextureSize = Dali::GetMaxTextureSize();

  //if the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis
  bool actualellipsis = mController->IsTextElideEnabled();
  if( verifiedSize.width > maxTextureSize )
  {
    verifiedSize.width = maxTextureSize;
    if( textNaturalSize.width > maxTextureSize )
    {
      mController->SetTextElideEnabled( true );
    }
    GetHeightForWidth( maxTextureSize );
    wrapGap = std::max( maxTextureSize - textNaturalSize.width, 0.0f );
  }

  Text::TypesetterPtr typesetter = Text::Typesetter::New( mController->GetTextModel() );

  PixelData data = typesetter->Render( verifiedSize, mController->GetTextDirection(), Text::Typesetter::RENDER_TEXT_AND_STYLES, true, Pixel::RGBA8888 ); // ignore the horizontal alignment
  Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                  data.GetPixelFormat(),
                                  data.GetWidth(),
                                  data.GetHeight() );
  texture.Upload( data );

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture( 0u, texture );

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );
  sampler.SetWrapMode( Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT ); // Wrap the texture in the x direction
  textureSet.SetSampler( 0u, sampler );

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>( GetImplementation( mVisual ) ).GetRenderer();
  mTextScroller->SetParameters( Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, direction, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment() );
  mController->SetTextElideEnabled( actualellipsis );
}

void TextLabel::ScrollingFinished()
{
  // Pure Virtual from TextScroller Interface
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::ScrollingFinished\n");
  mController->SetAutoScrollEnabled( false );
  RequestTextRelayout();
}

TextLabel::TextLabel()
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mTextUpdateNeeded( false )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
