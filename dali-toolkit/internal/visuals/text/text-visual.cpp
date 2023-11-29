/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <string.h>

// INTERNAL HEADER
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/devel-api/visuals/text-visual-properties-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/visual-base-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

const int CUSTOM_PROPERTY_COUNT(2); // uTextColorAnimatable, uHasMultipleTextColors

/**
 * Return Property index for the given string key
 * param[in] stringKey the string index key
 * return the key as an index
 */

Dali::Property::Index StringKeyToIndexKey(const std::string& stringKey)
{
  Dali::Property::Index result = Property::INVALID_KEY;

  if(stringKey == VISUAL_TYPE)
  {
    result = Toolkit::Visual::Property::TYPE;
  }
  else if(stringKey == TEXT_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::TEXT;
  }
  else if(stringKey == FONT_FAMILY_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::FONT_FAMILY;
  }
  else if(stringKey == FONT_STYLE_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::FONT_STYLE;
  }
  else if(stringKey == POINT_SIZE_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::POINT_SIZE;
  }
  else if(stringKey == MULTI_LINE_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::MULTI_LINE;
  }
  else if(stringKey == HORIZONTAL_ALIGNMENT_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT;
  }
  else if(stringKey == VERTICAL_ALIGNMENT_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT;
  }
  else if(stringKey == TEXT_COLOR_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::TEXT_COLOR;
  }
  else if(stringKey == ENABLE_MARKUP_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::ENABLE_MARKUP;
  }
  else if(stringKey == SHADOW_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::SHADOW;
  }
  else if(stringKey == UNDERLINE_PROPERTY)
  {
    result = Toolkit::TextVisual::Property::UNDERLINE;
  }
  else if(stringKey == OUTLINE_PROPERTY)
  {
    result = Toolkit::DevelTextVisual::Property::OUTLINE;
  }
  else if(stringKey == BACKGROUND_PROPERTY)
  {
    result = Toolkit::DevelTextVisual::Property::BACKGROUND;
  }

  return result;
}

void TextColorConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  Vector4 color = inputs[0]->GetVector4();
  current.r     = color.r * color.a;
  current.g     = color.g * color.a;
  current.b     = color.b * color.a;
  current.a     = color.a;
}

void OpacityConstraint(float& current, const PropertyInputContainer& inputs)
{
  // Make zero if the alpha value of text color is zero to skip rendering text
  if(EqualsZero(inputs[0]->GetVector4().a))
  {
    current = 0.0f;
  }
  else
  {
    current = 1.0f;
  }
}

} // unnamed namespace

TextVisualPtr TextVisual::New(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory, const Property::Map& properties)
{
  TextVisualPtr textVisualPtr(new TextVisual(factoryCache, shaderFactory));
  textVisualPtr->SetProperties(properties);
  textVisualPtr->Initialize();
  return textVisualPtr;
}

Property::Map TextVisual::ConvertStringKeysToIndexKeys(const Property::Map& propertyMap)
{
  Property::Map outMap;

  for(Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index)
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue(index);

    Property::Index indexKey = keyValue.first.indexKey;

    if(keyValue.first.type == Property::Key::STRING)
    {
      indexKey = StringKeyToIndexKey(keyValue.first.stringKey);
    }

    outMap.Insert(indexKey, keyValue.second);
  }

  return outMap;
}

float TextVisual::GetHeightForWidth(float width)
{
  return mController->GetHeightForWidth(width);
}

void TextVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = mController->GetNaturalSize().GetVectorXY();
}

