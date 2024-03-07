
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

#ifdef GLTF_CHANNELS
#define METALLIC b
#define ROUGHNESS g
#else //GLTF_CHANNELS
#define METALLIC r
#define ROUGHNESS a
#endif //GLTF_CHANNELS

uniform lowp vec4 uColor; // Color from SceneGraph
uniform lowp vec4 uColorFactor; // Color from material
uniform lowp float uMetallicFactor;
uniform lowp float uRoughnessFactor;
uniform lowp float uDielectricSpecular;

#ifdef THREE_TEX
#ifdef BASECOLOR_TEX
uniform sampler2D sAlbedoAlpha;
uniform float uBaseColorTextureTransformAvailable;
uniform mat3 uBaseColorTextureTransform;
#endif // BASECOLOR_TEX
#ifdef METALLIC_ROUGHNESS_TEX
uniform sampler2D sMetalRoughness;
uniform float uMetalRoughnessTextureTransformAvailable;
uniform mat3 uMetalRoughnessTextureTransform;
#endif // METALLIC_ROUGHNESS_TEX
#ifdef NORMAL_TEX
uniform sampler2D sNormal;
uniform float uNormalTextureTransformAvailable;
uniform mat3 uNormalTextureTransform;
uniform float uNormalScale;
#endif // NORMAL_TEX
#else // THREE_TEX
uniform sampler2D sAlbedoMetal;
uniform float uBaseColorTextureTransformAvailable;
uniform mat3 uBaseColorTextureTransform;
uniform sampler2D sNormalRoughness;
uniform float uNormalRoughnessTextureTransformAvailable;
uniform mat3 uNormalRoughnessTextureTransform;
#endif



#ifdef OCCLUSION
uniform sampler2D sOcclusion;
uniform float uOcclusionTextureTransformAvailable;
uniform mat3 uOcclusionTextureTransform;
uniform float uOcclusionStrength;
#endif

#ifdef EMISSIVE_TEXTURE
uniform sampler2D sEmissive;
uniform float uEmissiveTextureTransformAvailable;
uniform mat3 uEmissiveTextureTransform;
#endif
uniform vec3 uEmissiveFactor;

uniform float uSpecularFactor;
uniform vec3  uSpecularColorFactor;
#ifdef MATERIAL_SPECULAR_TEXTURE
uniform sampler2D sSpecular;
#endif
#ifdef MATERIAL_SPECULAR_COLOR_TEXTURE
uniform sampler2D sSpecularColor;
#endif

// For Light (Currently Directional Only)
#define MAX_LIGHTS 5
uniform mediump int uLightCount;
uniform mediump vec3 uLightDirection[MAX_LIGHTS];
uniform mediump vec3 uLightColor[MAX_LIGHTS];

// For Shadow Map
uniform lowp int uIsShadowEnabled;
uniform sampler2D sShadowMap;
#ifdef SL_VERSION_LOW
uniform int uShadowMapWidth;
uniform int uShadowMapHeight;
#endif
INPUT highp vec3 positionFromLightView;

//// For IBL
uniform sampler2D sbrdfLUT;
uniform samplerCube sDiffuseEnvSampler;
uniform samplerCube sSpecularEnvSampler;
uniform float uIblIntensity;
uniform mediump vec3 uYDirection;
uniform float uMaxLOD;

// For Alpha Mode.
uniform lowp float uOpaque;
uniform lowp float uMask;
uniform lowp float uAlphaThreshold;

// TODO: Multiple texture coordinate will be supported.
INPUT mediump vec2 vUV;
INPUT lowp mat3 vTBN;
INPUT lowp vec4 vColor;
INPUT highp vec3 vPositionToCamera;

const float c_MinRoughness = 0.04;
const float M_PI = 3.141592653589793;

// These properties can be used for circular sampling for PCF

// Percentage Closer Filtering to mitigate the banding artifacts.
const int kPcfSampleCount = 9;

const float kPi = 3.141592653589;
const float kInvSampleCount = 1.0 / float(kPcfSampleCount);
const float kPcfTheta = 2.0 * kPi * kInvSampleCount;
const float kSinPcfTheta = sin(kPcfTheta);
const float kCosPcfTheta = cos(kPcfTheta);

uniform lowp int uEnableShadowSoftFiltering;
uniform mediump float uShadowIntensity;
uniform highp float uShadowBias;

vec3 linear(vec3 color)
{
  return pow(color, vec3(2.2));
}

