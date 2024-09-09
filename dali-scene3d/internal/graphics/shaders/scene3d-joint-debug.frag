
precision mediump float;
uniform lowp vec4 uColor;
FLAT INPUT float vColor;

void main()
{
  vec3 rgb = vec3(fract(vColor), fract(vColor * 0.00390625), fract(vColor * 0.00390625 * 0.00390625));
  OUT_COLOR = vec4(rgb, 1.) * uColor;
}