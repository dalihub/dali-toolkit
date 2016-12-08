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

// INTERNAL HEADER
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/text/text-font-style.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Property names.
const char * const TEXT_PROPERTY( "text" );
const char * const FONT_FAMILY_PROPERTY( "fontFamily" );
const char * const FONT_STYLE_PROPERTY( "fontStyle" );
const char * const POINT_SIZE_PROPERTY( "pointSize" );
const char * const MULTI_LINE_PROPERTY( "multiLine" );
const char * const HORIZONTAL_ALIGNMENT_PROPERTY( "horizontalAlignment" );
const char * const VERTICAL_ALIGNMENT_PROPERTY( "verticalAlignment" );
const char * const TEXT_COLOR_PROPERTY( "textColor" );
const char * const ENABLE_MARKUP_PROPERTY( "enableMarkup" );

const std::string PIXEL_AREA_UNIFORM_NAME = "pixelArea";

const Scripting::StringEnum HORIZONTAL_ALIGNMENT_STRING_TABLE[] =
{
  { "BEGIN",  Toolkit::Text::Layout::HORIZONTAL_ALIGN_BEGIN  },
  { "CENTER", Toolkit::Text::Layout::HORIZONTAL_ALIGN_CENTER },
  { "END",    Toolkit::Text::Layout::HORIZONTAL_ALIGN_END    },
};
const unsigned int HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE ) / sizeof( HORIZONTAL_ALIGNMENT_STRING_TABLE[0] );

const Scripting::StringEnum VERTICAL_ALIGNMENT_STRING_TABLE[] =
{
  { "TOP",    Toolkit::Text::Layout::VERTICAL_ALIGN_TOP    },
  { "CENTER", Toolkit::Text::Layout::VERTICAL_ALIGN_CENTER },
  { "BOTTOM", Toolkit::Text::Layout::VERTICAL_ALIGN_BOTTOM },
};
const unsigned int VERTICAL_ALIGNMENT_STRING_TABLE_COUNT = sizeof( VERTICAL_ALIGNMENT_STRING_TABLE ) / sizeof( VERTICAL_ALIGNMENT_STRING_TABLE[0] );

const Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );

std::string GetHorizontalAlignment( Toolkit::Text::Layout::HorizontalAlignment alignment )
{
  const char* name = Scripting::GetEnumerationName<Toolkit::Text::Layout::HorizontalAlignment>( alignment,
                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE,
                                                                                                HORIZONTAL_ALIGNMENT_STRING_TABLE_COUNT );

  return std::string( name );
}

std::string GetVerticalAlignment( Toolkit::Text::Layout::VerticalAlignment alignment )
{
  const char* name = Scripting::GetEnumerationName< Toolkit::Text::Layout::VerticalAlignment >( alignment,
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

  //Visual size and offset
  uniform mediump vec2 offset;\n
  uniform mediump vec2 size;\n
  uniform mediump vec4 offsetSizeMode;\n
  uniform mediump vec2 origin;\n
  uniform mediump vec2 anchorPoint;\n

  vec4 ComputeVertexPosition()\n
  {\n
    vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );\n
    vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);\n
    return vec4( (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy, 0.0, 1.0 );\n
  }\n

  void main()\n
  {\n
    mediump vec4 vertexPosition = uMvpMatrix *ComputeVertexPosition();\n
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

} // unnamed namespace

TextVisualPtr TextVisual::New( VisualFactoryCache& factoryCache )
{
  return new TextVisual( factoryCache );
}

float TextVisual::GetHeightForWidth( float width )
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
  map.Insert( Toolkit::DevelVisual::Property::TYPE, Toolkit::DevelVisual::TEXT );

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

  map.Insert( Toolkit::TextVisual::Property::TEXT_COLOR, mController->GetDefaultColor() );

  map.Insert( Toolkit::TextVisual::Property::ENABLE_MARKUP, mController->IsMarkupProcessorEnabled() );
}

TextVisual::TextVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache ),
  mController( Text::Controller::New() ),
  mTypesetter( Text::Typesetter::New( mController->GetTextModel() ) )
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
        DoSetProperty( keyValue.first.indexKey, keyValue.second );
        break;
      }
      case Property::Key::STRING:
      {
        if( keyValue.first.stringKey == TEXT_PROPERTY )
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
        break;
      }
    }
  }

  // Elide the text if it exceeds the boundaries.
  mController->SetTextElideEnabled( true );

  // Retrieve the layout engine to set the cursor's width.
  Text::Layout::Engine& engine = mController->GetLayoutEngine();

  // Sets 0 as cursor's width.
  engine.SetCursorWidth( 0u ); // Do not layout space for the cursor.
}

void TextVisual::DoSetOnStage( Actor& actor )
{
  mControl = actor;

  Geometry geometry = mFactoryCache.GetGeometry( VisualFactoryCache::QUAD_GEOMETRY );
  if( !geometry )
  {
    geometry =  VisualFactoryCache::CreateQuadGeometry();
    mFactoryCache.SaveGeometry( VisualFactoryCache::QUAD_GEOMETRY , geometry );
  }

  Shader shader = mFactoryCache.GetShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP );
  if( !shader )
  {
    shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_ATLAS_CLAMP );
    mFactoryCache.SaveShader( VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP, shader );
  }
  shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );

  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, Toolkit::DepthIndex::TEXT );

  UpdateRenderer();
}

