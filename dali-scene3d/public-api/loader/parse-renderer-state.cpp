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

#include "dali-scene3d/public-api/loader/parse-renderer-state.h"
#include <cstring>
#include "dali/devel-api/common/map-wrapper.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace RendererState
{
namespace
{
std::map<std::string_view, Type> COMPARISONS{
#define DECL_COMPARISON(x) {#x, Comparison::x}
  DECL_COMPARISON(NEVER),
  DECL_COMPARISON(ALWAYS),
  DECL_COMPARISON(LESS),
  DECL_COMPARISON(GREATER),
  DECL_COMPARISON(EQUAL),
  DECL_COMPARISON(NOT_EQUAL),
  DECL_COMPARISON(LESS_EQUAL),
  DECL_COMPARISON(GREATER_EQUAL),
#undef DECL_COMPARISON
};

Type InterpretComparison(const std::string_view& str)
{
  Type value = 0x0;
  auto iFind = COMPARISONS.find(str);
  if(iFind != COMPARISONS.end())
  {
    value = iFind->second;
  }
  return value;
}

std::map<std::string_view, Type> BLEND_FACTORS{
#define DECL_BLEND_FACTOR(x) {#x, Dali::Scene3D::Loader::BlendFactor::x}
  DECL_BLEND_FACTOR(ZERO),
  DECL_BLEND_FACTOR(ONE),
  DECL_BLEND_FACTOR(SRC_COLOR),
  DECL_BLEND_FACTOR(ONE_MINUS_SRC_COLOR),
  DECL_BLEND_FACTOR(SRC_ALPHA),
  DECL_BLEND_FACTOR(ONE_MINUS_SRC_ALPHA),
  DECL_BLEND_FACTOR(DST_ALPHA),
  DECL_BLEND_FACTOR(ONE_MINUS_DST_ALPHA),
  DECL_BLEND_FACTOR(DST_COLOR),
  DECL_BLEND_FACTOR(ONE_MINUS_DST_COLOR),
  DECL_BLEND_FACTOR(SRC_ALPHA_SATURATE),
  DECL_BLEND_FACTOR(CONSTANT_COLOR),
  DECL_BLEND_FACTOR(ONE_MINUS_CONSTANT_COLOR),
  DECL_BLEND_FACTOR(CONSTANT_ALPHA),
  DECL_BLEND_FACTOR(ONE_MINUS_CONSTANT_ALPHA),
#undef DECL_BLEND_FACTOR
};

Type InterpretBlendFactor(const std::string_view& str, uint8_t item)
{
  Type value = 0x0;
  auto iFind = BLEND_FACTORS.find(str);
  if(iFind != BLEND_FACTORS.end())
  {
    value = iFind->second << (BLEND_FACTOR_BASE_SHIFT + BLEND_FACTOR_ITEM_BITS * item);
  }
  return value;
}

std::map<std::string_view, Type> BUFFER_MODES{
#define DECL_BUFFER_MODE(x) {#x, BufferMode::x}
  DECL_BUFFER_MODE(NONE),
  DECL_BUFFER_MODE(AUTO),
  DECL_BUFFER_MODE(COLOR),
  DECL_BUFFER_MODE(STENCIL),
  DECL_BUFFER_MODE(COLOR_STENCIL),
};

Type InterpretBufferMode(const std::string_view& str)
{
  Type value = 0x0;
  auto iFind = BUFFER_MODES.find(str);
  if(iFind != BUFFER_MODES.end())
  {
    value = iFind->second << BUFFER_MODE_SHIFT;
  }
  return value;
}

std::map<std::string_view, Type (*)(const std::string_view&)> RENDERER_STATE_PROCESSORS{
  {"DEPTH_WRITE", [](const std::string_view&) -> Type { return DEPTH_WRITE; }},
  {"DEPTH_TEST", [](const std::string_view&) -> Type { return DEPTH_TEST; }},
  {"CULL_FRONT", [](const std::string_view&) -> Type { return CULL_FRONT; }},
  {"CULL_BACK", [](const std::string_view&) -> Type { return CULL_BACK; }},
  {"ALPHA_BLEND", [](const std::string_view&) -> Type { return ALPHA_BLEND; }},
  {"DEPTH_FUNC", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? (InterpretComparison(std::string_view(arg.data() + 1, arg.size() - 1)) << DEPTH_FUNCTION_SHIFT) : 0x0;
     return value;
   }},
  {"BLEND_SRC_RGB", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? InterpretBlendFactor(std::string_view(arg.data() + 1, arg.size() - 1), 0) : 0x0;
     return value;
   }},
  {"BLEND_DST_RGB", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? InterpretBlendFactor(std::string_view(arg.data() + 1, arg.size() - 1), 1) : 0x0;
     return value;
   }},
  {"BLEND_SRC_ALPHA", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? InterpretBlendFactor(std::string_view(arg.data() + 1, arg.size() - 1), 2) : 0x0;
     return value;
   }},
  {"BLEND_DST_ALPHA", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? InterpretBlendFactor(std::string_view(arg.data() + 1, arg.size() - 1), 3) : 0x0;
     return value;
   }},
  {"BUFFER_MODE", [](const std::string_view& arg) -> Type {
     Type value = (arg[0] == ':') ? InterpretBufferMode(std::string_view(arg.data() + 1, arg.size() - 1)) : 0x0;
     return value;
   }},
};

} // namespace

Type Parse(const char* string, size_t length, StringCallback onError)
{
  if(length == 0)
  {
    length = strlen(string);
  }

  Type value = 0x0;
  auto iEnd  = string + length;
  while(string != iEnd)
  {
    auto iNextToken = std::find(string, iEnd, '|');
    auto iColon     = std::find(string, iNextToken, ':');
    auto i          = RENDERER_STATE_PROCESSORS.find(std::string_view(string, iColon - string));
    if(i != RENDERER_STATE_PROCESSORS.end() && size_t(std::distance(string, iNextToken)) >= i->first.size())
    {
      value |= i->second(std::string_view(string + i->first.size(), iNextToken - iColon));
    }
    else
    {
      onError("Not a valid RendererState: " + std::string(string, iNextToken));
    }

    string = iNextToken + (iNextToken != iEnd);
  }

  return value;
}

} // namespace RendererState
} // namespace Loader
} // namespace Scene3D
} // namespace Dali
