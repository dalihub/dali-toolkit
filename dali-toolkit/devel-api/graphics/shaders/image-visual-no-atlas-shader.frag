#version 430

layout( location=0 ) in mediump vec2 vTexCoord;

layout( set=0, binding=1, std140 ) uniform FragDataNoAtlas
{
    lowp vec4 uColor;
    lowp vec3 mixColor;
    lowp float preMultipliedAlpha;
};

layout( set=0, binding=2 ) uniform sampler2D sTexture;

layout( location=0 ) out vec4 fragColor;

void main()
{
    fragColor = texture( sTexture, vTexCoord ) * uColor * vec4(mixColor, 1.0);
}
