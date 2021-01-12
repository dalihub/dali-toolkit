uniform sampler2D sTexture; // sampler1D?
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
varying mediump vec2 vTexCoord;
varying mediump vec2 vPosition;
varying mediump vec2 vRectSize;
varying mediump float vCornerRadius;

void main()
{
  gl_FragColor = texture2D( sTexture, vec2( length(vTexCoord), 0.5 ) ) * vec4(mixColor, 1.0) * uColor;
  mediump vec2 diff = abs( vPosition ) - vRectSize;
  mediump float dist = length( max( diff, vec2( 0.0 ) ) ) - vCornerRadius;
  if( dist > 1.0 )
  {
    gl_FragColor = vec4( 0.0 );
  }
  else if( dist > -1.0 )
  {
    if( min( diff.x, diff.y ) < 0.0)
    {
      dist += min( diff.x, diff.y ) / vCornerRadius;
    }
    gl_FragColor *= 1.0 - smoothstep( -1.0, 1.0, dist );
  }
}
