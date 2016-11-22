#ifndef DALI_TOOLKIT_TEXT_TYPESETTER_H
#define DALI_TOOLKIT_TEXT_TYPESETTER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/pixel-data.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class ModelInterface;
class ViewModel;
class Typesetter;

typedef IntrusivePtr<Typesetter> TypesetterPtr;

/**
 * @brief This class is responsible of controlling the data flow of the text's rendering process.
 */
class Typesetter : public RefObject
{
public: // Constructor.
  /**
   * @brief Creates a Typesetter instance.
   *
   * The typesetter composes the final text retrieving the glyphs and the
   * styles from the text's model.
   *
   * @param[in] model Pointer to the text's data model.
   */
  static TypesetterPtr New( const ModelInterface* const model );

public:
  /**
   * @brief Retrieves the pointer to the view model.
   *
   * @return A pointer to the view model.
   */
  ViewModel* GetViewModel();

  /**
   * @brief Renders the text.
   *
   * Does the following operations:
   * - Finds the visible pages needed to be rendered.
   * - Elide glyphs if needed.
   * - Retrieves the data buffers from the text model.
   * - Creates the pixel data used to generate the final image with the given size.
   * - Traverse the visible glyphs, retrieve their bitmaps and compose the final pixel data.
   *
   * @param[in] size The renderer size.
   *
   * @return A pixel data with the text rendered.
   */
  PixelData Render( const Vector2& size );

private:
  /**
   * @brief Private constructor.
   *
   * @param[in] model Pointer to the text's data model.
   */
  Typesetter( const ModelInterface* const model );

  // Declared private and left undefined to avoid copies.
  Typesetter( const Typesetter& handle );

  // Declared private and left undefined to avoid copies.
  Typesetter& operator=( const Typesetter& handle );

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * Destroys the visual model.
   */
  virtual ~Typesetter();

private:
   ViewModel* mModel;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_TYPESETTER_H
