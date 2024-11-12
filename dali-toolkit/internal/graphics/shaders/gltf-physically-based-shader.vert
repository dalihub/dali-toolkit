//@name gltf-physically-based-shader.vert

//@version 100

INPUT highp vec3 aPosition;
INPUT mediump vec2 aTexCoord0;
INPUT mediump vec2 aTexCoord1;
INPUT lowp vec3 aNormal;
INPUT lowp vec4 aTangent;
INPUT lowp vec4 aVertexColor;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec3 uSize;
  UNIFORM mediump mat4 uModelMatrix;
  UNIFORM mediump mat4 uViewMatrix;
  UNIFORM mediump mat4 uProjection;
  UNIFORM mediump vec3 uLightVector;
  UNIFORM lowp float uIsPointLight;
  UNIFORM lowp float uHasVertexColor;
};

OUTPUT lowp vec2 vUV[2];
OUTPUT lowp mat3 vTBN;
OUTPUT lowp vec4 vColor;
OUTPUT highp vec3 vLightDirection;
OUTPUT highp vec3 vPositionToCamera;

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
