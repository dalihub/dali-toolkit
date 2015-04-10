#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_BACKEND_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_BACKEND_H__

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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/text-backend.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Internal
{

/**
 * Implementation of the text rendering backend
 */
class Backend : public BaseObject
{
public:

  /**
   * Constructor
   */
  Backend();

  /**
   * Destructor
   */
  ~Backend();

  /**
   * @copydoc Dali::Toolkit::Text::Backend::Get()
   */
  static Dali::Toolkit::Text::Backend Get();

  /**
   * @copydoc Dali::Toolkit::Text::Backend::NewRenderer()
   */
  RendererPtr NewRenderer( unsigned int renderingType );

private:

  // Undefined copy constructor.
  Backend( const Backend& );

  // Undefined assignment constructor.
  Backend& operator=( Backend& );

private:

  struct Impl;
  Impl* mImpl;

}; // class Backend

} // namespace Internal

inline static Internal::Backend& GetImplementation(Backend& backend)
{
  DALI_ASSERT_ALWAYS( backend && "backend handle is empty" );
  BaseObject& handle = backend.GetBaseObject();
  return static_cast<Internal::Backend&>(handle);
}

inline static const Internal::Backend& GetImplementation(const Backend& backend)
{
  DALI_ASSERT_ALWAYS( backend && "backend handle is empty" );
  const BaseObject& handle = backend.GetBaseObject();
  return static_cast<const Internal::Backend&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_BACKEND_H__
