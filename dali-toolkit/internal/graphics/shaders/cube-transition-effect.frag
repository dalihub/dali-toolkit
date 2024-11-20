//@name cube-transition-effect.frag

//@version 100

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 uSamplerRect;
};

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
}
