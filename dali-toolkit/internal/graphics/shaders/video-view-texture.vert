//@name video-view-texture.vert

//@version 100

INPUT mediump vec2 aPosition;
OUTPUT mediump vec2 vTexCoord;
UNIFORM_BLOCK VertBlock
{
  UNIFORM highp mat4 uMvpMatrix;
  UNIFORM highp vec3 uSize;
  UNIFORM highp vec2 uSizeRatio;
  UNIFORM highp mat2 uRotationMatrix;
};
OUTPUT mediump vec2 sTexCoordRect;

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
