#ifndef DALI_TOOLKIT_PRECOMPILE_SHADER_OPTION_H_
#define DALI_TOOLKIT_PRECOMPILE_SHADER_OPTION_H_

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
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <memory>
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{

/**
 * @brief PrecompiledShaderOption is a class for precompiled shader option.
 *
 * Below is the available shader list for precompile
 * -----------------------------------------------------------------
 * COLOR_SHADER,
 * COLOR_SHADER_ROUNDED_CORNER,
 * COLOR_SHADER_BORDERLINE,
 * COLOR_SHADER_ROUNDED_BORDERLINE,
 * COLOR_SHADER_BLUR_EDGE,
 * COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
 * COLOR_SHADER_CUTOUT,
 * COLOR_SHADER_CUTOUT_ROUNDED_CORNER,
 * COLOR_SHADER_CUTOUT_BORDERLINE,
 * COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE,
 * COLOR_SHADER_CUTOUT_BLUR_EDGE,
 * COLOR_SHADER_CUTOUT_ROUNDED_CORNER_BLUR_EDGE,
 * IMAGE_SHADER,
 * IMAGE_SHADER_ROUNDED_CORNER,
 * IMAGE_SHADER_BORDERLINE,
 * IMAGE_SHADER_ROUNDED_BORDERLINE,
 * IMAGE_SHADER_MASKING,
 * IMAGE_SHADER_ROUNDED_CORNER_MASKING,
 * IMAGE_SHADER_BORDERLINE_MASKING,
 * IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
 * IMAGE_SHADER_ATLAS_DEFAULT_WRAP,
 * IMAGE_SHADER_ATLAS_CUSTOM_WRAP,
 * IMAGE_SHADER_YUV_TO_RGB,
 * IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
 * IMAGE_SHADER_BORDERLINE_YUV_TO_RGB,
 * IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_TO_RGB,
 * IMAGE_SHADER_YUV_AND_RGB,
 * IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
 * IMAGE_SHADER_BORDERLINE_YUV_AND_RGB,
 * IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_AND_RGB,
 * NATIVE_IMAGE_SHADER,
 * NATIVE_IMAGE_SHADER_ROUNDED_CORNER,
 * NATIVE_IMAGE_SHADER_BORDERLINE,
 * NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE,
 * NATIVE_IMAGE_SHADER_MASKING,
 * NATIVE_IMAGE_SHADER_ROUNDED_CORNER_MASKING,
 * NATIVE_IMAGE_SHADER_BORDERLINE_MASKING,
 * NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
 * NATIVE_IMAGE_SHADER_ATLAS_DEFAULT_WRAP,
 * NATIVE_IMAGE_SHADER_ATLAS_CUSTOM_WRAP,
 * NINE_PATCH_SHADER,
 * NINE_PATCH_MASK_SHADER,
 * TEXT_SHADER_SINGLE_COLOR_TEXT,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY_AND_EMOJI,
 * TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY_AND_EMOJI,
 * TEXT_SHADER_MULTI_COLOR_TEXT,
 * TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE,
 * TEXT_SHADER_MULTI_COLOR_TEXT_WITH_OVERLAY,
 * TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
 * -----------------------------------------------------------------
 *
 * Please refer to precompile-shader-option.cpp if you want to check string key for each shader flag
 */
class DALI_TOOLKIT_API PrecompileShaderOption
{
public:
  enum class ShaderType
  {
    UNKNOWN = 0,
    COLOR,
    IMAGE,
    TEXT,
    NPATCH,
    MODEL_3D, // ToDO: Need to add more options
    CUSTOM,
  };

  enum class Flag
  {
    ROUNDED_CORNER = 0,
    BORDERLINE,
    BLUR_EDGE,
    CUTOUT,
    ATLAS_DEFAULT,
    ATLAS_CUSTOM,
    MASKING,
    YUV_TO_RGB,
    YUV_AND_RGB,
    NINE_PATCH,
    MULTI_COLOR,
    STYLES,
    OVERLAY,
    EMOJI,
    NATIVE,
    STRETCH_X,
    STRETCH_Y,
  };

  PrecompileShaderOption(const Property::Map& shaderOption);
  PrecompileShaderOption(const PrecompileShaderOption& rhs);
  PrecompileShaderOption& operator=(const PrecompileShaderOption& rhs);

  using ShaderOptions= std::vector<Flag>;

public:
  /**
   * @brief Convert the shader option
   * @param[in] shaderOption The shader option property map
   */
  void ConvertShaderMap(const Property::Map& shaderOption);

  /**
   * @brief Get the shader type
   *
   * @return The shader type
   */
  ShaderType GetShaderType() const;

  /**
   * @brief Get the shader optinons
   *
   * @return The shader options
   */
  ShaderOptions GetShaderOptions() const;

  /**
   * @brief Get the name of shader
   * @return The shader name
   */
  std::string GetShaderName() const;

  /**
   * @brief Get the vertex shader
   *
   * @return The vertex shader
   */
  std::string GetVertexShader() const;

  /**
   * @brief Get the fragment shader
   *
   * @return The fragment shader
   */
  std::string GetFragmentShader() const;

  /**
   * @brief Get the XStretchCount for npatch
   *
   * @return The NpatchXStretchCount
   */
  uint32_t GetNpatchXStretchCount() const;

    /**
   * @brief Get the YStretchCount for npatch
   *
   * @return The NpatchYStretchCount
   */
  uint32_t GetNpatchYStretchCount() const;

private:
  ShaderType mShaderType;
  std::vector<Flag> mShaderOptions;
  std::string mShaderName;
  std::string mVertexShader;
  std::string mFragmentShader;
  uint32_t mNpatchXStretchCount;
  uint32_t mNpatchYStretchCount;
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_PRECOMPILE_SHADER_OPTION_H_
