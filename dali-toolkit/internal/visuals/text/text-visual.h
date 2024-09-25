#ifndef DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/async-text/async-text-interface.h>
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/visuals/text/text-visual-shader-factory.h>
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
class TextVisual : public Visual::Base, public TextLoadObserver
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
   * @brief Set the text to be always rendered
   * @param[in] visual The text visual.
   * @param[in] requireRender Whether to text always rendered.
   */
  static void SetRequireRender(Toolkit::Visual::Base visual, bool requireRender)
  {
    GetVisualObject(visual).SetRequireRender(requireRender);
  };

  /**
   * @brief Instantly updates the renderer
   * @param[in] visual The text visual.
   */
  static void UpdateRenderer(Toolkit::Visual::Base visual)
  {
    GetVisualObject(visual).UpdateRenderer();
  };

  /**
   * @brief Instantly updates the async renderer
   * @param[in] visual The text visual.
   * @param[in] parameters The async text parameters.
   * @return true if the async text render request was successful, false otherwise.
   */
  static bool UpdateAsyncRenderer(Toolkit::Visual::Base visual, Text::AsyncTextParameters& parameters)
  {
    return GetVisualObject(visual).UpdateAsyncRenderer(parameters);
  };

  /**
   * @brief Instantly requests the async size computation.
   * @param[in] visual The text visual.
   * @param[in] parameters The async text parameters.
   */
  static void RequestAsyncSizeComputation(Toolkit::Visual::Base visual, Text::AsyncTextParameters& parameters)
  {
    GetVisualObject(visual).RequestAsyncSizeComputation(parameters);
  };

  /**
   * @brief Set the control's async text interface.
   * @param[in] visual The text visual.
   * @param[in] asyncTextInterface The async text interface.
   */
  static void SetAsyncTextInterface(Toolkit::Visual::Base visual, Text::AsyncTextInterface* asyncTextInterface)
  {
    GetVisualObject(visual).SetAsyncTextInterface(asyncTextInterface);
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
    PixelData textPixelData;
    PixelData stylePixelData;
    PixelData overlayStylePixelData;
    PixelData maskPixelData;
    int32_t   width;
    int32_t   height;
    uint32_t  offsetHeight;
    Vector2   transformOffset;

    TilingInfo(int32_t width, int32_t height)
    : textPixelData(),
      stylePixelData(),
      overlayStylePixelData(),
      maskPixelData(),
      width(width),
      height(height),
      offsetHeight(0u),
      transformOffset(0.f, 0.f)
    {
    }

    ~TilingInfo()
    {
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
   * @brief Updates the text's async renderer.
   * @param[in] parameters The async text parameters.
   * @return true if the async text render request was successful, false otherwise.
   */
  bool UpdateAsyncRenderer(Text::AsyncTextParameters& parameters);

  /**
   * @brief Requests the async size computation.
   * @param[in] parameters The async text parameters.
   */
  void RequestAsyncSizeComputation(Text::AsyncTextParameters& parameters);

  /**
   * @brief Set the control's async text interface.
   * @param[in] asyncTextInterface The async text interface.
   */
  void SetAsyncTextInterface(Text::AsyncTextInterface* asyncTextInterface);

  /**
   * @brief Removes the text's renderer.
   */
  void RemoveRenderer(Actor& actor, bool removeDefaultRenderer);

  /**
   * @brief Create a texture in textureSet and add it.
   * @param[in] textureSet The textureSet to which the texture will be added.
   * @param[in] data The PixelData to be uploaded to texture.
   * @param[in] sampler The sampler.
   * @param[in] textureSetIndex The Index of TextureSet.
   */
  void AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex);

  /**
   * @brief Create a texture in textureSet and add it.
   * @param[in] textureSet The textureSet to which the texture will be added.
   * @param[in] tilingInfo The tiling infomation to be uploaded to texture.
   * @param[in] data The PixelData to be uploaded to texture.
   * @param[in] sampler The sampler.
   * @param[in] textureSetIndex The Index of TextureSet.
   */
  void AddTilingTexture(TextureSet& textureSet, TilingInfo& tilingInfo, PixelData& data, Sampler& sampler, unsigned int textureSetIndex);

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
  Shader GetTextShader(VisualFactoryCache& factoryCache, TextVisualShaderFeature::FeatureBuilder& featureBuilder);

  /**
   * @brief Set the text to be always rendered
   * @param[in] requireRender Whether to text always rendered.
   */
  void SetRequireRender(bool requireRender);

  /**
   * @brief Retrieve the TextVisual object.
   * @param[in] visual A handle to the TextVisual
   * @return The TextVisual object
   */
  static TextVisual& GetVisualObject(Toolkit::Visual::Base visual)
  {
    return static_cast<TextVisual&>(Toolkit::GetImplementation(visual).GetVisualObject());
  };

  /**
   * @copydoc TextLoadObserver::LoadComplete
   *
   * Called when the TextLoadingTask's work is complete.
   *
   * @param[in] success True if the load was successful, false otherwise.
   * @param[in] textInformation The text information including render info and parameters.
   */
  void LoadComplete(bool success, const TextInformation& textInformation) override;

private:
  typedef std::vector<Renderer> RendererContainer;

private:
  Text::ControllerPtr       mController;         ///< The text's controller.
  Text::TypesetterPtr       mTypesetter;         ///< The text's typesetter.
  Text::AsyncTextInterface* mAsyncTextInterface; ///< The text's async interface.

  TextVisualShaderFactory&                mTextVisualShaderFactory; ///< The shader factory for text visual.
  TextVisualShaderFeature::FeatureBuilder mTextShaderFeatureCache;  ///< The cached shader feature for text visual.

  WeakHandle<Actor> mControl;                          ///< The control where the renderer is added.
  Constraint        mColorConstraint{};                ///< Color constraint
  Constraint        mOpacityConstraint{};              ///< Opacity constraint
  Property::Index   mHasMultipleTextColorsIndex;       ///< The index of uHasMultipleTextColors proeprty.
  Property::Index   mAnimatableTextColorPropertyIndex; ///< The index of animatable text color property registered by the control.
  Property::Index   mTextColorAnimatableIndex;         ///< The index of uTextColorAnimatable property.
  Property::Index   mTextRequireRenderPropertyIndex;   ///< The index of requireRender property.
  bool              mRendererUpdateNeeded : 1;         ///< The flag to indicate whether the renderer needs to be updated.
  bool              mTextRequireRender : 1;            ///< The flag to indicate whether the text needs to be rendered.
  RendererContainer mRendererList;

  uint32_t mTextLoadingTaskId;               ///< The currently requested text loading(render) task Id.
  uint32_t mNaturalSizeTaskId;               ///< The currently requested natural size task Id.
  uint32_t mHeightForWidthTaskId;            ///< The currently requested height for width task Id.
  bool     mIsTextLoadingTaskRunning : 1;    ///< Whether the requested text loading task is running or not.
  bool     mIsNaturalSizeTaskRunning : 1;    ///< Whether the requested natural size task is running or not.
  bool     mIsHeightForWidthTaskRunning : 1; ///< Whether the requested height for width task is running or not.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_TEXT_VISUAL_H */
