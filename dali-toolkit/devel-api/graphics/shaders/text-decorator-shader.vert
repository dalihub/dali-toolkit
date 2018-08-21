#version 430

// in variables
layout( location = 0 ) in mediump vec2 aPosition;

// uniforms
layout( set = 0, binding = 0, std140 ) uniform VertexData
{
  mediump mat4 uMvpMatrix;
};

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}

