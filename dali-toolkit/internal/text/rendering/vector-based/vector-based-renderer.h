#ifndef __DALI_TOOLKIT_TEXT_VECTOR_BASED_RENDERER_H__
#define __DALI_TOOLKIT_TEXT_VECTOR_BASED_RENDERER_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief A vector-based renderer
 *
 */
class VectorBasedRenderer : public Renderer
{
public:

  /**
   * @brief Create the renderer.
   */
  static RendererPtr New();

  /**
   * @copydoc Renderer::Render()
   */
  virtual Actor Render( ViewInterface& view,
                        float& alignmentOffset,
                        int depth );

protected:

  /**
   * @brief Constructor.
   */
  VectorBasedRenderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~VectorBasedRenderer();

private:

  // Undefined
  VectorBasedRenderer( const VectorBasedRenderer& handle );

  // Undefined
  VectorBasedRenderer& operator=( const VectorBasedRenderer& handle );

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_VECTOR_BASED_RENDERER_H__
