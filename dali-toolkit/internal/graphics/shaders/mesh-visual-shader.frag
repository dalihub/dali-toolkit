//@name mesh-visual-shader.frag

//@version 100

precision mediump float;
INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vIllumination;
INPUT mediump float vSpecular;
UNIFORM sampler2D sDiffuse;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  vec4 tex = TEXTURE( sDiffuse, vTexCoord );
  gl_FragColor = vec4( vIllumination.rgb * tex.rgb * uColor.rgb + vSpecular * 0.3, tex.a * uColor.a );
}