void TextVisual::DoCreatePropertyMap(Property::Map& map) const
{
  Property::Value value;

  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT);

  std::string text;
  mController->GetText(text);
  map.Insert(Toolkit::TextVisual::Property::TEXT, text);

  map.Insert(Toolkit::TextVisual::Property::FONT_FAMILY, mController->GetDefaultFontFamily());

  GetFontStyleProperty(mController, value, Text::FontStyle::DEFAULT);
  map.Insert(Toolkit::TextVisual::Property::FONT_STYLE, value);

  map.Insert(Toolkit::TextVisual::Property::POINT_SIZE, mController->GetDefaultFontSize(Text::Controller::POINT_SIZE));

  map.Insert(Toolkit::TextVisual::Property::MULTI_LINE, mController->IsMultiLineEnabled());

  map.Insert(Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, mController->GetHorizontalAlignment());

  map.Insert(Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, mController->GetVerticalAlignment());

  map.Insert(Toolkit::TextVisual::Property::TEXT_COLOR, mController->GetDefaultColor());

  map.Insert(Toolkit::TextVisual::Property::ENABLE_MARKUP, mController->IsMarkupProcessorEnabled());

  GetShadowProperties(mController, value, Text::EffectStyle::DEFAULT);
  map.Insert(Toolkit::TextVisual::Property::SHADOW, value);

  GetUnderlineProperties(mController, value, Text::EffectStyle::DEFAULT);
  map.Insert(Toolkit::TextVisual::Property::UNDERLINE, value);

  GetOutlineProperties(mController, value, Text::EffectStyle::DEFAULT);
  map.Insert(Toolkit::DevelTextVisual::Property::OUTLINE, value);

  GetBackgroundProperties(mController, value, Text::EffectStyle::DEFAULT);
  map.Insert(Toolkit::DevelTextVisual::Property::BACKGROUND, value);

  GetStrikethroughProperties(mController, value, Text::EffectStyle::DEFAULT);
  map.Insert(Toolkit::DevelTextVisual::Property::STRIKETHROUGH, value);
}

void TextVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT);
  std::string text;
  mController->GetText(text);
  map.Insert(Toolkit::TextVisual::Property::TEXT, text);
}

void TextVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  // Make always enable pre multiplied alpha whether preMultiplied value is false.
  if(!preMultiplied)
  {
    DALI_LOG_WARNING("Note : TextVisual cannot disable PreMultipliedAlpha\n");
  }
}

TextVisual::TextVisual(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory)
: Visual::Base(factoryCache, Visual::FittingMode::FIT_KEEP_ASPECT_RATIO, Toolkit::Visual::TEXT),
  mController(Text::Controller::New()),
  mTypesetter(Text::Typesetter::New(mController->GetTextModel())),
  mTextVisualShaderFactory(shaderFactory),
  mTextShaderFeatureCache(),
  mHasMultipleTextColorsIndex(Property::INVALID_INDEX),
  mAnimatableTextColorPropertyIndex(Property::INVALID_INDEX),
  mTextColorAnimatableIndex(Property::INVALID_INDEX),
  mRendererUpdateNeeded(false)
{
  // Enable the pre-multiplied alpha to improve the text quality
  mImpl->mFlags |= Impl::IS_PREMULTIPLIED_ALPHA;
}

TextVisual::~TextVisual()
{
}

void TextVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  Shader   shader   = GetTextShader(mFactoryCache, TextVisualShaderFeature::FeatureBuilder());

  mImpl->mRenderer = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);
  mHasMultipleTextColorsIndex = mImpl->mRenderer.RegisterUniqueProperty("uHasMultipleTextColors", false);
}

void TextVisual::DoSetProperties(const Property::Map& propertyMap)
{
  for(Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index)
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue(index);

    Property::Index indexKey = keyValue.first.indexKey;

    if(keyValue.first.type == Property::Key::STRING)
    {
      indexKey = StringKeyToIndexKey(keyValue.first.stringKey);
    }

    DoSetProperty(indexKey, keyValue.second);
  }

  // Elide the text if it exceeds the boundaries.
  mController->SetTextElideEnabled(true);

  // Retrieve the layout engine to set the cursor's width.
  Text::Layout::Engine& engine = mController->GetLayoutEngine();

  // Sets 0 as cursor's width.
  engine.SetCursorWidth(0u); // Do not layout space for the cursor.
}

