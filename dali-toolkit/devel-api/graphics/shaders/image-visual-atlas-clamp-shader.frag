#version 430

layout( location=0 ) in mediump vec2 vTexCoord;

layout( set=1, binding=0, std140 ) uniform FragData
{
    mediump vec4 uAtlasRect;
    lowp vec4 uColor;
    lowp vec3 mixColor;
    lowp float preMultipliedAlpha;
};

layout( set=1, binding=1 ) uniform sampler2D sTexture;

layout( location=0 ) out vec4 fragColor;

void main()
{
    mediump vec2 texCoord = clamp( mix( uAtlasRect.xy, uAtlasRect.zw, vTexCoord ), uAtlasRect.xy, uAtlasRect.zw );
    fragColor = texture( sTexture, texCoord ) * uColor * vec4(mixColor, 1.0);
}
