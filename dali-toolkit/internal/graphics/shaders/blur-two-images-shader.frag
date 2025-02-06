precision highp float;
uniform float uBlurStrength;
uniform sampler2D sTexture;
uniform sampler2D sEffect;
varying highp vec2 vTexCoord;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uBlurStrength
               + texture2D( sEffect, vTexCoord )*(1.0-uBlurStrength);
}