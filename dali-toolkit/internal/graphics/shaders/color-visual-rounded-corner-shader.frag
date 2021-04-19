INPUT mediump vec2 vPosition;
INPUT mediump vec2 vRectSize;
INPUT mediump float vCornerRadius;

uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;
  mediump vec2 diff = abs( vPosition ) - vRectSize;
  mediump float dist = length( max( diff, vec2( 0.0 ) ) ) - vCornerRadius;
  if( dist > 1.0 )
  {
    OUT_COLOR.a = 0.0;
  }
  else if( dist > -1.0 )
  {
    if( min( diff.x, diff.y ) < 0.0)
    {
      dist += min( diff.x, diff.y ) / vCornerRadius;
    }
    OUT_COLOR.a *= 1.0 - smoothstep( -1.0, 1.0, dist );
  }
}
