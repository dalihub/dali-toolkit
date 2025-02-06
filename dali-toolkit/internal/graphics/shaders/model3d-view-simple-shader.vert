precision highp float;
attribute highp vec3 aPosition;
attribute highp vec2 aTexCoord;
attribute highp vec3 aNormal;
varying highp vec3 vIllumination;
uniform highp vec3 uSize;
uniform highp mat4 uMvpMatrix;
uniform highp mat4 uModelView;
uniform highp mat3 uNormalMatrix;
uniform highp mat4 uObjectMatrix;
uniform highp vec3 uLightPosition;

void main()
{
  vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);
  vertexPosition = uObjectMatrix * vertexPosition;
  vertexPosition = uMvpMatrix * vertexPosition;

  //Illumination in Model-View space - Transform attributes and uniforms
  vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);
  vec3 normal = uNormalMatrix * aNormal;
  vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);
  vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );

  float lightDiffuse = max( dot( vecToLight, normal ), 0.0 );
  vIllumination = vec3(lightDiffuse * 0.5 + 0.5);

  gl_Position = vertexPosition;
}
