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

// CLASS HEADER
#include "bubble-emitter-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/texture.h>
// #include <dali/public-api/rendering/shader.h>
#include <dali/devel-api/rendering/shader-devel.h>


// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/bubble-effect/bubble-effect.h>
#include <dali-toolkit/internal/controls/bubble-effect/bubble-renderer.h>

namespace
{
struct Vertex
{
  Vertex()
  : index( 0.0f ), position(), textureCoord()
  {
  }

  Vertex( float index, const Dali::Vector2& position, const Dali::Vector2& textureCoord )
  : index( index ), position( position ), textureCoord( textureCoord )
  {
  }

  float index;
  Dali::Vector2 position;
  Dali::Vector2 textureCoord;
};

/**
 * Return a random value between the given interval.
 * @param[in] f0 The low bound
 * @param[in] f1 The up bound
 * @param[in] seed The seed to genergate random number
 * @return A random value between the given interval
 */
float RandomRange(float f0, float f1, unsigned int& seed)
{
  return f0 + (rand_r( &seed ) & 0xfff) * (f1-f0) * (1.0f/4095.0f);
}


/*
**** Vertex Shader
#version 430

layout( location = 0 ) in vec2 aPosition;
layout( location = 1 ) in vec2 aTexCoord;

layout( set = 0, binding = 0 , std140 ) uniform VertexData
{
  uniform vec3 uSize;
  uniform mat4 uMvpMatrix;
};

layout( location = 0 ) out vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
  vTexCoord = aTexCoord;
}



****Fragment Shader
#version 430

layout( location= 0 ) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140) uniform FragData
{
 uniform vec3 uHSVDelta;
};

layout( set = 0, binding =2 ) uniform sampler2D sTexture;

layout( location= 0 ) out vec4 fragColor;


float rand(vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }

vec3 rgb2hsv(vec3 c)
{
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c)
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {
  vec4 color = texture(sTexture, vTexCoord);
  vec3 hsvColor = rgb2hsv( color.rgb );
  // modify the hsv Value
  hsvColor += uHSVDelta * rand(vTexCoord);
  // if the new vale exceeds one, then decrease it
  hsvColor -= max(hsvColor*2.0 - vec3(2.0), 0.0);
  // if the new vale drops below zero, then increase it
  hsvColor -= min(hsvColor*2.0, 0.0);
  color = vec4( hsv2rgb( hsvColor ), 1.0 );
  fragColor = color;
}
*/

std::vector<uint32_t> VERTEX_SHADER = {
	0x07230203,0x00010000,0x00080007,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0009000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000d,0x0000001b,0x0000002b,
	0x0000002c,0x00030003,0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,0x00000000,
	0x00060005,0x0000000b,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x0000000b,
	0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000000b,0x00000001,0x505f6c67,
	0x746e696f,0x657a6953,0x00000000,0x00070006,0x0000000b,0x00000002,0x435f6c67,0x4470696c,
	0x61747369,0x0065636e,0x00030005,0x0000000d,0x00000000,0x00050005,0x00000012,0x74726556,
	0x61447865,0x00006174,0x00050006,0x00000012,0x00000000,0x7a695375,0x00000065,0x00060006,
	0x00000012,0x00000001,0x70764d75,0x7274614d,0x00007869,0x00030005,0x00000014,0x00000000,
	0x00050005,0x0000001b,0x736f5061,0x6f697469,0x0000006e,0x00050005,0x0000002b,0x78655476,
	0x726f6f43,0x00000064,0x00050005,0x0000002c,0x78655461,0x726f6f43,0x00000064,0x00050048,
	0x0000000b,0x00000000,0x0000000b,0x00000000,0x00050048,0x0000000b,0x00000001,0x0000000b,
	0x00000001,0x00050048,0x0000000b,0x00000002,0x0000000b,0x00000003,0x00030047,0x0000000b,
	0x00000002,0x00050048,0x00000012,0x00000000,0x00000023,0x00000000,0x00040048,0x00000012,
	0x00000001,0x00000005,0x00050048,0x00000012,0x00000001,0x00000023,0x00000010,0x00050048,
	0x00000012,0x00000001,0x00000007,0x00000010,0x00030047,0x00000012,0x00000002,0x00040047,
	0x00000014,0x00000022,0x00000000,0x00040047,0x00000014,0x00000021,0x00000000,0x00040047,
	0x0000001b,0x0000001e,0x00000000,0x00040047,0x0000002b,0x0000001e,0x00000000,0x00040047,
	0x0000002c,0x0000001e,0x00000001,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
	0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040015,
	0x00000008,0x00000020,0x00000000,0x0004002b,0x00000008,0x00000009,0x00000001,0x0004001c,
	0x0000000a,0x00000006,0x00000009,0x0005001e,0x0000000b,0x00000007,0x00000006,0x0000000a,
	0x00040020,0x0000000c,0x00000003,0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000003,
	0x00040015,0x0000000e,0x00000020,0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,
	0x00040017,0x00000010,0x00000006,0x00000003,0x00040018,0x00000011,0x00000007,0x00000004,
	0x0004001e,0x00000012,0x00000010,0x00000011,0x00040020,0x00000013,0x00000002,0x00000012,
	0x0004003b,0x00000013,0x00000014,0x00000002,0x0004002b,0x0000000e,0x00000015,0x00000001,
	0x00040020,0x00000016,0x00000002,0x00000011,0x00040017,0x00000019,0x00000006,0x00000002,
	0x00040020,0x0000001a,0x00000001,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000001,
	0x00040020,0x0000001d,0x00000002,0x00000010,0x0004002b,0x00000006,0x00000022,0x00000000,
	0x0004002b,0x00000006,0x00000023,0x3f800000,0x00040020,0x00000028,0x00000003,0x00000007,
	0x00040020,0x0000002a,0x00000003,0x00000019,0x0004003b,0x0000002a,0x0000002b,0x00000003,
	0x0004003b,0x0000001a,0x0000002c,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,
	0x00000003,0x000200f8,0x00000005,0x00050041,0x00000016,0x00000017,0x00000014,0x00000015,
	0x0004003d,0x00000011,0x00000018,0x00000017,0x0004003d,0x00000019,0x0000001c,0x0000001b,
	0x00050041,0x0000001d,0x0000001e,0x00000014,0x0000000f,0x0004003d,0x00000010,0x0000001f,
	0x0000001e,0x0007004f,0x00000019,0x00000020,0x0000001f,0x0000001f,0x00000000,0x00000001,
	0x00050085,0x00000019,0x00000021,0x0000001c,0x00000020,0x00050051,0x00000006,0x00000024,
	0x00000021,0x00000000,0x00050051,0x00000006,0x00000025,0x00000021,0x00000001,0x00070050,
	0x00000007,0x00000026,0x00000024,0x00000025,0x00000022,0x00000023,0x00050091,0x00000007,
	0x00000027,0x00000018,0x00000026,0x00050041,0x00000028,0x00000029,0x0000000d,0x0000000f,
	0x0003003e,0x00000029,0x00000027,0x0004003d,0x00000019,0x0000002d,0x0000002c,0x0003003e,
	0x0000002b,0x0000002d,0x000100fd,0x00010038
};

std::vector<uint32_t> FRAGMENT_SHADER = {
	0x07230203,0x00010000,0x00080007,0x000000df,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x000000b0,0x000000dd,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00050005,0x0000000b,0x646e6172,0x32667628,0x0000003b,0x00030005,0x0000000a,
	0x00006f63,0x00060005,0x00000011,0x32626772,0x28767368,0x3b336676,0x00000000,0x00030005,
	0x00000010,0x00000063,0x00060005,0x00000014,0x32767368,0x28626772,0x3b336676,0x00000000,
	0x00030005,0x00000013,0x00000063,0x00030005,0x00000023,0x0000004b,0x00030005,0x00000029,
	0x00000070,0x00030005,0x00000047,0x00000071,0x00030005,0x00000060,0x00000064,0x00030005,
	0x0000006a,0x00000065,0x00030005,0x00000086,0x0000004b,0x00030005,0x0000008b,0x00000070,
	0x00040005,0x000000a9,0x6f6c6f63,0x00000072,0x00050005,0x000000ad,0x78655473,0x65727574,
	0x00000000,0x00050005,0x000000b0,0x78655476,0x726f6f43,0x00000064,0x00050005,0x000000b3,
	0x43767368,0x726f6c6f,0x00000000,0x00040005,0x000000b4,0x61726170,0x0000006d,0x00050005,
	0x000000b8,0x67617246,0x61746144,0x00000000,0x00060006,0x000000b8,0x00000000,0x56534875,
	0x746c6544,0x00000061,0x00030005,0x000000ba,0x00000000,0x00040005,0x000000c0,0x61726170,
	0x0000006d,0x00040005,0x000000d5,0x61726170,0x0000006d,0x00050005,0x000000dd,0x67617266,
	0x6f6c6f43,0x00000072,0x00040047,0x000000ad,0x00000022,0x00000000,0x00040047,0x000000ad,
	0x00000021,0x00000002,0x00040047,0x000000b0,0x0000001e,0x00000000,0x00050048,0x000000b8,
	0x00000000,0x00000023,0x00000000,0x00030047,0x000000b8,0x00000002,0x00040047,0x000000ba,
	0x00000022,0x00000000,0x00040047,0x000000ba,0x00000021,0x00000001,0x00040047,0x000000dd,
	0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,
	0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000002,0x00040020,0x00000008,
	0x00000007,0x00000007,0x00040021,0x00000009,0x00000006,0x00000008,0x00040017,0x0000000d,
	0x00000006,0x00000003,0x00040020,0x0000000e,0x00000007,0x0000000d,0x00040021,0x0000000f,
	0x0000000d,0x0000000e,0x0004002b,0x00000006,0x00000017,0x414fd639,0x0004002b,0x00000006,
	0x00000018,0x429c774c,0x0005002c,0x00000007,0x00000019,0x00000017,0x00000018,0x0004002b,
	0x00000006,0x0000001c,0x472aee8c,0x00040017,0x00000021,0x00000006,0x00000004,0x00040020,
	0x00000022,0x00000007,0x00000021,0x0004002b,0x00000006,0x00000024,0x00000000,0x0004002b,
	0x00000006,0x00000025,0xbeaaaaab,0x0004002b,0x00000006,0x00000026,0x3f2aaaab,0x0004002b,
	0x00000006,0x00000027,0xbf800000,0x0007002c,0x00000021,0x00000028,0x00000024,0x00000025,
	0x00000026,0x00000027,0x00040015,0x0000003c,0x00000020,0x00000000,0x0004002b,0x0000003c,
	0x0000003d,0x00000002,0x00040020,0x0000003e,0x00000007,0x00000006,0x0004002b,0x0000003c,
	0x00000041,0x00000001,0x0004002b,0x0000003c,0x0000004a,0x00000000,0x0004002b,0x0000003c,
	0x00000063,0x00000003,0x0004002b,0x00000006,0x0000006b,0x2edbe6ff,0x0004002b,0x00000006,
	0x00000073,0x40c00000,0x0004002b,0x00000006,0x00000087,0x3f800000,0x0004002b,0x00000006,
	0x00000088,0x3eaaaaab,0x0004002b,0x00000006,0x00000089,0x40400000,0x0007002c,0x00000021,
	0x0000008a,0x00000087,0x00000026,0x00000088,0x00000089,0x00090019,0x000000aa,0x00000006,
	0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x0003001b,0x000000ab,
	0x000000aa,0x00040020,0x000000ac,0x00000000,0x000000ab,0x0004003b,0x000000ac,0x000000ad,
	0x00000000,0x00040020,0x000000af,0x00000001,0x00000007,0x0004003b,0x000000af,0x000000b0,
	0x00000001,0x0003001e,0x000000b8,0x0000000d,0x00040020,0x000000b9,0x00000002,0x000000b8,
	0x0004003b,0x000000b9,0x000000ba,0x00000002,0x00040015,0x000000bb,0x00000020,0x00000001,
	0x0004002b,0x000000bb,0x000000bc,0x00000000,0x00040020,0x000000bd,0x00000002,0x0000000d,
	0x0004002b,0x00000006,0x000000c7,0x40000000,0x0006002c,0x0000000d,0x000000c9,0x000000c7,
	0x000000c7,0x000000c7,0x00040020,0x000000dc,0x00000003,0x00000021,0x0004003b,0x000000dc,
	0x000000dd,0x00000003,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
	0x00000005,0x0004003b,0x00000022,0x000000a9,0x00000007,0x0004003b,0x0000000e,0x000000b3,
	0x00000007,0x0004003b,0x0000000e,0x000000b4,0x00000007,0x0004003b,0x00000008,0x000000c0,
	0x00000007,0x0004003b,0x0000000e,0x000000d5,0x00000007,0x0004003d,0x000000ab,0x000000ae,
	0x000000ad,0x0004003d,0x00000007,0x000000b1,0x000000b0,0x00050057,0x00000021,0x000000b2,
	0x000000ae,0x000000b1,0x0003003e,0x000000a9,0x000000b2,0x0004003d,0x00000021,0x000000b5,
	0x000000a9,0x0008004f,0x0000000d,0x000000b6,0x000000b5,0x000000b5,0x00000000,0x00000001,
	0x00000002,0x0003003e,0x000000b4,0x000000b6,0x00050039,0x0000000d,0x000000b7,0x00000011,
	0x000000b4,0x0003003e,0x000000b3,0x000000b7,0x00050041,0x000000bd,0x000000be,0x000000ba,
	0x000000bc,0x0004003d,0x0000000d,0x000000bf,0x000000be,0x0004003d,0x00000007,0x000000c1,
	0x000000b0,0x0003003e,0x000000c0,0x000000c1,0x00050039,0x00000006,0x000000c2,0x0000000b,
	0x000000c0,0x0005008e,0x0000000d,0x000000c3,0x000000bf,0x000000c2,0x0004003d,0x0000000d,
	0x000000c4,0x000000b3,0x00050081,0x0000000d,0x000000c5,0x000000c4,0x000000c3,0x0003003e,
	0x000000b3,0x000000c5,0x0004003d,0x0000000d,0x000000c6,0x000000b3,0x0005008e,0x0000000d,
	0x000000c8,0x000000c6,0x000000c7,0x00050083,0x0000000d,0x000000ca,0x000000c8,0x000000c9,
	0x00060050,0x0000000d,0x000000cb,0x00000024,0x00000024,0x00000024,0x0007000c,0x0000000d,
	0x000000cc,0x00000001,0x00000028,0x000000ca,0x000000cb,0x0004003d,0x0000000d,0x000000cd,
	0x000000b3,0x00050083,0x0000000d,0x000000ce,0x000000cd,0x000000cc,0x0003003e,0x000000b3,
	0x000000ce,0x0004003d,0x0000000d,0x000000cf,0x000000b3,0x0005008e,0x0000000d,0x000000d0,
	0x000000cf,0x000000c7,0x00060050,0x0000000d,0x000000d1,0x00000024,0x00000024,0x00000024,
	0x0007000c,0x0000000d,0x000000d2,0x00000001,0x00000025,0x000000d0,0x000000d1,0x0004003d,
	0x0000000d,0x000000d3,0x000000b3,0x00050083,0x0000000d,0x000000d4,0x000000d3,0x000000d2,
	0x0003003e,0x000000b3,0x000000d4,0x0004003d,0x0000000d,0x000000d6,0x000000b3,0x0003003e,
	0x000000d5,0x000000d6,0x00050039,0x0000000d,0x000000d7,0x00000014,0x000000d5,0x00050051,
	0x00000006,0x000000d8,0x000000d7,0x00000000,0x00050051,0x00000006,0x000000d9,0x000000d7,
	0x00000001,0x00050051,0x00000006,0x000000da,0x000000d7,0x00000002,0x00070050,0x00000021,
	0x000000db,0x000000d8,0x000000d9,0x000000da,0x00000087,0x0003003e,0x000000a9,0x000000db,
	0x0004003d,0x00000021,0x000000de,0x000000a9,0x0003003e,0x000000dd,0x000000de,0x000100fd,
	0x00010038,0x00050036,0x00000006,0x0000000b,0x00000000,0x00000009,0x00030037,0x00000008,
	0x0000000a,0x000200f8,0x0000000c,0x0004003d,0x00000007,0x00000016,0x0000000a,0x00050094,
	0x00000006,0x0000001a,0x00000016,0x00000019,0x0006000c,0x00000006,0x0000001b,0x00000001,
	0x0000000d,0x0000001a,0x00050085,0x00000006,0x0000001d,0x0000001b,0x0000001c,0x0006000c,
	0x00000006,0x0000001e,0x00000001,0x0000000a,0x0000001d,0x000200fe,0x0000001e,0x00010038,
	0x00050036,0x0000000d,0x00000011,0x00000000,0x0000000f,0x00030037,0x0000000e,0x00000010,
	0x000200f8,0x00000012,0x0004003b,0x00000022,0x00000023,0x00000007,0x0004003b,0x00000022,
	0x00000029,0x00000007,0x0004003b,0x00000022,0x00000047,0x00000007,0x0004003b,0x0000003e,
	0x00000060,0x00000007,0x0004003b,0x0000003e,0x0000006a,0x00000007,0x0003003e,0x00000023,
	0x00000028,0x0004003d,0x0000000d,0x0000002a,0x00000010,0x0007004f,0x00000007,0x0000002b,
	0x0000002a,0x0000002a,0x00000002,0x00000001,0x0004003d,0x00000021,0x0000002c,0x00000023,
	0x0007004f,0x00000007,0x0000002d,0x0000002c,0x0000002c,0x00000003,0x00000002,0x00050051,
	0x00000006,0x0000002e,0x0000002b,0x00000000,0x00050051,0x00000006,0x0000002f,0x0000002b,
	0x00000001,0x00050051,0x00000006,0x00000030,0x0000002d,0x00000000,0x00050051,0x00000006,
	0x00000031,0x0000002d,0x00000001,0x00070050,0x00000021,0x00000032,0x0000002e,0x0000002f,
	0x00000030,0x00000031,0x0004003d,0x0000000d,0x00000033,0x00000010,0x0007004f,0x00000007,
	0x00000034,0x00000033,0x00000033,0x00000001,0x00000002,0x0004003d,0x00000021,0x00000035,
	0x00000023,0x0007004f,0x00000007,0x00000036,0x00000035,0x00000035,0x00000000,0x00000001,
	0x00050051,0x00000006,0x00000037,0x00000034,0x00000000,0x00050051,0x00000006,0x00000038,
	0x00000034,0x00000001,0x00050051,0x00000006,0x00000039,0x00000036,0x00000000,0x00050051,
	0x00000006,0x0000003a,0x00000036,0x00000001,0x00070050,0x00000021,0x0000003b,0x00000037,
	0x00000038,0x00000039,0x0000003a,0x00050041,0x0000003e,0x0000003f,0x00000010,0x0000003d,
	0x0004003d,0x00000006,0x00000040,0x0000003f,0x00050041,0x0000003e,0x00000042,0x00000010,
	0x00000041,0x0004003d,0x00000006,0x00000043,0x00000042,0x0007000c,0x00000006,0x00000044,
	0x00000001,0x00000030,0x00000040,0x00000043,0x00070050,0x00000021,0x00000045,0x00000044,
	0x00000044,0x00000044,0x00000044,0x0008000c,0x00000021,0x00000046,0x00000001,0x0000002e,
	0x00000032,0x0000003b,0x00000045,0x0003003e,0x00000029,0x00000046,0x0004003d,0x00000021,
	0x00000048,0x00000029,0x0008004f,0x0000000d,0x00000049,0x00000048,0x00000048,0x00000000,
	0x00000001,0x00000003,0x00050041,0x0000003e,0x0000004b,0x00000010,0x0000004a,0x0004003d,
	0x00000006,0x0000004c,0x0000004b,0x00050051,0x00000006,0x0000004d,0x00000049,0x00000000,
	0x00050051,0x00000006,0x0000004e,0x00000049,0x00000001,0x00050051,0x00000006,0x0000004f,
	0x00000049,0x00000002,0x00070050,0x00000021,0x00000050,0x0000004d,0x0000004e,0x0000004f,
	0x0000004c,0x00050041,0x0000003e,0x00000051,0x00000010,0x0000004a,0x0004003d,0x00000006,
	0x00000052,0x00000051,0x0004003d,0x00000021,0x00000053,0x00000029,0x0008004f,0x0000000d,
	0x00000054,0x00000053,0x00000053,0x00000001,0x00000002,0x00000000,0x00050051,0x00000006,
	0x00000055,0x00000054,0x00000000,0x00050051,0x00000006,0x00000056,0x00000054,0x00000001,
	0x00050051,0x00000006,0x00000057,0x00000054,0x00000002,0x00070050,0x00000021,0x00000058,
	0x00000052,0x00000055,0x00000056,0x00000057,0x00050041,0x0000003e,0x00000059,0x00000029,
	0x0000004a,0x0004003d,0x00000006,0x0000005a,0x00000059,0x00050041,0x0000003e,0x0000005b,
	0x00000010,0x0000004a,0x0004003d,0x00000006,0x0000005c,0x0000005b,0x0007000c,0x00000006,
	0x0000005d,0x00000001,0x00000030,0x0000005a,0x0000005c,0x00070050,0x00000021,0x0000005e,
	0x0000005d,0x0000005d,0x0000005d,0x0000005d,0x0008000c,0x00000021,0x0000005f,0x00000001,
	0x0000002e,0x00000050,0x00000058,0x0000005e,0x0003003e,0x00000047,0x0000005f,0x00050041,
	0x0000003e,0x00000061,0x00000047,0x0000004a,0x0004003d,0x00000006,0x00000062,0x00000061,
	0x00050041,0x0000003e,0x00000064,0x00000047,0x00000063,0x0004003d,0x00000006,0x00000065,
	0x00000064,0x00050041,0x0000003e,0x00000066,0x00000047,0x00000041,0x0004003d,0x00000006,
	0x00000067,0x00000066,0x0007000c,0x00000006,0x00000068,0x00000001,0x00000025,0x00000065,
	0x00000067,0x00050083,0x00000006,0x00000069,0x00000062,0x00000068,0x0003003e,0x00000060,
	0x00000069,0x0003003e,0x0000006a,0x0000006b,0x00050041,0x0000003e,0x0000006c,0x00000047,
	0x0000003d,0x0004003d,0x00000006,0x0000006d,0x0000006c,0x00050041,0x0000003e,0x0000006e,
	0x00000047,0x00000063,0x0004003d,0x00000006,0x0000006f,0x0000006e,0x00050041,0x0000003e,
	0x00000070,0x00000047,0x00000041,0x0004003d,0x00000006,0x00000071,0x00000070,0x00050083,
	0x00000006,0x00000072,0x0000006f,0x00000071,0x0004003d,0x00000006,0x00000074,0x00000060,
	0x00050085,0x00000006,0x00000075,0x00000073,0x00000074,0x0004003d,0x00000006,0x00000076,
	0x0000006a,0x00050081,0x00000006,0x00000077,0x00000075,0x00000076,0x00050088,0x00000006,
	0x00000078,0x00000072,0x00000077,0x00050081,0x00000006,0x00000079,0x0000006d,0x00000078,
	0x0006000c,0x00000006,0x0000007a,0x00000001,0x00000004,0x00000079,0x0004003d,0x00000006,
	0x0000007b,0x00000060,0x00050041,0x0000003e,0x0000007c,0x00000047,0x0000004a,0x0004003d,
	0x00000006,0x0000007d,0x0000007c,0x0004003d,0x00000006,0x0000007e,0x0000006a,0x00050081,
	0x00000006,0x0000007f,0x0000007d,0x0000007e,0x00050088,0x00000006,0x00000080,0x0000007b,
	0x0000007f,0x00050041,0x0000003e,0x00000081,0x00000047,0x0000004a,0x0004003d,0x00000006,
	0x00000082,0x00000081,0x00060050,0x0000000d,0x00000083,0x0000007a,0x00000080,0x00000082,
	0x000200fe,0x00000083,0x00010038,0x00050036,0x0000000d,0x00000014,0x00000000,0x0000000f,
	0x00030037,0x0000000e,0x00000013,0x000200f8,0x00000015,0x0004003b,0x00000022,0x00000086,
	0x00000007,0x0004003b,0x0000000e,0x0000008b,0x00000007,0x0003003e,0x00000086,0x0000008a,
	0x0004003d,0x0000000d,0x0000008c,0x00000013,0x0008004f,0x0000000d,0x0000008d,0x0000008c,
	0x0000008c,0x00000000,0x00000000,0x00000000,0x0004003d,0x00000021,0x0000008e,0x00000086,
	0x0008004f,0x0000000d,0x0000008f,0x0000008e,0x0000008e,0x00000000,0x00000001,0x00000002,
	0x00050081,0x0000000d,0x00000090,0x0000008d,0x0000008f,0x0006000c,0x0000000d,0x00000091,
	0x00000001,0x0000000a,0x00000090,0x0005008e,0x0000000d,0x00000092,0x00000091,0x00000073,
	0x0004003d,0x00000021,0x00000093,0x00000086,0x0008004f,0x0000000d,0x00000094,0x00000093,
	0x00000093,0x00000003,0x00000003,0x00000003,0x00050083,0x0000000d,0x00000095,0x00000092,
	0x00000094,0x0006000c,0x0000000d,0x00000096,0x00000001,0x00000004,0x00000095,0x0003003e,
	0x0000008b,0x00000096,0x00050041,0x0000003e,0x00000097,0x00000013,0x0000003d,0x0004003d,
	0x00000006,0x00000098,0x00000097,0x0004003d,0x00000021,0x00000099,0x00000086,0x0008004f,
	0x0000000d,0x0000009a,0x00000099,0x00000099,0x00000000,0x00000000,0x00000000,0x0004003d,
	0x0000000d,0x0000009b,0x0000008b,0x0004003d,0x00000021,0x0000009c,0x00000086,0x0008004f,
	0x0000000d,0x0000009d,0x0000009c,0x0000009c,0x00000000,0x00000000,0x00000000,0x00050083,
	0x0000000d,0x0000009e,0x0000009b,0x0000009d,0x00060050,0x0000000d,0x0000009f,0x00000024,
	0x00000024,0x00000024,0x00060050,0x0000000d,0x000000a0,0x00000087,0x00000087,0x00000087,
	0x0008000c,0x0000000d,0x000000a1,0x00000001,0x0000002b,0x0000009e,0x0000009f,0x000000a0,
	0x00050041,0x0000003e,0x000000a2,0x00000013,0x00000041,0x0004003d,0x00000006,0x000000a3,
	0x000000a2,0x00060050,0x0000000d,0x000000a4,0x000000a3,0x000000a3,0x000000a3,0x0008000c,
	0x0000000d,0x000000a5,0x00000001,0x0000002e,0x0000009a,0x000000a1,0x000000a4,0x0005008e,
	0x0000000d,0x000000a6,0x000000a5,0x00000098,0x000200fe,0x000000a6,0x00010038
};



Dali::Geometry CreateTexturedQuad()
{
  struct Vertex
  {
    Dali::Vector2 position;
    Dali::Vector2 texCoord;
  };

  static const Vertex data[] = {{ Dali::Vector2( -0.5f, -0.5f ), Dali::Vector2( 0.0f, 0.0f ) },
                                { Dali::Vector2(  0.5f, -0.5f ), Dali::Vector2( 1.0f, 0.0f ) },
                                { Dali::Vector2( -0.5f,  0.5f ), Dali::Vector2( 0.0f, 1.0f ) },
                                { Dali::Vector2(  0.5f,  0.5f ), Dali::Vector2( 1.0f, 1.0f ) }};

  //Create a vertex buffer for vertex positions and texture coordinates
  Dali::PropertyBuffer vertexBuffer = Dali::PropertyBuffer::New( Dali::Property::Map()
                                              .Add( "aPosition", Dali::Property::VECTOR2 )
                                              .Add( "aTexCoord", Dali::Property::VECTOR2 ) );
  vertexBuffer.SetData( data, 4u );

  //Create the geometry
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType(Dali::Geometry::TRIANGLE_STRIP );

  return geometry;
}

}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
BubbleEmitter::BubbleEmitter( const Vector2& movementArea,
                              Texture shapeTexture,
                              unsigned int maximumNumberOfBubble,
                              const Vector2& bubbleSizeRange )
