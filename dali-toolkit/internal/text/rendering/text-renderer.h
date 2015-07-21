#ifndef __DALI_TOOLKIT_TEXT_RENDERER_H__
#define __DALI_TOOLKIT_TEXT_RENDERER_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class Renderer;
typedef IntrusivePtr<Renderer> RendererPtr;

class ViewInterface;

/**
 * @brief Abstract base class for Text renderers.
 *
 * This is reponsible for rendering the glyphs from a ViewInterface in the specified positions.
 * It is implemented by returning an Actor intended as the child of a UI control.
 */
class Renderer : public RefObject
{
public:

  /**
   * @brief Render the glyphs from a ViewInterface.
   *
   * @param[in] view The interface to a view.
   * @param[in] depth The depth in the tree of the parent.
   * @return The Renderable actor used to position the text.
   */
  virtual Actor Render( ViewInterface& view, int depth ) = 0;

protected:

  /**
   * @brief Constructor.
   */
  Renderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Renderer();

private:

  // Undefined
  Renderer( const Renderer& handle );

  // Undefined
  Renderer& operator=( const Renderer& handle );
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_RENDERER_H__
