precision mediump float;
varying mediump vec3 vIllumination;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a ) * vec4( mixColor, 1.0 );
}
