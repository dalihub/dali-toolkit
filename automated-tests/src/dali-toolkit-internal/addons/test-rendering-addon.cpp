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
 */


// EXTERNAL HEADERS
#include <dali/devel-api/addons/addon-base.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL HEADERS
// Needed to access the private class members
#define private public
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#undef private


using Dali::Toolkit::Internal::TextureManager;

namespace Dali
{
namespace AddOns
{

struct DummyTiler
{
};

void* CreateInstance( TextureManager* textureManager )
{
  fprintf(stderr, "AddOn::CreateInstance( %p )\n", textureManager);
  return new DummyTiler;
}

namespace GeometryTiler
{
std::vector<std::string> gCallStack;

static Geometry GetGeometryInternal(TextureManager::TextureId textureId, uint32_t& o0, uint32_t& o1 )
{
  gCallStack.emplace_back( "GetGeometry" );
  o0 = 10;
  o1 = 5;
  fprintf(stderr, "AddOn::GetGeometryInternal()\n");
  return Dali::Geometry::New();
}

static Geometry CreateGeometryInternal(TextureManager::TextureId textureId, const Devel::PixelBuffer& pixelBuffer )
{
  gCallStack.emplace_back( "CreateGeometry" );
  fprintf(stderr, "AddOn::CreateGeometryInternal()\n");
  return Dali::Geometry::New();
}

static Geometry CreateGeometryMapInternal(const void* opacityMap,
                                          const Uint16Pair& gridSize,
                                          uint32_t *outElements)
{
  gCallStack.emplace_back( "CreateGeometryGrid" );
  outElements[0] = 2;
  outElements[1] = 3;
  return Dali::Geometry::New();
}

static void* NPatchBuildInternal(const Devel::PixelBuffer& pixelBuffer, Toolkit::Internal::NPatchData* data )
{
  gCallStack.emplace_back( "BuildNPatch" );
  fprintf(stderr, "AddOn::NPatchBuild()\n");
  static char dummyData;
  return &dummyData;
}

static void NPatchDestroyInternal(void* object )
{
  gCallStack.emplace_back( "DestroyNPatch" );
  fprintf(stderr, "AddOn::NPatchDestroy()\n");
}

static void SubmitInternal(Renderer& renderer, const void* object  )
{
  gCallStack.emplace_back( "SubmitRenderTask" );
  fprintf(stderr, "AddOn::SubmitInternal()\n");
}

static std::vector<std::string> GetCallStack( bool clear )
{
  auto retval = gCallStack;
  if(clear)
  {
    gCallStack.clear();
  }
  return retval;
}


}
}
}

/**
 * OverdrawingAddOn implementation
 */
class TestRenderingAddOn : public Dali::AddOns::AddOnBase
{
public:

  void GetAddOnInfo( Dali::AddOnInfo& info ) override
  {
    info.type = Dali::AddOnType::GENERIC;
    info.name = "oo-rendering";
    info.version = Dali::DALI_ADDON_VERSION( 1, 0, 0 );
    info.next = nullptr;
  }

  /**
   * Dispatch table for global functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetGlobalDispatchTable() override
  {
    static Dali::AddOns::DispatchTable dispatchTable{};
    if( dispatchTable.Empty() )
    {
      dispatchTable["Initialize"]         = Dali::AddOns::CreateInstance;
      dispatchTable["CreateGeometry"]     = Dali::AddOns::GeometryTiler::CreateGeometryInternal;
      dispatchTable["GetGeometry"]        = Dali::AddOns::GeometryTiler::GetGeometryInternal;
      dispatchTable["CreateGeometryGrid"] = Dali::AddOns::GeometryTiler::CreateGeometryMapInternal;
      dispatchTable["BuildNPatch"]        = Dali::AddOns::GeometryTiler::NPatchBuildInternal;
      dispatchTable["DestroyNPatch"]      = Dali::AddOns::GeometryTiler::NPatchDestroyInternal;
      dispatchTable["SubmitRenderTask"]   = Dali::AddOns::GeometryTiler::SubmitInternal;
      dispatchTable["GetCallStack"]       = Dali::AddOns::GeometryTiler::GetCallStack;
    }
    return &dispatchTable;
  }

  /**
   * Lifecycle
   */

  void OnStart() override
  {
  }

  void OnStop() override
  {
  }

  void OnPause() override
  {
  }

  void OnResume() override
  {
  }

  /**
   * Dispatch table for instance functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetInstanceDispatchTable() override
  {
    return nullptr;
  }
};

REGISTER_ADDON_CLASS( TestRenderingAddOn );
