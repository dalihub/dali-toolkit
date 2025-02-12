attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;
uniform highp vec2 uSizeRatio;
uniform highp mat2 uRotationMatrix;
varying mediump vec2 sTexCoordRect;

highp vec2 getSize(highp vec2 size)
{
  if (size.x < 0.5 || size.y < 0.5)
  {
    return uSize.xy;
  }
  else
  {
    highp float ratio = min(uSize.x / size.x, uSize.y / size.y);
    return vec2(size.xy * ratio);
  }
}

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition * getSize(uSizeRatio), 0.0, 1.0);
  vTexCoord = aPosition * uRotationMatrix + vec2(0.5);
}
