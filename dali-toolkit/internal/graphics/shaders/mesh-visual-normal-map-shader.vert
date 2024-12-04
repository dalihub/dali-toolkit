//@name mesh-visual-normal-map-shader.vert

//@version 100

INPUT highp vec3 aPosition;
INPUT highp vec2 aTexCoord;
INPUT highp vec3 aNormal;
INPUT highp vec3 aTangent;
INPUT highp vec3 aBiNormal;
OUTPUT mediump vec2 vTexCoord;
OUTPUT mediump vec3 vLightDirection;
OUTPUT mediump vec3 vHalfVector;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec3 uSize;
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM mediump mat4 uModelView;
  UNIFORM mediump mat4 uViewMatrix;
  UNIFORM mediump mat3 uNormalMatrix;
  UNIFORM mediump mat4 uObjectMatrix;
  UNIFORM mediump vec3 lightPosition;
  UNIFORM mediump vec2 uStageOffset;

//Visual size and offset
  UNIFORM mediump vec2 offset;
  UNIFORM mediump vec2 size;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
};

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  float scaleFactor = min( visualSize.x, visualSize.y );
  vec3 originFlipY =vec3(origin.x, -origin.y, 0.0);
  vec3 anchorPointFlipY = vec3( anchorPoint.x, -anchorPoint.y, 0.0);
  vec3 visualOffset = vec3( offset * offsetSizeMode.xy + offset * uSize.xy * (1.0 - offsetSizeMode.xy), 0.0) * vec3(1.0,-1.0,1.0);
  return vec4( (aPosition + anchorPointFlipY)*scaleFactor + visualOffset + originFlipY * uSize, 1.0 );
}

void main()
{
  vec4 normalisedVertexPosition = ComputeVertexPosition();
  vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;
  vertexPosition = uMvpMatrix * vertexPosition;

  vec4 mvVertexPosition = uModelView * normalisedVertexPosition;

  vec3 tangent = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aTangent );
  vec3 binormal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aBiNormal );
  vec3 normal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aNormal );

  vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );
  mvLightPosition = uViewMatrix * mvLightPosition;
  vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );
  vLightDirection.x = dot( vectorToLight, tangent );
  vLightDirection.y = dot( vectorToLight, binormal );
  vLightDirection.z = dot( vectorToLight, normal );

  vec3 viewDirection = normalize( -mvVertexPosition.xyz );
  vec3 halfVector = normalize( viewDirection + vectorToLight );
  vHalfVector.x = dot( halfVector, tangent );
  vHalfVector.y = dot( halfVector, binormal );
  vHalfVector.z = dot( halfVector, normal );

  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
