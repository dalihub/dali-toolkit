/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// @todo: using generated file in the dali-core!!!!
#include <dali-toolkit/devel-api/graphics/builtin-shader-extern-gen.h>
#include <dali/devel-api/rendering/shader-devel.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

// INTERNAL HEADER
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// Property names - common properties defined in visual-string-constants.h/cpp
const char * const FONT_FAMILY_PROPERTY( "fontFamily" );
const char * const FONT_STYLE_PROPERTY( "fontStyle" );
const char * const POINT_SIZE_PROPERTY( "pointSize" );
const char * const MULTI_LINE_PROPERTY( "multiLine" );
const char * const HORIZONTAL_ALIGNMENT_PROPERTY( "horizontalAlignment" );
const char * const VERTICAL_ALIGNMENT_PROPERTY( "verticalAlignment" );
const char * const TEXT_COLOR_PROPERTY( "textColor" );
const char * const ENABLE_MARKUP_PROPERTY( "enableMarkup" );
const char * const SHADOW_PROPERTY( "shadow" );
const char * const UNDERLINE_PROPERTY( "underline" );
const char * const OUTLINE_PROPERTY( "outline" );
const char * const BACKGROUND_PROPERTY( "textBackground" );

const Vector4 FULL_TEXTURE_RECT( 0.f, 0.f, 1.f, 1.f );
#if 0
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  uniform mediump vec4 pixelArea;\n

  uniform mediump mat4 uModelMatrix;\n
  uniform mediump mat4 uViewMatrix;\n
  uniform mediump mat4 uProjection;\n

  varying mediump vec2 vTexCoord;\n

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
    mediump vec4 nonAlignedVertex = uViewMatrix*uModelMatrix*ComputeVertexPosition();\n
    mediump vec4 pixelAlignedVertex = vec4 ( floor(nonAlignedVertex.xyz), 1.0 );\n
    mediump vec4 vertexPosition = uProjection*pixelAlignedVertex;\n

    vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* FRAGMENT_SHADER_SINGLE_COLOR_TEXT = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uTextColorAnimatable;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump float textTexture = texture2D( sTexture, vTexCoord ).r;\n

    // Set the color of the text to what it is animated to.
    gl_FragColor = uTextColorAnimatable * textTexture * uColor * vec4( mixColor, 1.0 );
  }\n
);

const char* FRAGMENT_SHADER_MULTI_COLOR_TEXT = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump vec4 textTexture = texture2D( sTexture, vTexCoord );\n
    textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );\n

    gl_FragColor = textTexture * uColor * vec4( mixColor, 1.0 );
  }\n
);

const char* FRAGMENT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sStyle;\n
  uniform lowp vec4 uTextColorAnimatable;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump float textTexture = texture2D( sTexture, vTexCoord ).r;\n
    mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );\n

    // Draw the text as overlay above the style
    gl_FragColor = ( uTextColorAnimatable * textTexture + styleTexture * ( 1.0 - uTextColorAnimatable.a * textTexture ) ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);


const char* FRAGMENT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sStyle;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump vec4 textTexture = texture2D( sTexture, vTexCoord );\n
    mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );\n
    textTexture.rgb *= mix( 1.0, textTexture.a, preMultipliedAlpha );\n

    // Draw the text as overlay above the style
    gl_FragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);

const char* FRAGMENT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sMask;\n
  uniform lowp vec4 uTextColorAnimatable;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump vec4 textTexture = texture2D( sTexture, vTexCoord );\n
    mediump float maskTexture = texture2D( sMask, vTexCoord ).r;\n

    // Set the color of non-transparent pixel in text to what it is animated to.
    // Markup text with multiple text colors are not animated (but can be supported later on if required).
    // Emoji color are not animated.
    mediump float vstep = step( 0.0001, textTexture.a );\n
    textTexture.rgb = mix( textTexture.rgb, uTextColorAnimatable.rgb, vstep * maskTexture ) * mix( 1.0, textTexture.a, preMultipliedAlpha );\n

    // Draw the text as overlay above the style
    gl_FragColor = textTexture * uColor * vec4( mixColor, 1.0 );\n
  }\n
);

