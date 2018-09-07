#ifndef DALI_TOOLKIT_INTERNAL_GLTF_SHADER_H
#define DALI_TOOLKIT_INTERNAL_GLTF_SHADER_H

/*
 * Belows Vertex Shader and Fragment Shader code are based off glTF WebGL PBR.
 * https://github.com/KhronosGroup/glTF-WebGL-PBR/
 *
 * Copyright (c) 2016-2017 Mohamad Moneimne and Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

const char* GLES_VERSION_300 = {
  "#version 300 es\n\n"
  "precision highp float;\n\n"
};

const char* DEFINE_BASECOLOR_TEXTURE = {
  "#define TEXTURE_BASECOLOR\n\n"
  "uniform sampler2D uBaseColorSampler;\n"
  "uniform int uBaseColorTexCoordIndex;\n\n"
};

const char* DEFINE_METALLICROUGHNESS_TEXTURE = {
  "#define TEXTURE_METALLICROUGHNESS\n\n"
  "uniform sampler2D uMetallicRoughnessSampler;\n"
  "uniform int uMetallicRoughnessTexCoordIndex;\n\n"
};

const char* DEFINE_NORMAL_TEXTURE = {
  "#define TEXTURE_NORMAL\n\n"
  "uniform sampler2D uNormalSampler;\n"
  "uniform float uNormalScale;\n"
  "uniform int uNormalTexCoordIndex;\n\n"
};

const char* DEFINE_OCCLUSION_TEXTURE = {
  "#define TEXTURE_OCCLUSION\n\n"
  "uniform sampler2D uOcclusionSampler;\n"
  "uniform int uOcclusionTexCoordIndex;\n"
  "uniform float uOcclusionStrength;\n\n"
};

const char* DEFINE_EMIT_TEXTURE = {
  "#define TEXTURE_EMIT\n\n"
  "uniform sampler2D uEmissiveSampler;\n"
  "uniform int uEmissiveTexCoordIndex;\n"
  "uniform vec3 uEmissiveFactor;\n\n"
};

const char* DEFINE_IBL_TEXTURE = {
  "#define TEXTURE_IBL\n\n"
  "uniform sampler2D ubrdfLUT;\n"
  "uniform samplerCube uDiffuseEnvSampler;\n"
  "uniform samplerCube uSpecularEnvSampler;\n"
  "uniform vec4 uScaleIBLAmbient;\n"
  "uniform highp float uMipmapLevel;\n"
};

const char* PHYSICALLY_BASED_VERTEX_SHADER = {
  "in highp vec3 aPosition;\n"
  "in highp vec2 aTexCoord0;\n"
  "in highp vec2 aTexCoord1;\n"
  "in highp vec3 aNormal;\n"
  "in highp vec4 aTangent;\n"
  "in highp vec4 aVertexColor;\n"

  "uniform mediump vec3 uSize;\n"
  "uniform mediump mat4 uModelMatrix;\n"
  "uniform mediump mat4 uViewMatrix;\n"
  "uniform mediump mat4 uProjection;\n"
  "uniform mediump mat4 uMvpMatrix;\n"
  "uniform mediump mat3 uNormalMatrix;\n"
  "uniform mediump int uLightType;\n"
  "uniform mediump vec3 uLightVector;\n"
  "uniform mediump int uIsColor;\n"

  "out vec2 vUV[2];\n"
  "out mat3 vTBN;\n"
  "out vec4 vColor;\n"
  "flat out int visLight;\n"
  "out vec3 vLightDirection;\n"
  "out vec3 vPositionToCamera;\n"

  "void main()\n"
  "{\n"
  "  vec4 invY = vec4(1.0, -1.0, 1.0, 1.0);\n"
  "  vec4 positionW = uModelMatrix * vec4( aPosition * uSize, 1.0 );\n"
  "  vec4 positionV = uViewMatrix * ( invY * positionW );\n"

  "  vPositionToCamera = transpose( mat3( uViewMatrix ) ) * ( -vec3( positionV.xyz / positionV.w ) );\n"
  "  vPositionToCamera *= vec3( invY );\n"

  "  vec3 bitangent = cross(aNormal, aTangent.xyz) * aTangent.w;\n"
  "  vTBN = mat3( uModelMatrix ) * mat3(aTangent.xyz, bitangent, aNormal);\n"

  "  vUV[0] = aTexCoord0;\n"
  "  vUV[1] = aTexCoord1;\n"

  "  visLight = 0;\n"
  "  if( uLightType == 1 )\n"
  "  {\n"
  "    vLightDirection = ( invY.xyz * uLightVector ) - ( positionW.xyz / positionW.w );\n"
  "  }\n"
  "  else if( uLightType == 2 )\n"
  "  {\n"
  "    vLightDirection = -( invY.xyz * uLightVector );\n"
  "  }\n"
  "  else\n"
  "  {\n"
  "    visLight = 0;\n"
  "  }\n"

  "  vColor = vec4( 1.0 );\n"
  "  if( uIsColor == 1 )\n"
  "  {\n"
  "    vColor = aVertexColor;\n"
  "  }\n"

  "  gl_Position = uProjection * positionV;\n" // needs w for proper perspective correction
  "  gl_Position = gl_Position/gl_Position.w;\n"
  "}\n"
};

const char* PHYSICALLY_BASED_FRAGMENT_SHADER = {
  "uniform vec3 uLightColor;\n"
  "uniform vec4 uBaseColorFactor;\n"
  "uniform vec2 uMetallicRoughnessFactors;\n"
  "uniform int alphaMode;\n"
  "uniform float alphaCutoff;\n"

  "in vec2 vUV[2];\n"
  "in mat3 vTBN;\n"
  "in vec4 vColor;\n"
  "flat in int visLight;\n"
  "in vec3 vLightDirection;\n"
  "in vec3 vPositionToCamera;\n"

  "out vec4 FragColor;"

  "struct PBRInfo\n"
  "{\n"
  "  float NdotL;\n"                  // cos angle between normal and light direction
  "  float NdotV;\n"                  // cos angle between normal and view direction
  "  float NdotH;\n"                  // cos angle between normal and half vector
  "  float LdotH;\n"                  // cos angle between light direction and half vector
  "  float VdotH;\n"                  // cos angle between view direction and half vector
  "  float perceptualRoughness;\n"    // roughness value, as authored by the model creator (input to shader)
  "  float metalness;\n"              // metallic value at the surface
  "  vec3 reflectance0;\n"            // full reflectance color (normal incidence angle)
  "  vec3 reflectance90;\n"           // reflectance color at grazing angle
  "  float alphaRoughness;\n"         // roughness mapped to a more linear change in the roughness (proposed by [2])
  "  vec3 diffuseColor;\n"            // color contribution from diffuse lighting
  "  vec3 specularColor;\n"           // color contribution from specular lighting
  "};\n"

  "const float M_PI = 3.141592653589793;\n"
  "const float c_MinRoughness = 0.04;\n"

  "vec3 getNormal()\n"
  "{\n"
  "#ifdef TEXTURE_NORMAL\n"
  "  vec3 n = texture( uNormalSampler, vUV[uNormalTexCoordIndex] ).rgb;\n"
  "  n = normalize( vTBN * ( ( 2.0 * n - 1.0 ) * vec3( uNormalScale, uNormalScale, 1.0 ) ) );\n"
  "#else\n"
  "  vec3 n = normalize( vTBN[2].xyz );\n"
  "#endif\n"
  "  return n;\n"
  "}\n"

  "vec3 specularReflection( PBRInfo pbrInputs )\n"
  "{\n"
  "  return pbrInputs.reflectance0 + ( pbrInputs.reflectance90 - pbrInputs.reflectance0 ) * pow( clamp( 1.0 - pbrInputs.VdotH, 0.0, 1.0 ), 5.0 );\n"
  "}\n"

  "float geometricOcclusion( PBRInfo pbrInputs )\n"
  "{\n"
  "  float NdotL = pbrInputs.NdotL;\n"
  "  float NdotV = pbrInputs.NdotV;\n"
  "  float r = pbrInputs.alphaRoughness;\n"

  "  float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));\n"
  "  float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));\n"
  "  return attenuationL * attenuationV;\n"
  "}\n"

  "float microfacetDistribution(PBRInfo pbrInputs)\n"
  "{\n"
  "  float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;\n"
  "  float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;\n"
  "  return roughnessSq / (M_PI * f * f);\n"
  "}\n"

  "vec3 linear( vec3 color )\n"
  "{\n"
  "  return pow(color,vec3(2.2));\n"
  "}\n"

  "void main()\n"
  "{\n"
  // Metallic and Roughness material properties are packed together
  // In glTF, these factors can be specified by fixed scalar values
  // or from a metallic-roughness map
  "  float metallic = uMetallicRoughnessFactors.x;\n"
  "  float perceptualRoughness = uMetallicRoughnessFactors.y;\n"

  // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
  // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
  "#ifdef TEXTURE_METALLICROUGHNESS\n"
  "  vec4 metrou = texture(uMetallicRoughnessSampler, vUV[uMetallicRoughnessTexCoordIndex]);\n"
  "  metallic = metrou.b * metallic;\n"
  "  perceptualRoughness = metrou.g * perceptualRoughness;\n"
  "#endif\n"

  "  metallic = clamp(metallic, 0.0, 1.0);\n"
  "  perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);\n"
  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness [2].
  "  float alphaRoughness = perceptualRoughness * perceptualRoughness;\n"

  "#ifdef TEXTURE_BASECOLOR\n"
  // The albedo may be defined from a base texture or a flat color
  "  vec4 baseColor = texture(uBaseColorSampler, vUV[uBaseColorTexCoordIndex]) * uBaseColorFactor;\n"
  "  baseColor = vec4(linear(baseColor.rgb), baseColor.w);\n"
  "#else\n"
  "  vec4 baseColor = vColor * uBaseColorFactor;\n"
  "#endif\n"

  "  if( alphaMode == 0 )\n"
  "  {\n"
  "    baseColor.w = 1.0;\n"
  "  }\n"
  "  else if( alphaMode == 1 )\n"
  "  {\n"
  "    if( baseColor.w >= alphaCutoff )"
  "    {\n"
  "      baseColor.w = 1.0;\n"
  "    }\n"
  "    else\n"
  "    {\n"
  "      baseColor.w = 0.0;\n"
  "    }\n"
  "  }\n"

  "  vec3 f0 = vec3(0.04);\n"
  "  vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);\n"
  "  diffuseColor *= ( 1.0 - metallic );\n"
  "  vec3 specularColor = mix(f0, baseColor.rgb, metallic);\n"

  // Compute reflectance.
  "  float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);\n"

  // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
  // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
  "  float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);\n"
  "  vec3 specularEnvironmentR0 = specularColor.rgb;\n"
  "  vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;\n"

  "  vec3 n = getNormal();\n"                            // normal at surface point
  "  vec3 v = normalize(vPositionToCamera);\n"           // Vector from surface point to camera
  "  vec3 l = normalize(vLightDirection);\n"             // Vector from light to surface point
  "  vec3 h = normalize(l+v);\n"                         // Half vector between both l and v
  "  vec3 reflection = -normalize(reflect(v, n));\n"

  "  float NdotL = clamp(dot(n, l), 0.001, 1.0);\n"
  "  float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);\n"
  "  float NdotH = clamp(dot(n, h), 0.0, 1.0);\n"
  "  float LdotH = clamp(dot(l, h), 0.0, 1.0);\n"
  "  float VdotH = clamp(dot(v, h), 0.0, 1.0);\n"

  "  PBRInfo pbrInputs = PBRInfo(\n"
  "    NdotL,\n"
  "    NdotV,\n"
  "    NdotH,\n"
  "    LdotH,\n"
  "    VdotH,\n"
  "    perceptualRoughness,\n"
  "    metallic,\n"
  "    specularEnvironmentR0,\n"
  "    specularEnvironmentR90,\n"
  "    alphaRoughness,\n"
  "    diffuseColor,\n"
  "    specularColor\n"
  "  );\n"

  // Calculate the shading terms for the microfacet specular shading model
  "  vec3 color = vec3(0.0);\n"
  "  if( visLight == 1 )\n"
  "  {\n"
  "    vec3 F = specularReflection(pbrInputs);\n"
  "    float G = geometricOcclusion(pbrInputs);\n"
  "    float D = microfacetDistribution(pbrInputs);\n"

  // Calculation of analytical lighting contribution
  "    vec3 diffuseContrib = (1.0 - F) * ( pbrInputs.diffuseColor / M_PI );\n"
  "    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);\n"
  // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
  "    color = NdotL * uLightColor * (diffuseContrib + specContrib);\n"
  "  }\n"

  "#ifdef TEXTURE_IBL\n"
  "  float lod = (pbrInputs.perceptualRoughness * uMipmapLevel);\n"
  // retrieve a scale and bias to F0. See [1], Figure 3
  "  vec3 brdf = linear(texture(ubrdfLUT, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness)).rgb);\n"
  "  vec3 diffuseLight = linear(texture(uDiffuseEnvSampler, n).rgb);\n"
  "  vec3 specularLight = linear(textureLod(uSpecularEnvSampler, reflection, lod).rgb);\n"

  "  vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;\n"
  "  vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);\n"
  "  diffuse *= uScaleIBLAmbient.x;\n"
  "  specular *= uScaleIBLAmbient.y;\n"
  "  color += (diffuse+specular);\n"
  "#endif\n"

  "#ifdef TEXTURE_OCCLUSION\n"
  "  float ao = texture(uOcclusionSampler, vUV[uOcclusionTexCoordIndex]).r;\n"
  "  color = mix(color, color * ao, uOcclusionStrength);\n"
  "#endif\n"

  "#ifdef TEXTURE_EMIT\n"
  "  vec3 emissive = linear(texture(uEmissiveSampler, vUV[uEmissiveTexCoordIndex]).rgb) * uEmissiveFactor;\n"
  "  color += emissive;\n"
  "#endif\n"

  "  FragColor = vec4(pow(color,vec3(1.0/2.2)), baseColor.a);\n"
  "}\n"
};

} // namespace internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GLTF_SHADER_H