: Control( ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT ) ),
  mShapeTexture( shapeTexture ),
  mMovementArea( movementArea ),
  mBubbleSizeRange( bubbleSizeRange ),
  mDensity( 5 ),
  mTotalNumOfBubble( maximumNumberOfBubble ),
  mCurrentBubble( 0 ),
  mRandomSeed( 0 ),
  mRenderTaskRunning(false)
{
  // Calculate how many shaders are required
  if( mTotalNumOfBubble>100 )
  {
    mNumBubblePerRenderer = 100;
    mNumRenderer = mTotalNumOfBubble / 100;
    if( mNumRenderer*mNumBubblePerRenderer < mTotalNumOfBubble )
    {
      mNumRenderer++;
      mNumBubblePerRenderer =  mTotalNumOfBubble / mNumRenderer+1;
      mTotalNumOfBubble = mNumRenderer * mNumBubblePerRenderer;
    }
  }
  else
  {
    mNumBubblePerRenderer = mTotalNumOfBubble;
    mNumRenderer = 1;
  }

  mRandomSeed = time( NULL );
}

BubbleEmitter::~BubbleEmitter()
{
}

Toolkit::BubbleEmitter BubbleEmitter::New( const Vector2& winSize,
                                           Texture shapeTexture,
                                           unsigned int maximumNumberOfBubble,
                                           const Vector2& bubbleSizeRange )
{
  // Create the implementation
   IntrusivePtr<BubbleEmitter> internalBubbleEmitter ( new BubbleEmitter( winSize, shapeTexture,
                                                            maximumNumberOfBubble,bubbleSizeRange ) );

  // Pass ownership to Toolkit::BubbleEmitter handle
  Toolkit::BubbleEmitter bubbleEmitter( *internalBubbleEmitter );

  //Second phase of implementeation : Initialization
  internalBubbleEmitter->OnInitialize();

  return bubbleEmitter;
}

