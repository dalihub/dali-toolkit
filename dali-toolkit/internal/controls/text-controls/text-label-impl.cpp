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
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/property-string-parser.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>

using Dali::Toolkit::Text::LayoutEngine;
using Dali::Toolkit::Text::Backend;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const unsigned int DEFAULT_RENDERING_BACKEND = Dali::Toolkit::Text::DEFAULT_RENDERING_BACKEND;
}

namespace
{

#if defined ( DEBUG_ENABLED )
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

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
  return Toolkit::TextLabel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN( Toolkit::TextLabel, Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "renderingBackend",     INTEGER, RENDERING_BACKEND      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "text",                 STRING,  TEXT                   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "fontFamily",           STRING,  FONT_FAMILY            )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "fontStyle",            STRING,  FONT_STYLE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "pointSize",            FLOAT,   POINT_SIZE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "multiLine",            BOOLEAN, MULTI_LINE             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "horizontalAlignment",  STRING,  HORIZONTAL_ALIGNMENT   )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "verticalAlignment",    STRING,  VERTICAL_ALIGNMENT     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "textColor",            VECTOR4, TEXT_COLOR             )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadowOffset",         VECTOR2, SHADOW_OFFSET          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadowColor",          VECTOR4, SHADOW_COLOR           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineEnabled",     BOOLEAN, UNDERLINE_ENABLED      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineColor",       VECTOR4, UNDERLINE_COLOR        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underlineHeight",      FLOAT,   UNDERLINE_HEIGHT       )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "enableMarkup",         BOOLEAN, ENABLE_MARKUP          )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "enableAutoScroll",     BOOLEAN, ENABLE_AUTO_SCROLL     )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollSpeed",      INTEGER, AUTO_SCROLL_SPEED      )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollLoopCount",  INTEGER, AUTO_SCROLL_LOOP_COUNT )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "autoScrollGap",        FLOAT,   AUTO_SCROLL_GAP        )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "lineSpacing",          FLOAT,   LINE_SPACING           )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "underline",            STRING,  UNDERLINE              )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "shadow",               STRING,  SHADOW                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "emboss",               STRING,  EMBOSS                 )
