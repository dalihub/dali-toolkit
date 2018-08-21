#version 430

layout( location = 0 ) in mediump vec2 aPosition;
layout( location = 1 ) in mediump vec2 aTexCoord;
layout( location = 2 ) in mediump vec4 aColor;

layout( set = 0, binding = 0, std140 ) uniform VertData
{
    mediump vec2    uOffset;
    mediump mat4    uMvpMatrix;
};

layout( location = 0 ) out mediump vec2 vTexCoord;
layout( location = 1 ) out mediump vec4 vColor;

void main()
{
  mediump vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}