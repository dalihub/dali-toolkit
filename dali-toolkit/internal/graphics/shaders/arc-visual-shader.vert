//@name arc-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vPosition;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
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


vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  vPosition = aPosition* visualSize;
  return vec4( vPosition + anchorPoint*visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}