uniform samplerCube   uSkyBoxTexture;
uniform lowp    vec4  uColor;
uniform mediump float uIntensity;
varying mediump vec3  vTexCoord;

void main()
{
  mediump vec4 texColor = textureCube(uSkyBoxTexture, vTexCoord) * uIntensity;
  gl_FragColor = texColor * uColor;
}