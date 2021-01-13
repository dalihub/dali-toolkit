precision mediump float;
varying mediump vec2 vTexCoord;
varying mediump vec3 vLightDirection;
varying mediump vec3 vHalfVector;
uniform sampler2D sDiffuse;
uniform sampler2D sNormal;
uniform sampler2D sGloss;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  vec4 texture = texture2D( sDiffuse, vTexCoord );
  vec3 normal = normalize( texture2D( sNormal, vTexCoord ).xyz * 2.0 - 1.0 );
  vec4 glossMap = texture2D( sGloss, vTexCoord );
  vec4 visualMixColor = vec4( mixColor, 1.0 );

  float lightDiffuse = max( 0.0, dot( normal, normalize( vLightDirection ) ) );
  lightDiffuse = lightDiffuse * 0.5 + 0.5;

  float shininess = pow ( max ( dot ( normalize( vHalfVector ), normal ), 0.0 ), 16.0 );

  gl_FragColor = vec4( texture.rgb * uColor.rgb * visualMixColor.rgb * lightDiffuse + shininess * glossMap.rgb, texture.a * uColor.a * visualMixColor.a );
}
