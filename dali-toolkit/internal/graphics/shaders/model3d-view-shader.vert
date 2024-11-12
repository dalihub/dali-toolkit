//@name model3d-view-shader.vert

//@version 100

INPUT highp vec3 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec3 aNormal;
OUTPUT mediump vec2 vTexCoord;
OUTPUT mediump vec3 vIllumination;
OUTPUT mediump float vSpecular;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec3 uSize;
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM mediump mat4 uModelView;
  UNIFORM mediump mat3 uNormalMatrix;
  UNIFORM mediump mat4 uObjectMatrix;
  UNIFORM mediump vec3 uLightPosition;
};

void main()
{
  vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);
  vertexPosition = uObjectMatrix * vertexPosition;
  vertexPosition = uMvpMatrix * vertexPosition;

  //Illumination in Model-View space - Transform attributes and uniforms
  vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);
  vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);
  vec3 normal = normalize(uNormalMatrix * aNormal);

  vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );
  vec3 viewDir = normalize(-vertPos.xyz);

  vec3 halfVector = normalize(viewDir + vecToLight);

  float lightDiffuse = dot( vecToLight, normal );
  lightDiffuse = max(0.0,lightDiffuse);
  vIllumination = vec3(lightDiffuse * 0.5 + 0.5);

  vec3 reflectDir = reflect(-vecToLight, normal);
  vSpecular = pow( max(dot(reflectDir, viewDir), 0.0), 4.0 );

  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
