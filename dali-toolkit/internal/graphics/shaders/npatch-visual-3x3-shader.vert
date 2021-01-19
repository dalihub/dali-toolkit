attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
varying mediump vec2 vMaskTexCoord;
uniform highp   mat4 uMvpMatrix;
uniform highp   vec3 uSize;
uniform mediump vec2 uFixed[ 3 ];
uniform mediump vec2 uStretchTotal;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp   vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;
uniform mediump vec2 extraSize;

void main()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw ) + extraSize;
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);

  mediump vec2 size = visualSize.xy;

  mediump vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );
  mediump vec2 stretch  = floor( aPosition * 0.5 );
  mediump vec2 fixedTotal   = uFixed[ 2 ];

  mediump vec4 gridPosition = vec4( fixedFactor + ( size - fixedTotal ) * stretch, 0.0, 1.0 );
  mediump vec4 vertexPosition = gridPosition;
  vertexPosition.xy -= size * vec2( 0.5, 0.5 );
  vertexPosition.xy += anchorPoint*size + (visualOffset + origin)*uSize.xy;

  vertexPosition = uMvpMatrix * vertexPosition;

  vTexCoord = ( fixedFactor + stretch * uStretchTotal ) / ( fixedTotal + uStretchTotal );

  vMaskTexCoord = gridPosition.xy / size;
  gl_Position = vertexPosition;
}
