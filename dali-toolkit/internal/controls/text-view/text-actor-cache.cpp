/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/text-view/text-actor-cache.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

TextActorCache::TextActorCache()
: mTextActors()
{
}

TextActorCache::~TextActorCache()
{
}

void TextActorCache::InsertTextActors( const std::vector<TextActor>& textActors )
{
  mTextActors.insert( mTextActors.end(), textActors.rbegin(), textActors.rend() );
}

TextActor TextActorCache::RetrieveTextActor()
{
  // Text-actors are inserted in the order needed to retrieve always the last one.

  // Returns a non initialized handle if the cache is empty.
  TextActor textActor;

  if( !mTextActors.empty() )
  {
    textActor = mTextActors.back();
    mTextActors.pop_back();
  }

  return textActor;
}

void TextActorCache::ClearTexts()
{
  for( std::vector<TextActor>::iterator it = mTextActors.begin(); it != mTextActors.end(); ++it )
  {
    (*it).SetText("");
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
