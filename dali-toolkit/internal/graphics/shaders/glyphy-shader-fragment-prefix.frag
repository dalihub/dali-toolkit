//@name glyphy-shader-fragment-prefix.frag

//@version 100

struct Material
{
  mediump float mOpacity;
  mediump float mShininess;
  lowp    vec4  mAmbient;
  lowp    vec4  mDiffuse;
  lowp    vec4  mSpecular;
  lowp    vec4  mEmissive;
};

UNIFORM sampler2D     sTexture;
UNIFORM sampler2D     sOpacityTexture;
UNIFORM sampler2D     sNormalMapTexture;
UNIFORM sampler2D     sEffect;
INPUT mediump vec2 vTexCoord;
UNIFORM Material      uMaterial;
UNIFORM lowp  vec4    uColor;
INPUT highp vec4    vVertex;
INPUT highp vec3    vNormal;
INPUT mediump vec4  vColor;
UNIFORM vec4 u_atlas_info;

#define GLYPHY_TEXTURE1D_EXTRA_DECLS , sampler2D _tex, ivec4 _atlas_info, ivec2 _atlas_pos
#define GLYPHY_TEXTURE1D_EXTRA_ARGS , _tex, _atlas_info, _atlas_pos
#define GLYPHY_DEMO_EXTRA_ARGS , sTexture, uu_atlas_info, gi.atlas_pos

vec4 glyphy_texture1D_func (int offset GLYPHY_TEXTURE1D_EXTRA_DECLS)
{
  ivec2 item_geom = _atlas_info.zw;
  vec2 pos = (vec2 (_atlas_pos.xy * item_geom +
                    ivec2 (mod (float (offset), float (item_geom.x)), offset / item_geom.x)) +
             + vec2 (.5, .5)) / vec2(_atlas_info.xy);
  return TEXTURE (_tex, pos);
}
