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
  "in mediump vec2 aTexCoord0;\n"
  "in mediump vec2 aTexCoord1;\n"
  "in lowp vec3 aNormal;\n"
  "in lowp vec4 aTangent;\n"
  "in lowp vec4 aVertexColor;\n"

  "uniform mediump vec3 uSize;\n"
  "uniform mediump mat4 uModelMatrix;\n"
  "uniform mediump mat4 uViewMatrix;\n"
  "uniform mediump mat4 uProjection;\n"
  "uniform lowp int uLightType;\n"
  "uniform mediump vec3 uLightVector;\n"
  "uniform lowp int uIsColor;\n"

  "out lowp vec2 vUV[2];\n"
  "out lowp mat3 vTBN;\n"
  "out lowp vec4 vColor;\n"
  "flat out int visLight;\n"
  "out highp vec3 vLightDirection;\n"
  "out highp vec3 vPositionToCamera;\n"

  "void main()\n"
  "{\n"
  "  highp vec4 invY = vec4(1.0, -1.0, 1.0, 1.0);\n"
  "  highp vec4 positionW = uModelMatrix * vec4( aPosition * uSize, 1.0 );\n"
  "  highp vec4 positionV = uViewMatrix * ( invY * positionW );\n"

  "  vPositionToCamera = transpose( mat3( uViewMatrix ) ) * ( -vec3( positionV.xyz / positionV.w ) );\n"
  "  vPositionToCamera *= invY.xyz;\n"

  "  lowp vec3 bitangent = cross(aNormal, aTangent.xyz) * aTangent.w;\n"
  "  vTBN = mat3( uModelMatrix ) * mat3(aTangent.xyz, bitangent, aNormal);\n"

  "  vUV[0] = aTexCoord0;\n"
  "  vUV[1] = aTexCoord1;\n"

  "  visLight = 1;\n"
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
  "uniform lowp vec3 uLightColor;\n"
  "uniform lowp vec4 uBaseColorFactor;\n"
  "uniform lowp vec2 uMetallicRoughnessFactors;\n"
  "uniform lowp int alphaMode;\n"
  "uniform lowp float alphaCutoff;\n"

  "in lowp vec2 vUV[2];\n"
  "in lowp mat3 vTBN;\n"
  "in lowp vec4 vColor;\n"
  "flat in int visLight;\n"
  "in highp vec3 vLightDirection;\n"
  "in highp vec3 vPositionToCamera;\n"

  "out vec4 FragColor;"

  "struct PBRInfo\n"
  "{\n"
  "  mediump float NdotL;\n"                  // cos angle between normal and light direction
  "  mediump float NdotV;\n"                  // cos angle between normal and view direction
  "  mediump float NdotH;\n"                  // cos angle between normal and half vector
  "  mediump float VdotH;\n"                  // cos angle between view direction and half vector
  "  mediump vec3 reflectance0;\n"            // full reflectance color (normal incidence angle)
  "  mediump vec3 reflectance90;\n"           // reflectance color at grazing angle
  "  lowp float alphaRoughness;\n"         // roughness mapped to a more linear change in the roughness (proposed by [2])
  "};\n"

  "const float M_PI = 3.141592653589793;\n"
  "const float c_MinRoughness = 0.04;\n"

  "vec3 getNormal()\n"
  "{\n"
  "#ifdef TEXTURE_NORMAL\n"
  "  lowp vec3 n = texture( uNormalSampler, vUV[uNormalTexCoordIndex] ).rgb;\n"
  "  n = normalize( vTBN * ( ( 2.0 * n - 1.0 ) * vec3( uNormalScale, uNormalScale, 1.0 ) ) );\n"
  "#else\n"
  "  lowp vec3 n = normalize( vTBN[2].xyz );\n"
  "#endif\n"
  "  return n;\n"
  "}\n"

  "vec3 specularReflection( PBRInfo pbrInputs )\n"
  "{\n"
  "  return pbrInputs.reflectance0 + ( pbrInputs.reflectance90 - pbrInputs.reflectance0 ) * pow( clamp( 1.0 - pbrInputs.VdotH, 0.0, 1.0 ), 5.0 );\n"
  "}\n"

  "float geometricOcclusion( PBRInfo pbrInputs )\n"
  "{\n"
  "  mediump float NdotL = pbrInputs.NdotL;\n"
  "  mediump float NdotV = pbrInputs.NdotV;\n"
  "  lowp float r = pbrInputs.alphaRoughness;\n"

  "  lowp float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));\n"
  "  lowp float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));\n"
  "  return attenuationL * attenuationV;\n"
  "}\n"

  "float microfacetDistribution(PBRInfo pbrInputs)\n"
  "{\n"
  "  mediump float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;\n"
  "  lowp float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;\n"
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
  "  lowp float metallic = uMetallicRoughnessFactors.x;\n"
  "  lowp float perceptualRoughness = uMetallicRoughnessFactors.y;\n"

  // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
  // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
  "#ifdef TEXTURE_METALLICROUGHNESS\n"
  "  lowp vec4 metrou = texture(uMetallicRoughnessSampler, vUV[uMetallicRoughnessTexCoordIndex]);\n"
  "  metallic = metrou.b * metallic;\n"
  "  perceptualRoughness = metrou.g * perceptualRoughness;\n"
  "#endif\n"

  "  metallic = clamp(metallic, 0.0, 1.0);\n"
  "  perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);\n"
  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness [2].
  "  lowp float alphaRoughness = perceptualRoughness * perceptualRoughness;\n"

  "#ifdef TEXTURE_BASECOLOR\n"
  // The albedo may be defined from a base texture or a flat color
  "  lowp vec4 baseColor = texture(uBaseColorSampler, vUV[uBaseColorTexCoordIndex]) * uBaseColorFactor;\n"
  "  baseColor = vec4(linear(baseColor.rgb), baseColor.w);\n"
  "#else\n"
  "  lowp vec4 baseColor = vColor * uBaseColorFactor;\n"
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

  "  lowp vec3 f0 = vec3(0.04);\n"
  "  lowp vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);\n"
  "  diffuseColor *= ( 1.0 - metallic );\n"
  "  lowp vec3 specularColor = mix(f0, baseColor.rgb, metallic);\n"

  // Compute reflectance.
  "  lowp float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);\n"

  // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
  // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
  "  lowp float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);\n"
  "  lowp vec3 specularEnvironmentR0 = specularColor.rgb;\n"
  "  lowp vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;\n"

  "  mediump vec3 n = getNormal();\n"                            // normal at surface point
  "  mediump vec3 v = normalize(vPositionToCamera);\n"           // Vector from surface point to camera
  "  mediump vec3 l = normalize(vLightDirection);\n"             // Vector from light to surface point
  "  mediump vec3 h = normalize(l+v);\n"                         // Half vector between both l and v
  "  mediump vec3 reflection = -normalize(reflect(v, n));\n"

  "  mediump float NdotL = clamp(dot(n, l), 0.001, 1.0);\n"
  "  mediump float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);\n"
  "  mediump float NdotH = dot(n, h);\n"
  "  mediump float LdotH = dot(l, h);\n"
  "  mediump float VdotH = dot(v, h);\n"

  "  PBRInfo pbrInputs = PBRInfo(\n"
  "    NdotL,\n"
  "    NdotV,\n"
  "    NdotH,\n"
  "    VdotH,\n"
  "    specularEnvironmentR0,\n"
  "    specularEnvironmentR90,\n"
  "    alphaRoughness\n"
  "  );\n"

  // Calculate the shading terms for the microfacet specular shading model
  "  lowp vec3 color = vec3(0.0);\n"
  "  if( visLight == 1 )\n"
  "  {\n"
  "    lowp vec3 F = specularReflection( pbrInputs );\n"
  "    lowp float G = geometricOcclusion( pbrInputs );\n"
  "    lowp float D = microfacetDistribution( pbrInputs );\n"

  // Calculation of analytical lighting contribution
  "    lowp vec3 diffuseContrib = ( 1.0 - F ) * ( diffuseColor / M_PI );\n"
  "    lowp vec3 specContrib = F * G * D / ( 4.0 * NdotL * NdotV );\n"
  // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
  "    color = NdotL * uLightColor * (diffuseContrib + specContrib);\n"
  "  }\n"

  "#ifdef TEXTURE_IBL\n"
  "  lowp float lod = ( perceptualRoughness * uMipmapLevel );\n"
  // retrieve a scale and bias to F0. See [1], Figure 3
  "  lowp vec3 brdf = linear( texture( ubrdfLUT, vec2( NdotV, 1.0 - perceptualRoughness ) ).rgb );\n"
  "  lowp vec3 diffuseLight = linear( texture( uDiffuseEnvSampler, n ).rgb );\n"
  "  lowp vec3 specularLight = linear( textureLod( uSpecularEnvSampler, reflection, lod ).rgb );\n"

  "  lowp vec3 diffuse = diffuseLight * diffuseColor * uScaleIBLAmbient.x;\n"
  "  lowp vec3 specular = specularLight * ( specularColor * brdf.x + brdf.y ) * uScaleIBLAmbient.y;\n"
  "  color += ( diffuse + specular );\n"
  "#endif\n"

  "#ifdef TEXTURE_OCCLUSION\n"
  "  lowp float ao = texture( uOcclusionSampler, vUV[uOcclusionTexCoordIndex] ).r;\n"
  "  color = mix( color, color * ao, uOcclusionStrength );\n"
  "#endif\n"

  "#ifdef TEXTURE_EMIT\n"
  "  lowp vec3 emissive = linear( texture( uEmissiveSampler, vUV[uEmissiveTexCoordIndex] ).rgb ) * uEmissiveFactor;\n"
  "  color += emissive;\n"
  "#endif\n"

  "  FragColor = vec4( pow( color,vec3( 1.0 / 2.2 ) ), baseColor.a );\n"
  "}\n"
};

} // namespace internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GLTF_SHADER_H