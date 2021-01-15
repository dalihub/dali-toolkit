INPUT mediump vec2 aPosition;
INPUT mediump vec2 aDrift;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform mediump float borderSize;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec2 ComputeVertexPosition()
{
  vec2 visualSize = mix(uSize.xy*size, size, offsetSizeMode.zw );
  vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy);
  return (aPosition + anchorPoint)*visualSize + (visualOffset + origin)*uSize.xy;
}

void main()
{
  vec2 position = ComputeVertexPosition() + aDrift*borderSize;
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
}