precision highp float;
attribute highp vec2 aPosition;
varying highp vec2 vTexCoord;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition * uSize.xy, 0.0, 1.0);
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * vertexPosition;
}
