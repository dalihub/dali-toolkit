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
#include "atlas-upload-observer.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/image-loader/image-atlas-impl.h>

namespace Dali
{

namespace Toolkit
{

AtlasUploadObserver::AtlasUploadObserver()
{}

AtlasUploadObserver::~AtlasUploadObserver()
{
  // Notify the registerd ImageAtlas object about the destruction of observer.
  const std::size_t size( mAtlasList.Count() );
  for( std::size_t i = 0; i < size; ++i )
  {
    if( mAtlasList[i] )
    {
      mAtlasList[i]->ObserverDestroyed( this );
    }
  }
  mAtlasList.Clear();
}

void AtlasUploadObserver::Register( Internal::ImageAtlas& imageAtlas )
{
  // Add to the list so that the ImageAtlas could get notified in the destructor.
  // If the same atlas is exist in the list already, we would still save the duplicated copy.
  mAtlasList.PushBack( &imageAtlas );
}

void AtlasUploadObserver::Unregister( Internal::ImageAtlas& imageAtlas )
{
  const std::size_t size( mAtlasList.Count() );
  for( std::size_t i = 0; i < size; i++ )
  {
    if( mAtlasList[i] == &imageAtlas )
    {
      // Remove from list
      mAtlasList.Erase( mAtlasList.Begin() + i );
      // If there are duplicated copies of same pointer, only the first one is removed
      return;
    }
  }
}

}

}