const char* FRAGMENT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform sampler2D sStyle;\n
  uniform sampler2D sMask;\n
  uniform lowp float uHasMultipleTextColors;\n
  uniform lowp vec4 uTextColorAnimatable;\n
  uniform lowp vec4 uColor;\n
  uniform lowp vec3 mixColor;\n
  uniform lowp float preMultipliedAlpha;\n
  \n
  void main()\n
  {\n
    mediump vec4 textTexture = texture2D( sTexture, vTexCoord );\n
    mediump vec4 styleTexture = texture2D( sStyle, vTexCoord );\n
    mediump float maskTexture = texture2D( sMask, vTexCoord ).r;\n

    // Set the color of non-transparent pixel in text to what it is animated to.
    // Markup text with multiple text colors are not animated (but can be supported later on if required).
    // Emoji color are not animated.
    mediump float vstep = step( 0.0001, textTexture.a );\n
    textTexture.rgb = mix( textTexture.rgb, uTextColorAnimatable.rgb, vstep * maskTexture * ( 1.0 - uHasMultipleTextColors ) ) * mix( 1.0, textTexture.a, preMultipliedAlpha );\n

    // Draw the text as overlay above the style
    gl_FragColor = ( textTexture + styleTexture * ( 1.0 - textTexture.a ) ) * uColor * vec4( mixColor, 1.0 );\n
  }\n
);
#endif

/**
 * Return Property index for the given string key
 * param[in] stringKey the string index key
 * return the key as an index
 */

Dali::Property::Index StringKeyToIndexKey( const std::string& stringKey )
{
  Dali::Property::Index result = Property::INVALID_KEY;

  if( stringKey == VISUAL_TYPE )
  {
    result = Toolkit::Visual::Property::TYPE;
  }
  else if( stringKey == TEXT_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::TEXT;
  }
  else if( stringKey == FONT_FAMILY_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::FONT_FAMILY;
  }
  else if( stringKey == FONT_STYLE_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::FONT_STYLE;
  }
  else if( stringKey == POINT_SIZE_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::POINT_SIZE;
  }
  else if( stringKey == MULTI_LINE_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::MULTI_LINE;
  }
  else if( stringKey == HORIZONTAL_ALIGNMENT_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT;
  }
  else if( stringKey == VERTICAL_ALIGNMENT_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT;
  }
  else if( stringKey == TEXT_COLOR_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::TEXT_COLOR;
  }
  else if( stringKey == ENABLE_MARKUP_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::ENABLE_MARKUP;
  }
  else if( stringKey == SHADOW_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::SHADOW;
  }
  else if( stringKey == UNDERLINE_PROPERTY )
  {
    result = Toolkit::TextVisual::Property::UNDERLINE;
  }
  else if( stringKey == OUTLINE_PROPERTY )
  {
    result = Toolkit::DevelTextVisual::Property::OUTLINE;
  }
  else if( stringKey == BACKGROUND_PROPERTY )
  {
    result = Toolkit::DevelTextVisual::Property::BACKGROUND;
  }

  return result;
}

void TextColorConstraint( Vector4& current, const PropertyInputContainer& inputs )
{
  Vector4 color = inputs[0]->GetVector4();
  current.r = color.r * color.a;
  current.g = color.g * color.a;
  current.b = color.b * color.a;
  current.a = color.a;
}

void OpacityConstraint( float& current, const PropertyInputContainer& inputs )
{
  current = inputs[0]->GetVector4().a;
}

} // unnamed namespace

TextVisualPtr TextVisual::New( VisualFactoryCache& factoryCache, const Property::Map& properties )
{
  TextVisualPtr TextVisualPtr( new TextVisual( factoryCache ) );
  TextVisualPtr->SetProperties( properties );
  return TextVisualPtr;
}

