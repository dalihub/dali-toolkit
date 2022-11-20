#version 300 es

precision mediump float;
uniform lowp vec4 uColor;
flat in float vColor;
out vec4 FragColor;

void main()
{
  vec3 rgb = vec3(fract(vColor), fract(vColor * 0.00390625), fract(vColor * 0.00390625 * 0.00390625));
  FragColor = vec4(rgb, 1.) * uColor;
}