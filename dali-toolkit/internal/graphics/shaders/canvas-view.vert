attribute mediump vec2 aPosition;
varying mediump vec2 vTexCoord;
uniform highp mat4 uMvpMatrix;
uniform highp vec3 uSize;

void main()
{
  gl_Position = uMvpMatrix * vec4(aPosition * uSize.xy, 0.0, 1.0);
  vTexCoord = aPosition + vec2(0.5);
}
