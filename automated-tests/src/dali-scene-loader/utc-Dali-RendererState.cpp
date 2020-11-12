/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene-loader/public-api/renderer-state.h"
#include "dali-scene-loader/public-api/parse-renderer-state.h"
#include "dali-scene-loader/public-api/utils.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::SceneLoader;

namespace sl = SceneLoader;
namespace rs = RendererState;

int UtcDaliRendererStateFromBlendFactors(void)
{
  rs::Type state = rs::FromBlendFactors(sl::BlendFactor::ZERO, sl::BlendFactor::ONE,
    sl::BlendFactor::SRC_COLOR, sl::BlendFactor::ONE_MINUS_SRC_COLOR);

  DALI_TEST_EQUAL((state >> rs::BLEND_FACTOR_BASE_SHIFT) & rs::BLEND_FACTOR_ITEM_MASK, rs::Type(sl::BlendFactor::ZERO));
  DALI_TEST_EQUAL((state >> (rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS)) &
    rs::BLEND_FACTOR_ITEM_MASK, rs::Type(sl::BlendFactor::ONE));
  DALI_TEST_EQUAL((state >> (rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS * 2)) &
    rs::BLEND_FACTOR_ITEM_MASK, rs::Type(sl::BlendFactor::SRC_COLOR));
  DALI_TEST_EQUAL((state >> (rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS * 3)) &
    rs::BLEND_FACTOR_ITEM_MASK, rs::Type(sl::BlendFactor::ONE_MINUS_SRC_COLOR));

  END_TEST;
}

