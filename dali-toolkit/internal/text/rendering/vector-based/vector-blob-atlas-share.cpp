 /*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/rendering/vector-based/vector-blob-atlas-share.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/singleton-service.h>

namespace
{

const int INITIAL_VECTOR_BLOB_ATLAS_WIDTH = 512;
const int INITIAL_VECTOR_BLOB_ATLAS_HEIGHT = 512;

const int NEW_VECTOR_BLOB_ATLAS_WIDTH = 1024;
const int NEW_VECTOR_BLOB_ATLAS_HEIGHT = 1024;

const int VECTOR_BLOB_ATLAS_ITEM_WIDTH = 64;
const int VECTOR_BLOB_ATLAS_HEIGHT_QUANTUM = 8;

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class VectorBlobAtlasShare::Impl : public Dali::BaseObject
{
public:

  /**
   * @brief Constructor
   */
  Impl()
  {
  }

  VectorBlobAtlas* GetCurrentAtlas()
  {
    if( ! mCurrentAtlas )
    {
      mCurrentAtlas = new VectorBlobAtlas( INITIAL_VECTOR_BLOB_ATLAS_WIDTH, INITIAL_VECTOR_BLOB_ATLAS_HEIGHT, VECTOR_BLOB_ATLAS_ITEM_WIDTH, VECTOR_BLOB_ATLAS_HEIGHT_QUANTUM );
    }

    return mCurrentAtlas.Get();
  }

  VectorBlobAtlas* GetNewAtlas()
  {
    // The current atlas should have been filled, before asking for a new one
    DALI_ASSERT_DEBUG( mCurrentAtlas->IsFull() && "Current atlas is not full yet" );

    mCurrentAtlas = new VectorBlobAtlas( NEW_VECTOR_BLOB_ATLAS_WIDTH, NEW_VECTOR_BLOB_ATLAS_HEIGHT, VECTOR_BLOB_ATLAS_ITEM_WIDTH, VECTOR_BLOB_ATLAS_HEIGHT_QUANTUM );

    return mCurrentAtlas.Get();
  }

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Impl()
  {
  }

private:

  IntrusivePtr<VectorBlobAtlas> mCurrentAtlas;
};

VectorBlobAtlasShare::VectorBlobAtlasShare()
{
}

VectorBlobAtlasShare::~VectorBlobAtlasShare()
{
}

VectorBlobAtlasShare VectorBlobAtlasShare::Get()
{
  VectorBlobAtlasShare manager;

  // Check whether the VectorBlobAtlasShare is already created
  SingletonService singletonService( SingletonService::Get() );
  if ( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton( typeid( VectorBlobAtlasShare ) );
    if( handle )
    {
      // If so, downcast the handle of singleton to VectorBlobAtlasShare
      manager = VectorBlobAtlasShare( dynamic_cast<VectorBlobAtlasShare::Impl*>( handle.GetObjectPtr() ) );
    }

    if( !manager )
    {
      // If not, create the VectorBlobAtlasShare and register it as a singleton
      manager = VectorBlobAtlasShare( new VectorBlobAtlasShare::Impl() );
      singletonService.Register( typeid( manager ), manager );
    }
  }

  return manager;
}

VectorBlobAtlasShare::VectorBlobAtlasShare( VectorBlobAtlasShare::Impl* impl )
: BaseHandle( impl )
{
}

VectorBlobAtlas* VectorBlobAtlasShare::GetCurrentAtlas()
{
  VectorBlobAtlasShare::Impl& impl = static_cast<VectorBlobAtlasShare::Impl&>( GetBaseObject() );

  return impl.GetCurrentAtlas();
}

VectorBlobAtlas* VectorBlobAtlasShare::GetNewAtlas()
{
  VectorBlobAtlasShare::Impl& impl = static_cast<VectorBlobAtlasShare::Impl&>( GetBaseObject() );

  return impl.GetNewAtlas();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
