#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_ACTOR_CACHE_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_ACTOR_CACHE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/actors/text-actor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Stores text-actors to be reused.
 *
 * Is it asumed that the first text-actor of a group of text-actors added to the cache is the first one to be rehused.
 */
class TextActorCache
{

public:
  /**
   * Default constructor.
   */
  TextActorCache();

  /**
   * Destructor.
   */
  ~TextActorCache();

  /**
   * Inserts the given text-actors into the cache.
   *
   * First text-actor of the vector is the first one to be reused.
   *
   * @param[in] textActors The text-actors to be inserted into the cache.
   */
  void InsertTextActors( const std::vector<TextActor>& textActors );

  /**
   * Retrieves a text-actor from the cache.
   *
   * @return A handle to a text-actor. It returns a non initialized handle if the cache has no text-actors.
   */
  TextActor RetrieveTextActor();

  /**
   * Clears the text of the text-actor in the cache.
   */
  void ClearTexts();

private:
  std::vector<TextActor> mTextActors; ///< Stores cached text-actors.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_ACTOR_CACHE_H__
