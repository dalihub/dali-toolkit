#version 430

layout(location=0) in mediump vec2 vTexCoord;
layout(location=0) out vec4 fragColor;

layout(set=0, binding=1, std140) uniform RenderData
{
  lowp vec4 uShadowColor;
};

layout(set=0, binding=2) uniform sampler2D sTexture;

void main()
{
  lowp float alpha;
  alpha = texture(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;
  fragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);
}
