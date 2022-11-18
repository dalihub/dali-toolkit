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
in lowp vec4 vColor;
in highp vec3 vPositionToCamera;

out vec4 FragColor;

struct PBRInfo
{
  mediump float NdotL;        // cos angle between normal and light direction
  mediump float NdotV;        // cos angle between normal and view direction
  mediump float NdotH;        // cos angle between normal and half vector
  mediump float VdotH;        // cos angle between view direction and half vector
  mediump vec3 reflectance0;  // full reflectance color (normal incidence angle)
  mediump vec3 reflectance90; // reflectance color at grazing angle
  lowp float alphaRoughness;  // roughness mapped to a more linear change in the roughness (proposed by [2])
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

vec3 specularReflection(PBRInfo pbrInputs)
{
  return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

float geometricOcclusion(PBRInfo pbrInputs)
{
  mediump float NdotL = pbrInputs.NdotL;
  mediump float NdotV = pbrInputs.NdotV;
  lowp float r = pbrInputs.alphaRoughness;

  lowp float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
  lowp float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
  return attenuationL * attenuationV;
}

float microfacetDistribution(PBRInfo pbrInputs)
{
  mediump float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
  lowp float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
  return roughnessSq / (M_PI * f * f);
}

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
  lowp vec4 baseColor = vColor * uColorFactor;
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
  lowp vec4 baseColor = vec4(linear(albedoMetal.rgb), 1.0) * vColor * uColorFactor;

  metallic = albedoMetal.METALLIC * metallic;

  vec4 normalRoughness = texture(sNormalRoughness, vUV);
  perceptualRoughness = normalRoughness.ROUGHNESS * perceptualRoughness;

  n = normalRoughness.rgb;
  n = normalize(vTBN * ((2.0 * n - 1.0) * vec3(uNormalScale, uNormalScale, 1.0)));
#endif // THREE_TEX

  // The value of uOpaque and uMask can be 0.0 or 1.0.
  // If uOpaque is 1.0, alpha value of final color is 1.0;
  // If uOpaque is 0.0 and uMask is 1.0, alpha value of final color is 0.0 when input alpha is lower than uAlphaThreshold or
  // 1.0 when input alpha is larger than uAlphaThreshold.
  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#_material_alphamode
  baseColor.a = mix(baseColor.a, 1.0, uOpaque);
  baseColor.a = min(mix(baseColor.a, floor(baseColor.a - uAlphaThreshold + 1.0), uMask), 1.0);

  metallic = clamp(metallic, 0.0, 1.0);
  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness [2].
  perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
  lowp float alphaRoughness = perceptualRoughness * perceptualRoughness;

  lowp vec3 f0 = vec3(0.04);
  lowp vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
  diffuseColor *= (1.0 - metallic);
  lowp vec3 specularColor = mix(f0, baseColor.rgb, metallic);

  // Compute reflectance.
  lowp float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

  // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
  // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
  lowp float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
  lowp vec3 specularEnvironmentR0 = specularColor.rgb;
  lowp vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

  mediump vec3 v = normalize(vPositionToCamera); // Vector from surface point to camera
  mediump float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
  mediump vec3 reflection = -normalize(reflect(v, n));

  lowp vec3 color = vec3(0.0);
  lowp vec3 diffuseLight = linear(texture(sDiffuseEnvSampler, n * uYDirection).rgb);
  lowp vec3 specularLight = linear(texture(sSpecularEnvSampler, reflection * uYDirection).rgb);
  // retrieve a scale and bias to F0. See [1], Figure 3
  lowp vec3 brdf = linear(texture(sbrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness)).rgb);

  lowp vec3 diffuse = diffuseLight * diffuseColor;
  lowp vec3 specular = specularLight * (specularColor * brdf.x + brdf.y);
  color += (diffuse + specular) * uIblIntensity;

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
