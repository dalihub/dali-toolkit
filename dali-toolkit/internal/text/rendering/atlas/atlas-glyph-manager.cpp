 /*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

AtlasGlyphManager::AtlasGlyphManager()
{
}

AtlasGlyphManager::~AtlasGlyphManager()
{
}

AtlasGlyphManager AtlasGlyphManager::Get()
{
  AtlasGlyphManager manager;

  // Check whether the AtlasGlyphManager is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(AtlasGlyphManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to AtlasGlyphManager
      manager = AtlasGlyphManager(dynamic_cast<Internal::AtlasGlyphManager*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the AtlasGlyphManager and register it as a singleton
      manager = AtlasGlyphManager(new Internal::AtlasGlyphManager());
      singletonService.Register(typeid(manager), manager);
    }
  }
  return manager;
}

AtlasGlyphManager::AtlasGlyphManager(Internal::AtlasGlyphManager *impl)
  : BaseHandle(impl)
{
}

void AtlasGlyphManager::Add( const Text::GlyphInfo& glyph,
                             const GlyphStyle& style,
                             const PixelData& bitmap,
                             AtlasManager::AtlasSlot& slot )
{
  GetImplementation(*this).Add( glyph, style, bitmap, slot );
}

void AtlasGlyphManager::GenerateMeshData( uint32_t imageId,
                                          const Vector2& position,
                                          Toolkit::AtlasManager::Mesh2D& mesh )
{
  GetImplementation(*this).GenerateMeshData( imageId,
                                             position,
                                             mesh );
}

bool AtlasGlyphManager::IsCached( Text::FontId fontId,
                                  Text::GlyphIndex index,
                                  const GlyphStyle& style,
                                  AtlasManager::AtlasSlot& slot )
{
  return GetImplementation(*this).IsCached( fontId, index, style, slot );
}

void AtlasGlyphManager::SetNewAtlasSize( uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight )
{
  GetImplementation(*this).SetNewAtlasSize( width, height, blockWidth, blockHeight );
}

Vector2 AtlasGlyphManager::GetAtlasSize( uint32_t atlasId )
{
  return GetImplementation(*this).GetAtlasSize( atlasId );
}

Pixel::Format AtlasGlyphManager::GetPixelFormat( uint32_t atlasId )
{
  return GetImplementation(*this).GetPixelFormat( atlasId );
}

TextureSet AtlasGlyphManager::GetTextures( uint32_t atlasId ) const
{
  return GetImplementation(*this).GetTextures( atlasId );
}

const Toolkit::AtlasGlyphManager::Metrics& AtlasGlyphManager::GetMetrics()
{
  return GetImplementation(*this).GetMetrics();
}

void AtlasGlyphManager::AdjustReferenceCount( Text::FontId fontId, Text::GlyphIndex index, const GlyphStyle& style, int32_t delta )
{
  GetImplementation(*this).AdjustReferenceCount( fontId, index, style, delta );
}

} // namespace Toolkit

} // namespace Dali
