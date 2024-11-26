//@name npatch-visual-3x3-shader.vert

//@version 100

INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
OUTPUT mediump vec2 vMaskTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec2 uFixed[3];
  UNIFORM highp vec2 uStretchTotal;
  //Visual size and offset
  UNIFORM highp   vec2 offset;
  UNIFORM highp   vec2 size;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
  UNIFORM highp vec2 extraSize;
};

void main()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw) + extraSize;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  highp vec2 fixedFactor = vec2(uFixed[int((aPosition.x + 1.0) * 0.5)].x, uFixed[int((aPosition.y  + 1.0) * 0.5)].y);
  highp vec2 stretch  = floor(aPosition * 0.5);
  highp vec2 fixedTotal = uFixed[2];

  // Scale down if fixedTotal is bigger than visualSize
  highp float fixedScaleDownRate = min(1.0, min(visualSize.x / fixedTotal.x, visualSize.y / fixedTotal.y));

  highp vec4 gridPosition = vec4(fixedFactor * fixedScaleDownRate + (visualSize - fixedTotal * fixedScaleDownRate) * stretch, 0.0, 1.0);
  highp vec4 vertexPosition = gridPosition;
  vertexPosition.xy -= visualSize * vec2(0.5, 0.5);
  vertexPosition.xy += anchorPoint * visualSize + visualOffset + origin * uSize.xy;

  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = (fixedFactor + stretch * uStretchTotal) / (fixedTotal + uStretchTotal);

  vMaskTexCoord = gridPosition.xy / size;
  gl_Position = vertexPosition;
}
