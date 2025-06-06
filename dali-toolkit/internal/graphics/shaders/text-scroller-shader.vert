//@name text-scroller-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp vec3 uSize;
  UNIFORM highp float uDelta;
  UNIFORM highp vec2 uTextureSize;
  UNIFORM highp float uGap;
  UNIFORM highp float uHorizontalAlign;
  UNIFORM highp float uVerticalAlign;
  UNIFORM highp mat4 uMvpMatrix;
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

void main()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw) + extraSize;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  vTexCoord.x = ( uDelta + uHorizontalAlign * ( uTextureSize.x - visualSize.x - uGap ) + aPosition.x * visualSize.x - uGap * 0.5 ) / uTextureSize.x + 0.5;
  vTexCoord.y = ( uVerticalAlign * ( uTextureSize.y - visualSize.y ) + aPosition.y * visualSize.y ) / ( uTextureSize.y ) + 0.5;

  highp vec4 vertexPosition = vec4( ( aPosition + anchorPoint ) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );

  vec2 snappedPosition = vertexPosition.xy;
  snappedPosition.x = floor(snappedPosition.x * uScale.x + 0.5) / uScale.x;
  snappedPosition.y = floor(snappedPosition.y * uScale.y + 0.5) / uScale.y;

  snappedPosition.x = snappedPosition.x + (1.0 - abs(mod(uSize.x, 2.0) - 1.0)) * 0.5;
  snappedPosition.y = snappedPosition.y + (1.0 - abs(mod(uSize.y, 2.0) - 1.0)) * 0.5;

  vertexPosition.xy = mix(vertexPosition.xy, snappedPosition, pixelSnapFactor);

  gl_Position = uMvpMatrix * vertexPosition;
}