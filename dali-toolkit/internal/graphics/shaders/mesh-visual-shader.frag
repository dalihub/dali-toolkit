precision mediump float;
varying mediump vec2 vTexCoord;
varying mediump vec3 vIllumination;
varying mediump float vSpecular;
uniform sampler2D sDiffuse;
uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;
uniform lowp float preMultipliedAlpha;

void main()
{
  vec4 texture = texture2D( sDiffuse, vTexCoord );
  vec4 visualMixColor = vec4( mixColor, 1.0 );
  gl_FragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb * visualMixColor.rgb + vSpecular * 0.3, texture.a * uColor.a * visualMixColor.a );
}
