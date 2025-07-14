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
 *
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/visual-factory/precompile-shader-option.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/integration-api/debug.h>

namespace
{
// TYPE
const char* TOKEN_TYPE("shaderType");
const char* TOKEN_TYPE_IMAGE("image");
const char* TOKEN_TYPE_TEXT("text");
const char* TOKEN_TYPE_COLOR("color");
const char* TOKEN_TYPE_MODEL_3D("3d");
const char* TOKEN_TYPE_NPATCH("npatch");
const char* TOKEN_TYPE_CUSTOM("custom");

// OPTION
const char* TOKEN_OPTION("shaderOption");
const char* TOKEN_OPTION_ROUNDED_CORNER("ROUNDED_CORNER");
const char* TOKEN_OPTION_SQUIRCLE_CORNER("SQUIRCLE_CORNER");
const char* TOKEN_OPTION_BORDERLINE("BORDERLINE");
const char* TOKEN_OPTION_BLUR_EDGE("BLUR_EDGE");
const char* TOKEN_OPTION_CUTOUT("CUTOUT");
const char* TOKEN_OPTION_ATLAS_DEFAULT("ATLAS_DEFAULT");
const char* TOKEN_OPTION_ATLAS_CUSTOM("ATLAS_CUSTOM");
const char* TOKEN_OPTION_MASKING("MASKING");
const char* TOKEN_OPTION_YUV_TO_RGB("YUV_TO_RGB");
const char* TOKEN_OPTION_YUV_AND_RGB("YUV_AND_RGB");
const char* TOKEN_OPTION_MULTI_COLOR("MULTI_COLOR");
const char* TOKEN_OPTION_STYLES("STYLES");
const char* TOKEN_OPTION_OVERLAY("OVERLAY");
const char* TOKEN_OPTION_EMOJI("EMOJI");
const char* TOKEN_OPTION_EMBOSS("EMBOSS");
const char* TOKEN_OPTION_STRETCH_X("xStretchCount");
const char* TOKEN_OPTION_STRETCH_Y("yStretchCount");

// CUSTOM
const char* TOKEN_CUSTOM_VERTEX("vertexShader");
const char* TOKEN_CUSTOM_FRAMENT("fragmentShader");
const char* TOKEN_CUSTOM_NAME("shaderName");

// String to enum table
// clang-format off
DALI_ENUM_TO_STRING_TABLE_BEGIN(SHADER_TYPE)
  {TOKEN_TYPE_IMAGE,    static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::IMAGE)},
  {TOKEN_TYPE_TEXT,     static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::TEXT)},
  {TOKEN_TYPE_COLOR,    static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::COLOR)},
  {TOKEN_TYPE_MODEL_3D, static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::MODEL_3D)},
  {TOKEN_TYPE_NPATCH,   static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::NPATCH)},
  {TOKEN_TYPE_CUSTOM,   static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::ShaderType::CUSTOM)},
DALI_ENUM_TO_STRING_TABLE_END(SHADER_TYPE);

DALI_ENUM_TO_STRING_TABLE_BEGIN(SHADER_OPTION_FLAG)
  {TOKEN_OPTION_ROUNDED_CORNER,  static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::ROUNDED_CORNER)},
  {TOKEN_OPTION_SQUIRCLE_CORNER, static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::SQUIRCLE_CORNER)},
  {TOKEN_OPTION_BORDERLINE,      static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::BORDERLINE)},
  {TOKEN_OPTION_BLUR_EDGE,       static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::BLUR_EDGE)},
  {TOKEN_OPTION_CUTOUT,          static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::CUTOUT)},
  {TOKEN_OPTION_ATLAS_DEFAULT,   static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::ATLAS_DEFAULT)},
  {TOKEN_OPTION_ATLAS_CUSTOM,    static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::ATLAS_CUSTOM)},
  {TOKEN_OPTION_MASKING,         static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::MASKING)},
  {TOKEN_OPTION_YUV_TO_RGB,      static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::YUV_TO_RGB)},
  {TOKEN_OPTION_YUV_AND_RGB,     static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::YUV_AND_RGB)},
  {TOKEN_OPTION_MULTI_COLOR,     static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::MULTI_COLOR)},
  {TOKEN_OPTION_STYLES,          static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::STYLES)},
  {TOKEN_OPTION_OVERLAY,         static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::OVERLAY)},
  {TOKEN_OPTION_EMOJI,           static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::EMOJI)},
  {TOKEN_OPTION_EMBOSS,          static_cast<int32_t>(Dali::Toolkit::PrecompileShaderOption::Flag::EMBOSS)},
