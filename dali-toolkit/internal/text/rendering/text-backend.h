#ifndef __DALI_TOOLKIT_TEXT_BACKEND_H__
#define __DALI_TOOLKIT_TEXT_BACKEND_H__

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Internal DALI_INTERNAL
{
class Backend;
}

// The type of text renderer required
enum RenderingType
{
  RENDERING_BASIC,       ///< A bitmap-based reference implementation
  RENDERING_SHARED_ATLAS ///< A bitmap-based solution where renderers can share a texture atlas
};

/**
 * @brief Provides access to different text rendering backends.
 */
class DALI_IMPORT_API Backend : public BaseHandle
{
public:

  /**
   * @brief Retrieve a handle to the Backend instance.
   *
   * @return A handle to the Backend
   */
  static Backend Get();

  /**
   * @brief Create a renderer from a particluar rendering type.
   *
   * @param[in] renderingType The type of rendering required.
   * @return A handle to the newly created renderer.
   */
  RendererPtr NewRenderer( unsigned int renderingType );

  /**
   * @brief Create an uninitialized TextAbstraction handle.
   */
  Backend();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Backend();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  Backend( const Backend& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  Backend& operator=( const Backend& handle );

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Backend::Get().
   *
   * @param[in] backend A pointer to the internal backend object.
   */
  explicit DALI_INTERNAL Backend( Internal::Backend* backend );
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_BACKEND_H__
