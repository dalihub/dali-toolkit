attribute mediump vec2    aPosition;
attribute mediump vec4    aColor;
varying   mediump vec4    vColor;
uniform   highp mat4      uMvpMatrix;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vColor = aColor;
}
