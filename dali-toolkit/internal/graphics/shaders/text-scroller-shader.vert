precision highp float;
attribute highp vec2 aPosition;
varying highp vec2 vTexCoord;
uniform highp vec3 uSize;
uniform highp float uDelta;
uniform highp vec2 uTextureSize;
uniform highp float uGap;
uniform highp float uHorizontalAlign;
uniform highp float uVerticalAlign;

uniform highp mat4 uMvpMatrix;

//Visual size and offset
uniform highp vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

void main()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw);
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  vTexCoord.x = ( uDelta + (aPosition.x - uHorizontalAlign) * visualSize.x - uGap * (0.5 + uHorizontalAlign) ) / uTextureSize.x + uHorizontalAlign + 0.5;
  vTexCoord.y = ( (aPosition.y - uVerticalAlign) * visualSize.y ) / uTextureSize.y + uVerticalAlign + 0.5;

  highp vec4 vertexPosition = vec4( ( aPosition + anchorPoint ) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );

  gl_Position = uMvpMatrix * vertexPosition;
}