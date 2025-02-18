//@name mesh-visual-simple-shader.frag

//@version 100

precision highp float;

INPUT highp vec3 vIllumination;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  gl_FragColor = vec4( vIllumination.rgb * uColor.rgb, uColor.a );
}
