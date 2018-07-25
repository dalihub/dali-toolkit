#version 430

layout( location = 0 ) in vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mat4 uMvpMatrix;
  vec3 uSize;
};

void main()
{
  vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
