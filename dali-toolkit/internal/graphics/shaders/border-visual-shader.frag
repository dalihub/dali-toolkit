uniform lowp vec4 uColor;
uniform lowp vec4 borderColor;

void main()
{
  OUT_COLOR = borderColor * uColor;
}