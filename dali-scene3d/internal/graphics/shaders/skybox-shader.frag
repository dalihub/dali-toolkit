//@name skybox-shader.frag

//@version 100

precision highp float;

UNIFORM samplerCube   uSkyBoxTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp  vec4  uColor;
  UNIFORM highp float uIntensity;
};

INPUT highp vec3  vTexCoord;

void main()
{
  mediump vec4 texColor = TEXTURE_CUBE(uSkyBoxTexture, vTexCoord) * uIntensity;
  gl_FragColor = texColor * uColor;
}