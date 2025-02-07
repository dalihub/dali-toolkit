//@name border-visual-anti-aliasing-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
INPUT highp vec2 aDrift;
OUTPUT highp float vAlpha;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
}

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4 uColor;
  UNIFORM lowp vec4 borderColor;
  UNIFORM highp float borderSize;
};

UNIFORM_BLOCK VisualVertBlock
{
  //Visual size and offset
  UNIFORM highp vec2 offset;
  UNIFORM highp vec2 size;
  UNIFORM highp vec2 extraSize;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
};

vec2 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize + vec2(0.75, 0.75);
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  return (aPosition + anchorPoint)*visualSize + visualOffset + origin * uSize.xy;
}

void main()
{
  vec2 position = ComputeVertexPosition() + aDrift*(borderSize+1.5);
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
  vAlpha = min( abs(aDrift.x), abs(aDrift.y) )*(borderSize+1.5);
}