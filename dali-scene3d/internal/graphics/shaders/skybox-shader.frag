uniform samplerCube   uSkyBoxTexture;
uniform mediump float uIntensity;
varying mediump vec3  vTexCoord;

void main()
{
  mediump vec4 texColor = textureCube(uSkyBoxTexture, vTexCoord) * uIntensity;
  gl_FragColor = texColor;
}