void BubbleEmitter::OnInitialize()
{
  // Create the root actor, all the meshActor should be its children
  mBubbleRoot = Actor::New();
  mBubbleRoot.SetSize(mMovementArea);

  // Prepare the frame buffer to store the color adjusted background texture
  Vector2 imageSize = Vector2( mMovementArea.width/4.f, mMovementArea.height/4.f );
  mFrameBuffer = FrameBuffer::New( imageSize.x, imageSize.y, 0 );
  mEffectTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, imageSize.x, imageSize.y );
  mFrameBuffer.AttachColorTexture( mEffectTexture );

  // Generate the geometry, which is used by all bubbleActors
  mMeshGeometry =  CreateGeometry( mNumBubblePerRenderer*mDensity );

  Shader bubbleShader = CreateBubbleShader( mNumBubblePerRenderer );

  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture( 0u, mEffectTexture );
  mTextureSet.SetTexture( 1u, mShapeTexture );

  // Create the renderers to render the bubbles
  mBubbleRenderers.resize( mNumRenderer );
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].Initialize( mNumBubblePerRenderer, mMovementArea, mMeshGeometry, mTextureSet, bubbleShader );
    mBubbleRoot.AddRenderer( mBubbleRenderers[i].GetRenderer() );
  }

  // Create a cameraActor for the off screen render task.
  mCameraActor = CameraActor::New(mMovementArea);
  mCameraActor.SetParentOrigin(ParentOrigin::CENTER);

  Stage stage = Stage::GetCurrent();

  stage.Add(mCameraActor);
  stage.ContextRegainedSignal().Connect(this, &BubbleEmitter::OnContextRegained);
}

