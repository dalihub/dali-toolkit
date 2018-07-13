#version 430

layout( location = 0 ) in vec2 aPosition;
layout( location = 1 ) in vec2 aTexCoord;
layout( location = 2 ) in vec4 aColor;

layout( set = 0, binding = 0, std140 ) uniform VertData
{
    vec2    uOffset;
    mat4    uMvpMatrix;
};

layout( location = 0 ) out vec2 vTexCoord;
layout( location = 1 ) out vec4 vColor;

void main()
{
  vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}