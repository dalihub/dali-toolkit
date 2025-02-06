precision highp float;
varying highp vec2 vTexCoord;
varying highp vec3 vIllumination;
varying highp float vSpecular;
uniform sampler2D sDiffuse;
uniform lowp vec4 uColor;

void main()
{
  vec4 texture = texture2D( sDiffuse, vTexCoord );
  gl_FragColor = vec4( vIllumination.rgb * texture.rgb * uColor.rgb + vSpecular * 0.3, texture.a * uColor.a );
}
