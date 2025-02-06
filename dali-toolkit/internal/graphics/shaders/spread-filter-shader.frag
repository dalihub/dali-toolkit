precision highp float;
varying highp vec2 vTexCoord;
uniform sampler2D sTexture;
uniform highp int uSpread;
uniform highp vec2 uTexScale;

void main()
{
  vec4 color = texture2D( sTexture, vTexCoord);
  for( int i = 1; i <= uSpread; ++i )
  {
    vec2 offset = uTexScale * float(i);
    color = max( texture2D( sTexture, vTexCoord + offset), color );
    color = max( texture2D( sTexture, vTexCoord - offset), color );
  }
  gl_FragColor = color;
}