precision mediump float;
varying mediump vec3vIllumination;
uniform lowpvec4uColor;
uniform lowpvec3mixColor;

void main()
{
  vec4 baseColor = vec4(mixColor, 1.0) * uColor;
  gl_FragColor = vec4( vIllumination.rgb * baseColor.rgb, baseColor.a );
}
