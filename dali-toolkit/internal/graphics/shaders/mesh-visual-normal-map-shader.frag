//@name mesh-visual-normal-map-shader.frag

//@version 100

precision mediump float;
INPUT mediump vec2 vTexCoord;
INPUT mediump vec3 vLightDirection;
INPUT mediump vec3 vHalfVector;
UNIFORM sampler2D sDiffuse;
UNIFORM sampler2D sNormal;
UNIFORM sampler2D sGloss;
UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
};

void main()
{
  vec4 tex = TEXTURE( sDiffuse, vTexCoord );
  vec3 normal = normalize( TEXTURE( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );
  vec4 glossMap = TEXTURE( sGloss, vTexCoord );

  float lightDiffuse = max( 0.0, dot( normal, normalize( vLightDirection ) ) );
  lightDiffuse = lightDiffuse * 0.5 + 0.5;

  float shininess = pow ( max ( dot ( normalize( vHalfVector ), normal ), 0.0 ), 16.0 );

  gl_FragColor = vec4( tex.rgb * uColor.rgb * lightDiffuse + shininess * glossMap.rgb, tex.a * uColor.a );
}
