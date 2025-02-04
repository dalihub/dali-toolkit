#pragma once

const std::string_view SHADER_GL_VIEW_VERT
{
R"(//@name gl-view.vert

//@version 100

INPUT mediump vec2 aPosition;
UNIFORM_BLOCK VertBlock
{
  UNIFORM mediump mat4   uMvpMatrix;
  UNIFORM mediump vec3   uSize;
};
OUTPUT mediump vec2   vTexCoord;

void main()
{
  vec4 position = vec4(aPosition, 0.0, 1.0) * vec4(uSize, 1.0);
  vTexCoord     = aPosition + vec2(0.5);
  gl_Position   = uMvpMatrix * position;
}
)"
};
