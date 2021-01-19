attribute mediump vec2 aPosition;
uniform mediump mat4 uMvpMatrix;
uniform mediump mat4 uModelMatrix;
uniform vec3 uSize;
varying vec2 vTexCoord;

uniform mediump mat4 uLightCameraProjectionMatrix;
uniform mediump mat4 uLightCameraViewMatrix;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;
  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);
}