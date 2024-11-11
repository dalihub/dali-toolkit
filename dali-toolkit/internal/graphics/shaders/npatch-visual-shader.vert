attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
varying mediump vec2 vMaskTexCoord;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump vec2 uNinePatchFactorsX[ FACTOR_SIZE_X ];
uniform mediump vec2 uNinePatchFactorsY[ FACTOR_SIZE_Y ];

// Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump vec2 extraSize;

void main()
{
  mediump vec2 fixedFactor= vec2( uNinePatchFactorsX[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uNinePatchFactorsY[ int( ( aPosition.y + 1.0 ) * 0.5 ) ].x );
  mediump vec2 stretch= vec2( uNinePatchFactorsX[ int( ( aPosition.x ) * 0.5 ) ].y, uNinePatchFactorsY[ int( ( aPosition.y ) * 0.5 ) ].y );

  mediump vec2 fixedTotal = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].x, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].x );
  mediump vec2 stretchTotal = vec2( uNinePatchFactorsX[ FACTOR_SIZE_X - 1 ].y, uNinePatchFactorsY[ FACTOR_SIZE_Y - 1 ].y );

  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  // Scale down if fixedTotal is bigger than visualSize
  mediump float fixedScaleDownRate = min(1.0, min(visualSize.x / fixedTotal.x, visualSize.y / fixedTotal.y));

  mediump vec4 gridPosition = vec4( fixedFactor * fixedScaleDownRate + ( visualSize.xy - fixedTotal * fixedScaleDownRate ) * stretch / stretchTotal, 0.0, 1.0 );
  mediump vec4 vertexPosition = gridPosition;
  vertexPosition.xy -= visualSize.xy * vec2( 0.5, 0.5 );
  vertexPosition.xy += anchorPoint*visualSize + visualOffset + origin * uSize.xy;
  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = ( fixedFactor + stretch ) / ( fixedTotal + stretchTotal );
  vMaskTexCoord = gridPosition.xy / visualSize;

  gl_Position = vertexPosition;
}
