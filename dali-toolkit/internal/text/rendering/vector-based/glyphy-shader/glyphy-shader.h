#ifndef DALI_TOOLKIT_TEXT_GLYPHY_SHADER_H
#define DALI_TOOLKIT_TEXT_GLYPHY_SHADER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNEL INCLUDES
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/rendering/shader.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief A Shader based on GLyphy authored by Behdad Esfahbod & Maysum Panju.
 *
 * See https://github.com/behdad/glyphy for more details of GLyphy.
 */
class GlyphyShader : public Shader
{
public:

  /**
   * @brief Create the blob atlas.
   *
   * @param[in] atlasInfo The metrics of the texture atlas storing vector data.
   */
  static GlyphyShader New( const Vector4& atlasInfo );

  /**
   * @brief Create an uninitialized GlyphyShader handle.
   */
  GlyphyShader();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GlyphyShader();

private: // Not intended for application developer

  GlyphyShader( Shader handle );

};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_GLYPHY_SHADER_H