DALI_ENUM_TO_STRING_TABLE_END(SHADER_OPTION_FLAG);
// clang-format on
} // namespace

namespace Dali
{
namespace Toolkit
{
PrecompileShaderOption::PrecompileShaderOption(const Property::Map& shaderOption)
: mShaderType(ShaderType::UNKNOWN),
  mShaderOptions(),
  mShaderName(""),
  mVertexShader(""),
  mFragmentShader(""),
  mNpatchXStretchCount(0),
  mNpatchYStretchCount(0)
{
  ConvertShaderMap(shaderOption);
}

void PrecompileShaderOption::ConvertShaderMap(const Property::Map& shaderOption)
{
  for(unsigned int shaderIdx = 0; shaderIdx < shaderOption.Count(); ++shaderIdx)
  {
    const KeyValuePair pair(shaderOption.GetKeyValue(shaderIdx));
    if(pair.first.type == Property::Key::INDEX)
    {
      continue; // We don't consider index keys.
    }

    const std::string&     key(pair.first.stringKey);
    const Property::Value& value(pair.second);

    if(key == TOKEN_TYPE)
    {
      if(!GetEnumerationProperty(value, SHADER_TYPE_TABLE, SHADER_TYPE_TABLE_COUNT, mShaderType) || mShaderType == ShaderType::UNKNOWN)
      {
        DALI_LOG_ERROR("Can't find proper type[%s]\n", value.Get<std::string>().c_str());
        continue;
      }
    }
    else if(key == TOKEN_OPTION)
    {
      Property::Map optionMap = value.Get<Property::Map>();
      for(size_t optionMapIdx = 0; optionMapIdx < optionMap.Count(); ++optionMapIdx)
      {
        const KeyValuePair optionPair(optionMap.GetKeyValue(optionMapIdx));

        if(optionPair.first.type == Property::Key::INDEX)
        {
          continue; // We don't consider index keys.
        }

        Flag               flag = Flag::UNKNOWN;
        const std::string& optionKey(optionPair.first.stringKey);
        if(GetEnumeration(optionKey.c_str(), SHADER_OPTION_FLAG_TABLE, SHADER_OPTION_FLAG_TABLE_COUNT, flag) && flag != Flag::UNKNOWN)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(flag);
          }
        }
        else
        {
          DALI_LOG_ERROR("Can't find this flag[%s]\n", optionKey.c_str());
          continue;
        }
      }
    }
    else if(key == TOKEN_CUSTOM_VERTEX)
    {
      if(value.GetType() == Property::STRING)
      {
        mVertexShader = value.Get<std::string>();
      }
    }
    else if(key == TOKEN_CUSTOM_FRAMENT)
    {
      if(value.GetType() == Property::STRING)
      {
        mFragmentShader = value.Get<std::string>();
      }
    }
    else if(key == TOKEN_CUSTOM_NAME)
    {
      if(value.GetType() == Property::STRING)
      {
        mShaderName = value.Get<std::string>();
      }
    }
    else if(key == TOKEN_OPTION_STRETCH_X)
    {
      if(value.GetType() == Property::INTEGER)
      {
        mNpatchXStretchCount = value.Get<int>();
      }
    }
    else if(key == TOKEN_OPTION_STRETCH_Y)
    {
      if(value.GetType() == Property::INTEGER)
      {
        mNpatchYStretchCount = value.Get<int>();
      }
    }
  }
}

PrecompileShaderOption::ShaderType PrecompileShaderOption::GetShaderType() const
{
  return mShaderType;
}

std::vector<PrecompileShaderOption::Flag> PrecompileShaderOption::GetShaderOptions() const
{
  return mShaderOptions;
}

std::string PrecompileShaderOption::GetShaderName() const
{
  return mShaderName;
}

std::string PrecompileShaderOption::GetVertexShader() const
{
  return mVertexShader;
}

std::string PrecompileShaderOption::GetFragmentShader() const
{
  return mFragmentShader;
}

uint32_t PrecompileShaderOption::GetNpatchXStretchCount() const
{
  return mNpatchXStretchCount;
}

uint32_t PrecompileShaderOption::GetNpatchYStretchCount() const
{
  return mNpatchYStretchCount;
}

} // namespace Toolkit

} // namespace Dali
