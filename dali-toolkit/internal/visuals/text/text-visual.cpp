/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/images/pixel-data-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
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
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

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

TextVisualPtr TextVisual::New(VisualFactoryCache& factoryCache, const Property::Map& properties)
{
  TextVisualPtr textVisualPtr(new TextVisual(factoryCache));
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
}

void TextVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT);
  std::string text;
  mController->GetText(text);
  map.Insert(Toolkit::TextVisual::Property::TEXT, text);
}

TextVisual::TextVisual(VisualFactoryCache& factoryCache)
: Visual::Base(factoryCache, Visual::FittingMode::FIT_KEEP_ASPECT_RATIO, Toolkit::Visual::TEXT),
  mController(Text::Controller::New()),
  mTypesetter(Text::Typesetter::New(mController->GetTextModel())),
  mAnimatableTextColorPropertyIndex(Property::INVALID_INDEX),
  mRendererUpdateNeeded(false)
{
}

TextVisual::~TextVisual()
{
}

void TextVisual::OnInitialize()
{
  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  Shader   shader   = GetTextShader(mFactoryCache, TextType::SINGLE_COLOR_TEXT, TextType::NO_EMOJI, TextType::NO_STYLES);

  mImpl->mRenderer = Renderer::New(geometry, shader);
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

  // Enable the pre-multiplied alpha to improve the text quality
  EnablePreMultipliedAlpha(true);

  if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
  {
    const Vector4&        defaultColor         = mController->GetTextModel()->GetDefaultColor();
    Dali::Property::Index shaderTextColorIndex = mImpl->mRenderer.RegisterProperty("uTextColorAnimatable", defaultColor);

    // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
    if(shaderTextColorIndex != Property::INVALID_INDEX)
    {
      if(!mColorConstraint)
      {
        mColorConstraint = Constraint::New<Vector4>(mImpl->mRenderer, shaderTextColorIndex, TextColorConstraint);
        mColorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
      }
      mColorConstraint.Apply();

      // Make zero if the alpha value of text color is zero to skip rendering text
      if(!mOpacityConstraint)
      {
        mOpacityConstraint = Constraint::New<float>(mImpl->mRenderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
        mOpacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
      }
      mOpacityConstraint.Apply();
    }
  }

  // Renderer needs textures and to be added to control
  mRendererUpdateNeeded = true;

  mRendererList.push_back(mImpl->mRenderer);

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

  std::string text;
  mController->GetText(text);

  if((fabsf(relayoutSize.width) < Math::MACHINE_EPSILON_1000) || (fabsf(relayoutSize.height) < Math::MACHINE_EPSILON_1000) || text.empty())
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

      const bool underlineEnabled       = mController->GetTextModel()->IsUnderlineEnabled();
      const bool outlineEnabled         = (mController->GetTextModel()->GetOutlineWidth() > Math::MACHINE_EPSILON_1);
      const bool backgroundEnabled      = mController->GetTextModel()->IsBackgroundEnabled();
      const bool markupProcessorEnabled = mController->IsMarkupProcessorEnabled();

      const bool styleEnabled = (shadowEnabled || underlineEnabled || outlineEnabled || backgroundEnabled || markupProcessorEnabled);

      AddRenderer(control, relayoutSize, hasMultipleTextColors, containsColorGlyph, styleEnabled);

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

PixelData TextVisual::ConvertToPixelData(unsigned char* buffer, int width, int height, int offsetPosition, const Pixel::Format textPixelFormat)
{
  int            bpp        = Pixel::GetBytesPerPixel(textPixelFormat);
  unsigned int   bufferSize = width * height * bpp;
  unsigned char* dstBuffer  = static_cast<unsigned char*>(malloc(bufferSize));
  memcpy(dstBuffer, buffer + offsetPosition * bpp, bufferSize);
  PixelData pixelData = Dali::PixelData::New(dstBuffer,
                                             bufferSize,
                                             width,
                                             height,
                                             textPixelFormat,
                                             Dali::PixelData::FREE);
  return pixelData;
}

void TextVisual::CreateTextureSet(TilingInfo& info, Renderer& renderer, Sampler& sampler, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled)
{
  TextureSet   textureSet      = TextureSet::New();
  unsigned int textureSetIndex = 0u;

  // Convert the buffer to pixel data to make it a texture.
  if(info.textBuffer)
  {
    PixelData data = ConvertToPixelData(info.textBuffer, info.width, info.height, info.offsetPosition, info.textPixelFormat);
    AddTexture(textureSet, data, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(styleEnabled && info.styleBuffer)
  {
    PixelData styleData = ConvertToPixelData(info.styleBuffer, info.width, info.height, info.offsetPosition, Pixel::RGBA8888);
    AddTexture(textureSet, styleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(containsColorGlyph && !hasMultipleTextColors && info.maskBuffer)
  {
    PixelData maskData = ConvertToPixelData(info.maskBuffer, info.width, info.height, info.offsetPosition, Pixel::L8);
    AddTexture(textureSet, maskData, sampler, textureSetIndex);
  }

  renderer.SetTextures(textureSet);

  //Register transform properties
  mImpl->mTransform.RegisterUniforms(renderer, Direction::LEFT_TO_RIGHT);

  // Enable the pre-multiplied alpha to improve the text quality
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
  renderer.RegisterProperty(PREMULTIPLIED_ALPHA, 1.0f);

  // Set size and offset for the tiling.
  renderer.RegisterProperty(SIZE, Vector2(info.width, info.height));
  renderer.RegisterProperty(OFFSET, Vector2(info.offSet.x, info.offSet.y));
  renderer.RegisterProperty("uHasMultipleTextColors", static_cast<float>(hasMultipleTextColors));
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

  mRendererList.push_back(renderer);
}

void TextVisual::AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled)
{
  Shader shader = GetTextShader(mFactoryCache, hasMultipleTextColors, containsColorGlyph, styleEnabled);
  mImpl->mRenderer.SetShader(shader);

  // Get the maximum size.
  const int maxTextureSize = Dali::GetMaxTextureSize();

  // No tiling required. Use the default renderer.
  if(size.height < maxTextureSize)
  {
    TextureSet textureSet = GetTextTexture(size, hasMultipleTextColors, containsColorGlyph, styleEnabled);

    mImpl->mRenderer.SetTextures(textureSet);
    //Register transform properties
    mImpl->mTransform.RegisterUniforms(mImpl->mRenderer, Direction::LEFT_TO_RIGHT);
    mImpl->mRenderer.RegisterProperty("uHasMultipleTextColors", static_cast<float>(hasMultipleTextColors));
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

    // Get the buffer of text.
    Dali::DevelPixelData::PixelDataBuffer textPixelData = Dali::DevelPixelData::ReleasePixelDataBuffer(data);
    info.textBuffer                                     = textPixelData.buffer;

    if(styleEnabled)
    {
      // Create RGBA texture for all the text styles (without the text itself)
      PixelData                             styleData      = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
      Dali::DevelPixelData::PixelDataBuffer stylePixelData = Dali::DevelPixelData::ReleasePixelDataBuffer(styleData);
      info.styleBuffer                                     = stylePixelData.buffer;
    }

    if(containsColorGlyph && !hasMultipleTextColors)
    {
      // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
      PixelData                             maskData      = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
      Dali::DevelPixelData::PixelDataBuffer maskPixelData = Dali::DevelPixelData::ReleasePixelDataBuffer(maskData);
      info.maskBuffer                                     = maskPixelData.buffer;
    }

    // Get the current offset for recalculate the offset when tiling.
    Property::Map retMap;
    mImpl->mTransform.GetPropertyMap(retMap);
    Property::Value* offsetValue = retMap.Find(Dali::Toolkit::Visual::Transform::Property::OFFSET);
    if(offsetValue)
    {
      offsetValue->Get(info.offSet);
    }

    // Create a textureset in the default renderer.
    CreateTextureSet(info, mImpl->mRenderer, sampler, hasMultipleTextColors, containsColorGlyph, styleEnabled);

    verifiedHeight -= maxTextureSize;

    Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

    int offsetPosition = verifiedWidth * maxTextureSize;
    // Create a renderer by cutting maxTextureSize.
    while(verifiedHeight > 0)
    {
      Renderer tilingRenderer = Renderer::New(geometry, shader);
      tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Toolkit::DepthIndex::CONTENT);
      // New offset position of buffer for tiling.
      info.offsetPosition += offsetPosition;
      // New height for tiling.
      info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
      // New offset for tiling.
      info.offSet.y += maxTextureSize;
      // Create a textureset int the new tiling renderer.
      CreateTextureSet(info, tilingRenderer, sampler, hasMultipleTextColors, containsColorGlyph, styleEnabled);

      verifiedHeight -= maxTextureSize;
    }
  }

  mImpl->mFlags &= ~Impl::IS_ATLASING_APPLIED;

  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    Renderer renderer = (*iter);
    if(renderer)
    {
      actor.AddRenderer(renderer);
    }
  }
}

TextureSet TextVisual::GetTextTexture(const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled)
{
  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  TextureSet textureSet = TextureSet::New();

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat = (containsColorGlyph || hasMultipleTextColors) ? Pixel::RGBA8888 : Pixel::L8;

  // Check the text direction
  Toolkit::DevelText::TextDirection::Type textDirection = mController->GetTextDirection();

  // Create a texture for the text without any styles
  PixelData data = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

  // It may happen the image atlas can't handle a pixel data it exceeds the maximum size.
  // In that case, create a texture. TODO: should tile the text.
  unsigned int textureSetIndex = 0u;

  AddTexture(textureSet, data, sampler, textureSetIndex);
  ++textureSetIndex;

  if(styleEnabled)
  {
    // Create RGBA texture for all the text styles (without the text itself)
    PixelData styleData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);

    AddTexture(textureSet, styleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(containsColorGlyph && !hasMultipleTextColors)
  {
    // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    PixelData maskData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);

    AddTexture(textureSet, maskData, sampler, textureSetIndex);
  }

  return textureSet;
}

Shader TextVisual::GetTextShader(VisualFactoryCache& factoryCache, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled)
{
  Shader shader;

  if(hasMultipleTextColors && !styleEnabled)
  {
    // We don't animate text color if the text contains multiple colors
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT, shader);
    }
  }
  else if(hasMultipleTextColors && styleEnabled)
  {
    // We don't animate text color if the text contains multiple colors
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_WITH_STYLE_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE, shader);
    }
  }
  else if(!hasMultipleTextColors && !containsColorGlyph && !styleEnabled)
  {
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT, shader);
    }
  }
  else if(!hasMultipleTextColors && !containsColorGlyph && styleEnabled)
  {
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE, shader);
    }
  }
  else if(!hasMultipleTextColors && containsColorGlyph && !styleEnabled)
  {
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_EMOJI_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI, shader);
    }
  }
  else // if( !hasMultipleTextColors && containsColorGlyph && styleEnabled )
  {
    shader = factoryCache.GetShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI);
    if(!shader)
    {
      shader = Shader::New(SHADER_TEXT_VISUAL_SHADER_VERT, SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI_SHADER_FRAG);
      shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
      factoryCache.SaveShader(VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI, shader);
    }
  }

  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
