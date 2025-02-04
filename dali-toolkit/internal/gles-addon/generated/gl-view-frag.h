#pragma once

const std::string_view SHADER_GL_VIEW_FRAG
{
R"(//@name gl-view.frag

//@version 100
INPUT mediump vec2       vTexCoord;
UNIFORM samplerExternalOES sTexture;

UNIFORM_BLOCK FragBlock
{
  UNIFORM lowp vec4          uColor;
};

void main()
{
  gl_FragColor = TEXTURE(sTexture, vTexCoord) * uColor;
}
)"
};
