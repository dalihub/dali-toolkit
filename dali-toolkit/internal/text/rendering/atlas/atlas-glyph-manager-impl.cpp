 /*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/common/stage.h>

#define MAKE_SHADER(A)#A

namespace
{
const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute mediump vec2    aTexCoord;
uniform   mediump mat4    uMvpMatrix;
uniform   mediump vec3    uSize;
varying   mediump vec2    vTexCoord;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  position.xyz *= uSize;
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
}
);

const char* FRAGMENT_SHADER = MAKE_SHADER(
uniform         sampler2D sTexture;
varying mediump vec2      vTexCoord;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord );
}
);

const char* VERTEX_SHADER_SHADOW = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute mediump vec2    aTexCoord;
uniform   mediump vec3    uSize;
varying   mediump vec2    vTexCoord;

void main()
{
  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
  position.xyz *= uSize;
  gl_Position = position;
  vTexCoord = aTexCoord;
}
);

const char* FRAGMENT_SHADER_SHADOW = MAKE_SHADER(
uniform         sampler2D sTexture;
uniform lowp    vec4      uColor;
varying mediump vec2      vTexCoord;

void main()
{
  mediump vec4 color = texture2D( sTexture, vTexCoord );
  gl_FragColor = vec4(uColor.rgb, uColor.a*color.r);
}
);
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

AtlasGlyphManager::AtlasGlyphManager()
{
  mAtlasManager = Dali::Toolkit::AtlasManager::New();
  mEffectBufferShader = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER );
  mShadowShader = Shader::New( VERTEX_SHADER_SHADOW, FRAGMENT_SHADER_SHADOW, Dali::Shader::HINT_MODIFIES_GEOMETRY );
}

AtlasGlyphManager::~AtlasGlyphManager()
{
}

AtlasGlyphManagerPtr AtlasGlyphManager::New()
{
  AtlasGlyphManagerPtr internal = new AtlasGlyphManager();
  return internal;
}

void AtlasGlyphManager::Add( const Text::GlyphInfo& glyph,
                             const BufferImage& bitmap,
                             Dali::Toolkit::AtlasManager::AtlasSlot& slot )
{
  GlyphRecord record;
  record.mFontId = glyph.fontId;
  record.mIndex = glyph.index;

  mAtlasManager.Add( bitmap, slot );
  record.mImageId = slot.mImageId;
  mGlyphRecords.PushBack( record );
}

void AtlasGlyphManager::GenerateMeshData( uint32_t imageId,
                                          const Vector2& position,
                                          Toolkit::AtlasManager::Mesh2D& mesh )
{
  mAtlasManager.GenerateMeshData( imageId, position, mesh );
}

void AtlasGlyphManager::StitchMesh( Toolkit::AtlasManager::Mesh2D& first,
                                    const Toolkit::AtlasManager::Mesh2D& second )
{
  mAtlasManager.StitchMesh( first, second );
}

void AtlasGlyphManager::Cached( Text::FontId fontId,
                                uint32_t index,
                                Dali::Toolkit::AtlasManager::AtlasSlot& slot )
{
  for ( uint32_t i = 0; i < mGlyphRecords.Size(); ++i )
  {
    if ( fontId == mGlyphRecords[ i ].mFontId && index == mGlyphRecords[ i ].mIndex )
    {
      slot.mImageId = mGlyphRecords[ i ].mImageId;
      slot.mAtlasId = mAtlasManager.GetAtlas( slot.mImageId );
      return;
    }
  }
  slot.mImageId = 0;
}

Vector2 AtlasGlyphManager::GetAtlasSize( uint32_t atlasId )
{
  Toolkit::AtlasManager::AtlasSize size = mAtlasManager.GetAtlasSize( atlasId );
  return Vector2( static_cast< float >( size.mWidth ), static_cast< float >( size.mHeight ) );
}

void AtlasGlyphManager::SetNewAtlasSize( uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight )
{
  Toolkit::AtlasManager::AtlasSize size;
  size.mWidth = width;
  size.mHeight = height;
  size.mBlockWidth = blockWidth;
  size.mBlockHeight = blockHeight;
  mAtlasManager.SetNewAtlasSize( size );
}

void AtlasGlyphManager::Remove( uint32_t imageId )
{
  if ( mAtlasManager.Remove( imageId ) )
  {
    for ( uint32_t i = 0; i < mGlyphRecords.Size(); ++i )
    {
      if ( mGlyphRecords[ i ].mImageId == imageId )
      {
        mGlyphRecords.Remove( mGlyphRecords.Begin() + i );
        return;
      }
    }
  }
}

Pixel::Format AtlasGlyphManager::GetPixelFormat( uint32_t atlasId )
{
  return mAtlasManager.GetPixelFormat( atlasId );
}

Material AtlasGlyphManager::GetMaterial( uint32_t atlasId ) const
{
  return mAtlasManager.GetMaterial( atlasId );
}

Sampler AtlasGlyphManager::GetSampler( uint32_t atlasId ) const
{
  return mAtlasManager.GetSampler( atlasId );
}

const Toolkit::AtlasGlyphManager::Metrics& AtlasGlyphManager::GetMetrics()
{
  mMetrics.mGlyphCount = mGlyphRecords.Size();
  mAtlasManager.GetMetrics( mMetrics.mAtlasMetrics );
  return mMetrics;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
