//@name glyphy-shader-main.frag

//@version 100

UNIFORM float u_contrast;
UNIFORM float u_gamma_adjust;
UNIFORM float u_outline_thickness;
UNIFORM float u_outline;
UNIFORM float u_boldness;

INPUT vec4 v_glyph;

#define SQRT2_2 0.70711 /* 1 / sqrt(2.) */
#define SQRT2   1.41421

struct glyph_info_t
{
  ivec2 nominal_size;
  ivec2 atlas_pos;
};

glyph_info_t glyph_info_decode (vec4 v)
{
  glyph_info_t gi;
  gi.nominal_size = (ivec2 (mod (v.zw, 256.)) + 2) / 4;
  gi.atlas_pos = ivec2 (v_glyph.zw) / 256;
  return gi;
}


float antialias (float d)
{
  return smoothstep (-.75, +.75, d);
}

vec4 source_over (const vec4 src, const vec4 dst)
{
  // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators_srcover
  float alpha = src.a + (dst.a * (1. - src.a));
  return vec4 (((src.rgb * src.a) + (dst.rgb * dst.a * (1. - src.a))) / alpha, alpha);
}

void main()
{
  vec2 p = v_glyph.xy;
  glyph_info_t gi = glyph_info_decode (v_glyph);

  /* isotropic antialiasing */
  vec2 dpdx = dFdx (p);
  vec2 dpdy = dFdy (p);
  float m = length (vec2 (length (dpdx), length (dpdy))) * SQRT2_2;

  vec4 color = vec4( vColor.rgb * uColor.rgb, vColor.a * uColor.a );

  ivec4 uu_atlas_info = ivec4( u_atlas_info );
  float gsdist = glyphy_sdf (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);
  float sdist = gsdist / m * u_contrast;

  sdist -= u_boldness * 10.;
  if ( glyphy_iszero( u_outline ) )
    sdist = abs (sdist) - u_outline_thickness * .5;
  if (sdist > 1.)
    discard;
  float alpha = antialias (-sdist);
  if (u_gamma_adjust != 1.)
    alpha = pow (alpha, 1./u_gamma_adjust);
  color = vec4 (color.rgb,color.a * alpha);

  gl_FragColor = color;
}
