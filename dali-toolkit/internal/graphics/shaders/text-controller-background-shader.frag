//@name text-controller-background-shader.frag

//@version 100

precision highp float;

INPUT highp vec4      vColor;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp    vec4      uColor;
};

void main()
{
  gl_FragColor = vColor * uColor;
}
