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

// CLASS HEADER
#include <dali-toolkit/devel-api/visual-factory/precompile-shader-option.h>

// EXTERNAL INCLUDES
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
  const char* TOKEN_OPTION_STRETCH_X("xStretchCount");
  const char* TOKEN_OPTION_STRETCH_Y("yStretchCount");


  // CUSTOM
  const char* TOKEN_CUSTOM_VERTEX("vertexShader");
  const char* TOKEN_CUSTOM_FRAMENT("fragmentShader");
  const char* TOKEN_CUSTOM_NAME("shaderName");
}

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
      if(value.GetType() == Property::STRING)
      {
        auto shaderType = value.Get<std::string>();
        if(shaderType == TOKEN_TYPE_IMAGE)
        {
          mShaderType = ShaderType::IMAGE;
        }
        else if(shaderType == TOKEN_TYPE_TEXT)
        {
          mShaderType = ShaderType::TEXT;
        }
        else if(shaderType == TOKEN_TYPE_COLOR)
        {
          mShaderType = ShaderType::COLOR;
        }
        else if(shaderType == TOKEN_TYPE_MODEL_3D)
        {
          mShaderType = ShaderType::MODEL_3D;
        }
        else if(shaderType == TOKEN_TYPE_NPATCH)
        {
          mShaderType = ShaderType::NPATCH;
        }
        else if(shaderType == TOKEN_TYPE_CUSTOM)
        {
          mShaderType = ShaderType::CUSTOM;
        }
        else
        {
          mShaderType = ShaderType::UNKNOWN;
        }
      }

      if(mShaderType == ShaderType::UNKNOWN)
      {
        DALI_LOG_ERROR("Can't find proper type.");
        break;
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

        const std::string&     optionKey(optionPair.first.stringKey);

        if(optionKey == TOKEN_OPTION_ROUNDED_CORNER)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::ROUNDED_CORNER);
          }
        }
        else if(optionKey == TOKEN_OPTION_BORDERLINE)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::BORDERLINE);
          }
        }
        else if(optionKey == TOKEN_OPTION_CUTOUT)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::CUTOUT);
          }
        }
        else if(optionKey == TOKEN_OPTION_ATLAS_DEFAULT)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::ATLAS_DEFAULT);
          }
        }
        else if(optionKey == TOKEN_OPTION_ATLAS_CUSTOM)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::ATLAS_CUSTOM);
          }
        }
        else if(optionKey == TOKEN_OPTION_BLUR_EDGE)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::BLUR_EDGE);
          }
        }
        else if(optionKey == TOKEN_OPTION_MASKING)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::MASKING);
          }
        }
        else if(optionKey == TOKEN_OPTION_YUV_TO_RGB)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::YUV_TO_RGB);
          }
        }
        else if(optionKey == TOKEN_OPTION_YUV_AND_RGB)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::YUV_AND_RGB);
          }
        }
        else if(optionKey == TOKEN_OPTION_MULTI_COLOR)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::MULTI_COLOR);
          }
        }
        else if(optionKey == TOKEN_OPTION_STYLES)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::STYLES);
          }
        }
        else if(optionKey == TOKEN_OPTION_OVERLAY)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::OVERLAY);
          }
        }
        else if(optionKey == TOKEN_OPTION_EMOJI)
        {
          if(optionPair.second.Get<bool>())
          {
            mShaderOptions.push_back(Flag::EMOJI);
          }
        }
        else
        {
          DALI_LOG_WARNING("Can't find this flag[%s] \n",optionKey.c_str());
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

uint32_t PrecompileShaderOption::GetNpatchYStretchCount()  const
{
  return mNpatchYStretchCount;
}

} // namespace Toolkit

} // namespace Dali
