varying mediump vec4      vColor;
uniform lowp    vec4      uColor;

void main()
{
  gl_FragColor = vColor * uColor;
}