DALI_PROPERTY_REGISTRATION( Toolkit, TextLabel, "outline",              STRING,  OUTLINE                )

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
      case Toolkit::TextLabel::Property::RENDERING_BACKEND:
      {
        int backend = value.Get< int >();

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
        if( Text::RENDERING_VECTOR_BASED == backend )
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
            TextAbstraction::GlyphType glyphType = (Text::RENDERING_VECTOR_BASED == impl.mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
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
          const std::string fontFamily = value.Get< std::string >();

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

          if( !Equals( impl.mController->GetDefaultPointSize(), pointSize ) )
          {
            impl.mController->SetDefaultPointSize( pointSize );
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
          LayoutEngine::HorizontalAlignment alignment( LayoutEngine::HORIZONTAL_ALIGN_BEGIN );
          if( Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::HorizontalAlignment >( value.Get< std::string >().c_str(),
                                                                                             HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                             HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                                             alignment ) )
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
          LayoutEngine::VerticalAlignment alignment( LayoutEngine::VERTICAL_ALIGN_BOTTOM );
          if( Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::VerticalAlignment >( value.Get< std::string >().c_str(),
                                                                                           VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                           VERTICAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                                           alignment ) )
          {
            impl.mController->SetVerticalAlignment( alignment );
          }
        }
        break;
      }

      case Toolkit::TextLabel::Property::TEXT_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 textColor = value.Get< Vector4 >();
          if( impl.mController->GetTextColor() != textColor )
          {
            impl.mController->SetTextColor( textColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }

      case Toolkit::TextLabel::Property::SHADOW_OFFSET:
      {
        if( impl.mController )
        {
          const Vector2 shadowOffset = value.Get< Vector2 >();
          if ( impl.mController->GetShadowOffset() != shadowOffset )
          {
            impl.mController->SetShadowOffset( shadowOffset );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 shadowColor = value.Get< Vector4 >();
          if ( impl.mController->GetShadowColor() != shadowColor )
          {
            impl.mController->SetShadowColor( shadowColor );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_COLOR:
      {
        if( impl.mController )
        {
          const Vector4 color = value.Get< Vector4 >();
          if ( impl.mController->GetUnderlineColor() != color )
          {
            impl.mController->SetUnderlineColor( color );
            impl.mRenderer.Reset();
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_ENABLED:
      {
        if( impl.mController )
        {
          const bool enabled = value.Get< bool >();
          if ( impl.mController->IsUnderlineEnabled() != enabled )
          {
            impl.mController->SetUnderlineEnabled( enabled );
            impl.mRenderer.Reset();
          }
        }
        break;
      }

      case Toolkit::TextLabel::Property::UNDERLINE_HEIGHT:
      {
        if( impl.mController )
        {
          float height = value.Get< float >();
          if( fabsf( impl.mController->GetUnderlineHeight() - height ) > Math::MACHINE_EPSILON_1000 )
          {
            impl.mController->SetUnderlineHeight( height );
            impl.mRenderer.Reset();
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
                 impl.mTextScroller->SetLoopCount( 0 ); // Causes the current animation to finish playing (0)
               }
             }
             // If request is enable (true) then start autoscroll as not already running
             else
             {
               impl.mController->GetLayoutEngine().SetTextEllipsisEnabled( false );
               impl.mController->SetAutoScrollEnabled( enableAutoScroll );
             }
          }
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
          impl.mController->SetDefaultLineSpacing( lineSpacing );
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE:
      {
        const bool update = SetUnderlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW:
      {
        const bool update = SetShadowProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::EMBOSS:
      {
        const bool update = SetEmbossProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::OUTLINE:
      {
        const bool update = SetOutlineProperties( impl.mController, value, Text::EffectStyle::DEFAULT );
        if( update )
        {
          impl.mRenderer.Reset();
        }
        break;
      }
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
      case Toolkit::TextLabel::Property::RENDERING_BACKEND:
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
          value = impl.mController->GetDefaultPointSize();
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
          const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::HorizontalAlignment >( impl.mController->GetHorizontalAlignment(),
                                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );
          if( name )
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
          const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( impl.mController->GetVerticalAlignment(),
                                                                                                              VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                              VERTICAL_ALIGNMENT_STRING_TABLE_COUNT );
          if( name )
          {
            value = std::string( name );
          }
        }
        break;
      }
      case Toolkit::TextLabel::Property::TEXT_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetTextColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_OFFSET:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowOffset();
        }
        break;
      }
      case Toolkit::TextLabel::Property::SHADOW_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetShadowColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_COLOR:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetUnderlineColor();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_ENABLED:
      {
        if ( impl.mController )
        {
          value = impl.mController->IsUnderlineEnabled();
        }
        break;
      }
      case Toolkit::TextLabel::Property::UNDERLINE_HEIGHT:
      {
        if ( impl.mController )
        {
          value = impl.mController->GetUnderlineHeight();
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
    }
  }

  return value;
}

void TextLabel::OnInitialize()
{
  Actor self = Self();

  mController = Text::Controller::New( *this );

  // When using the vector-based rendering, the size of the GLyphs are different
  TextAbstraction::GlyphType glyphType = (Text::RENDERING_VECTOR_BASED == mRenderingBackend) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
  mController->SetGlyphType( glyphType );

  // Use height-for-width negotiation by default
  self.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  self.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

  // Enable the text ellipsis.
  LayoutEngine& engine = mController->GetLayoutEngine();

  engine.SetTextEllipsisEnabled( true );   // If false then text larger than control will overflow
  engine.SetCursorWidth( 0u ); // Do not layout space for the cursor.

  self.OnStageSignal().Connect( this, &TextLabel::OnStageConnect );
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

Vector3 TextLabel::GetNaturalSize()
{
  return mController->GetNaturalSize();
}

float TextLabel::GetHeightForWidth( float width )
{
  return mController->GetHeightForWidth( width );
}

void TextLabel::OnRelayout( const Vector2& size, RelayoutContainer& container )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::OnRelayout\n" );

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( size );

  if( ( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) ) ||
      !mRenderer )
  {
    if( !mRenderer )
    {
      mRenderer = Backend::Get().NewRenderer( mRenderingBackend );
    }
    RenderText();
  }
}

void TextLabel::RequestTextRelayout()
{
  RelayoutRequest();
}

void TextLabel::RenderText()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::RenderText IsAutoScrollEnabled[%s] [%p]\n", ( mController->IsAutoScrollEnabled())?"true":"false", this );

  Actor self = Self();
  Actor renderableActor;

  if( mRenderer )
  {
    renderableActor = mRenderer->Render( mController->GetView(), DepthIndex::TEXT );
  }

  if( renderableActor != mRenderableActor )
  {
    UnparentAndReset( mRenderableActor );

    if( renderableActor )
    {
      const Vector2& scrollOffset = mController->GetScrollPosition();
      renderableActor.SetPosition( scrollOffset.x, scrollOffset.y );

      self.Add( renderableActor );
    }
    mRenderableActor = renderableActor;

    if ( mController->IsAutoScrollEnabled() )
    {
      SetUpAutoScrolling();
    }
  }
}

void TextLabel::SetUpAutoScrolling()
{
  const Size& controlSize = mController->GetView().GetControlSize();
  const Size offScreenSize = GetNaturalSize().GetVectorXY(); // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
  const float alignmentOffset = mController->GetAutoScrollLineAlignment();
  const Text::CharacterDirection direction = mController->GetAutoScrollDirection();

  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling alignmentOffset[%f] offScreenSize[%f,%f] controlSize[%f,%f]\n",
                 alignmentOffset, offScreenSize.x,offScreenSize.y , controlSize.x,controlSize.y );

  if ( !mTextScroller )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::SetUpAutoScrolling Creating default TextScoller\n" );

    // If speed, loopCount or gap not set via property system then will need to create a TextScroller with defaults
    mTextScroller = Text::TextScroller::New( *this );
  }
  mTextScroller->SetParameters( mRenderableActor, controlSize, offScreenSize, direction, alignmentOffset );

  Actor self = Self();
  self.Add( mTextScroller->GetScrollingText() );
  self.Add( mTextScroller->GetSourceCamera() );
}

void TextLabel::OnStageConnect( Dali::Actor actor )
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

void TextLabel::AddDecoration( Actor& actor, bool needsClipping )
{
  // TextLabel does not show decorations
}

void TextLabel::OnStageConnection( int depth )
{
  // Call the Control::OnStageConnection() to set the depth of the background.
  Control::OnStageConnection( depth );

  // The depth of the text renderer is set in the RenderText() called from OnRelayout().
}

void TextLabel::TextChanged()
{
  // TextLabel does not provide a signal for this.
}

void TextLabel::MaxLengthReached()
{
  // Pure Virtual from TextController Interface, only needed when inputting text
}

void TextLabel::InputStyleChanged( Text::InputStyle::Mask inputStyleMask )
{
  // TextLabel does not provide a signal for this.
}

void TextLabel::ScrollingFinished()
{
  // Pure Virtual from TextScroller Interface
  DALI_LOG_INFO( gLogFilter, Debug::General, "TextLabel::ScrollingFinished\n");
  mController->SetAutoScrollEnabled( false );
  mController->GetLayoutEngine().SetTextEllipsisEnabled( true );
  RequestTextRelayout();
}

TextLabel::TextLabel()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mRenderingBackend( DEFAULT_RENDERING_BACKEND ),
  mHasBeenStaged( false )
{
}

TextLabel::~TextLabel()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
