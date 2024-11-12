//@name shadow-view-render-shader.vert

//@version 100

INPUT mediump vec2 aPosition;
OUTPUT vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM mediump mat4 uModelMatrix;
  UNIFORM vec3 uSize;
  UNIFORM mediump mat4 uLightCameraProjectionMatrix;
  UNIFORM mediump mat4 uLightCameraViewMatrix;
};

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vertexPosition;
  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);
}