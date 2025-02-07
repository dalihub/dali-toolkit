//@name npatch-visual-shader.vert

//@version 100

precision highp float;

INPUT highp vec2 aPosition;
OUTPUT highp vec2 vTexCoord;
OUTPUT highp vec2 vMaskTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec2 uNinePatchFactorsX[FACTOR_SIZE_X];
  UNIFORM highp vec2 uNinePatchFactorsY[FACTOR_SIZE_Y];
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
  highp vec2 fixedFactor = vec2(uNinePatchFactorsX[int((aPosition.x + 1.0) * 0.5)].x, uNinePatchFactorsY[int((aPosition.y + 1.0) * 0.5)].x);
  highp vec2 stretch= vec2(uNinePatchFactorsX[int((aPosition.x) * 0.5)].y, uNinePatchFactorsY[int((aPosition.y) * 0.5)].y );

  highp vec2 fixedTotal = vec2(uNinePatchFactorsX[FACTOR_SIZE_X - 1].x, uNinePatchFactorsY[FACTOR_SIZE_Y - 1].x);
  highp vec2 stretchTotal = vec2(uNinePatchFactorsX[FACTOR_SIZE_X - 1].y, uNinePatchFactorsY[FACTOR_SIZE_Y - 1].y);

  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw) + extraSize;
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  // Scale down if fixedTotal is bigger than visualSize
  highp float fixedScaleDownRate = min(1.0, min(visualSize.x / fixedTotal.x, visualSize.y / fixedTotal.y));

  highp vec4 gridPosition = vec4(fixedFactor * fixedScaleDownRate + (visualSize.xy - fixedTotal * fixedScaleDownRate) * stretch / stretchTotal, 0.0, 1.0);
  highp vec4 vertexPosition = gridPosition;
  vertexPosition.xy -= visualSize.xy * vec2(0.5, 0.5);
  vertexPosition.xy += anchorPoint*visualSize + visualOffset + origin * uSize.xy;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = (fixedFactor + stretch) / (fixedTotal + stretchTotal);
  vMaskTexCoord = gridPosition.xy / visualSize;

  gl_Position = vertexPosition;
}
