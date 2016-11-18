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
#include <dali-toolkit/internal/visuals/text/text-visual.h>

// EXTERNAL HEADER
#include <dali/devel-api/scripting/enum-helper.h>
// #include <dali/devel-api/scripting/scripting.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/text/rendering-backend.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/devel-visual-properties.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/rendering/text-backend.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-effects-style.h>

using Dali::Toolkit::Text::LayoutEngine;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Property names.
const char * const RENDERING_BACKEND_PROPERTY( "renderingBackend" );
const char * const TEXT_PROPERTY( "text" );
const char * const FONT_FAMILY_PROPERTY( "fontFamily" );
const char * const FONT_STYLE_PROPERTY( "fontStyle" );
const char * const POINT_SIZE_PROPERTY( "pointSize" );
const char * const MULTI_LINE_PROPERTY( "multiLine" );
const char * const HORIZONTAL_ALIGNMENT_PROPERTY( "horizontalAlignment" );
const char * const VERTICAL_ALIGNMENT_PROPERTY( "verticalAlignment" );
const char * const TEXT_COLOR_PROPERTY( "textColor" );
const char * const ENABLE_MARKUP_PROPERTY( "enableMarkup" );
const char * const ENABLE_AUTO_SCROLL_PROPERTY( "enableAutoScroll" );
const char * const AUTO_SCROLL_SPEED_PROPERTY( "autoScrollSpeed" );
const char * const AUTO_SCROLL_LOOP_COUNT_PROPERTY( "autoScrollLoopCount" );
const char * const AUTO_SCROLL_GAP_PROPERTY( "autoScrollGap" );
const char * const LINE_SPACING_PROPERTY( "lineSpacing" );
const char * const UNDERLINE_PROPERTY( "underline" );
const char * const SHADOW_PROPERTY( "shadow" );
const char * const OUTLINE_PROPERTY( "outline" );
const char * const BATCHING_ENABLED_PROPERTY( "batchingEnabled" );

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

std::string GetHorizontalAlignment( LayoutEngine::HorizontalAlignment alignment )
{
  const char* name = Scripting::GetEnumerationName<Toolkit::Text::LayoutEngine::HorizontalAlignment>( alignment,
                                                                                                      HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                      HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );

  return std::string( name );
}

std::string GetVerticalAlignment( LayoutEngine::VerticalAlignment alignment )
{
  const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( alignment,
                                                                                                      VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                      VERTICAL_ALIGNMENT_STRING_TABLE_COUNT );

  return std::string( name );
}

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 pixelArea;
  varying mediump vec2 vTexCoord;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER_ATLAS_CLAMP = DALI_COMPOSE_SHADER(
    varying mediump vec2 vTexCoord;\n
    uniform sampler2D sTexture;\n
    uniform mediump vec4 uAtlasRect;\n
    uniform lowp vec4 uColor;\n
    \n
    void main()\n
    {\n
      mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );\n
      gl_FragColor = texture2D( sTexture, texCoord ) * uColor;\n
    }\n
);

Geometry CreateGeometry( VisualFactoryCache& factoryCache, ImageDimensions gridSize )
{
  Geometry geometry;

  if( gridSize == ImageDimensions( 1, 1 ) )
  {
    geometry = factoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
    if( !geometry )
    {
      geometry =  VisualFactoryCache::CreateQuadGeometry();
      factoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY, geometry );
    }
  }
  else
  {
    geometry = VisualFactoryCache::CreateGridGeometry( gridSize );
  }

  return geometry;
}

} // unnamed namespace

TextVisualPtr TextVisual::New( VisualFactoryCache& factoryCache )
{
  return new TextVisual( factoryCache );
}

float TextVisual::GetHeightForWidth( float width ) const
{
  return mController->GetHeightForWidth( width );
}

void TextVisual::GetNaturalSize( Vector2& naturalSize )
{
  naturalSize = mController->GetNaturalSize().GetVectorXY();
}

void TextVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  Property::Value value;

  map.Clear();
  map.Insert( Toolkit::VisualProperty::TYPE, Toolkit::Visual::TEXT );

  map.Insert( Toolkit::TextVisual::Property::RENDERING_BACKEND, mRenderingBackend );

  std::string text;
  mController->GetText( text );
  map.Insert( Toolkit::TextVisual::Property::TEXT, text );

  map.Insert( Toolkit::TextVisual::Property::FONT_FAMILY, mController->GetDefaultFontFamily() );

  GetFontStyleProperty( mController, value, Text::FontStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::FONT_STYLE, value );

  map.Insert( Toolkit::TextVisual::Property::POINT_SIZE, mController->GetDefaultPointSize() );

  map.Insert( Toolkit::TextVisual::Property::MULTI_LINE, mController->IsMultiLineEnabled() );

  map.Insert( Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, GetHorizontalAlignment( mController->GetHorizontalAlignment() ) );

  map.Insert( Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, GetVerticalAlignment( mController->GetVerticalAlignment() ) );

  map.Insert( Toolkit::TextVisual::Property::TEXT_COLOR, mController->GetTextColor() );

  map.Insert( Toolkit::TextVisual::Property::ENABLE_MARKUP, mController->IsMarkupProcessorEnabled() );

  map.Insert( Toolkit::TextVisual::Property::LINE_SPACING, mController->GetDefaultLineSpacing() );

  GetUnderlineProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::UNDERLINE, value );

  GetShadowProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::SHADOW, value );

  GetOutlineProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::OUTLINE, value );

  map.Insert( Toolkit::TextVisual::Property::BATCHING_ENABLED, false ); // TODO
}

TextVisual::TextVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mController( Text::Controller::New() ),
  mRenderingBackend( Toolkit::Text::DEFAULT_RENDERING_BACKEND ),
  mHasBeenStaged( false )
{
}

TextVisual::~TextVisual()
{
}

void TextVisual::DoSetProperties( const Property::Map& propertyMap )
{
  for( Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index )
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue( index );

    switch( keyValue.first.type )
    {
      case Property::Key::INDEX:
      {
        if( Toolkit::VisualProperty::TYPE != keyValue.first.indexKey ) // Toolkit::VisualProperty::TYPE is not a TextVisual's property.
        {
          DoSetProperty( keyValue.first.indexKey, keyValue.second );
        }
        break;
      }
      case Property::Key::STRING:
      {
        if( keyValue.first.stringKey == RENDERING_BACKEND_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::RENDERING_BACKEND, keyValue.second );
        }
        else if( keyValue.first.stringKey == TEXT_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::TEXT, keyValue.second );
        }
        else if( keyValue.first.stringKey == FONT_FAMILY_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::FONT_FAMILY, keyValue.second );
        }
        else if( keyValue.first.stringKey == FONT_STYLE_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::FONT_STYLE, keyValue.second );
        }
        else if( keyValue.first.stringKey == POINT_SIZE_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::POINT_SIZE, keyValue.second );
        }
        else if( keyValue.first.stringKey == MULTI_LINE_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::MULTI_LINE, keyValue.second );
        }
        else if( keyValue.first.stringKey == HORIZONTAL_ALIGNMENT_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, keyValue.second );
        }
        else if( keyValue.first.stringKey == VERTICAL_ALIGNMENT_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, keyValue.second );
        }
        else if( keyValue.first.stringKey == TEXT_COLOR_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::TEXT_COLOR, keyValue.second );
        }
        else if( keyValue.first.stringKey == ENABLE_MARKUP_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::ENABLE_MARKUP, keyValue.second );
        }
        else if( keyValue.first.stringKey == ENABLE_AUTO_SCROLL_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::ENABLE_AUTO_SCROLL, keyValue.second );
        }
        else if( keyValue.first.stringKey == AUTO_SCROLL_SPEED_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::AUTO_SCROLL_SPEED, keyValue.second );
        }
        else if( keyValue.first.stringKey == AUTO_SCROLL_LOOP_COUNT_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::AUTO_SCROLL_LOOP_COUNT, keyValue.second );
        }
        else if( keyValue.first.stringKey == AUTO_SCROLL_GAP_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::AUTO_SCROLL_GAP, keyValue.second );
        }
        else if( keyValue.first.stringKey == LINE_SPACING_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::LINE_SPACING, keyValue.second );
        }
        else if( keyValue.first.stringKey == UNDERLINE_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::UNDERLINE, keyValue.second );
        }
        else if( keyValue.first.stringKey == SHADOW_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::SHADOW, keyValue.second );
        }
        else if( keyValue.first.stringKey == OUTLINE_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::OUTLINE, keyValue.second );
        }
        else if( keyValue.first.stringKey == BATCHING_ENABLED_PROPERTY )
        {
          DoSetProperty( Toolkit::TextVisual::Property::BATCHING_ENABLED, keyValue.second );
        }
        break;
      }
    }
  }

  // Retrieve the layout engine to set whether to elide the text and set the cursor's width.
  Text::LayoutEngine& engine = mController->GetLayoutEngine();

  // Elide the text if it exceeds the boundaries.
  engine.SetTextEllipsisEnabled( true );

  // Sets 0 as cursor's width.
  engine.SetCursorWidth( 0u ); // Do not layout space for the cursor.
}

