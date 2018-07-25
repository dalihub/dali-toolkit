#version 430

layout(location=0) in vec2 aPosition;

layout(location=0) out vec2 vTexCoord;

layout(set=0, binding=0, std140) uniform RenderData
{
  mat4 uMvpMatrix;
  mat4 uModelMatrix;
  vec3 uSize;
  mat4 uLightCameraProjectionMatrix;
  mat4 uLightCameraViewMatrix;
};

void main()
{
  vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;
  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);
}
