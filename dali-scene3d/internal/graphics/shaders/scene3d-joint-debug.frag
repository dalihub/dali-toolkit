//@name scene3d-joint-debug.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};
FLAT INPUT float vColor;

void main()
{
  vec3 rgb = vec3(fract(vColor), fract(vColor * 0.00390625), fract(vColor * 0.00390625 * 0.00390625));
  gl_FragColor = vec4(rgb, 1.) * uColor;
}