void TextVisual::DoSetOnStage( Actor& actor )
{
  // TODO Create the actual renderer(s) for the text!!!!
  //      Will crash if no mImpl->mRenderer is set.
  Geometry geometry;
  Shader shader;

  geometry = CreateGeometry( mFactoryCache, ImageDimensions( 1, 1 ) );

  shader = mFactoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_CLAMP );
    mFactoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader );
  }

  mImpl->mRenderer = Renderer::New( geometry, shader );

  mSelf = actor;

  if( mHasBeenStaged )
  {
    RenderText();
  }
  else
  {
    mHasBeenStaged = true;
  }
}

void TextVisual::DoSetOffStage( Actor& actor )
{
  mSelf.Reset();
}

void TextVisual::DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  switch( index )
  {
    case Toolkit::TextVisual::Property::RENDERING_BACKEND:
    {
      int backend = propertyValue.Get<int>();

#ifndef ENABLE_VECTOR_BASED_TEXT_RENDERING
      if( Text::RENDERING_VECTOR_BASED == backend )
      {
        backend = TextAbstraction::BITMAP_GLYPH; // Fallback to bitmap-based rendering
      }
#endif
      if( mRenderingBackend != backend )
      {
        mRenderingBackend = backend;
        mRenderer.Reset();

        // When using the vector-based rendering, the size of the GLyphs are different
        TextAbstraction::GlyphType glyphType = ( Text::RENDERING_VECTOR_BASED == mRenderingBackend ) ? TextAbstraction::VECTOR_GLYPH : TextAbstraction::BITMAP_GLYPH;
        mController->SetGlyphType( glyphType );
      }
      break;
    }
    case Toolkit::TextVisual::Property::TEXT:
    {
      mController->SetText( propertyValue.Get<std::string>() );
      break;
    }
    case Toolkit::TextVisual::Property::FONT_FAMILY:
    {
      SetFontFamilyProperty( mController, propertyValue );
      break;
    }
    case Toolkit::TextVisual::Property::FONT_STYLE:
    {
      SetFontStyleProperty( mController, propertyValue, Text::FontStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::POINT_SIZE:
    {
      const float pointSize = propertyValue.Get<float>();

      if( !Equals( mController->GetDefaultPointSize(), pointSize ) )
      {
        mController->SetDefaultPointSize( pointSize );
      }
      break;
    }
    case Toolkit::TextVisual::Property::MULTI_LINE:
    {
      mController->SetMultiLineEnabled( propertyValue.Get<bool>() );
      break;
    }
    case Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT:
    {
      LayoutEngine::HorizontalAlignment alignment( LayoutEngine::HORIZONTAL_ALIGN_BEGIN );
      if( Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::HorizontalAlignment >( propertyValue.Get< std::string >().c_str(),
                                                                                         HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                         HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                                         alignment ) )
      {
        mController->SetHorizontalAlignment( alignment );
      }
      break;
    }
    case Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT:
    {
      LayoutEngine::VerticalAlignment alignment( LayoutEngine::VERTICAL_ALIGN_BOTTOM );
      if( Scripting::GetEnumeration< Toolkit::Text::LayoutEngine::VerticalAlignment >( propertyValue.Get< std::string >().c_str(),
                                                                                       VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                       VERTICAL_ALIGNMENT_STRING_TABLE_COUNT,
                                                                                       alignment ) )
      {
        mController->SetVerticalAlignment( alignment );
      }
      break;
    }
    case Toolkit::TextVisual::Property::TEXT_COLOR:
    {
      const Vector4 textColor = propertyValue.Get< Vector4 >();
      if( mController->GetTextColor() != textColor )
      {
        mController->SetTextColor( textColor );
        mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextVisual::Property::ENABLE_MARKUP:
    {
      const bool enableMarkup = propertyValue.Get<bool>();
      mController->SetMarkupProcessorEnabled( enableMarkup );
      break;
    }
    case Toolkit::TextVisual::Property::ENABLE_AUTO_SCROLL:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_SPEED:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_LOOP_COUNT:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_GAP:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::LINE_SPACING:
    {
      const float lineSpacing = propertyValue.Get<float>();
      mController->SetDefaultLineSpacing( lineSpacing );
      mRenderer.Reset();
      break;
    }
    case Toolkit::TextVisual::Property::UNDERLINE:
    {
      // TODO : This switch can be removed when the deprecated SHADOW_OFFSET and SHADOW_COLOR properties are finally removed.
      //        Only the code for the MAP case should be kept.
      switch( propertyValue.GetType() )
      {
        case Property::VECTOR4:
        {
          const Vector4 color = propertyValue.Get<Vector4>();
          if( mController->GetUnderlineColor() != color )
          {
            mController->SetUnderlineColor( color );
            mRenderer.Reset();
          }
          break;
        }
        case Property::FLOAT:
        {
          float height = propertyValue.Get<float>();
          if( fabsf( mController->GetUnderlineHeight() - height ) > Math::MACHINE_EPSILON_1000 )
          {
            mController->SetUnderlineHeight( height );
            mRenderer.Reset();
          }
          break;
        }
        case Property::BOOLEAN:
        {
          const bool enabled = propertyValue.Get<bool>();
          if( mController->IsUnderlineEnabled() != enabled )
          {
            mController->SetUnderlineEnabled( enabled );
            mRenderer.Reset();
          }
          break;
        }
        case Property::MAP:
        {
          const bool update = SetUnderlineProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
          if( update )
          {
            mRenderer.Reset();
          }
          break;
        }
        default:
        {
          // Nothing to do.
          break;
        }
      }

      break;
    }
    case Toolkit::TextVisual::Property::SHADOW:
    {
      // TODO : This switch can be removed when the deprecated SHADOW_OFFSET and SHADOW_COLOR properties are finally removed.
      //        Only the code for the MAP case should be kept.
      switch( propertyValue.GetType() )
      {
        case Property::VECTOR2:
        {
          const Vector2 shadowOffset = propertyValue.Get<Vector2>();
          if( mController->GetShadowOffset() != shadowOffset )
          {
            mController->SetShadowOffset( shadowOffset );
            mRenderer.Reset();
          }
          break;
        }
        case Property::VECTOR4:
        {
          const Vector4 shadowColor = propertyValue.Get<Vector4>();
          if( mController->GetShadowColor() != shadowColor )
          {
            mController->SetShadowColor( shadowColor );
            mRenderer.Reset();
          }
          break;
        }
        case Property::MAP:
        {
          const bool update = SetShadowProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
          if( update )
          {
            mRenderer.Reset();
          }
          break;
        }
        default:
        {
          // Nothing to do.
          break;
        }
      }
      break;
    }
    case Toolkit::TextVisual::Property::EMBOSS:
    {
      const bool update = SetEmbossProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
      if( update )
      {
        mRenderer.Reset();
     }
      break;
    }
    case Toolkit::TextVisual::Property::OUTLINE:
    {
      const bool update = SetOutlineProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
      if( update )
      {
        mRenderer.Reset();
      }
      break;
    }
    case Toolkit::TextVisual::Property::BATCHING_ENABLED:
    {
      // TODO
      break;
    }
    default:
    {
      // Should not arrive here.
      DALI_ASSERT_DEBUG( false );
    }
  }
}

Dali::Property::Value TextVisual::DoGetProperty( Dali::Property::Index index )
{
  Dali::Property::Value value;

  switch( index )
  {
    case Toolkit::TextVisual::Property::RENDERING_BACKEND:
    {
      value = mRenderingBackend;
      break;
    }
    case Toolkit::TextVisual::Property::TEXT:
    {
      std::string text;
      mController->GetText( text );
      value = text;
      break;
    }
    case Toolkit::TextVisual::Property::FONT_FAMILY:
    {
      value = mController->GetDefaultFontFamily();
      break;
    }
    case Toolkit::TextVisual::Property::FONT_STYLE:
    {
      GetFontStyleProperty( mController, value, Text::FontStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::POINT_SIZE:
    {
      value = mController->GetDefaultPointSize();
      break;
    }
    case Toolkit::TextVisual::Property::MULTI_LINE:
    {
      value = mController->IsMultiLineEnabled();
      break;
    }
    case Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT:
    {
      const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::HorizontalAlignment >( mController->GetHorizontalAlignment(),
                                                                                                            HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                            HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );
      if( name )
      {
        value = std::string( name );
      }
      break;
    }
    case Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT:
    {
      const char* name = Scripting::GetEnumerationName< Toolkit::Text::LayoutEngine::VerticalAlignment >( mController->GetVerticalAlignment(),
                                                                                                          VERTICAL_ALIGNMENT_STRING_TABLE,
                                                                                                          VERTICAL_ALIGNMENT_STRING_TABLE_COUNT );
      if( name )
      {
        value = std::string( name );
      }
      break;
    }
    case Toolkit::TextVisual::Property::TEXT_COLOR:
    {
      value = mController->GetTextColor();
      break;
    }
    case Toolkit::TextVisual::Property::ENABLE_MARKUP:
    {
      value = mController->IsMarkupProcessorEnabled();
      break;
    }
    case Toolkit::TextVisual::Property::ENABLE_AUTO_SCROLL:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_SPEED:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_LOOP_COUNT:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::AUTO_SCROLL_GAP:
    {
      // nothing to do.
      break;
    }
    case Toolkit::TextVisual::Property::LINE_SPACING:
    {
      value = mController->GetDefaultLineSpacing();
      break;
    }
    case Toolkit::TextVisual::Property::UNDERLINE:
    {
      GetUnderlineProperties( mController, value, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::SHADOW:
    {
      GetShadowProperties( mController, value, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::EMBOSS:
    {
      GetEmbossProperties( mController, value, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::OUTLINE:
    {
      GetOutlineProperties( mController, value, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::BATCHING_ENABLED:
    {
      // TODO
      break;
    }
    default:
    {
      // Should not arrive here.
      DALI_ASSERT_DEBUG( false );
    }
  }

  return value;
}

void TextVisual::OnSetTransform()
{
  Vector2 visualSize = mImpl->mTransform.GetVisualSize( mImpl->mControlSize );

  // Note, the direction should come from the layout of the parent control
  mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( visualSize );

  if( ( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) ) ||
      !mRenderer )
  {
    if( !mRenderer )
    {
      mRenderer = Text::Backend::Get().NewRenderer( mRenderingBackend );
    }
    RenderText();
  }
}

void TextVisual::RenderText()
{
  Actor self = mSelf.GetHandle();
  if( !self )
  {
    // Nothing to do if the handle is not initialized.
    return;
  }

  Actor renderableActor;

  if( mRenderer )
  {
    renderableActor = mRenderer->Render( mController->GetView(), Toolkit::DepthIndex::TEXT );
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
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
