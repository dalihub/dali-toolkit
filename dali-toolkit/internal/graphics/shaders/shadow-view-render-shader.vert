precision highp float;
attribute highp vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp mat4 uModelMatrix;
uniform highp vec3 uSize;
varying highp vec2 vTexCoord;

uniform highp mat4 uLightCameraProjectionMatrix;
uniform highp mat4 uLightCameraViewMatrix;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;
  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);
}