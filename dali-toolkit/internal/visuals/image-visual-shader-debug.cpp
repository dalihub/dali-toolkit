/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/image-visual-shader-debug.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/style-monitor.h> ///< for load json file.

#include <regex> ///< for redefine shader
#include <string_view>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr auto DALI_DEBUG_IMAGE_VISUAL_SHADER_ENV = "DALI_DEBUG_IMAGE_VISUAL_SHADER";

bool DebugImageVisualShaderEnvironmentEnabled()
{
  static bool enabled       = false;
  static bool enabledSetted = false;
  if(!enabledSetted)
  {
    enabledSetted           = true;
    auto debugEnabledString = Dali::EnvironmentVariable::GetEnvironmentVariable(DALI_DEBUG_IMAGE_VISUAL_SHADER_ENV);
    enabled                 = debugEnabledString ? std::atoi(debugEnabledString) : false;
  }
  return enabled;
}

constexpr auto DALI_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME_ENV = "DALI_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME";
constexpr auto DEFAULT_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME  = "debug-image-visual-shader-script.json";

const std::string& GetScriptFilename()
{
  // Set the full path for the default script file.
  const static std::string styleDirPath{AssetManager::GetDaliStylePath()};
  static std::string       mScriptFileName{};

  if(DALI_UNLIKELY(mScriptFileName.empty()))
  {
    // Use user's own script if exist.
    auto environmentScriptFilename = Dali::EnvironmentVariable::GetEnvironmentVariable(DALI_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME_ENV);

    mScriptFileName = environmentScriptFilename ? std::string(environmentScriptFilename) : styleDirPath + std::string(DEFAULT_DEBUG_IMAGE_VISUAL_SHADER_SCRIPT_FILE_NAME);

    DALI_ASSERT_DEBUG(0 != mScriptFileName.length());
  }

  return mScriptFileName;
}

bool LoadJsonScript(std::string& stringOut)
{
  auto styleMonitor = StyleMonitor::Get();

  // as toolkit is platform agnostic, it cannot load files from filesystem
  // ask style monitor to load the style sheet
  if(styleMonitor)
  {
    try
    {
      return styleMonitor.LoadThemeFile(GetScriptFilename(), stringOut);
    }
    catch(const std::exception& e)
    {
      DALI_LOG_ERROR("Something system exception throwed during load script file![%s]\n", GetScriptFilename().c_str());
      DALI_LOG_ERROR("Error message : [%s]\n", e.what());
    }
    catch(const Dali::DaliException& e)
    {
      DALI_LOG_ERROR("Something Dali exception throwed during load script file![%s]\n", GetScriptFilename().c_str());
      DALI_LOG_ERROR("Error message : [%s]\n", e.condition);
    }
    catch(...)
    {
      DALI_LOG_ERROR("Something unkown exception throwed during load script file![%s]\n", GetScriptFilename().c_str());
    }
  }

  return false;
}
// Json keywords what we will get information from json.
constexpr std::string_view MINIMUM_DEBUG_COLOR_RATE_JSON_KEY = "minimumColorRate";
constexpr std::string_view MAXIMUM_DEBUG_COLOR_RATE_JSON_KEY = "maximumColorRate";
constexpr std::string_view DEBUG_RED_CHANNEL_CODE_JSON_KEY   = "redChannelCodes";
constexpr std::string_view DEBUG_GREEN_CHANNEL_CODE_JSON_KEY = "greenChannelCodes";
constexpr std::string_view DEBUG_BLUE_CHANNEL_CODE_JSON_KEY  = "blueChannelCodes";
constexpr std::string_view DEBUG_TRIGGER_CODE_JSON_KEY       = "triggerCode";
constexpr std::string_view DEBUG_RATIO_CODE_JSON_KEY         = "ratioCode";

