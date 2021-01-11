#version 300 es

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

#define ROUND_STARTCAP   0x1
#define ROUND_ENDCAP     0x2
#define SQUARE_STARTCAP  0x4
#define SQUARE_ENDCAP    0x8
precision mediump float;

in vec2 vUV;

uniform vec4 uColor;
uniform int antiAliasing;
uniform float radius;
uniform vec2 startAngle;
uniform vec2 endAngle;
uniform int arcCaps;

out vec4 FragColor;


void main()
{
////////////////////
//Inputs
  float radius1 = 1.0;
  float uvPixel = length( dFdx( vUV ) ) * 2.0;
  float dx = mix( 0.0001, uvPixel, antiAliasing > 0 );
  float radius2 = mix( 1.0 - radius * uvPixel - dx , -radius , step( 0.0, -radius ) );
////////////////////
  float alpha = 0.0;
  float circle = 0.0;

  vec2 uv = fract(vUV) * 2.0 - 1.0;

  float ang_img = endAngle.y * startAngle.x - endAngle.x * startAngle.y;
  float half1 = uv.x * startAngle.x - uv.y * startAngle.y;
  float half2 = uv.y * endAngle.y - uv.x * endAngle.x;
  float len = length(uv);
  float equalAngles = step(0.99999, dot(endAngle, startAngle));
  float right_side = step(0.0, ang_img);
  vec2 uv_norm = normalize(uv);
  float cap_radius = -0.5 + radius2 / 2.0;
  float square_cap;
  float sq_plane;

    /******************************************
   * Equivalent to:
   * if( len > radius1 || len < radius2 )
   */
  circle = alpha =  smoothstep( len, len + dx, radius1 ) * smoothstep( -len, -len + dx, -radius2 );
  float half1_step = smoothstep( -dx, 0.0, half1 );
  float half2_step = smoothstep( -dx, 0.0, half2 );

  float neg_angimg = max( half1_step, half2_step );
  float pos_angimg = min( half1_step, half2_step );
  /******************************************
   * Equivalent to:
  if(ang_img < 0.0)
  {
    alpha *= max(smoothstep( 0.0, dx, half1 ), smoothstep( 0.0, dx, half2));
  }
  else
  {
    alpha *= min(smoothstep( 0.0, dx, half1 ) , smoothstep( 0.0, dx, half2 ));
  }
  */
  alpha *= mix( neg_angimg, pos_angimg, step(0.0, ang_img) );

  alpha = mix( alpha, mix( circle, 0.0, right_side ), equalAngles );

  if((arcCaps & ROUND_STARTCAP) > 0)
  {
    len = length(uv - normalize( vec2( startAngle.y, startAngle.x )) * (1.0 + cap_radius));
    alpha = max(alpha, smoothstep(cap_radius, cap_radius + dx, -len) );
  }
  if((arcCaps & ROUND_ENDCAP) > 0)
  {
    len = length(uv - normalize( vec2( endAngle.y, endAngle.x )) * (1.0 + cap_radius));
    alpha = max(alpha, smoothstep(cap_radius, cap_radius + dx, -len) );
  }

  if((arcCaps & SQUARE_STARTCAP) > 0)
  {
    sq_plane = -uv.x * startAngle.y - uv.y * startAngle.x;
    square_cap = min(step( 0.0, -half1 ), smoothstep( cap_radius, cap_radius + dx, half1 ));
    square_cap = min(square_cap, smoothstep( radius2, radius2 + dx, -sq_plane ));
    square_cap = min(square_cap, smoothstep( -1.0, -1.0 + dx, sq_plane ));
    alpha = max(square_cap, alpha);
  }

  if((arcCaps & SQUARE_ENDCAP) > 0)
  {
    sq_plane = -uv.x * endAngle.y - uv.y * endAngle.x;
    square_cap = min(step( 0.0, -half2 ), smoothstep( cap_radius, cap_radius + dx, half2 ));
    square_cap = min(square_cap, smoothstep( radius2, radius2 + dx, -sq_plane ));
    square_cap = min(square_cap, smoothstep( -1.0, -1.0 + dx, sq_plane ));
    alpha = max(square_cap, alpha);
  }

  if( alpha == 0.0 )
  {
    discard;
  }

  FragColor = vec4(vec3(uColor), uColor.a * alpha);
}
