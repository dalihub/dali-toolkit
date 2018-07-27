#version 430

// in variables
layout( location = 0 ) in vec2 aPosition;

// uniforms
layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mat4 uMvpMatrix;
};

void main()
{
  vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}

