//@name bubble-emitter.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
INPUT highp vec2 aTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec3 uSize;
  UNIFORM highp mat4 uMvpMatrix;
};
OUTPUT highp vec2 vTexCoord;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy,0.0,1.0);
  vTexCoord = aTexCoord;
}
