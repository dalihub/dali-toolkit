precision highp float;
attribute highp vec2 aPosition;
uniform   highp mat4 uMvpMatrix;

void main()
{
  highp vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
}
