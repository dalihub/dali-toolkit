precision mediump float;
attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
uniform mediump mat4 uMvpMatrix;
uniform mediump vec3 uSize;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition * uSize.xy, 0.0, 1.0);
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * vertexPosition;
}
