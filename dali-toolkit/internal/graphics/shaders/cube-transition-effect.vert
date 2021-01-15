attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
uniform mediump mat4 uMvpMatrix;
uniform mediump vec3 uSize;
uniform mediump vec4 uTextureRect;

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = aPosition + vec2(0.5);
  vTexCoord = mix(uTextureRect.xy, uTextureRect.zw, vTexCoord);

  gl_Position = vertexPosition;
}