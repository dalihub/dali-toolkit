attribute mediump vec2    aPosition;
uniform   highp mat4      uMvpMatrix;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}