void TextVisual::DoSetOnScene(Actor& actor)
{
  mControl = actor;

  mImpl->mRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Toolkit::DepthIndex::CONTENT);

  const Vector4& defaultColor = mController->GetTextModel()->GetDefaultColor();
  if(mTextColorAnimatableIndex == Property::INVALID_INDEX)
  {
    mTextColorAnimatableIndex = mImpl->mRenderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);
  }
  else
  {
    mImpl->mRenderer.SetProperty(mTextColorAnimatableIndex, defaultColor);
  }

  if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
  {
    // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
    if(mTextColorAnimatableIndex != Property::INVALID_INDEX)
    {
      if(!mColorConstraint)
      {
        mColorConstraint = Constraint::New<Vector4>(mImpl->mRenderer, mTextColorAnimatableIndex, TextColorConstraint);
        mColorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
      }
      mColorConstraint.Apply();
    }

    // Make zero if the alpha value of text color is zero to skip rendering text
    if(!mOpacityConstraint)
    {
      // VisualRenderer::Property::OPACITY uses same animatable property internally.
      mOpacityConstraint = Constraint::New<float>(mImpl->mRenderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
      mOpacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
    }
    mOpacityConstraint.Apply();
  }

  // Renderer needs textures and to be added to control
  mRendererUpdateNeeded = true;

  UpdateRenderer();
}

void TextVisual::RemoveRenderer(Actor& actor)
{
  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    Renderer renderer = (*iter);
    if(renderer)
    {
      // Removes the renderer from the actor.
      actor.RemoveRenderer(renderer);
    }
  }
  // Clear the renderer list
  mRendererList.clear();
}

void TextVisual::DoSetOffScene(Actor& actor)
{
  if(mColorConstraint)
  {
    mColorConstraint.Remove();
  }
  if(mOpacityConstraint)
  {
    mOpacityConstraint.Remove();
  }

  RemoveRenderer(actor);

  // Resets the control handle.
  mControl.Reset();
}

void TextVisual::OnSetTransform()
{
  UpdateRenderer();
}

