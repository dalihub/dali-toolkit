//Very simple fragment shader that merely applies the vertex shading to the color at each fragment.

precision mediump float;
varying mediump vec3 vIllumination;
uniform lowp vec4 uColor;

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
}
