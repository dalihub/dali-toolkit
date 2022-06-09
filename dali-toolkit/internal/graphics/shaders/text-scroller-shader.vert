attribute mediump vec2 aPosition;
varying highp vec2 vTexCoord;
uniform highp vec3 uSize;
uniform highp float uDelta;
uniform mediump vec2 uTextureSize;
uniform highp float uGap;
uniform mediump float uHorizontalAlign;
uniform mediump float uVerticalAlign;

uniform highp mat4 uMvpMatrix;

//Visual size and offset
uniform mediump vec2 offset;
uniform highp vec2 size;
uniform mediump vec4 offsetSizeMode;
uniform mediump vec2 origin;
uniform mediump vec2 anchorPoint;

void main()
{
  highp vec2 visualSize = mix(size * uSize.xy, size, offsetSizeMode.zw);
  highp vec2 visualOffset = mix(offset * uSize.xy, offset, offsetSizeMode.xy);

  vTexCoord.x = ( uDelta + uHorizontalAlign * ( uTextureSize.x - visualSize.x - uGap ) + aPosition.x * visualSize.x - uGap * 0.5 ) / uTextureSize.x + 0.5;
  vTexCoord.y = ( uVerticalAlign * ( uTextureSize.y - visualSize.y ) + aPosition.y * visualSize.y ) / ( uTextureSize.y ) + 0.5;

  highp vec4 vertexPosition = vec4( ( aPosition + anchorPoint ) * visualSize + visualOffset + origin * uSize.xy, 0.0, 1.0 );

  gl_Position = uMvpMatrix * vertexPosition;
}