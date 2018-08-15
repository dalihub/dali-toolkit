#version 430

layout(location=0) in mediump vec2 aPosition;
layout(location=0) out mediump vec2 vTexCoord;
layout(location=1) out mediump vec2 vMaskTexCoord;

layout(set = 0, binding = 0, std140) uniform VertData
{
    mediump mat4 uModelMatrix;
    mediump mat4 uMvpMatrix;
    mediump vec3 uSize;
    mediump vec2 uFixed[ 3 ];
    mediump vec2 uStretchTotal;


    //Visual size and offset
    mediump vec2 offset;
    mediump vec2 size;
    mediump vec4 offsetSizeMode;
    mediump vec2 origin;
    mediump vec2 anchorPoint;
};

void main()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);

  mediump vec2 size         = visualSize.xy;

  mediump vec2 fixedFactor  = vec2( uFixed[ int( ( aPosition.x + 1.0 ) * 0.5 ) ].x, uFixed[ int( ( aPosition.y  + 1.0 ) * 0.5 ) ].y );
  mediump vec2 stretch      = floor( aPosition * 0.5 );
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
