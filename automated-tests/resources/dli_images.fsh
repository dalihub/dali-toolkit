#version 300 es

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

uniform sampler2D sAlbedo;

uniform vec4 uColor;

#ifdef ALPHA_TEST
  uniform float uAlphaThreshold;
#endif	//ALPHA_TEST

in vec2 vUV;

out vec4 FragColor;

void main()
{
  vec4 color = texture(sAlbedo, vUV.st);
#ifdef ALPHA_TEST
  if (color.a <= uAlphaThreshold)
  {
    discard;
  }
#endif
  FragColor = color * uColor;
}
