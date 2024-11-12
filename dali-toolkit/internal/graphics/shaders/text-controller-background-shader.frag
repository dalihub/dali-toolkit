//@name text-controller-background-shader.frag

//@version 100

INPUT mediump vec4      vColor;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp    vec4      uColor;
};

void main()
{
  gl_FragColor = vColor * uColor;
}
