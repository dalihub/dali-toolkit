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
#include <dali-toolkit/internal/text/rendering/vector-based/glyphy-shader/glyphy-shader.h>

// EXTERNAL HEADERS
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
namespace Text
{
GlyphyShader::GlyphyShader()
{
}

GlyphyShader::GlyphyShader(Shader handle)
: Shader(handle)
{
}

GlyphyShader::~GlyphyShader()
{
}

GlyphyShader GlyphyShader::New(const Dali::Vector4& atlasInfo)
{
  std::ostringstream vertexShaderStringStream;
  std::ostringstream fragmentShaderStringStream;

  vertexShaderStringStream << SHADER_GLYPHY_SHADER_EXTENTION_PREFIX_DEF.data() << SHADER_GLYPHY_SHADER_MAIN_VERT.data();

  fragmentShaderStringStream << SHADER_GLYPHY_SHADER_EXTENTION_PREFIX_DEF.data()
                             << SHADER_GLYPHY_SHADER_FRAGMENT_PREFIX_FRAG.data()
                             << SHADER_GLYPHY_COMMON_GLSL_SHADER_DEF.data()
                             << "#define GLYPHY_SDF_PSEUDO_DISTANCE 1\n"
                             << SHADER_GLYPHY_SDF_GLSL_SHADER_DEF.data()
                             << SHADER_GLYPHY_SHADER_MAIN_FRAG.data();

  Shader shaderEffectCustom = Shader::New(vertexShaderStringStream.str(),
                                          fragmentShaderStringStream.str(),
                                          static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL | Shader::Hint::OUTPUT_IS_TRANSPARENT),
                                          "GLYPHY_SHADER");

  GlyphyShader handle(shaderEffectCustom);

  handle.RegisterProperty("u_atlas_info", atlasInfo);
  handle.RegisterProperty("u_contrast", 1.f);
  handle.RegisterProperty("u_gamma_adjust", 1.f);
  handle.RegisterProperty("u_outline_thickness", 1.f);
  handle.RegisterProperty("u_outline", 1.f);
  handle.RegisterProperty("u_boldness", 0.f);

  return handle;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
