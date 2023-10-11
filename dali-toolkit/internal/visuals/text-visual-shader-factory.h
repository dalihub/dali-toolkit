#ifndef DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H

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
 */

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * TextVisualShaderFeature contains feature lists what text visual shader need to know.
 */
namespace TextVisualShaderFeature
{
namespace TextMultiColor
{
/**
 * @brief Whether text contains single color or not.
 */
enum Type
{
  SINGLE_COLOR_TEXT = 0, ///< The text contains single color only.
  MULTI_COLOR_TEXT       ///< The text contains multiple colorｓ.
};
} // namespace TextMultiColor

namespace TextEmoji
{
/**
 * @brief Whether text contains emoji or not.
 */
enum Type
{
  NO_EMOJI = 0, ///< The text contains no emoji.
  HAS_EMOJI     ///< The text contains emoji.
};
} // namespace TextEmoji

namespace TextStyle
{
/**
 * @brief Whether text contains styles (like shadow or background color) or not.
 */
enum Type
{
  NO_STYLES = 0, ///< The text contains contains no styles.
  HAS_STYLES     ///< The text contains contains styles.
};
} // namespace TextStyle

namespace TextOverlay
{
/**
 * @brief Whether text contains overlay styles (like markdown) or not.
 */
enum Type
{
  NO_OVERLAY = 0, ///< The text contains contains no overlay.
  HAS_OVERLAY     ///< The text contains contains overlay.
};
} // namespace TextOverlay

/**
 * @brief Collection of current text visual feature.
 */
struct FeatureBuilder
{
  FeatureBuilder()
  : mTextMultiColor(TextMultiColor::SINGLE_COLOR_TEXT),
    mTextEmoji(TextEmoji::NO_EMOJI),
    mTextStyle(TextStyle::NO_STYLES),
    mTextOverlay(TextOverlay::NO_OVERLAY)
  {
  }

  FeatureBuilder& EnableMultiColor(bool enableMultiColor);
  FeatureBuilder& EnableEmoji(bool enableEmoji);
  FeatureBuilder& EnableStyle(bool enableStyle);
  FeatureBuilder& EnableOverlay(bool enableOverlay);

  bool IsEnabledMultiColor() const
  {
    return mTextMultiColor == TextMultiColor::MULTI_COLOR_TEXT;
  }
  bool IsEnabledEmoji() const
  {
    return mTextEmoji == TextEmoji::HAS_EMOJI;
  }
  bool IsEnabledStyle() const
  {
    return mTextStyle == TextStyle::HAS_STYLES;
  }
  bool IsEnabledOverlay() const
  {
    return mTextOverlay == TextOverlay::HAS_OVERLAY;
  }

  TextMultiColor::Type mTextMultiColor : 2; ///< Whether text has multiple color, or not. default as TextMultiColor::SINGLE_COLOR_TEXT
  TextEmoji::Type      mTextEmoji : 2;      ///< Whether text has emoji, or not. default as TextEmoji::NO_EMOJI
  TextStyle::Type      mTextStyle : 2;      ///< Whether text has style, or not. default as TextStyle::NO_STYLES
  TextOverlay::Type    mTextOverlay : 2;    ///< Whether text has overlay style, or not. default as TextOverlay::NO_OVERLAY
};

} // namespace TextVisualShaderFeature

/**
 * TextVisualShaderFactory is an object that provides and shares shaders for text visuals
 */
class TextVisualShaderFactory
{
public:
  /**
   * @brief Constructor
   */
  TextVisualShaderFactory();

  /**
   * @brief Destructor
   */
  ~TextVisualShaderFactory();

  /**
   * @brief Get the standard text rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current text shader's features
   * @return The standard text rendering shader with features.
   */
  Shader GetShader(VisualFactoryCache& factoryCache, const TextVisualShaderFeature::FeatureBuilder& featureBuilder);

  /**
   * @brief Get the default shader source.
   * @param[in] shaders shaderList for precompile
   */
  void GetPrecompiledShader(RawShaderData& shaders);

protected:
  /**
   * Undefined copy constructor.
   */
  TextVisualShaderFactory(const TextVisualShaderFactory&);

  /**
   * Undefined assignment operator.
   */
  TextVisualShaderFactory& operator=(const TextVisualShaderFactory& rhs);

private:
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H
