//@name spread-filter-shader.frag

//@version 100

precision highp float;
INPUT mediump vec2 vTexCoord;
UNIFORM sampler2D sTexture;
UNIFORM_BLOCK FragBlock
{
  UNIFORM int uSpread;
  UNIFORM vec2 uTexScale;
};

void main()
{
  vec4 color = TEXTURE( sTexture, vTexCoord);
  for( int i = 1; i <= uSpread; ++i )
  {
    vec2 offset = uTexScale * float(i);
    color = max( TEXTURE( sTexture, vTexCoord + offset), color );
    color = max( TEXTURE( sTexture, vTexCoord - offset), color );
  }
  gl_FragColor = color;
}