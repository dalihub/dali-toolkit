in highp vec3 aPosition;
in mediump vec2 aTexCoord0;
in mediump vec2 aTexCoord1;
in lowp vec3 aNormal;
in lowp vec4 aTangent;
in lowp vec4 aVertexColor;

uniform mediump vec3 uSize;
uniform mediump mat4 uModelMatrix;
uniform mediump mat4 uViewMatrix;
uniform mediump mat4 uProjection;
uniform mediump vec3 uLightVector;
uniform lowp float uIsPointLight;
uniform lowp float uHasVertexColor;

out lowp vec2 vUV[2];
out lowp mat3 vTBN;
out lowp vec4 vColor;
out highp vec3 vLightDirection;
out highp vec3 vPositionToCamera;

void main()
{
  highp vec4 invY = vec4(1.0, -1.0, 1.0, 1.0);
  highp vec4 positionW = uModelMatrix * vec4(aPosition * uSize, 1.0);
  highp vec4 positionV = uViewMatrix * (invY * positionW);

  vPositionToCamera = transpose(mat3(uViewMatrix)) * (-vec3(positionV.xyz / positionV.w));
  vPositionToCamera *= invY.xyz;

  lowp vec3 bitangent = cross(aNormal, aTangent.xyz) * aTangent.w;
  vTBN = mat3(uModelMatrix) * mat3(aTangent.xyz, bitangent, aNormal);

  vUV[0] = aTexCoord0;
  vUV[1] = aTexCoord1;

  vLightDirection = mix(-(invY.xyz * uLightVector), (invY.xyz * uLightVector) - (positionW.xyz / positionW.w), uIsPointLight);

  vColor = mix(vec4(1.0f), aVertexColor, uHasVertexColor);

  gl_Position = uProjection * positionV; // needs w for proper perspective correction
  gl_Position = gl_Position/gl_Position.w;
}
