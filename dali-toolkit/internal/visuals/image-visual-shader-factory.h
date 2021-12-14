#ifndef DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H

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
 * ImageVisualShaderFeature contains feature lists what image visual shader need to know.
 */
namespace ImageVisualShaderFeature
{
namespace TextureAtlas
{
/**
 * @brief Whether use texture with atlas, or not
 */
enum Type
{
  DISABLED = 0, ///< Image visual doesn't use ATLAS
  ENABLED       ///< Image visual uses ATLAS
};
} // namespace TextureAtlas

namespace DefaultTextureWrapMode
{
/**
 * @brief Whether apply to texture wraping in default, or not
 */
enum Type
{
  APPLY = 0,   ///< Image visual applies to wraping texture in default
  DO_NOT_APPLY ///< Image visual doesn't apply to wraping texture in default
};
} // namespace DefaultTextureWrapMode

namespace RoundedCorner
{
/**
 * @brief Whether use rounded corner, or not
 */
enum Type
{
  DISABLED = 0, ///< Image visual doesn't use rounded corner
  ENABLED       ///< Image visual uses rounded corner
};
} // namespace RoundedCorner

namespace Borderline
{
/**
 * @brief Whether use borderline, or not
 */
enum Type
{
  DISABLED = 0, ///< Image visual doesn't use borderline
  ENABLED       ///< Image visual uses borderline
};
} // namespace Borderline

namespace ChangeFragmentShader
{
/**
 * @brief Whether native image change the default fragment shader, or not
 */
enum Type
{
  DONT_CHANGE = 0, ///< Native image doesn't change default fragment shader.
  NEED_CHANGE,     ///< Native image changes default fragment shader. We need another shader cache.
  UNDECIDED,       ///< Undecided.
};
} // namespace ChangeFragmentShader

namespace AlphaMasking
{
/**
 * @brief Whether use runtime alpha masking in shader, or not
 */
enum Type
{
  DISABLED = 0, ///< Image visual doesn't use runtime alpha masking
  ENABLED       ///< Image visual uses runtime alpha masking
};
} // namespace AlphaMasking

/**
 * @brief Collection of current image visual feature. Only use for ImageVisualShaderFactory::GetShader()
 */
struct FeatureBuilder
{
  FeatureBuilder()
  : mTextureAtlas(TextureAtlas::DISABLED),
    mDefaultTextureWrapMode(DefaultTextureWrapMode::APPLY),
    mRoundedCorner(RoundedCorner::DISABLED),
    mBorderline(Borderline::DISABLED),
    mAlphaMasking(AlphaMasking::DISABLED),
    mTexture()
  {
  }

  FeatureBuilder& EnableTextureAtlas(bool enableTextureAtlas);
  FeatureBuilder& ApplyDefaultTextureWrapMode(bool applyDefaultTextureWrapMode);
  FeatureBuilder& EnableRoundedCorner(bool enableRoundedCorner);
  FeatureBuilder& EnableBorderline(bool enableBorderline);
  FeatureBuilder& SetTextureForFragmentShaderCheck(const Dali::Texture& texture);
  FeatureBuilder& EnableAlphaMasking(bool enableAlphaMasking);

  TextureAtlas::Type           mTextureAtlas : 2;           ///< Whether use texture with atlas, or not. default as TextureAtlas::DISABLED
  DefaultTextureWrapMode::Type mDefaultTextureWrapMode : 2; ///< Whether apply to texture wraping in default, or not. default as DefaultTextureWrapMode::APPLY
  RoundedCorner::Type          mRoundedCorner : 2;          ///< Whether use rounded corner, or not. default as RoundedCorner::DISABLED
  Borderline::Type             mBorderline : 2;             ///< Whether use borderline, or not. default as Borderline::DISABLED
  AlphaMasking::Type           mAlphaMasking : 2;           ///< Whether use runtime alpha masking, or not. default as AlphaMasking::DISABLED
  Dali::Texture                mTexture;                    ///< Texture to check whether we need to change fragment shader or not
};

} // namespace ImageVisualShaderFeature

/**
 * ImageVisualShaderFactory is an object that provides and shares shaders between image visuals
 */
class ImageVisualShaderFactory
{
public:
  /**
   * @brief Constructor
   */
  ImageVisualShaderFactory();

  /**
   * @brief Destructor
   */
  ~ImageVisualShaderFactory();

  /**
   * @brief Get the standard image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current image shader's features
   * @return The standard image rendering shader with features.
   */
  Shader GetShader(VisualFactoryCache& factoryCache, const ImageVisualShaderFeature::FeatureBuilder& featureBuilder);

  /**
   * @brief Request the default vertex shader source.
   * @return The default vertex shader source.
   */
  std::string_view GetVertexShaderSource();

  /**
   * @brief Request the default fragment shader source.
   * @return The default fragment shader source.
   */
  std::string_view GetFragmentShaderSource();

protected:
  /**
   * Undefined copy constructor.
   */
  ImageVisualShaderFactory(const ImageVisualShaderFactory&);

  /**
   * Undefined assignment operator.
   */
  ImageVisualShaderFactory& operator=(const ImageVisualShaderFactory& rhs);

private:
  /**
   * @brief Cached information whether native image should change fragment shader.
   * Default it is ChangeFragmentShader::UNDECIDED.
   * If we have any chance to check native image source apply fragment shader,
   * this vaule will be changed one of these : ChangeFragmentShader::DONT_CHANGE or ChangeFragmentShader::NEED_CHANGE
   *
   * After result cached, this value will not be changed.
   *
   * If value is DONT_CHANGE, ImageVisualShaderFactory::GetShader never call ApplyNativeFragmentShader.
   * Else, ImageVisualShaderFactory::GetShader will call ApplyNativeFragmentShader if native image source texture come.
   */
  ImageVisualShaderFeature::ChangeFragmentShader::Type mFragmentShaderNeedChange : 3;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H
