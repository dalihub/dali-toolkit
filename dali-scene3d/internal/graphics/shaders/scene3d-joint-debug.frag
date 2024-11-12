//@version 100

precision mediump float;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};
INPUT flat float vColor;

void main()
{
  vec3 rgb = vec3(fract(vColor), fract(vColor * 0.00390625), fract(vColor * 0.00390625 * 0.00390625));
  gl_FragColor = vec4(rgb, 1.) * uColor;
}