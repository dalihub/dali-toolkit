//@version 100

UNIFORM samplerCube   uSkyBoxTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp    vec4  uColor;
  UNIFORM mediump float uIntensity;
};

INPUT mediump vec3  vTexCoord;

void main()
{
  mediump vec4 texColor = TEXTURE_CUBE(uSkyBoxTexture, vTexCoord) * uIntensity;
  gl_FragColor = texColor * uColor;
}