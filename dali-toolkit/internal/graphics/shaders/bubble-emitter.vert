//@name bubble-emitter.vert

//@version 100

INPUT mediump vec2 aPosition;
INPUT mediump vec2 aTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump vec3 uSize;
  UNIFORM mediump mat4 uMvpMatrix;
};
OUTPUT mediump vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
  vTexCoord = aTexCoord;
}
