#ifndef DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-shader-factory-interface.h>
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
  NO_STYLES = 0, ///< The text contains no styles.
  HAS_STYLES     ///< The text contains styles.
};
} // namespace TextStyle

namespace TextOverlay
{
/**
 * @brief Whether text contains overlay styles (like markdown) or not.
 */
enum Type
{
  NO_OVERLAY = 0, ///< The text contains no overlay.
  HAS_OVERLAY     ///< The text contains overlay.
};
} // namespace TextOverlay

namespace TextEmboss
{
/**
 * @brief Whether text contains emboss or not.
 */
enum Type
{
  NO_EMBOSS = 0, ///< The text contains no emboss.
  HAS_EMBOSS     ///< The text contains emboss.
};
} //namespace TextEmboss

/**
 * @brief Collection of current text visual feature.
 */
class FeatureBuilder
{
public:
  FeatureBuilder();
  FeatureBuilder& EnableMultiColor(bool enableMultiColor);
  FeatureBuilder& EnableEmoji(bool enableEmoji);
  FeatureBuilder& EnableStyle(bool enableStyle);
  FeatureBuilder& EnableOverlay(bool enableOverlay);
  FeatureBuilder& EnableEmboss(bool enableEmboss);

  VisualFactoryCache::ShaderType GetShaderType() const;
  void                           GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const;
  void                           GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const;

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
  bool isEnabledEmboss() const
  {
    return mTextEmboss == TextEmboss::HAS_EMBOSS;
  }

private:
  TextMultiColor::Type mTextMultiColor : 2; ///< Whether text has multiple color, or not. default as TextMultiColor::SINGLE_COLOR_TEXT
  TextEmoji::Type      mTextEmoji : 2;      ///< Whether text has emoji, or not. default as TextEmoji::NO_EMOJI
  TextStyle::Type      mTextStyle : 2;      ///< Whether text has style, or not. default as TextStyle::NO_STYLES
  TextOverlay::Type    mTextOverlay : 2;    ///< Whether text has overlay style, or not. default as TextOverlay::NO_OVERLAY
  TextEmboss::Type     mTextEmboss : 2;     ///< Whether text has emboss style, or not. default as TextEmboss::NO_EMBOSS
};

} // namespace TextVisualShaderFeature

/**
 * TextVisualShaderFactory is an object that provides and shares shaders for text visuals
 */
class TextVisualShaderFactory : public VisualShaderFactoryInterface
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

public: // Implementation of VisualShaderFactoryInterface
  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::AddPrecompiledShader
   */
  bool AddPrecompiledShader(PrecompileShaderOption& option) override;

  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::GetPreCompiledShader
   */
  void GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders) override;

private:
  /**
   * @brief Create pre-compiled shader for image with builder and option.
   */
  void CreatePrecompileShader(TextVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option);

  /**
   * @brief Check if cached hash value is valid or not.
   */
  bool SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix, std::string&& fragmentPrefix);

protected:
  /**
   * Undefined copy constructor.
   */
  TextVisualShaderFactory(const TextVisualShaderFactory&);

  /**
   * Undefined assignment operator.
   */
  TextVisualShaderFactory& operator=(const TextVisualShaderFactory& rhs);
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VISUAL_SHADER_FACTORY_H