void TextVisual::ConvertStringKeysToIndexKeys( Property::Map& propertyMap )
{
  Property::Map outMap;

  for( Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index )
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue( index );

    Property::Index indexKey = keyValue.first.indexKey;

    if ( keyValue.first.type == Property::Key::STRING )
    {
      indexKey = StringKeyToIndexKey( keyValue.first.stringKey );
    }

    outMap.Insert( indexKey, keyValue.second );
  }

  propertyMap = outMap;
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
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT );

  std::string text;
  mController->GetText( text );
  map.Insert( Toolkit::TextVisual::Property::TEXT, text );

  map.Insert( Toolkit::TextVisual::Property::FONT_FAMILY, mController->GetDefaultFontFamily() );

  GetFontStyleProperty( mController, value, Text::FontStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::FONT_STYLE, value );

  map.Insert( Toolkit::TextVisual::Property::POINT_SIZE, mController->GetDefaultFontSize( Text::Controller::POINT_SIZE ) );

  map.Insert( Toolkit::TextVisual::Property::MULTI_LINE, mController->IsMultiLineEnabled() );

  map.Insert( Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, mController->GetHorizontalAlignment() );

  map.Insert( Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, mController->GetVerticalAlignment() );

  map.Insert( Toolkit::TextVisual::Property::TEXT_COLOR, mController->GetDefaultColor() );

  map.Insert( Toolkit::TextVisual::Property::ENABLE_MARKUP, mController->IsMarkupProcessorEnabled() );

  GetShadowProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::SHADOW, value );

  GetUnderlineProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::TextVisual::Property::UNDERLINE, value );

  GetOutlineProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::DevelTextVisual::Property::OUTLINE, value );

  GetBackgroundProperties( mController, value, Text::EffectStyle::DEFAULT );
  map.Insert( Toolkit::DevelTextVisual::Property::BACKGROUND, value );
}

void TextVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  map.Clear();
  map.Insert( Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT );
  std::string text;
  mController->GetText( text );
  map.Insert( Toolkit::TextVisual::Property::TEXT, text );
}


