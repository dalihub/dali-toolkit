precision highp float;
attribute highp vec3 aPosition;
attribute highp vec2 aTexCoord;
attribute highp vec3 aNormal;
varying highp vec2 vTexCoord;
varying highp vec3 vIllumination;
varying highp float vSpecular;
uniform highp vec3 uSize;
uniform highp mat4 uMvpMatrix;
uniform highp mat4 uModelView;
uniform highp mat4 uViewMatrix;
uniform highp mat3 uNormalMatrix;
uniform highp mat4 uObjectMatrix;
uniform highp vec3 lightPosition;
uniform highp vec2 uStageOffset;

//Visual size and offset
uniform highp vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  float scaleFactor = min( visualSize.x, visualSize.y );
  vec3 originFlipY =vec3(origin.x, -origin.y, 0.0);
  vec3 anchorPointFlipY = vec3( anchorPoint.x, -anchorPoint.y, 0.0);
  vec3 visualOffset = vec3( offset * offsetSizeMode.xy + offset * uSize.xy * (1.0-offsetSizeMode.xy), 0.0) * vec3(1.0,-1.0,1.0);
  return vec4( (aPosition + anchorPointFlipY)*scaleFactor + visualOffset + originFlipY * uSize, 1.0 );
}

void main()
{
  vec4 normalisedVertexPosition = ComputeVertexPosition();
  vec4 vertexPosition = uObjectMatrix * normalisedVertexPosition;
  vertexPosition = uMvpMatrix * vertexPosition;

  //Illumination in Model-View space - Transform attributes and uniforms
  vec4 mvVertexPosition = uModelView * normalisedVertexPosition;
  vec3 normal = normalize( uNormalMatrix * mat3( uObjectMatrix ) * aNormal );

  vec4 mvLightPosition = vec4( ( lightPosition.xy - uStageOffset ), lightPosition.z, 1.0 );
  mvLightPosition = uViewMatrix * mvLightPosition;
  vec3 vectorToLight = normalize( mvLightPosition.xyz - mvVertexPosition.xyz );

  vec3 viewDirection = normalize( -mvVertexPosition.xyz );

  float lightDiffuse = dot( vectorToLight, normal );
  lightDiffuse = max( 0.0,lightDiffuse );
  vIllumination = vec3( lightDiffuse * 0.5 + 0.5 );

  vec3 reflectDirection = reflect( -vectorToLight, normal );
  vSpecular = pow( max( dot( reflectDirection, viewDirection ), 0.0 ), 4.0 );

  vTexCoord = aTexCoord;
  gl_Position = vertexPosition;
}
