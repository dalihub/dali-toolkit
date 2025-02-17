//@name model3d-view-simple-shader.vert

//@version 100

precision highp float;

INPUT highp vec3 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec3 aNormal;
OUTPUT highp vec3 vIllumination;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec3 uSize;
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp mat4 uModelView;
  UNIFORM highp mat3 uNormalMatrix;
  UNIFORM highp mat4 uObjectMatrix;
  UNIFORM highp vec3 uLightPosition;
};

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