void TextVisual::DoSetOffStage( Actor& actor )
{
  if( mImpl->mRenderer )
  {
    // Removes the renderer from the actor.
    actor.RemoveRenderer( mImpl->mRenderer );

    RemoveTextureSet();

    // Resets the renderer.
    mImpl->mRenderer.Reset();
  }

  // Resets the control handle.
  mControl.Reset();
}

void TextVisual::OnSetTransform()
{
  UpdateRenderer();
}

void TextVisual::DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
{
  switch( index )
  {
    case Toolkit::TextVisual::Property::ENABLE_MARKUP:
    {
      const bool enableMarkup = propertyValue.Get<bool>();
      mController->SetMarkupProcessorEnabled( enableMarkup );
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
      Toolkit::Text::Layout::HorizontalAlignment alignment( Toolkit::Text::Layout::HORIZONTAL_ALIGN_BEGIN );
      if( Scripting::GetEnumeration< Toolkit::Text::Layout::HorizontalAlignment >( propertyValue.Get< std::string >().c_str(),
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
      Toolkit::Text::Layout::VerticalAlignment alignment( Toolkit::Text::Layout::VERTICAL_ALIGN_BOTTOM );
      if( Scripting::GetEnumeration< Toolkit::Text::Layout::VerticalAlignment >( propertyValue.Get< std::string >().c_str(),
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
      const Vector4& textColor = propertyValue.Get< Vector4 >();
      if( mController->GetDefaultColor() != textColor )
      {
        mController->SetDefaultColor( textColor );
      }
      break;
    }
  }
}

void TextVisual::UpdateRenderer()
{
  Actor control = mControl.GetHandle();
  if( !control )
  {
    // Nothing to do.
    return;
  }

  // Calculates the size to be used to relayout.
  Vector2 relayoutSize;

  const bool isWidthRelative = fabsf( mImpl->mTransform.mOffsetSizeMode.z ) < Math::MACHINE_EPSILON_1000;
  const bool isHeightRelative = fabsf( mImpl->mTransform.mOffsetSizeMode.w ) < Math::MACHINE_EPSILON_1000;

  // Round the size and offset to avoid pixel alignement issues.
  relayoutSize.width = floorf( 0.5f + ( isWidthRelative ? mImpl->mControlSize.width * mImpl->mTransform.mSize.x : mImpl->mTransform.mSize.width ) );
  relayoutSize.height = floorf( 0.5f + ( isHeightRelative ? mImpl->mControlSize.height * mImpl->mTransform.mSize.y : mImpl->mTransform.mSize.height ) );

  if( ( fabsf( relayoutSize.width ) < Math::MACHINE_EPSILON_1000 ) || ( fabsf( relayoutSize.height ) < Math::MACHINE_EPSILON_1000 ) )
  {
    // Removes the texture set.
    RemoveTextureSet();

    // Remove any renderer previously set.
    if( mImpl->mRenderer )
    {
      control.RemoveRenderer( mImpl->mRenderer );
    }

    // Nothing else to do if the relayout size is zero.
    return;
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( relayoutSize );

  if( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType ) )
  {
    // Removes the texture set.
    RemoveTextureSet();

    // Remove any renderer previously set.
    if( mImpl->mRenderer )
    {
      control.RemoveRenderer( mImpl->mRenderer );
    }

    if( ( relayoutSize.width > Math::MACHINE_EPSILON_1000 ) &&
        ( relayoutSize.height > Math::MACHINE_EPSILON_1000 ) )
    {
      PixelData data = mTypesetter->Render( relayoutSize );

      Vector4 atlasRect = FULL_TEXTURE_RECT;
      TextureSet textureSet = mFactoryCache.GetAtlasManager()->Add( atlasRect, data );

      if( textureSet )
      {
        mImpl->mFlags |= Impl::IS_ATLASING_APPLIED;
      }
      else
      {
        // It may happen the image atlas can't handle a pixel data it exceeds the maximum size.
        // In that case, create a texture. TODO: should tile the text.

        Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                        data.GetPixelFormat(),
                                        data.GetWidth(),
                                        data.GetHeight() );

        texture.Upload( data );

        textureSet = TextureSet::New();
        textureSet.SetTexture( 0u, texture );

        mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;
      }

      mImpl->mRenderer.RegisterProperty( ATLAS_RECT_UNIFORM_NAME, atlasRect );

      //Register transform properties
      mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

      // Filter mode needs to be set to nearest to avoid blurry text.
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
      textureSet.SetSampler( 0u, sampler );

      mImpl->mRenderer.SetTextures( textureSet );

      control.AddRenderer( mImpl->mRenderer );
    }
  }
}

void TextVisual::RemoveTextureSet()
{
  if( mImpl->mFlags & Impl::IS_ATLASING_APPLIED )
  {
    // Removes the text's image from the texture atlas.
    Vector4 atlasRect;

    const Property::Index index = mImpl->mRenderer.GetPropertyIndex( ATLAS_RECT_UNIFORM_NAME );
    if( index != Property::INVALID_INDEX )
    {
      const Property::Value& atlasRectValue = mImpl->mRenderer.GetProperty( index );
      atlasRectValue.Get( atlasRect );

      const TextureSet& textureSet = mImpl->mRenderer.GetTextures();
      mFactoryCache.GetAtlasManager()->Remove( textureSet, atlasRect );
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
