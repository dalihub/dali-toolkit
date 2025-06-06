#ifndef DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FEATURE_BUILDER_H
#define DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FEATURE_BUILDER_H

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
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES

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
 * @brief Whether use rounded corner, or squircle corner, or not
 */
enum Type
{
  DISABLED = 0,   ///< Image visual doesn't use rounded corner
  ROUNDED_CORNER, ///< Image visual uses rounded corner
  SQUIRCLE_CORNER ///< Image visual use squircle corner
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

namespace AlphaMaskingOnRendering
{
/**
 * @brief Whether use runtime alpha masking in shader, or not
 */
enum Type
{
  DISABLED = 0, ///< Image visual doesn't use runtime alpha masking
  ENABLED       ///< Image visual uses runtime alpha masking
};
} // namespace AlphaMaskingOnRendering

namespace ColorConversion
{
/**
 * @brief Whether the color format conversion is needed or not
 */
enum Type
{
  DONT_NEED = 0,      ///< Not need to convert
  YUV_TO_RGB,         ///< Need yuv to rgb conversion
  UNIFIED_YUV_AND_RGB ///< Need to support both yuv conversion case and normal case.
};
} // namespace ColorConversion

/**
 * @brief Collection of current image visual feature. Only use for ImageVisualShaderFactory::GetShader()
 */
class FeatureBuilder
{
public:
  FeatureBuilder();

  FeatureBuilder& EnableTextureAtlas(bool enableTextureAtlas);

  FeatureBuilder& ApplyDefaultTextureWrapMode(bool applyDefaultTextureWrapMode);

  FeatureBuilder& EnableRoundedCorner(bool enableRoundedCorner, bool enableSquircleCorner = false);

  FeatureBuilder& EnableBorderline(bool enableBorderline);

  FeatureBuilder& SetTextureForFragmentShaderCheck(const Dali::Texture& texture);

  FeatureBuilder& EnableAlphaMaskingOnRendering(bool enableAlphaMaskingOnRendering);

  FeatureBuilder& EnableYuvToRgb(bool enableYuvToRgb, bool enableUnifiedYuvAndRgb = false);

  VisualFactoryCache::ShaderType GetShaderType() const;

  ChangeFragmentShader::Type NeedToChangeFragmentShader() const;

  void GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const;
  void GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const;

  Dali::Texture GetTexture() const;

  bool IsEnabledAlphaMaskingOnRendering() const;

private:
  TextureAtlas::Type            mTextureAtlas : 2;            ///< Whether use texture with atlas, or not. default as TextureAtlas::DISABLED
  DefaultTextureWrapMode::Type  mDefaultTextureWrapMode : 2;  ///< Whether apply to texture wraping in default, or not. default as DefaultTextureWrapMode::APPLY
  RoundedCorner::Type           mRoundedCorner : 2;           ///< Whether use rounded corner, or not. default as RoundedCorner::DISABLED
  Borderline::Type              mBorderline : 2;              ///< Whether use borderline, or not. default as Borderline::DISABLED
  AlphaMaskingOnRendering::Type mAlphaMaskingOnRendering : 2; ///< Whether use runtime alpha masking, or not. default as AlphaMaskingOnRendering::DISABLED
  ColorConversion::Type         mColorConversion : 2;         ///< Whether the color format conversion is needed or not

  Dali::Texture mTexture; ///< Texture to check whether we need to change fragment shader or not
};

} // namespace ImageVisualShaderFeature

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H
