//@name emboss-filter-composite-shader.frag

//@version 100

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uEffectColor;
};

void main()
{
  gl_FragColor = uEffectColor;
  gl_FragColor.a *= TEXTURE( sTexture, vTexCoord).a;
}