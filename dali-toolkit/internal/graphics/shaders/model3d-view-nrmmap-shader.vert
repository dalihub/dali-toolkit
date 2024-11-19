attribute highp vec3 aPosition;
attribute highp vec2 aTexCoord;
attribute highp vec3 aNormal;
attribute highp vec3 aTangent;
attribute highp vec3 aBiNormal;
varying mediump vec2 vTexCoord;
varying mediump vec3 vLightDirection;
varying mediump vec3 vHalfVector;
uniform mediump vec3 uSize;
uniform mediump mat4 uMvpMatrix;
uniform mediump mat4 uModelView;
uniform mediump mat3 uNormalMatrix;
uniform mediump mat4 uObjectMatrix;
uniform mediump vec3 uLightPosition;

void main()
{
  vec4 vertexPosition = vec4(aPosition*min(uSize.x, uSize.y), 1.0);
  vertexPosition = uObjectMatrix * vertexPosition;
  vertexPosition = uMvpMatrix * vertexPosition;

  vec4 vertPos = uModelView * vec4(aPosition.xyz, 1.0);
  vec4 lightPos = uModelView * vec4(uLightPosition, 1.0);

  vec3 tangent = normalize(uNormalMatrix * aTangent);
  vec3 binormal = normalize(uNormalMatrix * aBiNormal);
  vec3 normal = normalize(uNormalMatrix * aNormal);

  vec3 vecToLight = normalize( lightPos.xyz - vertPos.xyz );
  vLightDirection.x = dot(vecToLight, tangent);
  vLightDirection.y = dot(vecToLight, binormal);
  vLightDirection.z = dot(vecToLight, normal);

  vec3 viewDir = normalize(-vertPos.xyz);
  vec3 halfVector = normalize(viewDir + vecToLight);
  vHalfVector.x = dot(halfVector, tangent);
  vHalfVector.y = dot(halfVector, binormal);
  vHalfVector.z = dot(halfVector, normal);

  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
