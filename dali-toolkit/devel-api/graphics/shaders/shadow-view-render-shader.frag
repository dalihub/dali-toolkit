#version 430

layout(location=0) in vec2 vTexCoord;
layout(location=0) out vec4 fragColor;

layout(set=0, binding=1, std140) uniform RenderData
{
  vec4 uShadowColor;
};

layout(set=0, binding=2) uniform sampler2D sTexture;

void main()
{
  float alpha;
  alpha = texture(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;
  fragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);
}
