//@name border-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
INPUT highp vec2 aDrift;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
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
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  return (aPosition + anchorPoint)*visualSize + visualOffset + origin * uSize.xy;
}

void main()
{
  vec2 position = ComputeVertexPosition() + aDrift*borderSize;
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
}