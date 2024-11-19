//@name arc-visual-butt-cap-shader.frag

//@version 100

INPUT mediump vec2 vPosition;

UNIFORM_BLOCK VertBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM mediump float thickness;
  UNIFORM mediump float radius;
  UNIFORM mediump float startAngle;
  UNIFORM mediump float sweepAngle;
};

const mediump float M_PI_OVER_2 = 1.57079632679;
const mediump float M_PI = 3.14159265359;
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
  mediump vec2 q0 = vec2( dist * cos( start - M_PI_OVER_2 ), dist * sin( start - M_PI_OVER_2 ) );
  mediump vec2 q1 = vec2( dist * cos( end - M_PI_OVER_2 ), dist * sin( end - M_PI_OVER_2 ) );
  mediump float opacity = 1.0 - smoothstep( 0.0, 2.0, min( length( vPosition - q0 ), length( vPosition - q1 ) ) );
  opacity *= step( 0.0, thickness / 2.0 - abs( dist - radius ) );
  return opacity;
}

void main()
{
  gl_FragColor = uColor;
  gl_FragColor.a *= GetOpacity();
}
