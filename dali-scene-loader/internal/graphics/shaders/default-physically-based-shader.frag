#version 300 es

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

#ifdef THREE_TEX
#ifdef GLTF_CHANNELS
// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#pbrmetallicroughnessmetallicroughnesstexture
#define METALLIC b
#define ROUGHNESS g
#else //GLTF_CHANNELS
#define METALLIC r
#define ROUGHNESS a
#endif //GLTF_CHANNELS
#endif //THREE_TEX

#ifdef THREE_TEX
  uniform sampler2D sAlbedoAlpha;
  uniform sampler2D sMetalRoughness;
  uniform sampler2D sNormal;

#ifdef ALPHA_TEST
  uniform float uAlphaThreshold;
#endif //ALPHA_TEST

#else
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

uniform samplerCube sDiffuse;
uniform samplerCube sSpecular;

// Number of mip map levels in the texture
uniform float uMaxLOD;

// Transformation matrix of the cubemap texture
uniform mat4 uCubeMatrix;

uniform vec4 uColor;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

//IBL Light intensity
uniform float uIblIntensity;

// TODO: Multiple texture coordinate will be supported.
in vec2 vUV;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vViewVec;

out vec4 FragColor;

// Functions for BRDF calculation come from
// https://www.unrealengine.com/blog/physically-based-shading-on-mobile
// Based on the paper by Dimitar Lazarov
// http://blog.selfshadow.com/publications/s2013-shading-course/lazarov/s2013_pbs_black_ops_2_notes.pdf
vec3 EnvBRDFApprox( vec3 SpecularColor, float Roughness, float NoV )
{
  const vec4 c0 = vec4( -1.0, -0.0275, -0.572, 0.022 );
  const vec4 c1 = vec4( 1.0, 0.0425, 1.04, -0.04 );
  vec4 r = Roughness * c0 + c1;
  float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
  vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;

  return SpecularColor * AB.x + AB.y;
}

void main()
{
  // We get information from the maps (albedo, normal map, roughness, metalness
  // I access the maps in the order they will be used
#ifdef THREE_TEX
  vec4 albedoAlpha = texture(sAlbedoAlpha, vUV.st);
  float alpha = albedoAlpha.a;
#ifdef ALPHA_TEST
  if (alpha <= uAlphaThreshold)
  {
    discard;
  }
#endif	//ALPHA_TEST
  vec3 albedoColor = albedoAlpha.rgb * uColor.rgb;

  vec4 metalRoughness = texture(sMetalRoughness, vUV.st);
  float metallic = metalRoughness.METALLIC * uMetallicFactor;
  float roughness = metalRoughness.ROUGHNESS * uRoughnessFactor;

  vec3 normalMap = texture(sNormal, vUV.st).rgb;
#else  //THREE_TEX
  vec4 albedoMetal = texture(sAlbedoMetal, vUV.st);
  vec3 albedoColor = albedoMetal.rgb * uColor.rgb;
  float metallic = albedoMetal.a * uMetallicFactor;

  vec4 normalRoughness = texture(sNormalRoughness, vUV.st);
  vec3 normalMap = normalRoughness.rgb;
  float roughness = normalRoughness.a * uRoughnessFactor;
#endif
  //Normalize vectors
  vec3 normal = normalize(vNormal);
  vec3 tangent = normalize(vTangent);

  // NOTE: normal and tangent have to be orthogonal for the result of the cross()
  // product to be a unit vector. We might find that we need to normalize().
  vec3 bitangent = cross(normal, tangent);

  vec3 viewVec = normalize(vViewVec);

  // Create Inverse Local to world matrix
  mat3 vInvTBN = mat3(tangent, bitangent, normal);

  // Get normal map info in world space
  normalMap = normalize(normalMap - 0.5);
  vec3 newNormal = vInvTBN * normalMap.rgb;

  // Calculate normal dot view vector
  float NoV = max(dot(newNormal, -viewVec), 0.0);

  // Reflect vector
  vec3 reflectionVec = reflect(viewVec, newNormal);

  //transform it now to environment coordinates (used when the environment rotates)
  vec3 reflecCube = (uCubeMatrix * vec4( reflectionVec, 0.0 ) ).xyz;
  reflecCube = normalize( reflecCube );

  //transform it now to environment coordinates
  vec3 normalCube = ( uCubeMatrix * vec4( newNormal, 0.0 ) ).xyz;
  normalCube = normalize( normalCube );

  // Get irradiance from diffuse cubemap
  vec3 irradiance = texture( sDiffuse, normalCube ).rgb;

  // Access reflection color using roughness value
  float finalLod = mix( 0.0, uMaxLOD - 2.0, roughness);
  vec3 reflectionColor = textureLod(sSpecular, reflecCube, finalLod).rgb;

  // We are supposed to be using DielectricColor (0.04) of a plastic (almost everything)
  // http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf
  // however that seems to prevent achieving very dark tones (i.e. get dark gray blacks).
  vec3 DiffuseColor = albedoColor - albedoColor * metallic;  // 1 mad
  vec3 SpecularColor = mix( vec3(0.04), albedoColor, metallic); // 2 mad

  // Calculate specular color using Magic Function (takes original roughness and normal dot view).
  vec3 specColor =  reflectionColor.rgb * EnvBRDFApprox(SpecularColor, roughness, NoV );

  // Multiply the result by albedo texture and do energy conservation
  vec3 diffuseColor = irradiance * DiffuseColor;

  // Final color is the sum of the diffuse and specular term
  vec3 finalColor = diffuseColor + specColor;

  finalColor = sqrt( finalColor ) * uIblIntensity;


#ifdef OCCLUSION
  float ao = texture(sOcclusion, vUV.st).r;
  finalColor = mix( finalColor, finalColor * ao, uOcclusionStrength );
#endif

#ifdef EMISSIVE
  vec3 emissive = texture( sEmissive, vUV.st ).rgb * uEmissiveFactor;
  finalColor += emissive;
#endif

#ifdef THREE_TEX
  FragColor = vec4( finalColor, alpha );
#else //THREE_TEX
  FragColor = vec4( finalColor, 1.0 );
#endif //THREE_TEX
}
