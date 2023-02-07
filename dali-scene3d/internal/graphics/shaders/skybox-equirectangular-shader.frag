// Fragment shader for a skybox in equirectangular projection
precision mediump float;

uniform sampler2D uSkyBoxEquirectangularTexture;

uniform vec4  uColor;
uniform float uIntensity;

varying vec3 vTexCoord;

// Take the sample direction as interpolated from the cube's local position,
// and use this direction vector and the spherical to cartesian coordinate
// conversion to sample the equirectangular map as if it's a cube map.

const float M_1_PI = 0.3183; // The reciprocal of pi in radians
const float M_1_2PI = 0.1591; // The reciprocal of 2*pi in radians

const vec2 inverseAtan = vec2(M_1_2PI, M_1_PI);

vec2 SampleEquirectangularMapAsCubeMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inverseAtan;
    uv += 0.5;
    return uv;
}

void main()
{
  // Project the equirectangular map to a cube's faces
  vec2 uv = SampleEquirectangularMapAsCubeMap(normalize(vTexCoord));

  // Flip the texture UVs vertically
  vec2 uvFlippped = vec2(uv.x, 1.0 - uv.y);

  vec4 texColor = texture2D( uSkyBoxEquirectangularTexture, uvFlippped ) * uIntensity;
  gl_FragColor = texColor * uColor;
}
