attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
uniform mediump mat4 uMvpMatrix;
uniform mediump vec3 uSize;
uniform mediump vec3 effectOffset;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition.xyz += effectOffset;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = aPosition + vec2(0.5);
  gl_Position = vertexPosition;
}
