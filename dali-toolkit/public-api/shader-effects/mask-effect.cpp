/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/shader-effects/mask-effect.h>

namespace Dali
{

namespace Toolkit
{

MaskEffect::MaskEffect()
{
}

MaskEffect::~MaskEffect()
{
}

MaskEffect MaskEffect::New( Image maskImage )
{
  const char* ALPHA_MASK_FRAGMENT_SHADER_SOURCE =
  "void main()                                                                    \n"
  "{                                                                              \n"
  "  highp vec4 mask = texture2D(sEffect, vTexCoord);                             \n"
  "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(1,1,1,mask.a); \n"
  "}                                                                              \n";

  ShaderEffect shader = ShaderEffect::New( "", // Use default
                                           ALPHA_MASK_FRAGMENT_SHADER_SOURCE,
                                           GeometryType( GEOMETRY_TYPE_IMAGE ),
                                           ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

  shader.SetEffectImage( maskImage );

  return MaskEffect( shader );
}

//Call the Parent copy constructor to add reference to the implementation for this object
MaskEffect::MaskEffect( ShaderEffect handle )
: ShaderEffect( handle )
{
}

} // namespace Toolkit

} // namespace Dali
