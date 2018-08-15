#version 430

layout(location=0) in mediump vec2 vTexCoord;
layout(location=1) in mediump vec2 vMaskTexCoord;

layout(set=0, binding=1, std140) uniform FragData
{
    lowp vec4 uColor;
    lowp vec3 mixColor;
    mediump float auxiliaryImageAlpha;
};

layout(set=0, binding=2) uniform sampler2D sTexture;
layout(set=0, binding=3) uniform sampler2D sMask;

layout(location=0) out vec4 fragColor;

void main()
{
  mediump vec4 color = texture( sTexture, vTexCoord );
  mediump vec4 mask  = texture( sMask, vMaskTexCoord );

  mediump vec3 mixedColor = color.rgb * mix( 1.0-mask.a, 1.0, 1.0-auxiliaryImageAlpha) + mask.rgb*mask.a * auxiliaryImageAlpha;

  fragColor = vec4(mixedColor,1.0) * uColor * vec4( mixColor, 1.0 );
}
