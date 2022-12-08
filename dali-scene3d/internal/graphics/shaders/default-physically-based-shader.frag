#version 300 es

// Original Code
// https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-frag.glsl
// Commit dc84b5e374fb3d23153d2248a338ef88173f9eb6
//
// This fragment shader defines a reference implementation for Physically Based Shading of
// a microfacet surface material defined by a glTF model.For the DamagedHelmet.gltf and its Assets
//
// References:
// [1] Real Shading in Unreal Engine 4
//     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [2] Physically Based Shading at Disney
//     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [3] README.md - Environment Maps
//     https://github.com/KhronosGroup/glTF-Sample-Viewer/#environment-maps
// [4] \"An Inexpensive BRDF Model for Physically based Rendering\" by Christophe Schlick
//     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf

#ifdef HIGHP
precision highp float;
#else
precision mediump float;
#endif

#ifdef THREE_TEX
#ifdef GLTF_CHANNELS
#define METALLIC b
#define ROUGHNESS g
#else //GLTF_CHANNELS
#define METALLIC r
#define ROUGHNESS a
#endif //GLTF_CHANNELS
#endif //THREE_TEX

uniform lowp vec4 uColor; // Color from SceneGraph
uniform lowp vec4 uColorFactor; // Color from material
uniform lowp float uMetallicFactor;
uniform lowp float uRoughnessFactor;
uniform lowp float uDielectricSpecular;

#ifdef THREE_TEX
#ifdef BASECOLOR_TEX
uniform sampler2D sAlbedoAlpha;
#endif // BASECOLOR_TEX
#ifdef METALLIC_ROUGHNESS_TEX
uniform sampler2D sMetalRoughness;
#endif // METALLIC_ROUGHNESS_TEX
#ifdef NORMAL_TEX
uniform sampler2D sNormal;
uniform float uNormalScale;
#endif // NORMAL_TEX
#else // THREE_TEX
uniform sampler2D sAlbedoMetal;
uniform sampler2D sNormalRoughness;
#endif

#ifdef OCCLUSION
uniform sampler2D sOcclusion;
uniform float uOcclusionStrength;
#endif

#ifdef EMISSIVE
uniform sampler2D sEmissive;
uniform vec3 uEmissiveFactor;
#endif

uniform float uSpecularFactor;
uniform vec3  uSpecularColorFactor;
#ifdef MATERIAL_SPECULAR_TEXTURE
uniform sampler2D sSpecular;
#endif
#ifdef MATERIAL_SPECULAR_COLOR_TEXTURE
uniform sampler2D sSpecularColor;
#endif

//// For IBL
uniform sampler2D sbrdfLUT;
uniform samplerCube sDiffuseEnvSampler;
uniform samplerCube sSpecularEnvSampler;
uniform float uIblIntensity;
uniform vec3 uYDirection;

// For Alpha Mode.
uniform lowp float uOpaque;
uniform lowp float uMask;
uniform lowp float uAlphaThreshold;

// TODO: Multiple texture coordinate will be supported.
in lowp vec2 vUV;
in lowp mat3 vTBN;
#ifdef COLOR_ATTRIBUTE
in lowp vec4 vColor;
#endif
in highp vec3 vPositionToCamera;

out vec4 FragColor;

const float c_MinRoughness = 0.04;

vec3 linear(vec3 color)
{
  return pow(color, vec3(2.2));
}

