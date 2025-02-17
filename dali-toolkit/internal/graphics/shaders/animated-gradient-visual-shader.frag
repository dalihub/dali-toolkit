//@name animated-gradient-visual-shader.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM highp vec4 start_color;
  UNIFORM highp vec4 end_color;
  UNIFORM highp float gradient_offset;
};

INPUT highp vec2 vTexCoord;
INPUT highp vec2 vStart;
INPUT highp vec2 vEnd;

float get_position(vec2 x, vec2 s, vec2 e)
{
  vec2 df = e - s;
  vec2 dx = x - s;

#ifdef GRADIENT_TYPE_LINEAR
  return dot(dx,df)/dot(df,df);
#endif
/* GradientType::LINEAR */

#ifdef GRADIENT_TYPE_RADIAL
  return sqrt(dot(dx,dx)/dot(df,df));
#endif
/* GradientType::RADIAL */
}

float recalculate(float r)
{
#ifdef SPREAD_TYPE_REFLECT
  return 1.0 - abs(mod(r, 2.0) - 1.0);
#endif
/* SpreadType::REFLECT */

#ifdef SPREAD_TYPE_REPEAT
  return fract(r);
#endif
/* SpreadType::REPEAT*/

#ifdef SPREAD_TYPE_CLAMP
  return clamp(r, 0.0, 1.0);
#endif
/* SpreadType::CLAMP */
}

void main()
{
  float r = get_position( vTexCoord, vStart, vEnd );
  r = recalculate( r + gradient_offset );
  vec4 color = mix( start_color, end_color, r );
  gl_FragColor = color;
}
