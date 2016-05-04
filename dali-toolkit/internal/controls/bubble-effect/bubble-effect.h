#ifndef __DALI_TOOLKIT_INTERNAL_BUBBLE_EFFECT_H_
#define __DALI_TOOLKIT_INTERNAL_BUBBLE_EFFECT_H_

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

// EXTERNAL INCLUDES
#include <sstream>
#include <dali/devel-api/rendering/shader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Create the shader to be used by the renderer
 * @param[in] numberOfBubble How many groups of uniforms are used to control the bubble movement.
 * @return A handle to the newly created shader.
 */
inline Shader CreateBubbleShader( unsigned int numBubble )
{
  const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump float   aIndex;\n
  attribute mediump vec2    aPosition;\n
  attribute highp   vec2    aTexCoord;\n
  varying   mediump vec2    vTexCoord;\n
  uniform   mediump mat4    uMvpMatrix;\n
  // the gravity applied to the y direction
  uniform mediump float uGravity;\n
  // xy: the emit position of the bubble; zw: the destination of the bubble.
  // The bubble is moving from (xy) to (zw plus the y drop influenced by gravity).
  uniform vec4 uStartEndPosition[NUMBER_OF_BUBBLE];\n
  // The undergoing percentage of the bubble movement. 0.0: start from emit position, 1.0: reach the destination
  uniform float uPercentage[NUMBER_OF_BUBBLE];\n
  uniform vec2 uInvertedMovementArea;\n
  // The bubble number is restricted by the available uniform num.
  // To increase the displayed bubble, every uStartEndPosition and uPercentage uniform is applied to a small bunch of bubbles (9 here)
  // The offset defines the random offset between bubbles within the bunch.
  uniform vec2 uOffset[9];\n
  // This uniform is used to change the bubble size during running time
  uniform float uDynamicScale;\n
  varying float vPercentage;\n
  varying vec2  vEffectTexCoord;\n
  void main()\n
  {\n
    vec4 position = vec4( aPosition, 0.0, 1.0 );\n
    // The Z coordinate is used to record the bubble index within current mesh actor
    int index = int(aIndex); \n
    //for some i between 0 ~ NUMBER_OF_BUBBLE-1: i,i+NUMBER_OF_BUBBLE, i+NUMBER_OF_BUBBLE*2, ... (up to i+NUMBER_OF_BUBBLE*8) belongs to the same bunch.
    int groupIdx = index / NUMBER_OF_BUBBLE;\n
    // The bubbles within the same bunch applies the same uniforms uStartEndPosition[idx] & uPercentage[idx]
    int idx = index - groupIdx*NUMBER_OF_BUBBLE;\n
    float percentage = uPercentage[idx];
    // early out if uPercentage is (zero || one) setting position to zero (zero sized triangles)
    if( percentage <= 0.0 || percentage >= 1.0 )\n
    {\n
      gl_Position = vec4(0.0);\n
      return;\n
    }\n
    vec4 startAndEnd = uStartEndPosition[idx];\n
    // The final position is added up different offset for bubbles
    startAndEnd.zw += uOffset[groupIdx];\n
    \n
    // increase the bubble size from 0% to 100% during the first 1/5 of movement & apply the dynamic scale
    // the new xy value containes both the new scale and new bubble position
    position.xy *= uDynamicScale*min(percentage*5.0, 1.0);\n
    position.xy += mix(startAndEnd.xy, startAndEnd.zw, percentage);\n
    // The gravity is g*t*t on the y direction
    position.y += uGravity * pow(percentage, 2.0);\n
    gl_Position = uMvpMatrix * position;\n
    \n
    // Add multiple bubble shapes in the effect
    vTexCoord = aTexCoord;\n
    vPercentage = percentage;\n
    // Use the emit position color for the bubble
    vEffectTexCoord = startAndEnd.xy * uInvertedMovementArea;\n
  }\n
  );

  const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2  vTexCoord;\n
  uniform lowp    vec4  uColor;\n
  uniform sampler2D     sBackground;\n
  uniform sampler2D     sBubbleShape;\n
  varying mediump float vPercentage;\n
  varying mediump vec2  vEffectTexCoord;\n
  \n
  void main()\n
  {\n
    // Get the emit pisition color, and Mix with the actor color
    mediump vec4 fragColor = texture2D(sBackground, vEffectTexCoord)*uColor;\n
    // Apply the shape defined by the texture contained sBubbleShape
    // And make the opacity being 0.7, and animate from 0.7 to 0 during the last 1/3 of movement
    fragColor.a  *= texture2D(sBubbleShape, vTexCoord).a * ( 2.1 - max( vPercentage*2.1, 1.4 ) );\n
    gl_FragColor = fragColor;\n
  }\n
  );

  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream << "#define NUMBER_OF_BUBBLE "<< numBubble << "\n"
                           << VERTEX_SHADER;
  Shader shader = Shader::New( vertexShaderStringStream.str(), FRAGMENT_SHADER );

  return shader;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_INTERNAL_BUBBLE_EFFECT_H_ */
