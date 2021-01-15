attribute mediump vec2 aPosition;
varying highp vec2 vTexCoord;
uniform highp vec3 uSize;
uniform mediump float uDelta;
uniform mediump vec2 uTextureSize;
uniform mediump float uGap;
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
  mediump vec2 visualOffset = mix( offset, offset/uSize.xy, offsetSizeMode.xy );
  mediump vec2 visualSize = mix( uSize.xy * size, size, offsetSizeMode.zw );

  vTexCoord.x = ( uDelta + uHorizontalAlign * ( uTextureSize.x - visualSize.x - uGap ) + floor( aPosition.x * visualSize.x ) + 0.5 - uGap * 0.5 ) / uTextureSize.x + 0.5;
  vTexCoord.y = ( uVerticalAlign * ( uTextureSize.y - visualSize.y ) + floor( aPosition.y * visualSize.y ) + 0.5 ) / ( uTextureSize.y ) + 0.5;

  mediump vec4 vertexPosition = vec4( floor( ( aPosition + anchorPoint ) * visualSize + ( visualOffset + origin ) * uSize.xy ), 0.0, 1.0 );

  gl_Position = uMvpMatrix * vertexPosition;
}