Actor BubbleEmitter::GetRootActor()
{
  return mBubbleRoot;
}

void BubbleEmitter::SetBackground( Texture bgTexture, const Vector3& hsvDelta )
{
  mBackgroundTexture = bgTexture;
  mHSVDelta = hsvDelta;

  //Create RenderTask source actor
  Actor sourceActor = Actor::New();
  sourceActor.SetSize( mMovementArea );
  sourceActor.SetParentOrigin(ParentOrigin::CENTER);
  sourceActor.RegisterProperty( "uHSVDelta", hsvDelta );
  Stage::GetCurrent().Add( sourceActor );

  //Create renderer
  Dali::Geometry geometry = CreateTexturedQuad();
  // Shader shader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  Shader shader = DevelShader::New<uint32_t>(
  VERTEX_SHADER,
  FRAGMENT_SHADER,
  DevelShader::ShaderLanguage::SPIRV_1_0, Property::Map() );



  Renderer renderer = Renderer::New( geometry, shader );
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, bgTexture );
  renderer.SetTextures( textureSet );
  sourceActor.AddRenderer( renderer );

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.CreateTask();
  task.SetRefreshRate( RenderTask::REFRESH_ONCE );
  task.SetSourceActor( sourceActor );
  task.SetExclusive(true);
  task.SetCameraActor(mCameraActor);
  task.GetCameraActor().SetInvertYAxis(true);
  task.SetFrameBuffer( mFrameBuffer );
  task.FinishedSignal().Connect(this, &BubbleEmitter::OnRenderFinished);
  mRenderTaskRunning = true;
}

