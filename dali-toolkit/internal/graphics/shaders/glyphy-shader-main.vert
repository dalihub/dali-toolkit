uniform   mediump mat4    uProjection;
uniform   mediump mat4    uModelView;
uniform   mediump mat4    uMvpMatrix;
uniform           bool    uTextureMapped;
uniform   mediump vec4    uCustomTextureCoords;
attribute highp   vec2    aTexCoord;
varying   mediump vec2    vTexCoord;
uniform   mat3            uModelViewIT;
attribute mediump vec3    aNormal;
varying   mediump vec3    vNormal;
attribute mediump vec2    aPosition;
varying   mediump vec4    vVertex;
attribute mediump vec4    aColor;
varying   mediump vec4    vColor;
varying vec4 v_glyph;

vec4 glyph_vertex_transcode (vec2 v)
{
  ivec2 g = ivec2 (v);
  ivec2 corner = ivec2 (mod (v, 2.));
  g /= 2;
  ivec2 nominal_size = ivec2 (mod (vec2(g), 64.));
  return vec4 (corner * nominal_size, g * 4);
}

void main()
{
  gl_Position = uMvpMatrix * vec4 (aPosition, 0.0, 1.0);
  v_glyph = glyph_vertex_transcode (aTexCoord);
  vColor = aColor;
}