// Macro keywords what we will replace at fragment shader.
constexpr std::string_view MINIMUM_DEBUG_COLOR_RATE_MACRO_KEY = "MINIMUM_DEBUG_COLOR_RATE";
constexpr std::string_view MAXIMUM_DEBUG_COLOR_RATE_MACRO_KEY = "MAXIMUM_DEBUG_COLOR_RATE";
constexpr std::string_view DEBUG_TRIGGER_RED_CODE_MACRO_KEY   = "DEBUG_TRIGGER_RED_CODE";
constexpr std::string_view DEBUG_TRIGGER_GREEN_CODE_MACRO_KEY = "DEBUG_TRIGGER_GREEN_CODE";
constexpr std::string_view DEBUG_TRIGGER_BLUE_CODE_MACRO_KEY  = "DEBUG_TRIGGER_BLUE_CODE";
constexpr std::string_view DEBUG_RATIO_RED_CODE_MACRO_KEY     = "DEBUG_RATIO_RED_CODE";
constexpr std::string_view DEBUG_RATIO_GREEN_CODE_MACRO_KEY   = "DEBUG_RATIO_GREEN_CODE";
constexpr std::string_view DEBUG_RATIO_BLUE_CODE_MACRO_KEY    = "DEBUG_RATIO_BLUE_CODE";

// Default macro keywords when we fail to parse script.
constexpr std::string_view DEFAULT_DEBUG_COLOR_RATE_MACRO_VALUE   = "0.0";
constexpr std::string_view DEFAULT_DEBUG_TRIGGER_CODE_MACRO_VALUE = "return false;";
constexpr std::string_view DEFAULT_DEBUG_RATIO_CODE_MACRO_VALUE   = "return 0.0;";

bool ParseScriptInfomation(Property::Map& result)
{
  std::string stringOut;
  if(!LoadJsonScript(stringOut))
  {
    DALI_LOG_ERROR("Fail to load script file [%s]\n", GetScriptFilename().c_str());
    return false;
  }

  Toolkit::JsonParser parser = Toolkit::JsonParser::New();

  if(!parser.Parse(stringOut))
  {
    std::ostringstream stream;
    if(parser.ParseError())
    {
      stream << "position: " << parser.GetErrorPosition() << ", line: " << parser.GetErrorLineNumber() << ", column: " << parser.GetErrorColumn() << ", description: " << parser.GetErrorDescription() << ".";
    }
    DALI_LOG_ERROR("Fail to parse json script\nError : %s\nJson : %s\n", stream.str().c_str(), stringOut.c_str());
    return false;
  }

  const auto* rootNode = parser.GetRoot();
  if(!rootNode)
  {
    DALI_LOG_ERROR("Fail to get root node\n");
    return false;
  }

  auto InsertScriptMap = [](Property::Map& result, const TreeNode* node, const std::string_view& jsonKey, const std::string_view& macroKey, const std::string_view& defaultValue) {
    std::ostringstream oss;
    oss.clear();

    if(node)
    {
      const auto* childNode = node->GetChild(jsonKey);

      if(childNode)
      {
        if(childNode->GetType() == TreeNode::FLOAT)
        {
          oss << childNode->GetFloat();
        }
        else if(childNode->GetType() == TreeNode::STRING)
        {
          oss << childNode->GetString();
        }
        else if(childNode->GetType() == TreeNode::ARRAY)
        {
          // Concat strings with line feed
          bool isFirst = true;
          for(auto iter = childNode->CBegin(), endIter = childNode->CEnd(); iter != endIter; ++iter)
          {
            if((*iter).second.GetType() == TreeNode::STRING)
            {
              if(isFirst)
              {
                isFirst = false;
              }
              else
              {
                oss << "\n";
              }
              oss << (*iter).second.GetString();
            }
          }
        }
      }
    }

    if(oss.str().empty())
    {
      oss << defaultValue;
    }
    result.Insert(std::string(macroKey), oss.str());
  };

  auto InsertChannelScriptMap = [&InsertScriptMap](Property::Map& result, const TreeNode* node, const std::string_view& channelJsonKey, const std::string_view& triggerMacroKey, const std::string_view& ratioMacroKey) {
    const auto* channelNode = node->GetChild(channelJsonKey);
    InsertScriptMap(result, channelNode, DEBUG_TRIGGER_CODE_JSON_KEY, triggerMacroKey, DEFAULT_DEBUG_TRIGGER_CODE_MACRO_VALUE);
    InsertScriptMap(result, channelNode, DEBUG_RATIO_CODE_JSON_KEY, ratioMacroKey, DEFAULT_DEBUG_RATIO_CODE_MACRO_VALUE);
  };

  // Get color rate
  InsertScriptMap(result, rootNode, MINIMUM_DEBUG_COLOR_RATE_JSON_KEY, MINIMUM_DEBUG_COLOR_RATE_MACRO_KEY, DEFAULT_DEBUG_COLOR_RATE_MACRO_VALUE);
  InsertScriptMap(result, rootNode, MAXIMUM_DEBUG_COLOR_RATE_JSON_KEY, MAXIMUM_DEBUG_COLOR_RATE_MACRO_KEY, DEFAULT_DEBUG_COLOR_RATE_MACRO_VALUE);

  // Get each color ChannelCodes
  InsertChannelScriptMap(result, rootNode, DEBUG_RED_CHANNEL_CODE_JSON_KEY, DEBUG_TRIGGER_RED_CODE_MACRO_KEY, DEBUG_RATIO_RED_CODE_MACRO_KEY);
  InsertChannelScriptMap(result, rootNode, DEBUG_GREEN_CHANNEL_CODE_JSON_KEY, DEBUG_TRIGGER_GREEN_CODE_MACRO_KEY, DEBUG_RATIO_GREEN_CODE_MACRO_KEY);
  InsertChannelScriptMap(result, rootNode, DEBUG_BLUE_CHANNEL_CODE_JSON_KEY, DEBUG_TRIGGER_BLUE_CODE_MACRO_KEY, DEBUG_RATIO_BLUE_CODE_MACRO_KEY);

  return true;
}

