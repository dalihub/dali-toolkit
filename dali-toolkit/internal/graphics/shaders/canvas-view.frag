//@name canvas-view.frag

//@version 100

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;

void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord) * uColor;
}
