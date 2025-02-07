//@name canvas-view.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;

void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord) * uColor;
}
