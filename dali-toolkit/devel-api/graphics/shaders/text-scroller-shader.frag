#version 430

layout(location=0) in vec2 vTexCoord;

layout( set = 0, binding = 1, std140 ) uniform FragData
{
    vec4 uColor;
    vec3 mixColor;
    float preMultipliedAlpha;
};

layout( set = 0, binding = 2 ) uniform sampler2D sTexture;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    if ( vTexCoord.y > 1.0 )
      discard;

    vec4 textTexture = texture( sTexture, vTexCoord );

    fragColor = textTexture * uColor * vec4( mixColor, 1.0 );
}
