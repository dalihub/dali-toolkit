//@name cube-transition-effect.vert

//@version 100

INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump mat4 uMvpMatrix;
  UNIFORM mediump vec3 uSize;
  UNIFORM mediump vec4 uTextureRect;
};

void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = aPosition + vec2(0.5);
  vTexCoord = mix(uTextureRect.xy, uTextureRect.zw, vTexCoord);

  gl_Position = vertexPosition;
}