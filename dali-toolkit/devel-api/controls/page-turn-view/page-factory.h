#ifndef __DALI_TOOLKIT_PAGE_FACTORY_H__
#define __DALI_TOOLKIT_PAGE_FACTORY_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief PageFactory is an abstract interface for providing textures to PageTurnView
 * Each page is identified by a unique ID, and has a linear order from 0 to GetNumberOfPages()-1
 *
 * @SINCE_1_1.4
 */
class DALI_TOOLKIT_API PageFactory
{
public:

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Virtual destructor
   */
  virtual ~PageFactory(){};

  /**
   * @brief Query the number of pages available from the factory.
   *
   * The maximum available page has an ID of GetNumberOfPages()-1.
   * @return The page count.
   */
  virtual unsigned int GetNumberOfPages() = 0;

  /**
   * @brief Return the texture for the page
   *
   * For double-sided page( PageTurnLandscapeView ), the left half of texture is used as page front side, and the right half as page back side.
   *
   * @note Must return a valid texture handle!
   *
   * @param[in] pageId The ID of the page to create.
   * @return An actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Texture NewPage( unsigned int pageId ) = 0;

  /**
   * @brief Retrieve the extension for this factory
   *
   * @return The extension if available, NULL otherwise.
   */
  virtual Extension* GetExtension()
  {
    return NULL;
  }
};

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_PAGE_FACTORY_H__ */