void BubbleEmitter::SetBubbleShape( Texture shapeTexture )
{
  mTextureSet.SetTexture( 1, shapeTexture );
}

void BubbleEmitter::SetBubbleScale( float scale )
{
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].SetDynamicScale( scale );
  }
}

void BubbleEmitter::SetBubbleDensity( unsigned int density )
{
  DALI_ASSERT_ALWAYS( density>0 && density<=9 && " Only densities between 1 to 9 are valid " );

  if( density == mDensity )
  {
    return;
  }
  else
  {
    mDensity = density;
    mMeshGeometry =  CreateGeometry( mNumBubblePerRenderer*mDensity );
    for(unsigned int i=0; i < mNumRenderer; i++ )
    {
      mBubbleRenderers[i].SetGeometry( mMeshGeometry );
    }
  }
}

// clear the resources created for the off screen rendering
void BubbleEmitter::OnRenderFinished(RenderTask& source)
{
  mRenderTaskRunning = false;
  Actor sourceActor = source.GetSourceActor();
  Stage stage = Stage::GetCurrent();
  stage.Remove(sourceActor);
  stage.GetRenderTaskList().RemoveTask(source);
}

void BubbleEmitter::OnContextRegained()
{
  // Context was lost, so the framebuffer has been destroyed. Re-create render task
  // and trigger re-draw if not already running
  if( ! mRenderTaskRunning )
  {
    SetBackground( mBackgroundTexture, mHSVDelta );
  }
}

