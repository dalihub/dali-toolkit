//@name text-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec3 uScale;
  UNIFORM highp float pixelSnapFactor;
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
  vec4 result = vec4( (aPosition + anchorPoint) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );

  vec2 snappedPosition = result.xy;
  snappedPosition.x = floor(snappedPosition.x * uScale.x + 0.5) / uScale.x;
  snappedPosition.y = floor(snappedPosition.y * uScale.y + 0.5) / uScale.y;

  snappedPosition.x = snappedPosition.x + (1.0 - abs(mod(uSize.x, 2.0) - 1.0)) * 0.5;
  snappedPosition.y = snappedPosition.y + (1.0 - abs(mod(uSize.y, 2.0) - 1.0)) * 0.5;

  result.xy = mix(result.xy, snappedPosition, pixelSnapFactor);

  return result;
}

void main()
{
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
