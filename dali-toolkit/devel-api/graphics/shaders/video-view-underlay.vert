#version 430

layout( location = 0 ) in mediump vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mediump mat4 uMvpMatrix;
  mediump vec3 uSize;
};

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
