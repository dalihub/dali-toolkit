INPUT mediump vec2 vPosition;

uniform lowp vec4 uColor;
uniform mediump float thickness;
uniform mediump float radius;
uniform mediump float startAngle;
uniform mediump float sweepAngle;

const mediump float M_PI_OVER_2 = 1.57079632679;
const mediump float M_PI_2 = 6.28318530718;

mediump float GetOpacity()
{
  mediump float start = radians( mod( startAngle, 360.0 ) );
  mediump float angle = mod( atan( vPosition.y, vPosition.x ) + M_PI_OVER_2 - start, M_PI_2 );
  mediump float dist = length( vPosition );
  if( angle <= radians( sweepAngle ) )
  {
    return smoothstep( -1.0, 1.0, thickness / 2.0 - ( abs( dist - radius ) ) );
  }
  mediump float end = radians( mod( startAngle + sweepAngle, 360.0 ) );
  mediump vec2 q0 = vec2( radius * cos( start - M_PI_OVER_2 ), radius * sin( start - M_PI_OVER_2 ) );
  mediump vec2 q1 = vec2( radius * cos( end - M_PI_OVER_2 ), radius * sin( end - M_PI_OVER_2 ) );
  return smoothstep( -1.0, 1.0, thickness / 2.0 - min( length( vPosition - q0 ), length( vPosition - q1 ) ) );
}

void main()
{
  OUT_COLOR = uColor;
  OUT_COLOR.a *= GetOpacity();
}