void TextVisual::DoSetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    case Toolkit::TextVisual::Property::ENABLE_MARKUP:
    {
      const bool enableMarkup = propertyValue.Get<bool>();
      mController->SetMarkupProcessorEnabled(enableMarkup);
      break;
    }
    case Toolkit::TextVisual::Property::TEXT:
    {
      mController->SetText(propertyValue.Get<std::string>());
      break;
    }
    case Toolkit::TextVisual::Property::FONT_FAMILY:
    {
      SetFontFamilyProperty(mController, propertyValue);
      break;
    }
    case Toolkit::TextVisual::Property::FONT_STYLE:
    {
      SetFontStyleProperty(mController, propertyValue, Text::FontStyle::DEFAULT);
      break;
    }
    case Toolkit::TextVisual::Property::POINT_SIZE:
    {
      const float pointSize = propertyValue.Get<float>();
      if(!Equals(mController->GetDefaultFontSize(Text::Controller::POINT_SIZE), pointSize))
      {
        mController->SetDefaultFontSize(pointSize, Text::Controller::POINT_SIZE);
      }
      break;
    }
    case Toolkit::TextVisual::Property::MULTI_LINE:
    {
      mController->SetMultiLineEnabled(propertyValue.Get<bool>());
      break;
    }
    case Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT:
    {
      if(mController)
      {
        Text::HorizontalAlignment::Type alignment(static_cast<Text::HorizontalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Toolkit::Text::GetHorizontalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetHorizontalAlignment(alignment);
        }
      }
      break;
    }
    case Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT:
    {
      if(mController)
      {
        Toolkit::Text::VerticalAlignment::Type alignment(static_cast<Text::VerticalAlignment::Type>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Toolkit::Text::GetVerticalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetVerticalAlignment(alignment);
        }
      }
      break;
    }
    case Toolkit::TextVisual::Property::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
      }
      break;
    }
    case Toolkit::TextVisual::Property::SHADOW:
    {
      SetShadowProperties(mController, propertyValue, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::TextVisual::Property::UNDERLINE:
    {
      SetUnderlineProperties(mController, propertyValue, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::DevelTextVisual::Property::OUTLINE:
    {
      SetOutlineProperties(mController, propertyValue, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::DevelTextVisual::Property::BACKGROUND:
    {
      SetBackgroundProperties(mController, propertyValue, Text::EffectStyle::DEFAULT);
      break;
    }
    case Toolkit::DevelTextVisual::Property::STRIKETHROUGH:
    {
      SetStrikethroughProperties(mController, propertyValue, Text::EffectStyle::DEFAULT);
      break;
    }
  }
}

void TextVisual::UpdateRenderer()
{
  Actor control = mControl.GetHandle();
  if(!control)
  {
    // Nothing to do.
    return;
  }

  // Calculates the size to be used to relayout.
  Vector2 relayoutSize;

  const bool isWidthRelative  = fabsf(mImpl->mTransform.mOffsetSizeMode.z) < Math::MACHINE_EPSILON_1000;
  const bool isHeightRelative = fabsf(mImpl->mTransform.mOffsetSizeMode.w) < Math::MACHINE_EPSILON_1000;

  // Round the size and offset to avoid pixel alignement issues.
  relayoutSize.width  = floorf(0.5f + (isWidthRelative ? mImpl->mControlSize.width * mImpl->mTransform.mSize.x : mImpl->mTransform.mSize.width));
  relayoutSize.height = floorf(0.5f + (isHeightRelative ? mImpl->mControlSize.height * mImpl->mTransform.mSize.y : mImpl->mTransform.mSize.height));

  auto textLengthUtf32 = mController->GetNumberOfCharacters();

  if((fabsf(relayoutSize.width) < Math::MACHINE_EPSILON_1000) || (fabsf(relayoutSize.height) < Math::MACHINE_EPSILON_1000) || textLengthUtf32 == 0u)
  {
    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control);

    // Nothing else to do if the relayout size is zero.
    ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    return;
  }

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(control);

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(relayoutSize, layoutDirection);

  if(Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType) || mRendererUpdateNeeded)
  {
    mRendererUpdateNeeded = false;

    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control);

    if((relayoutSize.width > Math::MACHINE_EPSILON_1000) &&
       (relayoutSize.height > Math::MACHINE_EPSILON_1000))
    {
      // Check whether it is a markup text with multiple text colors
      const Vector4* const colorsBuffer          = mController->GetTextModel()->GetColors();
      bool                 hasMultipleTextColors = (NULL != colorsBuffer);

      // Check whether the text contains any color glyph
      bool containsColorGlyph = false;

      TextAbstraction::FontClient  fontClient     = TextAbstraction::FontClient::Get();
      const Text::GlyphInfo* const glyphsBuffer   = mController->GetTextModel()->GetGlyphs();
      const Text::Length           numberOfGlyphs = mController->GetTextModel()->GetNumberOfGlyphs();
      for(Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++)
      {
        // Retrieve the glyph's info.
        const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

        // Whether the current glyph is a color one.
        if(fontClient.IsColorGlyph(glyphInfo->fontId, glyphInfo->index))
        {
          containsColorGlyph = true;
          break;
        }
      }

      // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)

      bool           shadowEnabled = false;
      const Vector2& shadowOffset  = mController->GetTextModel()->GetShadowOffset();
      if(fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1)
      {
        shadowEnabled = true;
      }

      const bool outlineEnabled             = (mController->GetTextModel()->GetOutlineWidth() > Math::MACHINE_EPSILON_1);
      const bool backgroundEnabled          = mController->GetTextModel()->IsBackgroundEnabled();
      const bool markupOrSpannedText        = mController->IsMarkupProcessorEnabled() || mController->GetTextModel()->IsSpannedTextPlaced();
      const bool markupUnderlineEnabled     = markupOrSpannedText && mController->GetTextModel()->IsMarkupUnderlineSet();
      const bool markupStrikethroughEnabled = markupOrSpannedText && mController->GetTextModel()->IsMarkupStrikethroughSet();
      const bool underlineEnabled           = mController->GetTextModel()->IsUnderlineEnabled() || markupUnderlineEnabled;
      const bool strikethroughEnabled       = mController->GetTextModel()->IsStrikethroughEnabled() || markupStrikethroughEnabled;
      const bool backgroundMarkupSet        = mController->GetTextModel()->IsMarkupBackgroundColorSet();
      const bool styleEnabled               = (shadowEnabled || outlineEnabled || backgroundEnabled || markupOrSpannedText || backgroundMarkupSet);
      const bool isOverlayStyle             = underlineEnabled || strikethroughEnabled;

      AddRenderer(control, relayoutSize, hasMultipleTextColors, containsColorGlyph, styleEnabled, isOverlayStyle);

      // Text rendered and ready to display
      ResourceReady(Toolkit::Visual::ResourceStatus::READY);
    }
  }
}

void TextVisual::AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex)
{
  Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D,
                                 data.GetPixelFormat(),
                                 data.GetWidth(),
                                 data.GetHeight());
  texture.Upload(data);

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::AddTilingTexture(TextureSet& textureSet, TilingInfo& tilingInfo, PixelData& data, Sampler& sampler, unsigned int textureSetIndex)
{
  Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D,
                                 tilingInfo.textPixelFormat,
                                 tilingInfo.width,
                                 tilingInfo.height);
  DevelTexture::UploadSubPixelData(texture, data, 0u, tilingInfo.offsetHeight, tilingInfo.width, tilingInfo.height);

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::CreateTextureSet(TilingInfo& info, VisualRenderer& renderer, Sampler& sampler)
{
  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 0u;

  // Convert the buffer to pixel data to make it a texture.

  if(info.textPixelData)
  {
    AddTilingTexture(textureSet, info, info.textPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledStyle() && info.stylePixelData)
  {
    AddTilingTexture(textureSet, info, info.stylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay() && info.overlayStylePixelData)
  {
    AddTilingTexture(textureSet, info, info.overlayStylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor() && info.maskPixelData)
  {
    AddTilingTexture(textureSet, info, info.maskPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  renderer.SetTextures(textureSet);

  //Register transform properties
  mImpl->mTransform.SetUniforms(renderer, Direction::LEFT_TO_RIGHT);

  // Enable the pre-multiplied alpha to improve the text quality
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
  renderer.SetProperty(VisualRenderer::Property::VISUAL_PRE_MULTIPLIED_ALPHA, true);

  // Set size and offset for the tiling.
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, Vector2(info.width, info.height));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, info.transformOffset);
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  renderer.RegisterProperty("uHasMultipleTextColors", static_cast<float>(mTextShaderFeatureCache.IsEnabledMultiColor()));

  mRendererList.push_back(renderer);
}

void TextVisual::AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled, bool isOverlayStyle)
{
  Shader shader = GetTextShader(mFactoryCache, TextVisualShaderFeature::FeatureBuilder().EnableMultiColor(hasMultipleTextColors).EnableEmoji(containsColorGlyph).EnableStyle(styleEnabled).EnableOverlay(isOverlayStyle));
  mImpl->mRenderer.SetShader(shader);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_VISUAL_UPDATE_RENDERER");

  // Get the maximum size.
  const int maxTextureSize = Dali::GetMaxTextureSize();

  // No tiling required. Use the default renderer.
  if(size.height < maxTextureSize)
  {
    TextureSet textureSet = GetTextTexture(size);

    mImpl->mRenderer.SetTextures(textureSet);
    //Register transform properties
    mImpl->mTransform.SetUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
    mImpl->mRenderer.SetProperty(mHasMultipleTextColorsIndex, static_cast<float>(hasMultipleTextColors));
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

    mRendererList.push_back(mImpl->mRenderer);
  }
  // If the pixel data exceeds the maximum size, tiling is required.
  else
  {
    // Filter mode needs to be set to linear to produce better quality while scaling.
    Sampler sampler = Sampler::New();
    sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

    // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
    Pixel::Format textPixelFormat = (containsColorGlyph || hasMultipleTextColors) ? Pixel::RGBA8888 : Pixel::L8;

    // Check the text direction
    Toolkit::DevelText::TextDirection::Type textDirection = mController->GetTextDirection();

    // Create a texture for the text without any styles
    PixelData data = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

    int verifiedWidth  = data.GetWidth();
    int verifiedHeight = data.GetHeight();

    // Set information for creating textures.
    TilingInfo info(verifiedWidth, maxTextureSize, textPixelFormat);

    // Get the pixel data of text.
    info.textPixelData = data;

    if(mTextShaderFeatureCache.IsEnabledStyle())
    {
      // Create RGBA texture for all the text styles (without the text itself)
      info.stylePixelData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledOverlay())
    {
      // Create RGBA texture for all the overlay styles
      info.overlayStylePixelData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
    {
      // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
      info.maskPixelData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
    }

    // Get the current offset for recalculate the offset when tiling.
    Property::Map retMap;
    mImpl->mTransform.GetPropertyMap(retMap);
    Property::Value* offsetValue = retMap.Find(Dali::Toolkit::Visual::Transform::Property::OFFSET);
    if(offsetValue)
    {
      offsetValue->Get(info.transformOffset);
    }

    // Create a textureset in the default renderer.
    CreateTextureSet(info, mImpl->mRenderer, sampler);

    verifiedHeight -= maxTextureSize;

    Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

    // Create a renderer by cutting maxTextureSize.
    while(verifiedHeight > 0)
    {
      VisualRenderer tilingRenderer = VisualRenderer::New(geometry, shader);
      tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Toolkit::DepthIndex::CONTENT);
      // New offset position of buffer for tiling.
      info.offsetHeight += maxTextureSize;
      // New height for tiling.
      info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
      // New offset for tiling.
      info.transformOffset.y += maxTextureSize;
      // Create a textureset int the new tiling renderer.
      CreateTextureSet(info, tilingRenderer, sampler);

      verifiedHeight -= maxTextureSize;
    }
  }

  mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

  const Vector4& defaultColor = mController->GetTextModel()->GetDefaultColor();

  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    Renderer renderer = (*iter);
    if(renderer)
    {
      actor.AddRenderer(renderer);

      if(renderer != mImpl->mRenderer)
      {
        // Set constraint for text label's color for non-default renderers.
        if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
        {
          // Register unique property, or get property for default renderer.
          Property::Index index = renderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);

          // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
          if(index != Property::INVALID_INDEX)
          {
            Constraint colorConstraint = Constraint::New<Vector4>(renderer, index, TextColorConstraint);
            colorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
            colorConstraint.Apply();
          }

          // Make zero if the alpha value of text color is zero to skip rendering text
          // VisualRenderer::Property::OPACITY uses same animatable property internally.
          Constraint opacityConstraint = Constraint::New<float>(renderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
          opacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
          opacityConstraint.Apply();
        }
      }
    }
  }
}

TextureSet TextVisual::GetTextTexture(const Vector2& size)
{
  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  TextureSet textureSet = TextureSet::New();

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat = (mTextShaderFeatureCache.IsEnabledEmoji() || mTextShaderFeatureCache.IsEnabledMultiColor()) ? Pixel::RGBA8888 : Pixel::L8;

  // Check the text direction
  Toolkit::DevelText::TextDirection::Type textDirection = mController->GetTextDirection();

  // Create a texture for the text without any styles
  PixelData data = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

  uint32_t textureSetIndex = 0u;
  AddTexture(textureSet, data, sampler, textureSetIndex);
  ++textureSetIndex;

  if(mTextShaderFeatureCache.IsEnabledStyle())
  {
    // Create RGBA texture for all the text styles that render in the background (without the text itself)
    PixelData styleData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    AddTexture(textureSet, styleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay())
  {
    // Create RGBA texture for overlay styles such as underline and strikethrough (without the text itself)
    PixelData overlayStyleData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    AddTexture(textureSet, overlayStyleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
  {
    // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    PixelData maskData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);

    AddTexture(textureSet, maskData, sampler, textureSetIndex);
  }

  return textureSet;
}

Shader TextVisual::GetTextShader(VisualFactoryCache& factoryCache, const TextVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  // Cache feature builder informations.
  mTextShaderFeatureCache = featureBuilder;

  Shader shader = mTextVisualShaderFactory.GetShader(factoryCache, mTextShaderFeatureCache);
  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