const Property::Map& GetScriptInfomation()
{
  static Property::Map result;

  if(DALI_UNLIKELY(result.Empty()))
  {
    if(!ParseScriptInfomation(result))
    {
      // Use default script information if parse failed.
      result.Clear();

      result.Insert(std::string(MINIMUM_DEBUG_COLOR_RATE_MACRO_KEY), std::string(DEFAULT_DEBUG_COLOR_RATE_MACRO_VALUE));
      result.Insert(std::string(MAXIMUM_DEBUG_COLOR_RATE_MACRO_KEY), std::string(DEFAULT_DEBUG_COLOR_RATE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_TRIGGER_RED_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_TRIGGER_CODE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_TRIGGER_GREEN_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_TRIGGER_CODE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_TRIGGER_BLUE_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_TRIGGER_CODE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_RATIO_RED_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_RATIO_CODE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_RATIO_GREEN_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_RATIO_CODE_MACRO_VALUE));
      result.Insert(std::string(DEBUG_RATIO_BLUE_CODE_MACRO_KEY), std::string(DEFAULT_DEBUG_RATIO_CODE_MACRO_VALUE));
    }
  }

  return result;
}

void RedefineMacro(std::string& shaderCode, std::string macro, std::string value)
{
  std::string definition = "#define " + macro;
  std::size_t found      = shaderCode.find(definition);
  DALI_ASSERT_ALWAYS(found != std::string::npos && "Macro keyword was not exist in shader code!");

  std::size_t insertionPoint = found + definition.length();

  // Automatically insert line-continuation character into value
  std::regex                 re("\n");
  std::sregex_token_iterator first{value.begin(), value.end(), re, -1}, last;
  for(auto i = first; i != last; ++i)
  {
    std::string line = std::string(" \\\n") + (*i).str();
    shaderCode.insert(insertionPoint, line);
    insertionPoint += line.length();
  }
}

} // namespace

namespace ImageVisualShaderDebug
{
bool DebugImageVisualShaderEnabled()
{
  return DebugImageVisualShaderEnvironmentEnabled();
}

void ApplyImageVisualShaderDebugScriptCode(std::string& fragmentShader)
{
  const auto& resultMap = GetScriptInfomation();

  for(std::size_t i = 0u; i < resultMap.Count(); ++i)
  {
    auto key   = resultMap.GetKeyAt(i);
    auto value = resultMap.GetValue(i);

    RedefineMacro(fragmentShader, std::move(key.stringKey), std::move(value.Get<std::string>()));
  }
}
} // namespace ImageVisualShaderDebug

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
