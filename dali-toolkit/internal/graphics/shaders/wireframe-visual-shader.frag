uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  gl_FragColor = uColor * vec4( mixColor, 1.0 );
}
