attribute mediump vec2 aPosition;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump vec4 pixelArea;

varying mediump vec2 vTexCoord;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  return vec4( (aPosition + anchorPoint) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
}

void main()
{
  gl_Position = uMvpMatrix * ComputeVertexPosition();
  vTexCoord = pixelArea.xy+pixelArea.zw*(aPosition + vec2(0.5) );
}
