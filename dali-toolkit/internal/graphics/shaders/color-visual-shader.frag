uniform lowp vec4 uColor;
uniform lowp vec3 mixColor;

void main()
{
  OUT_COLOR = vec4(mixColor, 1.0) * uColor;
}