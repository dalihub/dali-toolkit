//@name page-turn-book-spine-effect.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM float uTextureWidth;
};

UNIFORM_BLOCK SharedBlock
{
UNIFORM vec3 uSize;
};

OUTPUT vec2 vTexCoord;

void main()
{
  highp vec4 vertexPosition = vec4(aPosition*uSize.xy, 0.0, 1.0);
  gl_Position = uMvpMatrix * vertexPosition;
  vTexCoord = aPosition + vec2(0.5);
  vTexCoord.x /= uTextureWidth;
}