#define HELP_TEST_RENDERER_STATE(property, resetValue, state, checkValue, renderer)\
  renderer.SetProperty(property, ~(resetValue));\
  rs::Apply((state), (renderer));\
  printf("%s %s vs %s\n", #property, #state, #checkValue);\
  DALI_TEST_EQUAL(renderer.GetProperty(property).Get<decltype(checkValue)>(), (checkValue));

int UtcDaliRendererStateApply(void)
{
  TestApplication app;
  auto vsh = "void main() { gl_Position = vec4(0.); }";
  auto fsh = "void main() { gl_FragColor = vec4(1.); }";
  Geometry geom = Geometry::New();
  Shader shader = Shader::New(vsh, fsh);
  Renderer renderer = Renderer::New(geom, shader);

  HELP_TEST_RENDERER_STATE(Renderer::Property::DEPTH_WRITE_MODE, DepthWriteMode::OFF, rs::DEPTH_WRITE, DepthWriteMode::ON, renderer);
  HELP_TEST_RENDERER_STATE(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::OFF, rs::DEPTH_TEST, DepthTestMode::ON, renderer);

  HELP_TEST_RENDERER_STATE(Renderer::Property::BLEND_MODE, BlendMode::OFF, rs::ALPHA_BLEND, BlendMode::ON, renderer);

  HELP_TEST_RENDERER_STATE(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE, rs::CULL_FRONT, FaceCullingMode::FRONT, renderer);
  HELP_TEST_RENDERER_STATE(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE, rs::CULL_BACK, FaceCullingMode::BACK, renderer);
  HELP_TEST_RENDERER_STATE(Renderer::Property::FACE_CULLING_MODE, FaceCullingMode::NONE, rs::CULL_FRONT | rs::CULL_BACK,
    FaceCullingMode::FRONT_AND_BACK, renderer);

#define DEPTH_FUNC_PAIR(x) { Comparison::x << rs::DEPTH_FUNCTION_SHIFT, DepthFunction::x }
  const std::pair<rs::Type, DepthFunction::Type> depthFunctionPairs[] {
    DEPTH_FUNC_PAIR(NEVER),
    DEPTH_FUNC_PAIR(ALWAYS),
    DEPTH_FUNC_PAIR(LESS),
    DEPTH_FUNC_PAIR(GREATER),
    DEPTH_FUNC_PAIR(EQUAL),
    DEPTH_FUNC_PAIR(NOT_EQUAL),
    DEPTH_FUNC_PAIR(LESS_EQUAL),
    DEPTH_FUNC_PAIR(GREATER_EQUAL),
  };
#undef DEPTH_FUNC_PAIR
  for (auto& p : depthFunctionPairs)
  {
    HELP_TEST_RENDERER_STATE(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS, p.first, p.second, renderer);
  }

#define BLEND_FACTOR_PAIR(x) { sl::BlendFactor::x, BlendFactor::x }
  const std::pair<rs::Type, BlendFactor::Type> BLEND_FACTORS[] {
    BLEND_FACTOR_PAIR(ZERO),
    BLEND_FACTOR_PAIR(ONE),
    BLEND_FACTOR_PAIR(SRC_COLOR),
    BLEND_FACTOR_PAIR(ONE_MINUS_SRC_COLOR),
    BLEND_FACTOR_PAIR(SRC_ALPHA),
    BLEND_FACTOR_PAIR(ONE_MINUS_SRC_ALPHA),
    BLEND_FACTOR_PAIR(DST_COLOR),
    BLEND_FACTOR_PAIR(ONE_MINUS_DST_COLOR),
    BLEND_FACTOR_PAIR(DST_ALPHA),
    BLEND_FACTOR_PAIR(ONE_MINUS_DST_ALPHA),
    BLEND_FACTOR_PAIR(SRC_ALPHA_SATURATE),
    BLEND_FACTOR_PAIR(CONSTANT_COLOR),
    BLEND_FACTOR_PAIR(ONE_MINUS_CONSTANT_COLOR),
    BLEND_FACTOR_PAIR(CONSTANT_ALPHA),
    BLEND_FACTOR_PAIR(ONE_MINUS_CONSTANT_ALPHA),
  };
#undef BLEND_FACTOR_PAIR
  for (auto property: {
    Renderer::Property::BLEND_FACTOR_SRC_RGB,
    Renderer::Property::BLEND_FACTOR_DEST_RGB,
    Renderer::Property::BLEND_FACTOR_SRC_ALPHA,
    Renderer::Property::BLEND_FACTOR_DEST_ALPHA,
  })
  {
    const auto itemShift = rs::BLEND_FACTOR_ITEM_BITS * (property - Renderer::Property::BLEND_FACTOR_SRC_RGB);
    const auto shift = rs::BLEND_FACTOR_BASE_SHIFT + itemShift;
    for (auto& value: BLEND_FACTORS)
    {
      HELP_TEST_RENDERER_STATE(property, BlendFactor::ZERO, value.first << shift, value.second, renderer);
    }
  }

#define RENDER_MODE_PAIR(x) { BufferMode::x << rs::BUFFER_MODE_SHIFT, RenderMode::x }
  std::pair<rs::Type, RenderMode::Type> renderModePairs[] {
    // same as our reset value: RENDER_MODE_PAIR(NONE),
    RENDER_MODE_PAIR(AUTO),
    RENDER_MODE_PAIR(COLOR),
    RENDER_MODE_PAIR(STENCIL),
    RENDER_MODE_PAIR(COLOR_STENCIL),
  };
#undef RENDER_MODE_PAIR
  for (auto& p: renderModePairs)
  {
    HELP_TEST_RENDERER_STATE(Renderer::Property::RENDER_MODE, RenderMode::NONE, p.first, p.second, renderer);
  }

  END_TEST;
}

int UtcDaliRendererStateParseEmpty(void)
{
  std::string error;
  auto onError = [&](const std::string& e) { error = e; };

  DALI_TEST_EQUAL(rs::Parse("", 0, onError), uint32_t(rs::NONE));
  DALI_TEST_CHECK(error.empty());
  END_TEST;
}

int UtcDaliRendererStateParseInvalid(void)
{
  std::string error;
  auto onError = [&](const std::string& e) { error = e; };

  DALI_TEST_EQUAL(rs::Parse("definitelyNotAValidRendererState", 0, onError), uint32_t(rs::NONE));
  DALI_TEST_CHECK(strstr(error.c_str(), "Not a valid RendererState") != nullptr);
  END_TEST;
}

namespace
{
struct Option
{
  std::string_view name;
  rs::Type expected;

  void Apply(std::ostream& stream, uint8_t shift, uint32_t& result) const
  {
    stream << name;
    result |= expected << shift;
  }
};

struct StateGenerator
{
  std::string_view name;

  uint8_t shift;
  std::vector<Option> permutations;
};

#define STRING_STATE_PAIR(x, y) { #x, y::x }
const decltype(StateGenerator::permutations) BLEND_FACTORS {
  { "", sl::BlendFactor::OMIT },
  STRING_STATE_PAIR(ZERO, sl::BlendFactor),
  STRING_STATE_PAIR(ONE, sl::BlendFactor),
  STRING_STATE_PAIR(SRC_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_SRC_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(SRC_ALPHA, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_SRC_ALPHA, sl::BlendFactor),
  STRING_STATE_PAIR(DST_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_DST_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(DST_ALPHA, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_DST_ALPHA, sl::BlendFactor),
  STRING_STATE_PAIR(SRC_ALPHA_SATURATE, sl::BlendFactor),
  STRING_STATE_PAIR(CONSTANT_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_CONSTANT_COLOR, sl::BlendFactor),
  STRING_STATE_PAIR(CONSTANT_ALPHA, sl::BlendFactor),
  STRING_STATE_PAIR(ONE_MINUS_CONSTANT_ALPHA, sl::BlendFactor),
};

const StateGenerator PERMUTATORS[] {
  { "DEPTH_FUNC:", rs::DEPTH_FUNCTION_SHIFT, {
    { "", Comparison::OMIT },
    STRING_STATE_PAIR(NEVER, Comparison),
    STRING_STATE_PAIR(ALWAYS, Comparison),
    STRING_STATE_PAIR(LESS, Comparison),
    STRING_STATE_PAIR(GREATER, Comparison),
    STRING_STATE_PAIR(EQUAL, Comparison),
    STRING_STATE_PAIR(NOT_EQUAL, Comparison),
    STRING_STATE_PAIR(LESS_EQUAL, Comparison),
    STRING_STATE_PAIR(GREATER_EQUAL, Comparison),
  } },
  { "BLEND_SRC_RGB:", rs::BLEND_FACTOR_BASE_SHIFT, BLEND_FACTORS },
  { "BLEND_DST_RGB:", rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS, BLEND_FACTORS  },
  { "BLEND_SRC_ALPHA:", rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS * 2, BLEND_FACTORS },
  { "BLEND_DST_ALPHA:", rs::BLEND_FACTOR_BASE_SHIFT + rs::BLEND_FACTOR_ITEM_BITS * 3, BLEND_FACTORS },
  { "BUFFER_MODE:", rs::BUFFER_MODE_SHIFT, {
    { "", BufferMode::OMIT },
    STRING_STATE_PAIR(NONE, BufferMode),
    STRING_STATE_PAIR(AUTO, BufferMode),
    STRING_STATE_PAIR(COLOR, BufferMode),
    STRING_STATE_PAIR(STENCIL, BufferMode),
    STRING_STATE_PAIR(COLOR_STENCIL, BufferMode),
  } },
  // binary options
  { "", 0, { STRING_STATE_PAIR(DEPTH_WRITE, rs) } },
  { "", 0, { STRING_STATE_PAIR(DEPTH_TEST, rs) } },
  { "", 0, { STRING_STATE_PAIR(CULL_FRONT, rs) } },
  { "", 0, { STRING_STATE_PAIR(CULL_BACK, rs) } },
  { "", 0, { STRING_STATE_PAIR(ALPHA_BLEND, rs) } },
};

#undef STRING_STATE_PAIR
}

int UtcDaliRendererStateParseIndividual(void)
{
  std::string error;
  auto onError = [&](const std::string& e) { error = e; };

  char buffer[512] {};
  for (auto& p: PERMUTATORS)
  {
    for (auto& o: p.permutations)
    {
      StreamBuffer streamBuf(buffer, sizeof(buffer));
      std::ostream stream(&streamBuf);

      stream << p.name;

      uint32_t expected = 0x0;
      o.Apply(stream, p.shift, expected);
      stream << '\0';

      printf("%s -> %d\n", buffer, expected);
      DALI_TEST_EQUAL(rs::Parse(buffer, 0, onError), expected);
      DALI_TEST_CHECK(error.empty());
    }
  }

  END_TEST;
}

int UtcDaliRendererStateParseCombined(void)
{
  constexpr uint32_t count = std::extent<decltype(PERMUTATORS)>::value;
  constexpr uint32_t prime = 13;
  static_assert(prime > count);
  constexpr uint32_t skip = 3 * count * count + 7 * count + 1;
  static_assert(skip % prime != 0);

  std::string error;
  auto onError = [&](const std::string& e) { error = e; };

  std::vector<rs::Type> expectedValues;

  char buffer[512] {};
  for (uint32_t i = 0; i < count; ++i)
  {
    StreamBuffer streamBuf(buffer, sizeof(buffer));
    std::ostream stream(&streamBuf);

    uint32_t expected = 0x0;

    uint32_t iTmp = i;
    for (uint32_t j = 0; j < count; ++j)
    {
      iTmp = (iTmp + skip) % count;
      DALI_TEST_CHECK(iTmp < count);

      if (expected)
      {
        stream << "|";
      }

      auto& perm = PERMUTATORS[iTmp];
      stream << perm.name;
      perm.permutations.back().Apply(stream, perm.shift, expected);
    }

    stream << '\0';

    DALI_TEST_EQUAL(rs::Parse(buffer, 0, onError), expected);
    DALI_TEST_CHECK(error.empty());

    auto iFind = std::lower_bound(expectedValues.begin(), expectedValues.end(), expected);
    if (iFind == expectedValues.end() || *iFind != expected)
    {
      expectedValues.insert(iFind, expected);
    }
    DALI_TEST_EQUAL(expectedValues.size(), 1u);
  }

  END_TEST;
}
