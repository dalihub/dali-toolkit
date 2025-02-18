//@name bubble-effect-color-adjuster.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM vec3 uHSVDelta;
  UNIFORM float uIgnoreAlpha;
};

INPUT highp vec2 vTexCoord;
UNIFORM sampler2D sTexture;

float rand(vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 rgb2hsv(vec3 c)
{
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  vec4 color = TEXTURE(sTexture, vTexCoord);
  vec3 hsvColor = rgb2hsv( color.rgb );
  // modify the hsv Value
  hsvColor += uHSVDelta * rand(vTexCoord);
  // if the new vale exceeds one, then decrease it
  hsvColor -= max(hsvColor*2.0 - vec3(2.0), 0.0);
  // if the new vale drops below zero, then increase it
  hsvColor -= min(hsvColor*2.0, 0.0);
  color.rgb = hsv2rgb( hsvColor );
  // uIgnoreAlpha decide the result alpha will be 1.0 or source's alpha
  color.a += uIgnoreAlpha;
  gl_FragColor = color;
}