vec2 computeTextureTransform(vec2 texCoord, mat3 textureTransform)
{
    return vec2(textureTransform * vec3(texCoord, 1.0));
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
  highp vec3 n = normalize(vTBN[2].xyz);

#ifdef THREE_TEX
  // The albedo may be defined from a base texture or a flat color
#ifdef BASECOLOR_TEX
  mediump vec2 baseColorTexCoords = mix(vUV, computeTextureTransform(vUV, uBaseColorTextureTransform), uBaseColorTextureTransformAvailable);
  lowp vec4 baseColor = TEXTURE(sAlbedoAlpha, baseColorTexCoords);
  baseColor = vColor * vec4(linear(baseColor.rgb), baseColor.w) * uColorFactor;
#else // BASECOLOR_TEX
  lowp vec4 baseColor = vColor * uColorFactor;
#endif // BASECOLOR_TEX

#ifdef METALLIC_ROUGHNESS_TEX
  mediump vec2 metalRoughnessTexCoords = mix(vUV, computeTextureTransform(vUV, uMetalRoughnessTextureTransform), uMetalRoughnessTextureTransformAvailable);
  lowp vec4 metrou = TEXTURE(sMetalRoughness, metalRoughnessTexCoords);
  metallic = metrou.METALLIC * metallic;
  perceptualRoughness = metrou.ROUGHNESS * perceptualRoughness;
#endif // METALLIC_ROUGHNESS_TEX

#ifdef NORMAL_TEX
  mediump vec2 normalTexCoords = mix(vUV, computeTextureTransform(vUV, uNormalTextureTransform), uNormalTextureTransformAvailable);
  n = TEXTURE(sNormal, normalTexCoords).rgb;
  n = normalize(vTBN * ((2.0 * n - 1.0) * vec3(uNormalScale, uNormalScale, 1.0)));
#endif // NORMAL_TEX
#else // THREE_TEX
  mediump vec2 baseColorTexCoords = mix(vUV, computeTextureTransform(vUV, uBaseColorTextureTransform), uBaseColorTextureTransformAvailable);
  vec4 albedoMetal = TEXTURE(sAlbedoMetal, baseColorTexCoords);
  lowp vec4 baseColor = vec4(linear(albedoMetal.rgb), 1.0) * vColor * uColorFactor;

  metallic = albedoMetal.METALLIC * metallic;

  mediump vec2 normalRoughnessTexCoords = mix(vUV, computeTextureTransform(vUV, uNormalRoughnessTextureTransform), uNormalRoughnessTextureTransformAvailable);
  vec4 normalRoughness = TEXTURE(sNormalRoughness, normalRoughnessTexCoords);
  perceptualRoughness = normalRoughness.ROUGHNESS * perceptualRoughness;

  n = normalRoughness.rgb;
  n = normalize(vTBN * ((2.0 * n - 1.0) * vec3(uNormalScale, uNormalScale, 1.0)));
#endif // THREE_TEX

  // The value of uOpaque and uMask can be 0.0 or 1.0.
  // If uMask is 1.0, a Pixel that has bigger alpha than uAlphaThreashold becomes fully opaque,
  // and, a pixel that has smaller alpha than uAlphaThreashold becomes fully transparent.
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
  materialSpecularTexture.a = TEXTURE(sSpecular, vUV).a;
#endif
#ifdef MATERIAL_SPECULAR_COLOR_TEXTURE
  materialSpecularTexture.rgb = TEXTURE(sSpecularColor, vUV).rgb;
#endif
  specularWeight = uSpecularFactor * materialSpecularTexture.a;
  f0 = min(f0 * uSpecularColorFactor * materialSpecularTexture.rgb, vec3(1.0));
  f0 = mix(f0, baseColor.rgb, metallic);

  mediump vec3 v = normalize(vPositionToCamera); // Vector from surface point to camera
  mediump float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
  mediump vec3 reflection = -normalize(reflect(v, n));
  lowp vec3 brdf = TEXTURE(sbrdfLUT, vec2(NdotV, 1.0 - perceptualRoughness)).rgb;
  vec3 Fr = max(vec3(1.0 - perceptualRoughness), f0) - f0;
  vec3 k_S = f0 + Fr * pow(1.0 - NdotV, 5.0);
  vec3 FssEss = specularWeight * (k_S * brdf.x + brdf.y);

  // Specular Light
  // uMaxLOD that means mipmap level of specular texture is used for bluring of reflection of specular following roughness.
  float lod = perceptualRoughness * (uMaxLOD - 1.0);
#ifdef SL_VERSION_LOW
  // glsl 1.0 doesn't support textureLod. Let we just use textureCube instead.
  lowp vec3 specularLight = linear(textureCube(sSpecularEnvSampler, reflection * uYDirection).rgb);
#else
  lowp vec3 specularLight = linear(textureLod(sSpecularEnvSampler, reflection * uYDirection, lod).rgb);
#endif
  lowp vec3 specular = specularLight * FssEss;

  // Diffuse Light
  lowp vec3 diffuseColor = mix(baseColor.rgb, vec3(0), metallic);
#ifdef SL_VERSION_LOW
  lowp vec3 irradiance = linear(textureCube(sDiffuseEnvSampler, n * uYDirection).rgb);
#else
  lowp vec3 irradiance = linear(TEXTURE(sDiffuseEnvSampler, n * uYDirection).rgb);
#endif
  float Ems = (1.0 - (brdf.x + brdf.y));
  vec3 F_avg = specularWeight * (f0 + (1.0 - f0) / 21.0);
  vec3 FmsEms = Ems * FssEss * F_avg / (1.0 - F_avg * Ems);
  vec3 k_D = diffuseColor * (1.0 - FssEss + FmsEms);
  lowp vec3 diffuse = (FmsEms + k_D) * irradiance;

  lowp vec3 color = (diffuse + specular) * uIblIntensity;

  // Punctual Light
  if(uLightCount > 0)
  {
    // Compute reflectance.
    highp float reflectance = max(max(f0.r, f0.g), f0.b);
    highp float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    highp float r = perceptualRoughness * perceptualRoughness;
    highp float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    highp float roughnessSq = r * r;
    highp vec3 diffuseColorPunctual = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColorPunctual *= ( 1.0 - metallic );

    for(int i = 0; i < uLightCount; ++i)
    {
      highp vec3 l = normalize(-uLightDirection[i]); // Vector from surface point to light
      highp vec3 h = normalize(l+v);               // Half vector between both l and v
      highp float VdotH = dot(v, h);
      highp vec3 specularReflection = f0 + (reflectance90 - f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);

      highp float NdotL = clamp(dot(n, l), 0.001, 1.0);
      highp float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
      highp float geometricOcclusion = attenuationL * attenuationV;

      highp float NdotH = dot(n, h);
      highp float f = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
      highp float microfacetDistribution = roughnessSq / (M_PI * f * f);;

      // Calculation of analytical lighting contribution
      highp vec3 diffuseContrib = ( 1.0 - specularReflection ) * ( diffuseColorPunctual / M_PI );
      highp vec3 specContrib = specularReflection * geometricOcclusion * microfacetDistribution / ( 4.0 * NdotL * NdotV );

      // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
      color += NdotL * uLightColor[i] * (diffuseContrib + specContrib);
    }
  }

  if(float(uIsShadowEnabled) * uShadowIntensity > 0.0)
  {
    mediump float exposureFactor = 0.0;
    if(uEnableShadowSoftFiltering > 0)
    {
#ifdef SL_VERSION_LOW
      ivec2 texSize = ivec2(uShadowMapWidth, uShadowMapHeight);
#else
      ivec2 texSize = textureSize(sShadowMap, 0);
#endif
      mediump vec2 texelSize = vec2(1.0) / vec2(texSize.x, texSize.y);
      mediump vec2 pcfSample = vec2(1.0, 0.0);
      for (int i = 0; i < kPcfSampleCount; ++i)
      {
        pcfSample = vec2(kCosPcfTheta * pcfSample.x - kSinPcfTheta * pcfSample.y,
                         kSinPcfTheta * pcfSample.x + kCosPcfTheta * pcfSample.y);
        lowp float depthValue = TEXTURE(sShadowMap, positionFromLightView.xy + pcfSample * texelSize).r;
        exposureFactor += (depthValue < positionFromLightView.z - uShadowBias) ? 0.0 : 1.0;
      }
      exposureFactor *= kInvSampleCount;
    }
    else
    {
      mediump float depthValue = TEXTURE(sShadowMap, positionFromLightView.xy).r;
      exposureFactor           = (depthValue < positionFromLightView.z - uShadowBias) ? 0.0 : 1.0;
    }
    color *= (1.0 - (1.0 - exposureFactor) * uShadowIntensity);
  }

#ifdef OCCLUSION
  mediump vec2 occlusionTexCoords = mix(vUV, computeTextureTransform(vUV, uOcclusionTextureTransform), uOcclusionTextureTransformAvailable);
  lowp float ao = TEXTURE(sOcclusion, occlusionTexCoords).r;
  color = mix(color, color * ao, uOcclusionStrength);
#endif // OCCLUSION

#ifdef EMISSIVE_TEXTURE
  mediump vec2 emissiveTexCoords = mix(vUV, computeTextureTransform(vUV, uEmissiveTextureTransform), uEmissiveTextureTransformAvailable);
  lowp vec3 emissive = linear(TEXTURE(sEmissive, emissiveTexCoords).rgb) * uEmissiveFactor;
#else
  lowp vec3 emissive = uEmissiveFactor;
#endif // EMISSIVE_TEXTURE
  color += emissive;

  OUT_COLOR = vec4(pow(color, vec3(1.0 / 2.2)), baseColor.a) * uColor;
}
