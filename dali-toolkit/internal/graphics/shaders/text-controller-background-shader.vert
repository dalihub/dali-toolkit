precision highp float;
attribute highp vec2 aPosition;
attribute highp vec4 aColor;
varying   highp vec4 vColor;
uniform   highp mat4 uMvpMatrix;

void main()
{
  highp vec4 position = vec4( aPosition, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vColor = aColor;
}
