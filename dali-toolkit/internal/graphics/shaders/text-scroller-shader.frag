//@name text-scroller-shader.frag

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
  if ( vTexCoord.y > 1.0 )
    discard;

  mediump vec4 textTexture = TEXTURE( sTexture, vTexCoord );

  gl_FragColor = textTexture * uColor;
}
