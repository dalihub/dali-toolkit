//@name cube-transition-effect.frag

//@version 100

precision highp float;

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
}
