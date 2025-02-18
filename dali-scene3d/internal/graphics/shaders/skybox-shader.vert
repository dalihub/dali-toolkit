//@name skybox-shader.vert

//@version 100

precision highp float;

INPUT highp vec3 aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uModelView;
  UNIFORM highp mat4 uModelMatrix;
  UNIFORM highp mat4 uViewMatrix;
  UNIFORM highp mat4 uProjection;
  UNIFORM highp mat4 uMvpMatrix;
};
OUTPUT highp vec3 vTexCoord;

void main()
{
  vTexCoord.x = aPosition.x;
  vTexCoord.y = -aPosition.y; // convert to GL coords
  vTexCoord.z = aPosition.z;

  highp vec4 vertexPosition = vec4(aPosition, 1.0);
  highp vec4 clipSpacePosition = uProjection * mat4(mat3(uModelView)) * vertexPosition;
  // Writes 1.0, the maximum depth value, into the depth buffer.
  // This is an optimization to avoid running the fragment shader
  // for the pixels hidden by the scene's objects.
  gl_Position = clipSpacePosition.xyww;
}