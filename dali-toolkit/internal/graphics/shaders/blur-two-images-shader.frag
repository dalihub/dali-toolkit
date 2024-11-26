//@name blur-two-images-shader.frag

//@version 100

precision highp float;

UNIFORM_BLOCK FragBlock
{
    UNIFORM float uBlurStrength;
};

UNIFORM sampler2D sTexture;
UNIFORM sampler2D sEffect;
INPUT mediump vec2 vTexCoord;

void main()
{
  gl_FragColor = TEXTURE( sTexture, vTexCoord ) * uBlurStrength
               + TEXTURE( sEffect, vTexCoord )*(1.0-uBlurStrength);
}