TextVisual::TextVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FIT_KEEP_ASPECT_RATIO ),
  mController( Text::Controller::New() ),
  mTypesetter( Text::Typesetter::New( mController->GetTextModel() ) ),
  mAnimatableTextColorPropertyIndex( Property::INVALID_INDEX ),
  mRendererUpdateNeeded( false )
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

    Property::Index indexKey = keyValue.first.indexKey;

    if( keyValue.first.type == Property::Key::STRING )
    {
      indexKey = StringKeyToIndexKey( keyValue.first.stringKey );
    }

    DoSetProperty( indexKey, keyValue.second );
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
  Shader shader = GetTextShader(mFactoryCache, TextType::SINGLE_COLOR_TEXT, TextType::NO_EMOJI, TextType::NO_STYLES);

  mImpl->mRenderer = Renderer::New( geometry, shader );
  mImpl->mRenderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, Toolkit::DepthIndex::CONTENT );

  // Enable the pre-multiplied alpha to improve the text quality
  EnablePreMultipliedAlpha(true);

  const Vector4& defaultColor = mController->GetTextModel()->GetDefaultColor();
  Dali::Property::Index shaderTextColorIndex = mImpl->mRenderer.RegisterProperty( "uTextColorAnimatable", defaultColor );

  if ( mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX )
  {
    // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
    if( shaderTextColorIndex != Property::INVALID_INDEX )
    {
      Constraint colorConstraint = Constraint::New<Vector4>( mImpl->mRenderer, shaderTextColorIndex, TextColorConstraint );
      colorConstraint.AddSource( Source( actor, mAnimatableTextColorPropertyIndex ) );
      colorConstraint.Apply();

      Constraint opacityConstraint = Constraint::New< float >( mImpl->mRenderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint );
      opacityConstraint.AddSource( Source( actor, mAnimatableTextColorPropertyIndex ) );
      opacityConstraint.Apply();
    }
  }

  // Renderer needs textures and to be added to control
  mRendererUpdateNeeded = true;

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
      if( !Equals( mController->GetDefaultFontSize( Text::Controller::POINT_SIZE ), pointSize ) )
      {
        mController->SetDefaultFontSize( pointSize, Text::Controller::POINT_SIZE );
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
      if( mController )
      {
        Text::HorizontalAlignment::Type alignment( static_cast< Text::HorizontalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
        if( Toolkit::Text::GetHorizontalAlignmentEnumeration( propertyValue, alignment ) )
        {
          mController->SetHorizontalAlignment( alignment );
        }
      }
      break;
    }
    case Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT:
    {
      if( mController )
      {
        Toolkit::Text::VerticalAlignment::Type alignment( static_cast< Text::VerticalAlignment::Type >( -1 ) ); // Set to invalid value to ensure a valid mode does get set
        if( Toolkit::Text::GetVerticalAlignmentEnumeration( propertyValue, alignment) )
        {
          mController->SetVerticalAlignment( alignment );
        }
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
    case Toolkit::TextVisual::Property::SHADOW:
    {
      SetShadowProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::TextVisual::Property::UNDERLINE:
    {
      SetUnderlineProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::DevelTextVisual::Property::OUTLINE:
    {
      SetOutlineProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
      break;
    }
    case Toolkit::DevelTextVisual::Property::BACKGROUND:
    {
      SetBackgroundProperties( mController, propertyValue, Text::EffectStyle::DEFAULT );
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

  std::string text;
  mController->GetText( text );

  if( ( fabsf( relayoutSize.width ) < Math::MACHINE_EPSILON_1000 ) || ( fabsf( relayoutSize.height ) < Math::MACHINE_EPSILON_1000 ) || text.empty() )
  {
    // Removes the texture set.
    RemoveTextureSet();

    // Remove any renderer previously set.
    if( mImpl->mRenderer )
    {
      control.RemoveRenderer( mImpl->mRenderer );
    }

    // Nothing else to do if the relayout size is zero.
    ResourceReady( Toolkit::Visual::ResourceStatus::READY );
    return;
  }

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout( relayoutSize );

  if( Text::Controller::NONE_UPDATED != ( Text::Controller::MODEL_UPDATED & updateTextType )
   || mRendererUpdateNeeded )
  {
    mRendererUpdateNeeded = false;

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
      // Check whether it is a markup text with multiple text colors
      const Vector4* const colorsBuffer = mController->GetTextModel()->GetColors();
      bool hasMultipleTextColors = ( NULL != colorsBuffer );

      // Check whether the text contains any color glyph
      bool containsColorGlyph = false;

      TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
      const Text::GlyphInfo* const glyphsBuffer = mController->GetTextModel()->GetGlyphs();
      const Text::Length numberOfGlyphs = mController->GetTextModel()->GetNumberOfGlyphs();
      for ( Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++ )
      {
        // Retrieve the glyph's info.
        const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

        // Whether the current glyph is a color one.
        if( fontClient.IsColorGlyph( glyphInfo->fontId, glyphInfo->index ) )
        {
          containsColorGlyph = true;
          break;
        }
      }

      // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)

      bool shadowEnabled = false;
      const Vector2& shadowOffset = mController->GetTextModel()->GetShadowOffset();
      if ( fabsf( shadowOffset.x ) > Math::MACHINE_EPSILON_1 || fabsf( shadowOffset.y ) > Math::MACHINE_EPSILON_1 )
      {
        shadowEnabled = true;
      }

      const bool underlineEnabled = mController->GetTextModel()->IsUnderlineEnabled();
      const bool outlineEnabled = ( mController->GetTextModel()->GetOutlineWidth() > Math::MACHINE_EPSILON_1 );
      const bool backgroundEnabled = mController->GetTextModel()->IsBackgroundEnabled();;

      const bool styleEnabled = ( shadowEnabled || underlineEnabled || outlineEnabled || backgroundEnabled );

      TextureSet textureSet = GetTextTexture( relayoutSize, hasMultipleTextColors, containsColorGlyph, styleEnabled );
      mImpl->mRenderer.SetTextures( textureSet );

      Shader shader = GetTextShader( mFactoryCache, hasMultipleTextColors, containsColorGlyph, styleEnabled );
      mImpl->mRenderer.SetShader(shader);

      mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

      mImpl->mRenderer.RegisterProperty( "uHasMultipleTextColors", static_cast<float>( hasMultipleTextColors ) );

      mImpl->mRenderer.SetProperty( Renderer::Property::BLEND_MODE, BlendMode::ON);

      //Register transform properties
      mImpl->mTransform.RegisterUniforms( mImpl->mRenderer, Direction::LEFT_TO_RIGHT );

      control.AddRenderer( mImpl->mRenderer );

      // Text rendered and ready to display
      ResourceReady( Toolkit::Visual::ResourceStatus::READY );
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

TextureSet TextVisual::GetTextTexture( const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled )
{
  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );

  TextureSet textureSet = TextureSet::New();

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat = ( containsColorGlyph || hasMultipleTextColors ) ? Pixel::RGBA8888 : Pixel::L8;

  // Check the text direction
  Toolkit::DevelText::TextDirection::Type textDirection = mController->GetTextDirection();

  // Create a texture for the text without any styles
  PixelData data = mTypesetter->Render( size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat );

  // It may happen the image atlas can't handle a pixel data it exceeds the maximum size.
  // In that case, create a texture. TODO: should tile the text.

  Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                  data.GetPixelFormat(),
                                  data.GetWidth(),
                                  data.GetHeight() );

  texture.Upload( data );

  textureSet.SetTexture( 0u, texture );
  textureSet.SetSampler( 0u, sampler );

  if ( styleEnabled )
  {
    // Create RGBA texture for all the text styles (without the text itself)
    PixelData styleData = mTypesetter->Render( size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888 );

    Texture styleTexture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                         styleData.GetPixelFormat(),
                                         styleData.GetWidth(),
                                         styleData.GetHeight() );

    styleTexture.Upload( styleData );

    textureSet.SetTexture( 1u, styleTexture );
    textureSet.SetSampler( 1u, sampler );
  }

  if ( containsColorGlyph && !hasMultipleTextColors )
  {
    // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    PixelData maskData = mTypesetter->Render( size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8 );

    Texture maskTexture = Texture::New( Dali::TextureType::TEXTURE_2D,
                                        maskData.GetPixelFormat(),
                                        maskData.GetWidth(),
                                        maskData.GetHeight() );

    maskTexture.Upload( maskData );

    if ( !styleEnabled )
    {
      textureSet.SetTexture( 1u, maskTexture );
      textureSet.SetSampler( 1u, sampler );
    }
    else
    {
      textureSet.SetTexture( 2u, maskTexture );
      textureSet.SetSampler( 2u, sampler );
    }
  }

  return textureSet;
}

Shader TextVisual::GetTextShader( VisualFactoryCache& factoryCache, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled )
{
  Shader shader;

  if( hasMultipleTextColors && !styleEnabled )
  {
    // We don't animate text color if the text contains multiple colors
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT );
    if( !shader )
    {

      shader = DevelShader::New(
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SHADER_VERT" ),
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_SHADER_FRAG" ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );

      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, shader );
    }
  }
  else if( hasMultipleTextColors && styleEnabled )
  {
    // We don't animate text color if the text contains multiple colors
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE );
    if( !shader )
    {
      shader = DevelShader::New(
        GraphicsGetBuiltinShaderId( SHADER_TEXT_VISUAL_SHADER_VERT ),
        GraphicsGetBuiltinShaderId( SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_WITH_STYLE_SHADER_FRAG ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE, shader );
    }
  }
  else if( !hasMultipleTextColors && !containsColorGlyph && !styleEnabled )
  {
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT );
    if( !shader )
    {
      shader = DevelShader::New(
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SHADER_VERT" ),
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_SHADER_FRAG" ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );

      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, shader );
    }
  }
  else if( !hasMultipleTextColors && !containsColorGlyph && styleEnabled )
  {
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE );
    if( !shader )
    {
      shader = DevelShader::New(
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SHADER_VERT" ),
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_SHADER_FRAG" ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );

      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE, shader );
    }
  }
  else if( !hasMultipleTextColors && containsColorGlyph && !styleEnabled )
  {
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI );
    if( !shader )
    {
      shader = DevelShader::New(
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SHADER_VERT" ),
        GraphicsGetBuiltinShader( "SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_EMOJI_SHADER_FRAG" ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI, shader );
    }
  }
  else // if( !hasMultipleTextColors && containsColorGlyph && styleEnabled )
  {
    shader = factoryCache.GetShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI );
    if( !shader )
    {
      shader = DevelShader::New(
        GraphicsGetBuiltinShaderId( SHADER_TEXT_VISUAL_SHADER_VERT ),
        GraphicsGetBuiltinShaderId( SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI_SHADER_FRAG ),
        DevelShader::ShaderLanguage::SPIRV_1_0,
        Property::Map()
      );
      shader.RegisterProperty( PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT );
      factoryCache.SaveShader( VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI, shader );
    }
  }

  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
