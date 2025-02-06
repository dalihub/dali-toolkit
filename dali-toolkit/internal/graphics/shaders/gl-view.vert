precision highp float;
attribute highp vec2 aPosition;
uniform highp mat4   uMvpMatrix;
uniform highp vec3   uSize;
varying highp vec2   vTexCoord;

void main()
{
  vec4 position = vec4(aPosition, 0.0, 1.0) * vec4(uSize, 1.0);
  vTexCoord     = aPosition + vec2(0.5);
  gl_Position   = uMvpMatrix * position;
}