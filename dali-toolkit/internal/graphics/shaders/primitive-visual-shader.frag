//Very simple fragment shader that merely applies the vertex shading to the color at each fragment.

precision mediump float;
varying mediump vec3 vIllumination;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  vec4 baseColor = vec4(mixColor, 1.0) * uColor;
  gl_FragColor = vec4( vIllumination.rgb * baseColor.rgb, baseColor.a );
}
