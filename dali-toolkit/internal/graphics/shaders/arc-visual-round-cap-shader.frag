//@name arc-visual-round-cap-shader.frag

//@version 100

precision highp float;

INPUT highp vec2 vPosition;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM highp float thickness;
  UNIFORM highp float radius;
  UNIFORM highp float startAngle;
  UNIFORM highp float sweepAngle;
};

const highp float M_PI_OVER_2 = 1.57079632679;
const highp float M_PI_2 = 6.28318530718;

mediump float GetOpacity()
{
  highp float start = radians( mod( startAngle, 360.0 ) );
  highp float angle = mod( atan( vPosition.y, vPosition.x ) + M_PI_OVER_2 - start, M_PI_2 );
  highp float dist = length( vPosition );
  if( angle <= radians( sweepAngle ) )
  {
    return smoothstep( -1.0, 1.0, thickness / 2.0 - ( abs( dist - radius ) ) );
  }
  highp float end = radians( mod( startAngle + sweepAngle, 360.0 ) );
  highp vec2 q0 = vec2( radius * cos( start - M_PI_OVER_2 ), radius * sin( start - M_PI_OVER_2 ) );
  highp vec2 q1 = vec2( radius * cos( end - M_PI_OVER_2 ), radius * sin( end - M_PI_OVER_2 ) );
  return smoothstep( -1.0, 1.0, thickness / 2.0 - min( length( vPosition - q0 ), length( vPosition - q1 ) ) );
}

void main()
{
  gl_FragColor = uColor;
  gl_FragColor.a *= GetOpacity();
}