precision highp float;
varying highp vec2 vTexCoord;
varying highp vec3 vLightDirection;
varying highp vec3 vHalfVector;
uniform sampler2D sDiffuse;
uniform sampler2D sNormal;
uniform sampler2D sGloss;
uniform lowp vec4 uColor;

void main()
{
  vec4 texture = texture2D( sDiffuse, vTexCoord );
  vec3 normal = normalize( texture2D( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );
  vec4 glossMap = texture2D( sGloss, vTexCoord );

  float lightDiffuse = max( 0.0, dot( normal, normalize( vLightDirection ) ) );
  lightDiffuse = lightDiffuse * 0.5 + 0.5;

  float shininess = pow ( max ( dot ( normalize( vHalfVector ), normal ), 0.0 ), 16.0 );

  gl_FragColor = vec4( texture.rgb * uColor.rgb * lightDiffuse + shininess * glossMap.rgb, texture.a * uColor.a );
}
