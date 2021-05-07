struct Material
{
  mediump float mOpacity;
  mediump float mShininess;
  lowp    vec4  mAmbient;
  lowp    vec4  mDiffuse;
  lowp    vec4  mSpecular;
  lowp    vec4  mEmissive;
};

uniform sampler2D     sTexture;
uniform sampler2D     sOpacityTexture;
uniform sampler2D     sNormalMapTexture;
uniform sampler2D     sEffect;
varying mediump vec2 vTexCoord;
uniform Material      uMaterial;
uniform lowp  vec4    uColor;
varying highp vec4    vVertex;
varying highp vec3    vNormal;
varying mediump vec4  vColor;
uniform vec4 u_atlas_info;

#define GLYPHY_TEXTURE1D_EXTRA_DECLS , sampler2D _tex, ivec4 _atlas_info, ivec2 _atlas_pos
#define GLYPHY_TEXTURE1D_EXTRA_ARGS , _tex, _atlas_info, _atlas_pos
#define GLYPHY_DEMO_EXTRA_ARGS , sTexture, uu_atlas_info, gi.atlas_pos

vec4 glyphy_texture1D_func (int offset GLYPHY_TEXTURE1D_EXTRA_DECLS)
{
  ivec2 item_geom = _atlas_info.zw;
  vec2 pos = (vec2 (_atlas_pos.xy * item_geom +
                    ivec2 (mod (float (offset), float (item_geom.x)), offset / item_geom.x)) +
             + vec2 (.5, .5)) / vec2(_atlas_info.xy);
  return texture2D (_tex, pos);
}