void main()
{
  // Metallic and Roughness material properties are packed together
  // In glTF, these factors can be specified by fixed scalar values
  // or from a metallic-roughness map
  // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
  // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
  lowp float metallic = uMetallicFactor;
  lowp float perceptualRoughness = uRoughnessFactor;
  // If there isn't normal texture, use surface normal
  mediump vec3 n = normalize(vTBN[2].xyz);

#ifdef THREE_TEX
  // The albedo may be defined from a base texture or a flat color
#ifdef BASECOLOR_TEX
  lowp vec4 baseColor = texture(sAlbedoAlpha, vUV);
  baseColor = vec4(linear(baseColor.rgb), baseColor.w) * uColorFactor;
#else // BASECOLOR_TEX
#ifdef COLOR_ATTRIBUTE
  lowp vec4 baseColor = vColor * uColorFactor;
#else // COLOR_ATTRIBUTE
  lowp vec4 baseColor = uColorFactor;
#endif // COLOR_ATTRIBUTE
#endif // BASECOLOR_TEX

#ifdef METALLIC_ROUGHNESS_TEX
  lowp vec4 metrou = texture(sMetalRoughness, vUV);
  metallic = metrou.METALLIC * metallic;
  perceptualRoughness = metrou.ROUGHNESS * perceptualRoughness;
#endif // METALLIC_ROUGHNESS_TEX

#ifdef NORMAL_TEX
  n = texture(sNormal, vUV).rgb;
  n = normalize(vTBN * ((2.0 * n - 1.0) * vec3(uNormalScale, uNormalScale, 1.0)));
#endif // NORMAL_TEX
#else // THREE_TEX
  vec4 albedoMetal = texture(sAlbedoMetal, vUV);
#ifdef COLOR_ATTRIBUTE
  lowp vec4 baseColor = vec4(linear(albedoMetal.rgb), 1.0) * vColor * uColorFactor;
#else // COLOR_ATTRIBUTE
  lowp vec4 baseColor = vec4(linear(albedoMetal.rgb), 1.0) * uColorFactor;
#endif // COLOR_ATTRIBUTE

  metallic = albedoMetal.METALLIC * metallic;

  vec4 normalRoughness = texture(sNormalRoughness, vUV);
  perceptualRoughness = normalRoughness.ROUGHNESS * perceptualRoughness;

  n = normalRoughness.rgb;
  n = normalize(vTBN * ((2.0 * n - 1.0) * vec3(uNormalScale, uNormalScale, 1.0)));
#endif // THREE_TEX

  // The value of uOpaque and uMask can be 0.0 or 1.0.
  // If uMask is 0.0, a Pixel that has bigger alpha than uAlphaThreashold becomes fully opaque,
  // in other hand, a pixel that has smaller alpha than uAlphaThreashold becomes fully transparent.
  // If uOpaque is 1.0, alpha value of final color is 1.0;
  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#_material_alphamode
  if(uMask > 0.5 && baseColor.a < uAlphaThreshold)
  {
    discard;
  }
  baseColor.a = mix(baseColor.a, 1.0, uOpaque);

  metallic = clamp(metallic, 0.0, 1.0);
  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness [2].
  perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);

  // Material ior
  lowp vec3 f0 = vec3(uDielectricSpecular);

  // Material Specular
  float specularWeight = 1.0;
  vec4 materialSpecularTexture = vec4(1.0);
#ifdef MATERIAL_SPECULAR_TEXTURE
  materialSpecularTexture.a = texture(sSpecular, vUV).a;
#endif
#ifdef MATERIAL_SPECULAR_COLOR_TEXTURE
  materialSpecularTexture.rgb = texture(sSpecularColor, vUV).rgb;
#endif
  specularWeight = uSpecularFactor * materialSpecularTexture.a;
  f0 = min(f0 * uSpecularColorFactor * materialSpecularTexture.rgb, vec3(1.0));
  f0 = mix(f0, baseColor.rgb, metallic);

  mediump vec3 v = normalize(vPositionToCamera); // Vector from surface point to camera
  mediump float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
  mediump vec3 reflection = -normalize(reflect(v, n));
  lowp vec3 brdf = linear(texture(sbrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness)).rgb);
  vec3 Fr = max(vec3(1.0 - perceptualRoughness), f0) - f0;
  vec3 k_S = f0 + Fr * pow(1.0 - NdotV, 5.0);
  vec3 FssEss = specularWeight * (k_S * brdf.x + brdf.y);

  // Specular Light
  lowp vec3 specularLight = linear(texture(sSpecularEnvSampler, reflection * uYDirection).rgb);
  lowp vec3 specular = specularLight * FssEss;

  // Diffuse Light
  lowp vec3 diffuseColor = mix(baseColor.rgb, vec3(0), metallic);
  lowp vec3 irradiance = linear(texture(sDiffuseEnvSampler, n * uYDirection).rgb);
  float Ems = (1.0 - (brdf.x + brdf.y));
  vec3 F_avg = specularWeight * (f0 + (1.0 - f0) / 21.0);
  vec3 FmsEms = Ems * FssEss * F_avg / (1.0 - F_avg * Ems);
  vec3 k_D = diffuseColor * (1.0 - FssEss + FmsEms);
  lowp vec3 diffuse = (FmsEms + k_D) * irradiance;

  lowp vec3 color = (diffuse + specular) * uIblIntensity;

#ifdef OCCLUSION
  lowp float ao = texture(sOcclusion, vUV).r;
  color = mix(color, color * ao, uOcclusionStrength);
#endif // OCCLUSION

#ifdef EMISSIVE
  lowp vec3 emissive = linear(texture(sEmissive, vUV).rgb) * uEmissiveFactor;
  color += emissive;
#endif // EMISSIVE

  FragColor = vec4(pow(color, vec3(1.0 / 2.2)), baseColor.a) * uColor;
}
