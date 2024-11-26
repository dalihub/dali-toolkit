//@name glyphy-shader-main.vert

//@version 100

UNIFORM   mediump mat4    uProjection;
UNIFORM   mediump mat4    uModelView;
UNIFORM   mediump mat4    uMvpMatrix;
UNIFORM           bool    uTextureMapped;
UNIFORM   mediump vec4    uCustomTextureCoords;
INPUT highp   vec2    aTexCoord;
OUTPUT   mediump vec2    vTexCoord;
UNIFORM   mat3            uModelViewIT;
INPUT mediump vec3    aNormal;
OUTPUT   mediump vec3    vNormal;
INPUT mediump vec2    aPosition;
OUTPUT   mediump vec4    vVertex;
INPUT mediump vec4    aColor;
OUTPUT   mediump vec4    vColor;
OUTPUT vec4 v_glyph;

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