#ifndef DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/visual-renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/visuals/text-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class TextVisual;
typedef IntrusivePtr<TextVisual> TextVisualPtr;

/**
 * The visual which renders text
 *
 * The following properties are optional:
 *
 * | %Property Name      | Type    |
 * |---------------------|---------|
 * | renderingBackend    | INTEGER |
 * | text                | STRING  |
 * | fontFamily          | STRING  |
 * | fontStyle           | STRING  |
 * | pointSize           | FLOAT   |
 * | multiLine           | BOOLEAN |
 * | horizontalAlignment | STRING  |
 * | verticalAlignment   | STRING  |
 * | textColor           | VECTOR4 |
 * | enableMarkup        | BOOLEAN |
 * | enableAutoScroll    | BOOLEAN |
 * | autoScrollSpeed     | INTEGER |
 * | autoScrollLoopCount | INTEGER |
 * | autoScrollGap       | INTEGER |
 * | lineSpacing         | FLOAT   |
 * | underline           | STRING  |
 * | shadow              | STRING  |
 * | outline             | STRING  |
 *
 */
class TextVisual : public Visual::Base
{
public:
  /**
   * @brief Create a new text visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The TextVisualShaderFactory object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static TextVisualPtr New(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory, const Property::Map& properties);

  /**
   * @brief Converts all strings keys in property map to index keys.  Property Map can then be merged correctly.
   * @param[in] propertyMap containing string keys or a mix of strings and indexes.
   * @return Property::Map containing index keys.
   */
  static Property::Map ConvertStringKeysToIndexKeys(const Property::Map& propertyMap);

  /**
   * @brief Retrieve the text's controller.
   * @param[in] visual The text visual.
   * @return The text controller
   */
  static Text::ControllerPtr GetController(Toolkit::Visual::Base visual)
  {
    return GetVisualObject(visual).mController;
  };

  /**
   * @brief Set the index of the animatable text color property.
   * @param[in] visual The text visual.
   * @param[in] animatablePropertyIndex The index of the animatable property
   */
  static void SetAnimatableTextColorProperty(Toolkit::Visual::Base visual, Property::Index animatablePropertyIndex)
  {
    GetVisualObject(visual).mAnimatableTextColorPropertyIndex = animatablePropertyIndex;
  };

  /**
   * @brief Set the flag to trigger the textures to be initialized and renderer to be added to the control.
   * @param[in] visual The text visual.
   */
  static void EnableRendererUpdate(Toolkit::Visual::Base visual)
  {
    GetVisualObject(visual).mRendererUpdateNeeded = true;
  };

