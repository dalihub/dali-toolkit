//@version 100

INPUT mediump vec2 vTexCoord;

UNIFORM sampler2D sTexture;

UNIFORM_BLOCK FragBlock
{
UNIFORM lowp vec4 uColor;
};


void main()
{
  mediump vec4 color = texture2D( sTexture, vTexCoord );
  if(color.a <= 0.0001)
  {
    discard;
  }
  gl_FragColor = color * uColor;
}
