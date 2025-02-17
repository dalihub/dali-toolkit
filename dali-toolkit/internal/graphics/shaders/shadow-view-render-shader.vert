//@name shadow-view-render-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp mat4 uModelMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp mat4 uLightCameraProjectionMatrix;
  UNIFORM highp mat4 uLightCameraViewMatrix;
};

void main()
{
  highp vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;
  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);
}