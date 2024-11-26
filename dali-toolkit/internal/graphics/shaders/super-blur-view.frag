//@name super-blur-view.frag

//@version 100

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp float uAlpha;
};

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uColor;
  gl_FragColor.a *= uAlpha;
}
