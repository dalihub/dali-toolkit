precision highp float;
INPUT highp vec2 aPosition;
INPUT highp vec2 aDrift;

uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp float borderSize;

//Visual size and offset
uniform highp vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

vec2 ComputeVertexPosition()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw );
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);
  return (aPosition + anchorPoint)*visualSize + visualOffset + origin * uSize.xy;
}

void main()
{
  highp vec2 position = ComputeVertexPosition() + aDrift*borderSize;
  gl_Position = uMvpMatrix * vec4(position, 0.0, 1.0);
}