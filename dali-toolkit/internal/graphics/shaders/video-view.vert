precision highp float;
attribute highp vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  gl_Position = uMvpMatrix * vertexPosition;
}
