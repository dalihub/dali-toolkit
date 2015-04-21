/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/shader-effects/alpha-discard-effect.h>

namespace Dali
{

namespace Toolkit
{

AlphaDiscardEffect::AlphaDiscardEffect()
{
}

AlphaDiscardEffect::~AlphaDiscardEffect()
{
}

AlphaDiscardEffect AlphaDiscardEffect::New()
{
  const char* ALPHA_DISCARD_FRAGMENT_SHADER_SOURCE =
      "void main()                                                    \n"
      "{                                                              \n"
      "  mediump vec4 color = texture2D( sTexture, vTexCoord );       \n"
      "  if(color.a <= 0.0001)                                        \n"
      "  {                                                            \n"
      "    discard;                                                   \n"
      "  }                                                            \n"
      "  gl_FragColor = color * uColor;                               \n"
      "}                                                              \n";

  ShaderEffect shader = ShaderEffect::New( "", // Use default
                                           ALPHA_DISCARD_FRAGMENT_SHADER_SOURCE );
  return AlphaDiscardEffect( shader );
}

//Call the Parent copy constructor to add reference to the implementation for this object
AlphaDiscardEffect::AlphaDiscardEffect( ShaderEffect handle )
: ShaderEffect( handle )
{
}

} // namespace Toolkit

} // namespace Dali