  /**
   * @brief Instantly updates the renderer
   * @param[in] visual The text visual.
   */
  static void UpdateRenderer(Toolkit::Visual::Base visual)
  {
    GetVisualObject(visual).UpdateRenderer();
  };

public: // from Visual::Base
  /**
   * @copydoc Visual::Base::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Visual::Base::GetNaturalSize()
   */
  void GetNaturalSize(Vector2& naturalSize) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap()
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::EnablePreMultipliedAlpha
   */
  void EnablePreMultipliedAlpha(bool preMultiplied) override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] shaderFactory The TextVisualShaderFactory object
   */
  TextVisual(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~TextVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  // from Visual::Base

  /**
   * @copydoc Visual::Base::DoSetProperties()
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene()
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene()
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

private:
  struct TilingInfo
  {
    unsigned char* textBuffer;
    unsigned char* styleBuffer;
    unsigned char* overlayStyleBuffer;
    unsigned char* maskBuffer;
    int            width;
    int            height;
    Pixel::Format  textPixelFormat;
    int            offsetPosition;
    Vector2        offSet;

    TilingInfo(int width, int height, Pixel::Format textPixelFormat)
    : textBuffer(NULL),
      styleBuffer(NULL),
      overlayStyleBuffer(NULL),
      maskBuffer(NULL),
      width(width),
      height(height),
      textPixelFormat(textPixelFormat),
      offsetPosition(0),
      offSet(0.f, 0.f)
    {
    }

    ~TilingInfo()
    {
      if(textBuffer)
      {
        free(textBuffer);
      }
      if(styleBuffer)
      {
        free(styleBuffer);
      }
      if(overlayStyleBuffer)
      {
        free(overlayStyleBuffer);
      }
      if(maskBuffer)
      {
        free(maskBuffer);
      }
    }
  };

  /**
   * @brief Set the individual property to the given value.
   *
   * @param[in] index The index key used to reference this value within the initial property map.
   *
   * @param[in] propertyValue The value to set.
   */
  void DoSetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue);

  /**
   * @brief Updates the text's renderer.
   */
  void UpdateRenderer();

  /**
   * @brief Removes the text's renderer.
   */
  void RemoveRenderer(Actor& actor);

  /**
   * @brief Create a texture in textureSet and add it.
   * @param[in] textureSet The textureSet to which the texture will be added.
   * @param[in] data The PixelData to be uploaded to texture
   * @param[in] sampler The sampler.
   * @param[in] textureSetIndex The Index of TextureSet.
   */
  void AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex);

  /**
   * @brief Convert the buffer to pixelData.
   * @param[in] buffer The Buffer to be converted to pixelData.
   * @param[in] width The width of pixel data.
   * @param[in] height The height of pixel data.
   * @param[in] offsetPosition The The buffer's start position.
   * @param[in] textPixelFormat The PixelForma of text.
   */
  PixelData ConvertToPixelData(unsigned char* buffer, int width, int height, int offsetPosition, const Pixel::Format textPixelFormat);

  /**
   * @brief Create the text's texture. It will use cached shader feature for text visual.
   * @param[in] info This is the information you need to create a Tiling.
   * @param[in] renderer The renderer to which the TextureSet will be added.
   * @param[in] sampler The sampler.
   */
  void CreateTextureSet(TilingInfo& info, VisualRenderer& renderer, Sampler& sampler);

  /**
   * Create renderer of the text for rendering.
   * @param[in] actor The actor.
   * @param[in] size The texture size.
   * @param[in] hasMultipleTextColors Whether the text contains multiple colors.
   * @param[in] containsColorGlyph Whether the text contains color glyph.
   * @param[in] styleEnabled Whether the text contains any styles (e.g. shadow, underline, etc.).
   * @param[in] isOverlayStyle Whether the style needs to overlay on the text (e.g. strikethrough, underline, etc.).
   */
  void AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph, bool styleEnabled, bool isOverlayStyle);

  /**
   * Get the texture of the text for rendering. It will use cached shader feature for text visual.
   * @param[in] size The texture size.
   */
  TextureSet GetTextTexture(const Vector2& size);

  /**
   * Get the text rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current text shader's features. It will be cached as text visual.
   */
  Shader GetTextShader(VisualFactoryCache& factoryCache, const TextVisualShaderFeature::FeatureBuilder& featureBuilder);

  /**
   * @brief Retrieve the TextVisual object.
   * @param[in] visual A handle to the TextVisual
   * @return The TextVisual object
   */
  static TextVisual& GetVisualObject(Toolkit::Visual::Base visual)
  {
    return static_cast<TextVisual&>(Toolkit::GetImplementation(visual).GetVisualObject());
  };

private:
  typedef std::vector<Renderer> RendererContainer;

private:
  Text::ControllerPtr mController; ///< The text's controller.
  Text::TypesetterPtr mTypesetter; ///< The text's typesetter.

  TextVisualShaderFactory&                mTextVisualShaderFactory; ///< The shader factory for text visual.
  TextVisualShaderFeature::FeatureBuilder mTextShaderFeatureCache;  ///< The cached shader feature for text visual.

  WeakHandle<Actor> mControl;                          ///< The control where the renderer is added.
  Constraint        mColorConstraint{};                ///< Color constraint
  Constraint        mOpacityConstraint{};              ///< Opacity constraint
  Property::Index   mAnimatableTextColorPropertyIndex; ///< The index of animatable text color property registered by the control.
  Property::Index   mTextColorAnimatableIndex;         ///< The index of uTextColorAnimatable property.
  bool              mRendererUpdateNeeded : 1;         ///< The flag to indicate whether the renderer needs to be updated.
  RendererContainer mRendererList;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H */
