//@name text-visual-shader.vert

//@version 100

INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;

UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;

  //Visual size and offset
  UNIFORM mediump vec2 offset;
  UNIFORM highp vec2 size;
  UNIFORM mediump vec4 offsetSizeMode;
  UNIFORM mediump vec2 origin;
  UNIFORM mediump vec2 anchorPoint;
};

vec4 ComputeVertexPosition()
{
  vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  return vec4( (aPosition + anchorPoint) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );
}

void main()
{
  vTexCoord = aPosition + vec2(0.5);
  gl_Position = uMvpMatrix * ComputeVertexPosition();
}