void BubbleEmitter::EmitBubble( Animation& animation, const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  unsigned int curUniform = mCurrentBubble  % mNumBubblePerRenderer;
  unsigned int groupIdx = mCurrentBubble / mNumBubblePerRenderer;
  SetBubbleParameter( mBubbleRenderers[groupIdx], curUniform, emitPosition - Vector2(mMovementArea.x*0.5f,mMovementArea.y*0.5f), direction, displacement);
  animation.AnimateTo( mBubbleRenderers[groupIdx].GetPercentageProperty(curUniform),
                       1.f, AlphaFunction::LINEAR );

  mCurrentBubble = (mCurrentBubble + 1) % mTotalNumOfBubble;
}

void BubbleEmitter::Restore()
{
  for(unsigned int i=0; i < mNumRenderer; i++ )
  {
    mBubbleRenderers[i].ResetProperties();
  }
}

Geometry BubbleEmitter::CreateGeometry( unsigned int numOfPatch )
{
  unsigned int numVertex = numOfPatch*4u;
  Vector<Vertex> vertexData;
  vertexData.Reserve( numVertex );

  unsigned int numIndex = numOfPatch*6u;
  Vector<unsigned short> indexData;
  indexData.Reserve( numIndex );

  for(unsigned int i = 0; i < numOfPatch; i++)
  {
    float halfSize = RandomRange(mBubbleSizeRange.x, mBubbleSizeRange.y, mRandomSeed) * 0.5f;

    float index = static_cast<float>( i );
    vertexData.PushBack( Vertex( index, Vector2(-halfSize,-halfSize),Vector2(0.f,0.f) ) );
    vertexData.PushBack( Vertex( index, Vector2(-halfSize, halfSize), Vector2(0.f,1.f) ) );
    vertexData.PushBack( Vertex( index, Vector2( halfSize, halfSize),  Vector2(1.f,1.f) ) );
    vertexData.PushBack( Vertex( index, Vector2( halfSize,-halfSize), Vector2(1.f,0.f) ) );

    unsigned short idx = index * 4;
    indexData.PushBack( idx );
    indexData.PushBack( idx+1 );
    indexData.PushBack( idx+2 );
    indexData.PushBack( idx );
    indexData.PushBack( idx+2 );
    indexData.PushBack( idx+3 );
  }

  Property::Map vertexFormat;
  vertexFormat["aIndex"] = Property::FLOAT;
  vertexFormat["aPosition"] = Property::VECTOR2;
  vertexFormat["aTexCoord"] = Property::VECTOR2;
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat );
  vertices.SetData( &vertexData[0], numVertex );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertices );
  geometry.SetIndexBuffer( &indexData[0], numIndex );

  return geometry;
}

void BubbleEmitter::SetBubbleParameter( BubbleRenderer& bubbleRenderer, unsigned int curUniform,
                                        const Vector2& emitPosition, const Vector2& direction, const Vector2& displacement )
{
  Vector2 dir(direction);

  int halfRange = displacement.x / 2;
  // for the y coordinate, always negative, so bubbles always go upwards
  Vector2 randomVec( rand_r( &mRandomSeed ) % static_cast<int>(displacement.x) - halfRange, -rand_r( &mRandomSeed ) % static_cast<int>(displacement.y) );
  dir.Normalize();
  randomVec.x -= dir.x*halfRange;
  randomVec.y *= 1.0f - fabsf(dir.x)*0.33f;

  if(randomVec.y > 0.0f)
  {
    randomVec.y *= 0.33f;
  }
  Vector4 startAndEndPos( emitPosition.x, emitPosition.y, emitPosition.x+randomVec.x, emitPosition.y+randomVec.y );
  bubbleRenderer.SetStartAndEndPosition( curUniform, startAndEndPos );

  bubbleRenderer.SetPercentage( curUniform, 0.f);
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
