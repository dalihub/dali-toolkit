#ifndef DALI_RENDERING_ADDON_H
#define DALI_RENDERING_ADDON_H

/*
* Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/common/addon-binder.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * Interface of Overdrawing AddOn
 */
class RenderingAddOn : public Dali::AddOn::AddOnBinder
{
  using TextureManager = Dali::Toolkit::Internal::TextureManager;
public:
  RenderingAddOn() : Dali::AddOn::AddOnBinder( "oo-rendering", 0u )
  {}

  // Bind AddOn functions
  ADDON_BIND_FUNCTION(
    GetGeometry,
    Dali::Geometry(TextureManager::TextureId, uint32_t&, uint32_t& ) );

  ADDON_BIND_FUNCTION(
    CreateGeometry,
    Dali::Geometry( TextureManager::TextureId, const Dali::Devel::PixelBuffer& pixelBuffer ) );

  ADDON_BIND_FUNCTION(
    Initialize,
    void*() );

  ADDON_BIND_FUNCTION(
    CreateGeometryGrid,
    Dali::Geometry( const void*, const Uint16Pair&, uint32_t*) );

  ADDON_BIND_FUNCTION(
    SubmitRenderTask,
    void( Renderer&, const void* ) );

  ADDON_BIND_FUNCTION(
    BuildNPatch,
    void*( const Devel::PixelBuffer&, void*) );

  ADDON_BIND_FUNCTION(
    DestroyNPatch,
    void( void* ) );

  /**
   * Single instance of the addon
   * @return
   */
  static RenderingAddOn& Get()
  {
    static RenderingAddOn* addon = nullptr;
    if( !addon )
    {
      addon = new RenderingAddOn();
      if(addon->IsValid())
      {
        addon->Initialize();
      }
    }
    return *addon;
  }
};

} // Internal
} // Toolkit
} // Dali

#endif //DALI_CMAKE_RENDERING_ADDON_H
