precision highp float;
varying highp vec3 vIllumination;
uniform lowp vec4 uColor;

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a);
}
