uniform lowp vec3 uLightColor;
uniform lowp vec4 uBaseColorFactor;
uniform lowp vec2 uMetallicRoughnessFactors;
uniform lowp int alphaMode;
uniform lowp float alphaCutoff;

in lowp vec2 vUV[2];
in lowp mat3 vTBN;
in lowp vec4 vColor;
flat in int visLight;
in highp vec3 vLightDirection;
in highp vec3 vPositionToCamera;

out vec4 FragColor;

struct PBRInfo
{
  mediump float NdotL;                  // cos angle between normal and light direction
  mediump float NdotV;                  // cos angle between normal and view direction
  mediump float NdotH;                  // cos angle between normal and half vector
  mediump float VdotH;                  // cos angle between view direction and half vector
  mediump vec3 reflectance0;            // full reflectance color (normal incidence angle)
  mediump vec3 reflectance90;           // reflectance color at grazing angle
  lowp float alphaRoughness;            // roughness mapped to a more linear change in the roughness (proposed by [2])
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

vec3 getNormal()
{
#ifdef TEXTURE_NORMAL
  lowp vec3 n = texture( uNormalSampler, vUV[uNormalTexCoordIndex] ).rgb;
  n = normalize( vTBN * ( ( 2.0 * n - 1.0 ) * vec3( uNormalScale, uNormalScale, 1.0 ) ) );
#else
  lowp vec3 n = normalize( vTBN[2].xyz );
#endif
  return n;
}

vec3 specularReflection( PBRInfo pbrInputs )
{
  return pbrInputs.reflectance0 + ( pbrInputs.reflectance90 - pbrInputs.reflectance0 ) * pow( clamp( 1.0 - pbrInputs.VdotH, 0.0, 1.0 ), 5.0 );
}

float geometricOcclusion( PBRInfo pbrInputs )
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

vec3 linear( vec3 color )
{
  return pow(color,vec3(2.2));
}

void main()
{
  // Metallic and Roughness material properties are packed together
  // In glTF, these factors can be specified by fixed scalar values
  // or from a metallic-roughness map
  lowp float metallic = uMetallicRoughnessFactors.x;
  lowp float perceptualRoughness = uMetallicRoughnessFactors.y;

  // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
  // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
#ifdef TEXTURE_METALLICROUGHNESS
  lowp vec4 metrou = texture(uMetallicRoughnessSampler, vUV[uMetallicRoughnessTexCoordIndex]);
  metallic = metrou.b * metallic;
  perceptualRoughness = metrou.g * perceptualRoughness;
#endif

  metallic = clamp(metallic, 0.0, 1.0);
  perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
  // Roughness is authored as perceptual roughness; as is convention,
  // convert to material roughness by squaring the perceptual roughness [2].
  lowp float alphaRoughness = perceptualRoughness * perceptualRoughness;

#ifdef TEXTURE_BASECOLOR
  // The albedo may be defined from a base texture or a flat color
  lowp vec4 baseColor = texture(uBaseColorSampler, vUV[uBaseColorTexCoordIndex]) * uBaseColorFactor;
  baseColor = vec4(linear(baseColor.rgb), baseColor.w);
#else
  lowp vec4 baseColor = vColor * uBaseColorFactor;
#endif

  if( alphaMode == 0 )
  {
    baseColor.w = 1.0;
  }
  else if( alphaMode == 1 )
  {
    if( baseColor.w >= alphaCutoff )
    {
      baseColor.w = 1.0;
    }
    else
    {
      baseColor.w = 0.0;
    }
  }

  lowp vec3 f0 = vec3(0.04);
  lowp vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
  diffuseColor *= ( 1.0 - metallic );
  lowp vec3 specularColor = mix(f0, baseColor.rgb, metallic);

  // Compute reflectance.
  lowp float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

  // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
  // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
  lowp float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
  lowp vec3 specularEnvironmentR0 = specularColor.rgb;
  lowp vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

  mediump vec3 n = getNormal();                            // normal at surface point
  mediump vec3 v = normalize(vPositionToCamera);           // Vector from surface point to camera
  mediump vec3 l = normalize(vLightDirection);             // Vector from light to surface point
  mediump vec3 h = normalize(l+v);                         // Half vector between both l and v
  mediump vec3 reflection = -normalize(reflect(v, n));

  mediump float NdotL = clamp(dot(n, l), 0.001, 1.0);
  mediump float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
  mediump float NdotH = dot(n, h);
  mediump float LdotH = dot(l, h);
  mediump float VdotH = dot(v, h);

  PBRInfo pbrInputs = PBRInfo(
    NdotL,
    NdotV,
    NdotH,
    VdotH,
    specularEnvironmentR0,
    specularEnvironmentR90,
    alphaRoughness
  );

  // Calculate the shading terms for the microfacet specular shading model
  lowp vec3 color = vec3(0.0);
  if( visLight == 1 )
  {
    lowp vec3 F = specularReflection( pbrInputs );
    lowp float G = geometricOcclusion( pbrInputs );
    lowp float D = microfacetDistribution( pbrInputs );

  // Calculation of analytical lighting contribution
    lowp vec3 diffuseContrib = ( 1.0 - F ) * ( diffuseColor / M_PI );
    lowp vec3 specContrib = F * G * D / ( 4.0 * NdotL * NdotV );
  // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    color = NdotL * uLightColor * (diffuseContrib + specContrib);
  }

#ifdef TEXTURE_IBL
  lowp float lod = ( perceptualRoughness * uMipmapLevel );
  // retrieve a scale and bias to F0. See [1], Figure 3
  lowp vec3 brdf = linear( texture( ubrdfLUT, vec2( NdotV, 1.0 - perceptualRoughness ) ).rgb );
  lowp vec3 diffuseLight = linear( texture( uDiffuseEnvSampler, n ).rgb );
  lowp vec3 specularLight = linear( textureLod( uSpecularEnvSampler, reflection, lod ).rgb );

  lowp vec3 diffuse = diffuseLight * diffuseColor * uScaleIBLAmbient.x;
  lowp vec3 specular = specularLight * ( specularColor * brdf.x + brdf.y ) * uScaleIBLAmbient.y;
  color += ( diffuse + specular );
#endif

#ifdef TEXTURE_OCCLUSION
  lowp float ao = texture( uOcclusionSampler, vUV[uOcclusionTexCoordIndex] ).r;
  color = mix( color, color * ao, uOcclusionStrength );
#endif

#ifdef TEXTURE_EMIT
  lowp vec3 emissive = linear( texture( uEmissiveSampler, vUV[uEmissiveTexCoordIndex] ).rgb ) * uEmissiveFactor;
  color += emissive;
#endif

  FragColor = vec4( pow( color,vec3( 1.0 / 2.2 ) ), baseColor.a );
}
