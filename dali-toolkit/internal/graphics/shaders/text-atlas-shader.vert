attribute mediump vec2    aPosition;
attribute mediump vec2    aTexCoord;
attribute mediump vec4    aColor;
uniform   mediump vec2    uOffset;
uniform     highp mat4    uMvpMatrix;
varying   mediump vec2    vTexCoord;
varying   mediump vec4    vColor;

void main()
{
  mediump vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}
